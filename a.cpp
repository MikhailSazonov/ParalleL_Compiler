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

template <typename T>
struct SimpleContainer {
    T value;

    auto& Eval() {
        return *this;
    }
};



typedef SimpleContainer<Unit> Void;
typedef SimpleContainer<std::string> String;
typedef Func<String, Func<void, Void>> BStringVoidB;

typedef Func<Void, Func<Void, Func<void, Void>>> BVoidBVoidVoidBB;

Void print (std::tuple<String>& args) {
    std::cout << std::get<0>(args).value << '\n';
    return Void{};
}


Void seq (std::tuple<Void, Void>& args) {
    return Void{};
}


int main() {

{
    Data<Void, String> data18446744073709551615_0{.fun = &print};
    Data<Void, String> data18446744073709551615_1{.fun = &print};
    Data<Void, Void, Void> data18446744073709551615_2{.fun = &seq};
    auto arg18446744073709551615_0ret = ((BVoidBVoidVoidBB{&data18446744073709551615_2}).Eval((BStringVoidB{&data18446744073709551615_1}).Eval(String{"hello "}).Eval()).Eval()).Eval((BStringVoidB{&data18446744073709551615_0}).Eval(String{"world"}).Eval()).Eval();
}
}



