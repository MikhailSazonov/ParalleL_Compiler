#include <iostream>
#include <tuple>


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

    auto& Eval() {
        return *this;
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


typedef void Void;
typedef int Int;
typedef std::string String;
typedef Func<Int, Func<Void, Int>> BIntIntB;

typedef Func<Int, Func<Int, Func<Void, Int>>> BIntBIntIntBB;

typedef Func<BIntBIntIntBB, Func<Void, Void>> BBIntBIntIntBBVoidB;

typedef Func<String, Func<Void, Void>> BStringVoidB;

Int Id (std::tuple<Int>& args) {
	return std::move(std::get<0>(args));
}


BIntIntB Hui (std::tuple<Int>& args) {
	Data<Int, Int> data0{.fun = &Id};
	return BIntIntB{&data0};
}


void print (std::tuple<String>& args) {
    std::cout << std::get<0>(args) << '\n';
}


Void Arc (std::tuple<BIntBIntIntBB>& args) {
	Data<Void, String> data0{.fun = &print};
	(BStringVoidB{&data0}).Eval("Hello world").Eval();
}


int main() {
	Data<BIntIntB, Int> data0{.fun = &Hui};
	Data<Void, BIntBIntIntBB> data1{.fun = &Arc};
	(BBIntBIntIntBBVoidB{&data1}).Eval(BIntBIntIntBB{&data0}).Eval();
}



