%%Id : a -> a
%%Id x = x

%%AppId : Int -> Int
%%AppId x = Id x

class TreeNode {
    Int value
    TreeNode left
    TreeNode right
}

GetVal : TreeNode -> Int
GetVal {} = 0
GetVal {x l r} = x

Fact : Int -> Int
Fact 0 = 1
Fact x = x * (Fact (x - 1))

Arc : (TreeNode -> Int) -> Void
Arc f = print "Hello world"

Fib : Int -> Int
Fib x = 1 where x <= 1
Fib x = (1! Fib (x - 1)) + (1! Fib (x - 2))

main = Arc GetVal
