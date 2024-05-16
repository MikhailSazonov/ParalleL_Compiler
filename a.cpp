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
#include <atomic>
#include <tuple>
#include <iostream>
template <typename R, typename... Args>
struct Data {
    void* ptr;
    R(*fun)(std::tuple<Args...>&);
    std::tuple<Args...> args;
};


template <typename T, typename U>
struct Func {
    struct ControlBlock {
        virtual U Call(T&&) = 0;
    };


using type = U;

    template <typename R, size_t idx, typename... Args>
    struct FunctionBlock : public ControlBlock {
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


typedef Unit Void;
typedef int Int;
typedef std::string String;
typedef Func<Int, Func<void, Int>> BIntIntB;

typedef Func<BIntIntB, Func<Int, Func<Int, Func<void, Int>>>> BBIntIntBBIntBIntIntBBB;

typedef Func<Int, Func<Int, Func<void, Int>>> BIntBIntIntBB;

typedef Func<BBIntIntBBIntBIntIntBBB, Func<void, Void>> BBBIntIntBBIntBIntIntBBBVoidB;

typedef Func<String, Func<void, Void>> BStringVoidB;

int add (std::tuple<Int, Int>& args) {
    return std::get<0>(args) + std::get<1>(args);
}


Int Foo (std::tuple<BIntIntB, Int, Int>& args) {
    std::atomic<uint64_t> counter0{0};

    tp.CutTheBounds(2);
    auto arg1_0 = (std::move(std::get<0>(args))).Eval(std::move(std::get<2>(args)));
    void* arg1_0storage[6];
    decltype(arg1_0)::type arg1_0ret;
    tp.Reset(arg1_0storage, &arg1_0ret, &decltype(arg1_0)::Eval, &arg1_0, &counter0);

    auto arg1_1 = (std::move(std::get<0>(args))).Eval(std::move(std::get<1>(args)));
    void* arg1_1storage[6];
    decltype(arg1_1)::type arg1_1ret;
    tp.Reset(arg1_1storage, &arg1_1ret, &decltype(arg1_1)::Eval, &arg1_1, &counter0);

    while (counter0.load() < 2) {
        SpinAndWait();
    }

    tp.PopTheBounds();
    Data<Int, Int, Int> data18446744073709551615_2{.fun = &add};
    auto arg18446744073709551615_0ret = ((BIntBIntIntBB{&data18446744073709551615_2}).Eval(std::move(arg1_1ret))).Eval(std::move(arg1_0ret)).Eval();
	return arg18446744073709551615_0ret;
}


Void print (std::tuple<String>& args) {
    std::cout << std::get<0>(args) << '\n';
    return Void{};
}


Void Arc (std::tuple<BBIntIntBBIntBIntIntBBB>& args) {
    Data<Void, String> data18446744073709551615_0{.fun = &print};
    auto arg18446744073709551615_0ret = (BStringVoidB{&data18446744073709551615_0}).Eval("Hello world").Eval();
	return arg18446744073709551615_0ret;
}


int main() {
    Data<Int, BIntIntB, Int, Int> data18446744073709551615_0{.fun = &Foo};
    Data<Void, BBIntIntBBIntBIntIntBBB> data18446744073709551615_1{.fun = &Arc};
    auto arg18446744073709551615_0ret = (BBBIntIntBBIntBIntIntBBBVoidB{&data18446744073709551615_1}).Eval(BBIntIntBBIntBIntIntBBB{&data18446744073709551615_0}).Eval();
}



