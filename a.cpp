#ifdef __linux__
#include <sched.h>
#elif _WIN32
#include <processthreadsapi.h>
#include <windows.h>
#endif
#include <optional>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <thread>
#include <memory>
#include <iostream>
#include <atomic>
#include <tuple>
#include <fstream>
template <typename R, typename... Args>
struct Data {
    void* ptr;
    R(*fun)(std::tuple<Args...>&);
    std::tuple<Args...> args;
    bool heapAllocated{false};
    size_t refCount{0};
};


template <typename T, typename U>
struct Func {
    struct ControlBlock {
        virtual U Call(T&&) = 0;

        virtual void Replicate() = 0;
    };


    using type = U;

    template <typename R, size_t idx, typename... Args>
    struct FunctionBlock : public ControlBlock {
        FunctionBlock() {
            auto* data = GetData();
            if (data->heapAllocated) {
                ++data->refCount;
            }
        }

        ~FunctionBlock() {
            auto* data = GetData();
            if (data->heapAllocated) {
                --data->refCount;
                if (data->refCount == 0) {
                    delete data;
                }
            }
        }

        U Call(T&& arg) {
            auto* data = GetData();
            std::get<idx>(data->args) = std::move(arg);
            if constexpr(std::is_same_v<U, R>) {
                return data->fun(data->args);
            } else {
                return U{}.template Set<R, idx + 1, Args...>(data);
            }
        }

        Data<R, Args...>* GetData() {
            return (Data<R, Args...>*)((void*)this);
        }

        void Replicate() {
            auto* data = GetData();
            auto* newData = new Data<R, Args...>(*data);
            newData->heapAllocated = true;
            newData->refCount = 1;
        }
    };

    Func() = default;

    template <typename R, typename... Args>
    Func(Data<R, Args...>* data) : fn(new(&data->ptr) FunctionBlock<R, 0, Args...>()) {}

    U Eval(T&& arg) {
        return fn->Call(std::move(arg));
    }

    auto&& Eval() {
        return std::move(*this);
    }

    template <typename F, size_t idx, typename... Args>
    auto Set(Data<F, Args...>* data) && {
        fn = new(&data->ptr) FunctionBlock<F, idx, Args...>();
        return *this;
    }

    auto& Replicate() {
        fn->Replicate();
        return *this;
    }

    ControlBlock* fn;
};


template <typename U>
struct Func<void, U> {
    struct ControlBlock {
        virtual U Call() = 0;
    };


    using type = U;

    template <typename F, size_t idx, typename... Args>
    struct FunctionBlock : public ControlBlock {
        U Call() {
            auto* data = (Data<F, Args...>*)((void*)this);
            return data->fun(data->args);
        }
    };

    template <typename F, size_t idx, typename... Args>
    auto Set(Data<F, Args...>* data) {
        fn = new(&data->ptr) FunctionBlock<F, idx, Args...>();
        return *this;
    }

    U Eval() {
        return fn->Call();
    }

    ControlBlock* fn;
};

struct Unit {};

template <typename T>
struct SimpleContainer {
    T value;

    auto& Eval() {
        return *this;
    }

    auto& Replicate() {
        return *this;
    }

};

template <typename T>
struct PtrContainer {
    T* ptr{nullptr};

    auto& Eval() {
        return *this;
    }

    PtrContainer<T> Copy(void* newStorage) {
        if (!ptr) {
            return *this;
        }
        new(newStorage)T(Get());
        return PtrContainer<T>{(T*)newStorage};
    }

    auto& Get() {
        if (ptr == nullptr) {
            ptr = new T();
        }
        return *ptr;
    }

    auto& Replicate() {
        return *this;
    }

    constexpr size_t Size() {
        return sizeof(T);
    }
};

const size_t SPIN_COUNT{1 << 13};

void SpinAndWait() {
    for (size_t i = 0; i < SPIN_COUNT; ++i) {}
    std::this_thread::yield();
}

struct Functor {
    struct ControlBlock {
        virtual void Call() = 0;
    };

    template <typename R, typename C>
    struct FunctionBlock : public ControlBlock {
        R* ret_;
        R(C::*fun_)();
        C* caller;
        std::atomic<uint64_t>* cntr;

        FunctionBlock(R* ret, R(C::*fun)(), C* c, std::atomic<uint64_t>* cnt)
                : ret_(ret), fun_(fun), caller(c), cntr(cnt) {}

        void Call() {
            *ret_ = (*caller.*fun_)();
            cntr->fetch_add(1, std::memory_order_release);
        }
    };

    template <typename R, typename C>
    void Load(void* ptr, R* ret, R(C::*fun)(), C* c, std::atomic<size_t>* cnt) {
        cb = new(ptr) FunctionBlock<R, C>(ret, fun, c, cnt);
    }

