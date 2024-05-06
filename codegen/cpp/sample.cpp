#include <iostream>
#include <tuple>




//
// WhatPrint : String -> String
// WhatPrint x = x
//
// MyPrint : String -> Void
// MyPrint x = print (WhatPrint x)
//
//
//
// Bar : ((Int -> Int) -> (Int -> Int)) -> (Int -> Int) -> (Int -> Int) -> Int -> Int
// Bar f g k x = (f g) (k x)
//
// main = MyPrint "hello world"










































int sum(std::tuple<int, int> args) {
    return std::get<0>(args) + std::get<1>(args);
}


template <typename T, typename U, typename F, typename... Args>
struct Func {
    F* f;
    std::tuple<Args...> args;

    U Eval(T&& arg) {
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


//int main() {
//    Func<int, Func<int, Func<void, int, decltype(sum), int, int>, decltype(sum), int>, decltype(sum)> add{&sum};
//    int ans = add.Eval(5).Eval(10).Eval();
//}



//
// Add : Int -> Int -> Int
// Add x y = sum x y
//