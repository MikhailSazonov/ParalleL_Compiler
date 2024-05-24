class TreeNode {
    Int value
    TreeNode left
    TreeNode right
}

TraverseTree : TreeNode -> Int
TraverseTree {} = 0
TraverseTree {x l r} = 1 + (TraverseTree l) + (TraverseTree r) where (isNull l) ^ (isNull r)
TraverseTree {x l r} = 0 + (TraverseTree l) + (TraverseTree r)

Append : Int -> TreeNode -> TreeNode
Append x {} = {x ; {} ; {}}
Append x {y l r} = {y ; Append x l ; r} where x < y
Append x {y l r} = {y ; l ; Append x r}

ReadTree : TreeNode -> TreeNode

MakeTree : Int -> TreeNode -> TreeNode
MakeTree 0 t = t
MakeTree x t = ReadTree (Append x t)

ReadTree t = MakeTree (readInt ()) t

main = print (TraverseTree (ReadTree {}))
