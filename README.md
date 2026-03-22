# NUB
A replacement for Makefile cause i personally hate it.
## Snippet code
```c
text = hello
text2 =  world!
main{
    sh: echo (text)(text2)
}
```
Then run **nub** in the directory of your **.nub** file.
```c
//command:
joe@pc: ~/path/to/nubfile$ nub
//output:
hello world!
```
Find more examples in file **build.nub**