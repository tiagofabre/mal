Simple lisp implementation in C that supports basic arithmetic functions

```
mal-user> (+ 4 1 (* 2 (/ 7 (- 5 500 50))))
5
```

How to build and run
```
cc nob.c -o nob && ./nob && ./build/mal
```