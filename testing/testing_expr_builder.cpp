#include "../builder.hpp"

bool CompareExpr(Expression& expr1, Expression& expr2) {
    if (expr1.type != expr2.type) {
        return false;
    }
    if (expr1.type == ExpressionType::APP) {
        auto& app1 = (AppExpression&)expr1;
        auto& app2 = (AppExpression&)expr2;
        return CompareExpr(*app1.arg, *app2.arg) && CompareExpr(*app1.fun, *app2.fun);
    }
    // TODO: proper value
    auto& var1 = (VarExpression&)expr1;
    auto& var2 = (VarExpression&)expr2;
    return var1.name == var2.name;
}

void assert2(Expression& expr1, Expression& expr2) {
    if (!CompareExpr(expr1, expr2)) {
        throw 5;
    }
}

//int main() {
//    // testing
//    {
//        std::string a = "f";
//        auto type = BuildExpression(a.data(), {});
//        auto type2 = VarExpression("f");
//        assert2(*type, type2);
//    }
//    {
//        std::string a = "f g";
//        auto type = BuildExpression(a.data(), {});
//        auto type2 = AppExpression(new VarExpression("f"), new VarExpression("g"));
//        assert2(*type, type2);
//    }
//    {
//        std::string a = "f g h";
//        auto type = BuildExpression(a.data(), {});
//        auto type2 = AppExpression(new AppExpression(new VarExpression("f"), new VarExpression("g")),
//                                   new VarExpression("h"));
//        assert2(*type, type2);
//    }
//    {
//        std::string a = "f (g h)";
//        auto type = BuildExpression(a.data(), {});
//        auto type2 = AppExpression(new VarExpression("f"),
//                                   new AppExpression(new VarExpression("g"), new VarExpression("h")));
//        assert2(*type, type2);
//    }
//    {
//        std::string a = "(f g) h";
//        auto type = BuildExpression(a.data(), {});
//        auto type2 = AppExpression(new AppExpression(new VarExpression("f"), new VarExpression("g")),
//                                   new VarExpression("h"));
//        assert2(*type, type2);
//    }
//    {
//        std::string a = "(f g) (h k)";
//        auto type = BuildExpression(a.data(), {});
//        auto type2 = AppExpression(new AppExpression(new VarExpression("f"), new VarExpression("g")),
//                                   new AppExpression(new VarExpression("h"), new VarExpression("k")));
//        assert2(*type, type2);
//    }
//    {
//        std::string a = "(f (g h)) (f g)";
//        auto type = BuildExpression(a.data(), {});
//        auto type2 = AppExpression(new AppExpression(new VarExpression("f"),
//                                                     new AppExpression(new VarExpression("g"), new VarExpression("h"))),
//                                   new AppExpression(new VarExpression("f"), new VarExpression("g")));
//        assert2(*type, type2);
//    }
//    {
//        std::string a = "((f g) h) (f g)";
//        auto type = BuildExpression(a.data(), {});
//        auto type2 = AppExpression(new AppExpression(new AppExpression(new VarExpression("f"), new VarExpression("g")),
//                                                     new VarExpression("h")),
//                                   new AppExpression(new VarExpression("f"), new VarExpression("g")));
//        assert2(*type, type2);
//    }
//    {
//        std::string a = "(f)";
//        auto type = BuildExpression(a.data(), {});
//        auto type2 = VarExpression("f");
//        assert2(*type, type2);
//    }
//    {
//        std::string a = "(f g)";
//        auto type = BuildExpression(a.data(), {});
//        auto type2 = AppExpression(new VarExpression("f"), new VarExpression("g"));
//        assert2(*type, type2);
//    }
//    {
//        std::string a = "(f ((g)))";
//        auto type = BuildExpression(a.data(), {});
//        auto type2 = AppExpression(new VarExpression("f"), new VarExpression("g"));
//        assert2(*type, type2);
//    }
//    {
//        std::string a = "x + y";
//        auto type = BuildExpression(a.data(), {});
//        auto type2 = AppExpression(new AppExpression(new VarExpression("add"), new VarExpression("x")),
//                                   new VarExpression("y"));
//        assert2(*type, type2);
//    }
//    {
//        std::string a = "x / y";
//        auto type = BuildExpression(a.data(), {});
//        auto type2 = AppExpression(new AppExpression(new VarExpression("div"), new VarExpression("x")),
//                                   new VarExpression("y"));
//        assert2(*type, type2);
//    }
//    {
//        std::string a = "x + y - z";
//        auto type = BuildExpression(a.data(), {});
//        auto type2 = AppExpression(new AppExpression(new VarExpression("sub"),
//                                                     new AppExpression(new AppExpression(new VarExpression("add"),
//                                                                                         new VarExpression("x")),
//                                                                       new VarExpression("y"))),
//                                   new VarExpression("z"));
//        assert2(*type, type2);
//    }
//}