    ControlBlock* cb;
};

thread_local size_t currentThreadNo;
std::pair<size_t, size_t> threadBounds[12];
thread_local std::vector<std::pair<size_t, size_t>> prevBoundsStack;
thread_local size_t threadsQueue[12] = {(size_t)-1};
thread_local size_t currentQueueIdx{0};

struct PLThread {
    std::mutex mt;
    std::condition_variable cv;
    Functor f;
};

template <size_t N>
class ThreadPool {
public:
    ThreadPool() {
        currentThreadNo = 0;
        threadBounds[0] = {0, N - 1};
#ifdef __linux__
        cpu_set_t mask;
        CPU_ZERO(&mask);
        CPU_SET(1, &mask);
        sched_setaffinity(0, sizeof(mask), &mask);
#elif _WIN32
        SetThreadAffinityMask(GetCurrentThread(),(1));
#endif
        for (size_t i = 1; i < N; ++i) {
            workers[i].emplace([pt = &infos[i], this, i]() {
                currentThreadNo = i;
                pt->f.cb = nullptr;
#ifdef __linux__
                cpu_set_t mask;
                CPU_ZERO(&mask);
                CPU_SET(1 << i, &mask);
                sched_setaffinity(0, sizeof(mask), &mask);
#elif _WIN32
                SetThreadAffinityMask(GetCurrentThread(),(1<<i));
#endif
                Process(*pt);
            });
        }
    }
    void CutTheBounds(size_t pieces) {
        auto& curThreadBounds = threadBounds[currentThreadNo];
        prevBoundsStack.push_back(curThreadBounds);

        size_t baseValue = (curThreadBounds.second - curThreadBounds.first + 1) / pieces;
        size_t additionalValue = (curThreadBounds.second - curThreadBounds.first + 1) % pieces;
        size_t start = threadBounds[currentThreadNo].first;

        size_t j = 0;
        size_t end = 0;

        size_t fin = threadBounds[currentThreadNo].second;
        while (end < fin) {
            end = start + baseValue + (additionalValue > 0 ? 1 : 0) - 1;
            threadBounds[start] = {start, end};
            additionalValue = (additionalValue > 0 ? additionalValue - 1 : 0);

            threadsQueue[pieces - j - 1] = start;
            start = end + 1;
            ++j;
        }
    }

    void PopTheBounds() {
        threadBounds[currentThreadNo] = prevBoundsStack.back();
        prevBoundsStack.pop_back();
    }

    template <typename R, typename C>
    void Reset(void* ptr, R* ret, R(C::*fun)(), C* caller, std::atomic<size_t>* cnt) {
        size_t ctn = currentThreadNo;
        size_t nextThread = threadsQueue[currentQueueIdx];

        if (nextThread == -1 || nextThread == currentThreadNo) {
            *ret = (*caller.*fun)();
            cnt->fetch_add(1, std::memory_order_release);
            return;
        }

        std::unique_lock<std::mutex> lk(infos[nextThread].mt);
        infos[nextThread].f.Load(ptr, ret, fun, caller, cnt);
        infos[nextThread].cv.notify_all();
        ++currentQueueIdx;
    }

    void Process(PLThread& pt) {
        for (;;) {
            std::unique_lock<std::mutex> lk(pt.mt);
            pt.cv.wait(lk, [&pt]() {
                return pt.f.cb != nullptr;
            });
            pt.f.cb->Call();
            pt.f.cb = nullptr;
        }
    }

    ~ThreadPool() {
        for (size_t i = 1; i < N; ++i) {
            workers[i]->detach();
        }
    }


private:
    std::optional<std::thread> workers[N];
    PLThread infos[N];
};

ThreadPool<12> tp;


typedef SimpleContainer<Unit> Void;
typedef SimpleContainer<std::string> String;
typedef SimpleContainer<int> Int;
typedef SimpleContainer<bool> Bool;
struct TreeNodeGen {
    auto& Eval() {
        return *this;
    }

    Int value;

    auto& Getvalue() {
        return value;
    }

    PtrContainer<TreeNodeGen> left;

    auto& Getleft() {
        return left;
    }

    PtrContainer<TreeNodeGen> right;

    auto& Getright() {
        return right;
    }

};

typedef PtrContainer<TreeNodeGen> TreeNode;


typedef Func<TreeNode, Func<void, TreeNode>> BTreeNodeTreeNodeB;

typedef Func<String, Func<void, Int>> BStringIntB;

typedef Func<Int, Func<TreeNode, Func<void, TreeNode>>> BIntBTreeNodeTreeNodeBB;

typedef Func<Int, Func<Int, Func<void, Bool>>> BIntBIntBoolBB;

typedef Func<TreeNode, Func<void, Bool>> BTreeNodeBoolB;

