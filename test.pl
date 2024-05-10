Id : Int -> Int
Id x = x

Hui : Int -> (Int -> Int)
Hui x = Id

Arc : (Int -> (Int -> Int)) -> Void
Arc f = print "Hello world"

main = Arc Hui
