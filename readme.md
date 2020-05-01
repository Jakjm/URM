This is a project that aims to allow students to run URM programs using an emulator written in C++.
Unbounded Register Machines (URMs) are a creation of Shepherdson and Sturgis.
URMs are not very practical - something as simple as variable assignment takes multiple loops, as demonstrated in the program swap.uxe that I wrote to test the URM.
However, it is enjoyable to write programs that overcome the challenges posed by the language - which makes this quite valuable. 

How to write URM programs:
There are 5 different kinds of URM instructions. 

L: Xi <- a				//Assignment Instruction
L: Xi <- Xi + 1				//Increment Instruction 
L: Xi <- Xi - 1				//Decrement Instruction 
L: if Xi = 0 goto X else goto Y 	//Conditional Goto Instruction
L: stop					//Stop Instruction 

The instructions of a URM program are ordered using a label (denoted L above).
The first label of a URM program is 1. 
Each additional instruction has a label that is one greater than the previous label. 

The stop instruction occurs exactly ONCE in a URM program, and must be the last instruction in the program.

The variables of a URM program are of the following form: X1, X11, X111, etc.

An example URM program is as follows:
1: X1 <- 5 
2: X1 <- X1 + 1
3: if X1 = 0 goto 4 else goto 1
4: stop

As you can see, there is no requirement that a URM program comes to a halt - the above program is partial computable, but it is not in the set of recursive functions (asit does not halt).
 

Todo list: 
-Variables currently have a maximum value of (2^31) -1
-Metavariables are not currently supported. 
