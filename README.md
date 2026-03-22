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
    //if your variable does not exist, the interpreter will fill it with NULL and bash will complain about lack of operrand
}
```
## To call a function
As of right now, you can't call a function from main. I will change that soon.
```c
/functioname{
    sh: echo printed from function! 
}
```
And to actually call it, you do:
```bash
nub functioname
output: printed from function!
```



Find more in file **build.nub**