typedef Func<TreeNode, Func<void, Int>> BTreeNodeIntB;

typedef Func<Int, Func<Int, Func<void, Int>>> BIntBIntIntBB;

typedef Func<Int, Func<void, Void>> BIntVoidB;

TreeNode ReadTree (std::tuple<TreeNode>& args);

TreeNode MakeTree (std::tuple<Int, TreeNode>& args);

TreeNode Append (std::tuple<Int, TreeNode>& args);

Int TraverseTree (std::tuple<TreeNode>& args);

Int staticReadInt(std::tuple<String>& args) {
    static std::ifstream fs(std::get<0>(args).value);
    Int i;
    fs >> i.value;
    return i;
}

template <typename a>Bool eq (std::tuple<a, a>& args) {
    return {std::get<0>(args).value == std::get<1>(args).value};
}


template <typename a>
Bool isNull (std::tuple<a>& args) {
    return {std::get<0>(args).ptr == nullptr};
}


Bool ls (std::tuple<Int, Int>& args) {
    return {std::get<0>(args).value < std::get<1>(args).value};
}


TreeNode Append (std::tuple<Int, TreeNode>& args) {

{
    Data<Bool, TreeNode> data18446744073709551615_0{.fun = &isNull};
        auto copy3 = std::move(std::get<1>(args));
    auto arg18446744073709551615_0ret = (BTreeNodeBoolB{&data18446744073709551615_0}).Eval(std::move(copy3)).Eval();
if (arg18446744073709551615_0ret.value) {
		    auto arg18446744073709551615_0ret = std::move(std::get<0>(args)).Eval();
    TreeNode pack_0;
    pack_0.Get().value = arg18446744073709551615_0ret;
    auto arg18446744073709551615_1ret = pack_0.Eval();
	return arg18446744073709551615_1ret.Replicate();
}
}

{
    Data<Bool, Int, Int> data18446744073709551615_0{.fun = &ls};
        auto copy4 = std::move(std::get<1>(args).Get().Getvalue());
    auto arg18446744073709551615_1ret = ((BIntBIntBoolBB{&data18446744073709551615_0}).Eval(std::move(std::get<0>(args))).Eval()).Eval(std::move(copy4)).Eval();
if (arg18446744073709551615_1ret.value) {
		        auto copy5 = std::move(std::get<1>(args).Get().Getvalue());
    auto arg18446744073709551615_1ret = std::move(copy5).Eval();
    Data<TreeNode, Int, TreeNode> data18446744073709551615_0{.fun = &Append};
        auto copy6 = std::move(std::get<1>(args).Get().Getleft());
    auto arg18446744073709551615_2ret = ((BIntBTreeNodeTreeNodeBB{&data18446744073709551615_0}).Eval(std::move(std::get<0>(args))).Eval()).Eval(std::move(copy6)).Eval();
        auto copy7 = std::move(std::get<1>(args).Get().Getright());
    auto arg18446744073709551615_3ret = std::move(copy7).Eval();
    TreeNode pack_1;
    pack_1.Get().value = arg18446744073709551615_1ret;
    pack_1.Get().left = arg18446744073709551615_2ret;
    pack_1.Get().right = arg18446744073709551615_3ret;
    auto arg18446744073709551615_4ret = pack_1.Eval();
	return arg18446744073709551615_4ret.Replicate();
}
}

{
        auto copy8 = std::move(std::get<1>(args).Get().Getvalue());
    auto arg18446744073709551615_4ret = std::move(copy8).Eval();
        auto copy9 = std::move(std::get<1>(args).Get().Getleft());
    auto arg18446744073709551615_5ret = std::move(copy9).Eval();
    Data<TreeNode, Int, TreeNode> data18446744073709551615_0{.fun = &Append};
        auto copy10 = std::move(std::get<1>(args).Get().Getright());
    auto arg18446744073709551615_6ret = ((BIntBTreeNodeTreeNodeBB{&data18446744073709551615_0}).Eval(std::move(std::get<0>(args))).Eval()).Eval(std::move(copy10)).Eval();
    TreeNode pack_2;
    pack_2.Get().value = arg18446744073709551615_4ret;
    pack_2.Get().left = arg18446744073709551615_5ret;
    pack_2.Get().right = arg18446744073709551615_6ret;
    auto arg18446744073709551615_7ret = pack_2.Eval();
	return arg18446744073709551615_7ret.Replicate();
}
}


