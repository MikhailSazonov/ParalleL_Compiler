#include <memory>
#include <tuple>
#include <iostream>
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



typedef SimpleContainer<Unit> Void;
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

typedef Func<Void, Func<void, Int>> BVoidIntB;

typedef Func<Int, Func<TreeNode, Func<void, TreeNode>>> BIntBTreeNodeTreeNodeBB;

typedef Func<Int, Func<Int, Func<void, Bool>>> BIntBIntBoolBB;

typedef Func<TreeNode, Func<void, Bool>> BTreeNodeBoolB;

typedef Func<TreeNode, Func<void, Int>> BTreeNodeIntB;

typedef Func<Bool, Func<Bool, Func<void, Bool>>> BBoolBBoolBoolBB;

typedef Func<Int, Func<Int, Func<void, Int>>> BIntBIntIntBB;

typedef Func<Int, Func<void, Void>> BIntVoidB;

TreeNode ReadTree (std::tuple<TreeNode>& args);

TreeNode MakeTree (std::tuple<Int, TreeNode>& args);

TreeNode Append (std::tuple<Int, TreeNode>& args);

Int TraverseTree (std::tuple<TreeNode>& args);

Int readInt (std::tuple<Void>& args) {
    Int i;
    std::cin >> i.value;
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
    Data<Int, Void> data18446744073709551615_0{.fun = &readInt};
    Data<TreeNode, Int, TreeNode> data18446744073709551615_1{.fun = &MakeTree};
        auto copy0 = std::move(std::get<0>(args));
    auto arg18446744073709551615_0ret = ((BIntBTreeNodeTreeNodeBB{&data18446744073709551615_1}).Eval((BVoidIntB{&data18446744073709551615_0}).Eval(Void{}).Eval()).Eval()).Eval(std::move(copy0)).Eval();
return arg18446744073709551615_0ret.Replicate();
}
}


Bool Xor (std::tuple<Bool, Bool>& args) {
    return {std::get<0>(args).value && !std::get<1>(args).value ||
        !std::get<0>(args).value && std::get<1>(args).value};
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
    Data<Bool, TreeNode> data18446744073709551615_0{.fun = &isNull};
    Data<Bool, TreeNode> data18446744073709551615_1{.fun = &isNull};
    Data<Bool, Bool, Bool> data18446744073709551615_2{.fun = &Xor};
    auto arg18446744073709551615_0ret = ((BBoolBBoolBoolBB{&data18446744073709551615_2}).Eval((BTreeNodeBoolB{&data18446744073709551615_1}).Eval(std::move(std::get<0>(args).Get().Getleft())).Eval()).Eval()).Eval((BTreeNodeBoolB{&data18446744073709551615_0}).Eval(std::move(std::get<0>(args).Get().Getright())).Eval()).Eval();
if (arg18446744073709551615_0ret.value) {
    Data<Int, TreeNode> data18446744073709551615_0{.fun = &TraverseTree};
    Data<Int, TreeNode> data18446744073709551615_1{.fun = &TraverseTree};
    Data<Int, Int, Int> data18446744073709551615_2{.fun = &add};
    Data<Int, Int, Int> data18446744073709551615_3{.fun = &add};
    auto arg18446744073709551615_0ret = ((BIntBIntIntBB{&data18446744073709551615_3}).Eval(((BIntBIntIntBB{&data18446744073709551615_2}).Eval(Int{1}).Eval()).Eval((BTreeNodeIntB{&data18446744073709551615_1}).Eval(std::move(std::get<0>(args).Get().Getleft())).Eval()).Eval()).Eval()).Eval((BTreeNodeIntB{&data18446744073709551615_0}).Eval(std::move(std::get<0>(args).Get().Getright())).Eval()).Eval();
return arg18446744073709551615_0ret.Replicate();
}
}

{
    Data<Int, TreeNode> data18446744073709551615_0{.fun = &TraverseTree};
    Data<Int, TreeNode> data18446744073709551615_1{.fun = &TraverseTree};
    Data<Int, Int, Int> data18446744073709551615_2{.fun = &add};
    Data<Int, Int, Int> data18446744073709551615_3{.fun = &add};
    auto arg18446744073709551615_0ret = ((BIntBIntIntBB{&data18446744073709551615_3}).Eval(((BIntBIntIntBB{&data18446744073709551615_2}).Eval(Int{0}).Eval()).Eval((BTreeNodeIntB{&data18446744073709551615_1}).Eval(std::move(std::get<0>(args).Get().Getleft())).Eval()).Eval()).Eval()).Eval((BTreeNodeIntB{&data18446744073709551615_0}).Eval(std::move(std::get<0>(args).Get().Getright())).Eval()).Eval();
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



