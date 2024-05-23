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



class TreeNode {
    Int value
    TreeNode left
    TreeNode right
}

GetValue : TreeNode -> Int
GetValue {} = 0
GetValue {x l r} = x

DoubleGetValue : TreeNode -> Int
DoubleGetValue x = (GetValue x) + (GetValue x)

Arc : (TreeNode -> Int) -> Void
Arc f = print "hello world"

main = Arc DoubleGetValue




MyAdd : Int -> Int -> Int
MyAdd x y = x + y


Id : a -> a
Id x = x


class IntPair {
    Int left
    Int right
}


class ListNode {
    Int value
    ListNode nextNode
}


main = print "Hello world!"


GetMin : (Int -> Int) -> (Int -> Int) -> Int -> Int
GetMin f g x = min (1! f x) (1! g x)


IsPositive : Int -> Bool
IsPositive x = x > 0

IsPositive False



class TreeNode {
    Int value
    TreeNode left
    TreeNode right
}

class IntPair {
    Int left
    Int right
}

Bar : Int -> IntPair
Bar x = { x ; x }

Foo : (Int -> Int) -> Int -> Int -> Int
Foo f x y = (1! f x) + (1! f y)

Arc : ((Int -> Int) -> Int -> Int -> Int) -> (Int -> IntPair) -> Void
Arc f g = print "Hello world"

main = Arc Foo Bar


