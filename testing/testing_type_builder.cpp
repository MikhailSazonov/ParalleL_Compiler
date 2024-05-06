#include "../builder.hpp"

void assert(Type& type1, Type& type2) {
    if (type1 != type2) {
        throw 5;
    }
}

//int main() {
//    // testing
//    {
//        std::string a = "Int";
//        auto type = BuildType(a.data());
//        auto type2 = Pod("Int");
//        assert(*type, type2);
//    }
//    {
//        std::string a = "Int -> Int";
//        auto type = BuildType(a.data());
//        auto type2 = Arrow(new Pod("Int"), new Pod("Int"));
//        assert(*type, type2);
//    }
//    {
//        std::string a = "(Int -> Int) -> Int";
//        auto type = BuildType(a.data());
//        auto type2 = Arrow(new Arrow(new Pod("Int"), new Pod("Int")), new Pod("Int"));
//        assert(*type, type2);
//    }
//    {
//        std::string a = "Int -> Int -> Int";
//        auto type = BuildType(a.data());
//        auto type2 = Arrow(new Pod("Int"), new Arrow(new Pod("Int"), new Pod("Int")));
//        assert(*type, type2);
//    }
//    {
//        std::string a = "Int -> (Int -> Int)";
//        auto type = BuildType(a.data());
//        auto type2 = Arrow(new Pod("Int"), new Arrow(new Pod("Int"), new Pod("Int")));
//        assert(*type, type2);
//    }
//    {
//        std::string a = "Int -> ((Int -> Int) -> Int)";
//        auto type = BuildType(a.data());
//        auto type2 = Arrow(new Pod("Int"), new Arrow(new Arrow(new Pod("Int"), new Pod("Int")), new Pod("Int")));
//        assert(*type, type2);
//    }
//    {
//        std::string a = "Int -> (Int -> (Int -> Int))";
//        auto type = BuildType(a.data());
//        auto type2 = Arrow(new Pod("Int"), new Arrow(new Pod("Int"), new Arrow(new Pod("Int"), new Pod("Int"))));
//        assert(*type, type2);
//    }
//    {
//        std::string a = "(Int -> Int) -> (Int -> Int)";
//        auto type = BuildType(a.data());
//        auto type2 = Arrow(new Arrow(new Pod("Int"), new Pod("Int")), new Arrow(new Pod("Int"), new Pod("Int")));
//        assert(*type, type2);
//    }
//    {
//        std::string a = "(Int)";
//        auto type = BuildType(a.data());
//        auto type2 = Pod("Int");
//        assert(*type, type2);
//    }
//    {
//        std::string a = "(Int -> Int)";
//        auto type = BuildType(a.data());
//        auto type2 = Arrow(new Pod("Int"), new Pod("Int"));
//        assert(*type, type2);
//    }
//    {
//        std::string a = "(Int -> ((Int)))";
//        auto type = BuildType(a.data());
//        auto type2 = Arrow(new Pod("Int"), new Pod("Int"));
//        assert(*type, type2);
//    }
//}
