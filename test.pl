Foo : (Int -> Int) -> Int -> Int -> Int
Foo f x y = (1! f x) + (1! f y)

Arc : ((Int -> Int) -> Int -> Int -> Int) -> Void
Arc f = print "Hello world"

main = Arc Foo
