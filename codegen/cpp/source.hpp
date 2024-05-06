#include <tuple>

template <typename T, typename U, typename F, typename... Args>
struct Func {
    F* f;
    std::tuple<Args...> args;

    U Eval(T arg) {
        return U{f, std::tuple_cat(args, std::make_tuple(arg))};
    }
};


template <typename U, typename F, typename... Args>
struct Func<void, U, F, Args...> {
    F* f;
    std::tuple<Args...> args;

    U Eval() {
        return f(args);
    }
};


template <typename F, typename... Args>
struct Func<void, void, F, Args...> {
    F* f;
    std::tuple<Args...> args;

    void Eval() {
        f(args);
    }
};
