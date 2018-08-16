user name: rachelive
name: racheli   Id:305710071

***********what this program do************

In this exercise we will simulate a processor's approach to memory. 
  we will use the paging mechanism, which allows you to run a program by parts
 Program memory is divided into pages that are brought to the main memory

**********Description of the function*******************

to reach the main memory we work with 2 function: store and load.
1)store - write to main memory
2)load - read from main memory

********** How to Compile this program*************

write : "make"

***********How to run this program***********

write: ./ex5

**********Description of program files*************

#makefile 
#README 
#mem_sim.h 
#mem_sim.c 
#main.c

**********Description What input we needed*************
there are no input from the user.
	the main.c call to the function: init_system, clear_system, store and load 
	the input to this functions:
		init_system - name of the swap file, name of executable file, the text size and the bass_heap_stack_size
		clear_system -sim_database
		store -  sim_database, address and char
		load -  sim_database and address

***********An explanation of the output we get************

output:	prints page table, swap, main memory according to the main 
