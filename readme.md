# COMP304 HShell

## Table of Contents
- [Introduction](#introduction)
- [Part 1](#part-1)
- [Part 2](#part-2)
- [Part 3](#part-3)
- [Part 4](#part-4)
- [Usage](#usage)


## Introduction
In this project, I implemented the HShell, which there was a template and I implemented some functionalities.

## Part 1
In order to run the project, first you have to compile the shell, by typing 
```bash
make
```
and then you can run the shell by using the command
```bash
./mishell
```
## Part 2
In order to see the pipe feature, you dont have to do anything more, you can use the shell as it is.
To see it you can just use a command such as 
```bash
ls | grep src
```

## Part 3
In order to see the autocomplete feature, you dont have to do anything more, you can use the shell as it is.
For using the hdiff and askzip, you have compile them by using these commands
```bash
make
gcc hdiff.c -o hdiff
gcc askzip.c -o askzip
```
After that, you can just run them by typing the commands name

## Part 4
In order to use the kernel module, first you have to change the directory to module by
```bash
cd module
```
and then compile and load the module by
```bash 
    make
    sudo insmod psvis_kernel_module.ko
    gcc psvis.c -o psvis
```

After these commands, you can run the psvis command and you will see the process_tree.dot and process_tree.png

