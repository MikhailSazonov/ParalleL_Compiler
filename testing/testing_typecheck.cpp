#include "../typecheck.hpp"

#include "../errors.hpp"

void assert(bool cond) {
    if (!cond) {
        throw "abcde";
    }
}

//int main() {
//    {
//        std::unordered_map<std::string, std::shared_ptr<Type>> types;
//        types["x"] = std::shared_ptr<Type>(new Pod("Int"));
//        assert(*GetTermType(types, VarExpression("x")) == Pod("Int"));
//    }
//    {
//        std::unordered_map<std::string, std::shared_ptr<Type>> types;
//        types["f"] = std::shared_ptr<Type>(new Arrow(new Pod("Int"), new Pod("Bool")));
//        types["x"] = std::shared_ptr<Type>(new Pod("Int"));
//        assert(*GetTermType(types, AppExpression(new VarExpression("f"), new VarExpression("x"))) == Pod("Bool"));
//    }
//    {
//        std::unordered_map<std::string, std::shared_ptr<Type>> types;
//        types["f"] = std::shared_ptr<Type>(new Arrow(new Pod("Bool"), new Pod("Char")));
//        types["g"] = std::shared_ptr<Type>(new Arrow(new Pod("Int"), new Pod("Bool")));
//        types["x"] = std::shared_ptr<Type>(new Pod("Int"));
//        assert(*GetTermType(types, AppExpression(new VarExpression("f"),
//                                                 new AppExpression(new VarExpression("g"), new VarExpression("x")))) == Pod("Char"));
//    }
//    {
//        std::unordered_map<std::string, std::shared_ptr<Type>> types;
//        types["f"] = std::shared_ptr<Type>(new Arrow(new Arrow(new Pod("Int"), new Pod("Char")),
//                                                     new Arrow(new Pod("Char"), new Pod("Bool"))));
//        types["g"] = std::shared_ptr<Type>(new Arrow(new Pod("Int"), new Pod("Char")));
//        types["x"] = std::shared_ptr<Type>(new Pod("Char"));
//        assert(*GetTermType(types, AppExpression(new AppExpression(new VarExpression("f"),
//                                                                   new VarExpression("g")),
//                                                 new VarExpression("x"))) == Pod("Bool"));
//    }
//    {
//        std::unordered_map<std::string, std::shared_ptr<Type>> types;
//        types["f"] = std::shared_ptr<Type>(new Arrow(new Arrow(new Pod("Int"), new Pod("Char")),
//                                                     new Arrow(new Pod("Char"), new Pod("Bool"))));
//        types["g"] = std::shared_ptr<Type>(new Arrow(new Pod("Int"), new Pod("Char")));
//        types["x"] = std::shared_ptr<Type>(new Pod("Char"));
//        assert(*GetTermType(types, AppExpression(new AppExpression(new VarExpression("f"),
//                                                                   new VarExpression("g")),
//                                                 new VarExpression("x"))) == Pod("Bool"));
//    }
//    // faulty
//    {
//        try {
//            std::unordered_map<std::string, std::shared_ptr<Type>> types;
//            types["x"] = std::shared_ptr<Type>(new Pod("Int"));
//            GetTermType(types, VarExpression("y"));
//            throw std::runtime_error{"Test for UndefinedVariableError failed."};
//        } catch (const UndefinedVariableError& err) {
//        }
//    }
//    {
//        try {
//            std::unordered_map<std::string, std::shared_ptr<Type>> types;
//            types["f"] = std::shared_ptr<Type>(new Pod("Bool"));
//            types["x"] = std::shared_ptr<Type>(new Pod("Int"));
//            GetTermType(types, AppExpression(new VarExpression("f"), new VarExpression("x")));
//            throw std::runtime_error{"Test for IsNotFuncError failed."};
//        } catch (const IsNotFuncError& err) {
//        }
//    }
//    {
//        try {
//            std::unordered_map<std::string, std::shared_ptr<Type>> types;
//            types["f"] = std::shared_ptr<Type>(new Arrow(new Pod("Int"), new Pod("Bool")));
//            types["x"] = std::shared_ptr<Type>(new Pod("Bool"));
//            GetTermType(types, AppExpression(new VarExpression("f"), new VarExpression("x")));
//            throw std::runtime_error{"Test for TypeMismatchError failed."};
//        } catch (const TypeMismatchError& err) {
//        }
//    }
//}
