annotate Pure3! 1! 1! 1!

Foo : (Int -> Int) -> Int -> Int -> Int
Foo f x y = (f x) + (f y)

Arc : ((Int -> Int) -> Int -> Int -> Int) -> Void
Arc f = print "Hello world"

main = Arc Foo
