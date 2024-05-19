class TreeNode {
    Int value
    TreeNode left
    TreeNode right
}

Foo : (Int -> Int) -> Int -> Int -> Int
Foo f x y = (1! f x) + (1! f y)

Bar : TreeNode -> TreeNode
Bar x = x

Arc : ((Int -> Int) -> Int -> Int -> Int) -> (TreeNode -> TreeNode) -> Void
Arc f g = print "Hello world"

main = Arc Foo Bar






Fact : Int -> Int
Fact x = 3! x * (2! Fact (1! x - 1)) where x > 0
Fact x = 1

Arc : (Int -> Int) -> Void
Arc f = print "Hello world"

main = Arc Fact