TreeNode MakeTree (std::tuple<Int, TreeNode>& args) {

{
    Data<Bool, Int, Int> data18446744073709551615_0{.fun = &eq};
    auto arg18446744073709551615_0ret = ((BIntBIntBoolBB{&data18446744073709551615_0}).Eval(std::move(std::get<0>(args))).Eval()).Eval(Int{0}).Eval();
if (arg18446744073709551615_0ret.value) {
		        auto copy1 = std::move(std::get<1>(args));
    auto arg18446744073709551615_0ret = std::move(copy1).Eval();
	return arg18446744073709551615_0ret.Replicate();
}
}

{
    Data<TreeNode, Int, TreeNode> data18446744073709551615_0{.fun = &Append};
    Data<TreeNode, TreeNode> data18446744073709551615_1{.fun = &ReadTree};
        auto copy2 = std::move(std::get<1>(args));
    auto arg18446744073709551615_0ret = (BTreeNodeTreeNodeB{&data18446744073709551615_1}).Eval(((BIntBTreeNodeTreeNodeBB{&data18446744073709551615_0}).Eval(std::move(std::get<0>(args))).Eval()).Eval(std::move(copy2)).Eval()).Eval();
	return arg18446744073709551615_0ret.Replicate();
}
}


TreeNode ReadTree (std::tuple<TreeNode>& args) {

{
    Data<Int, String> data18446744073709551615_0{.fun = &staticReadInt};
    Data<TreeNode, Int, TreeNode> data18446744073709551615_1{.fun = &MakeTree};
        auto copy0 = std::move(std::get<0>(args));
    auto arg18446744073709551615_0ret = ((BIntBTreeNodeTreeNodeBB{&data18446744073709551615_1}).Eval((BStringIntB{&data18446744073709551615_0}).Eval(String{"C:\\Users\\godot\\CLionProjects\\untitled3\\a.txt"}).Eval()).Eval()).Eval(std::move(copy0)).Eval();
	return arg18446744073709551615_0ret.Replicate();
}
}


Int add (std::tuple<Int, Int>& args) {
    return {std::get<0>(args).value + std::get<1>(args).value};
}


Int TraverseTree (std::tuple<TreeNode>& args) {

{
    Data<Bool, TreeNode> data18446744073709551615_0{.fun = &isNull};
        auto copy11 = std::move(std::get<0>(args));
    auto arg18446744073709551615_0ret = (BTreeNodeBoolB{&data18446744073709551615_0}).Eval(std::move(copy11)).Eval();
if (arg18446744073709551615_0ret.value) {
		    auto arg18446744073709551615_0ret = Int{0}.Eval();
	return arg18446744073709551615_0ret.Replicate();
}
}

{
    std::atomic<uint64_t> counter0{0};

    tp.CutTheBounds(2);
    Data<Int, TreeNode> data1_0{.fun = &TraverseTree};
    auto arg1_0 = (BTreeNodeIntB{&data1_0}).Eval(std::move(std::get<0>(args).Get().Getright()));
    void* arg1_0storage[6];
    decltype(arg1_0)::type arg1_0ret;
    tp.Reset(arg1_0storage, &arg1_0ret, &decltype(arg1_0)::Eval, &arg1_0, &counter0);

    Data<Int, TreeNode> data1_1{.fun = &TraverseTree};
    auto arg1_1 = (BTreeNodeIntB{&data1_1}).Eval(std::move(std::get<0>(args).Get().Getleft()));
    void* arg1_1storage[6];
    decltype(arg1_1)::type arg1_1ret;
    tp.Reset(arg1_1storage, &arg1_1ret, &decltype(arg1_1)::Eval, &arg1_1, &counter0);

    while (counter0.load() < 2) {
        SpinAndWait();
    }

    tp.PopTheBounds();
    Data<Int, Int, Int> data18446744073709551615_0{.fun = &add};
    Data<Int, Int, Int> data18446744073709551615_1{.fun = &add};
    auto arg18446744073709551615_0ret = ((BIntBIntIntBB{&data18446744073709551615_1}).Eval(((BIntBIntIntBB{&data18446744073709551615_0}).Eval(std::move(std::get<0>(args).Get().Getvalue())).Eval()).Eval(std::move(arg1_1ret)).Eval()).Eval()).Eval(std::move(arg1_0ret)).Eval();
	return arg18446744073709551615_0ret.Replicate();
}
}


template <typename a>
Void print (std::tuple<a>& args) {
    std::cout << std::get<0>(args).value << '\n';
    return Void{};
}


int main() {

{
    Data<TreeNode, TreeNode> data18446744073709551615_0{.fun = &ReadTree};
    Data<Int, TreeNode> data18446744073709551615_1{.fun = &TraverseTree};
    Data<Void, Int> data18446744073709551615_2{.fun = &print};
    auto arg18446744073709551615_0ret = (BIntVoidB{&data18446744073709551615_2}).Eval((BTreeNodeIntB{&data18446744073709551615_1}).Eval((BTreeNodeTreeNodeB{&data18446744073709551615_0}).Eval(TreeNode{}).Eval()).Eval()).Eval();
}
}



