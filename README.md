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
# Syntax
## To announce a variable:
```c
variablename = value //you can only announce a variable outside main
```
## To run any bash command:
```c
main{ //this is important. any <sh: > you announce outside of main will not do anything
    sh: <your command here>
}
```
## To reference a variable
```c
main{
    sh: echo (variablename) //encase your variable name in parenthesis
}
```