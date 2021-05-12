Kernel Simulator
[Version 2.00]
March 5, 2021

#-----------------------------------------------------------------------------
===================
CONTACT INFORMATION
___________________

Contact Name:		Trong Nguyen, 100848232
			Evan Smedley, 100148695
Affiliation: 		Carleton University - Systems and Computer Engineering

#-----------------------------------------------------------------------------
===========
DESCRIPTION
___________

- Kernel Simulator is a program that generates an executable file that
simulates a small OS kernel, which can be used for performance analysis of
various scheduling algorithms such as first-come first-serve, external
priority, and round-robin. The simulation can also perform simulations using a 
first fit memory management algorithm.

- The application is composed of the following two files:
	main.c				(main C programming script)
	main.exe			(executable file generated from main.c)

- The application is supported with various test cases and scenarios for
demonstrating the robustness of the simulator. The test cases and scenarios
are organized into the following folders*:
	Algorithm_Analysis_Cases	
	Test_Cases		
	Memory_Management_Scenarios 	
	Scheduling_Algorithm_Scenarios 
	
	*Each folder contains a copy of the executable and a batch file to make
	running the tests extra easy.

#-----------------------------------------------------------------------------
============
INSTALLATION
____________

This program has only been tested for Windows 10.

-------------------------- Installation Dependencies -------------------------

The application should work with the most recent version of C programming 
language update C17 ISO/IEC 9899:2018. 

No other supplementary libraries and testing file with modular dependencies
are required to run this application.

#-----------------------------------------------------------------------------
=====
USAGE
_____

---------------------------- Compiling the program  --------------------------

An executable file is provided, but should you want to change main.c and
re-compile, any C compiler can be used to generate a new executable. Please note
that the executable has been included 4 times to make running the provided
test cases and scenarios as easy as possible, so you don't have to move it
into the folder with the tests each time. main.exe is included once in each of
the 3 test/scenario folders and once in the main folder.

----------------------------- Running the program  ---------------------------

The program can either be run from command prompt or by writing the commands
in a batch file and running it. 

Many batch files are provided to demonstrate the program. The test cases 
folder contains three test files along with batch files that will each create 
5 output files, one for each of the schedulers and memory management 
partitions. There are also test scenarios that can be run with a batch file
in both the Scheduling_Algorithm_Scenarios and Memory_Management_Scenarios
folders. These scenarios are referenced in the report.

The format for running the program in a batch file or in the command prompt 
is:

main input.txt output.txt 0 0 0

This command calls the main.exe executable with 5 arguments. The first two
arguments are an input file and output file that can be any text file. The 
input file should have the input format shown in the input data section of
this readme.

The last three arguments to the right of the input and output files are:

scheduler_type time_quantum memory_management_mode

Scheduler type is an integer argument that selects between three schedulers: 
(0) for first-come first-serve (FCFS), (1) for external priority, and 
(2) for round-robin (RR). The time quantum argument (second last argument) is 
only used when the round-robin scheduler is selected. The final argument is
also an integer and indicates if a memory partition should be used. (0) for no
memory management, (1) for the first memory partition in assignment 2, (2)
for the second memory partition in assignment 2.

For example, entry for the round-robin scheduler algorithm with 100 ms time 
quantum and no memory management could be the following:

main test.txt output.txt 2 100 0

---------------------------------- Input Data --------------------------------

Any input file (text file) should contain one process per line, with a 
space between each field for example:

PID_1 Arrival_T1 Total_CPU_T1 I/O_Freq_1 I/O_Dur_1 Priority_1 Memory_1

PID_2 Arrival_T2 Total_CPU_T2 I/O_Freq_2 I/O_Dur_2 Priority_2 Memory_2

Note that the input file should not contain any other information such as
header, additional information, etc. All the data in the input file should be 
considered to be integers. Which looks like this in the .txt file:

	1111 05 50 20 10 1 350
	2222 10 50 20 10 2 400

Meaning: Two processes. Named 1111 and 2222. Both require 50 "ticks" to 
execute, and call a trap to request I/O service access every 20 ticks. Each 
I/O lasts 10 ticks. Process 1111 is available at tick 5, process 2222 is 
available at tick 10. We are assuming that one tick represents 1 ms. The second
last column to the right representing the priority level such that the lower 
the number the higher the priority. The last column to the right represents
the values in Mb for memory. The priority column and memory columns are only
required and used during the batch file settings for external priority 
scheduling and memory management respectively.

---------------------------------- Output Data --------------------------------

Similarly to the input file, the output file (output.txt) should contain one 
process per line, with a space between each field, for example:

Time_Of_Transition_1 PID_1 Old_State_1 New_State_1

Time_Of_Transition_2 PID_2 Old_State_2 New_State_2

Note that the Old State and New State should be output to the file as a string 
(Ready, Running, Waiting, etc).

In this version of the simulation, we have also created another field to display
different metrics based on the simulation results such as Throughput, Wait Time, 
Average Turnaround Time, and Average Response Time. These metrics are used to 
compare how the algorithm performs with mostly I/O bound, mostly CPU-bound 
processes, or processes with similar I/O and CPU bursts. 

------------------------------ Scheduling Algorithm ---------------------------

(0) For the First-come first-serve scheduling algorithm, the process that 
arrives first is allocated the CPU first. This is easily managed with a FIFO 
queue. When a process enters the ready queue its PCB is enqueued to the tail of 
the queue. When the CPU is free, it is allocated to the process at the head of 
the ready queue. The running process is then removed from the ready queue.

(1) External priorities is a special case of scheduling where a priority value 
is associated with each process, and the CPU is allocated to the process with 
the highest priority. Equal-priority processes are scheduled in FCFS order. 
In our simulation, lower numbers are higher priority. 

(2) Round-robin scheduling is similar to FCFS scheduling, but preemption is 
added to enable the system to switch between processes. A time quantum is set 
to determine how long a process can have access to the CPU. New processes are 
added to the tail of the ready queue. The CPU is allocated to the first process 
in the ready queue and sets a time to interrupt the process if it is still
using the CPU after an 1 quantum of time.


Scheduling output details executed by the .bat will generate the following 
metric which can be used for scheduling algorithm comparison.

	NUMBER OF PROCESSES >>> 5

	THROUGHPUT >>> 3801.00 ms/process

	AVERAGE TURNAROUND TIME >>> 12002.00 ms/process

	TOTAL WAIT TIME >>> 41010 ms

	AVERAGE WAIT TIME >>> 8202.00 ms/process

	AVERAGE CPU BURST TIME >>> 3800.00 ms/process

------------------------------- Memory Management -----------------------------

The simulation is also equipped to run a simulation for memory allocation. The 
PCB was updated to include information about the memory positions where the 
process will be stored. The assumption in this case is that low memory is not 
taken up be the OS. When a process starts, it will demand the memory needed. 
If it is not available, the process cannot start and must wait until the 
memory becomes available. The available memory partitions can be set in main.c 
where indicated by comments. If the memory partitions are changed, recompile 
main.c and then it will work.

If memory management is used, it will generate the following metrics in the 
output file each time memory is allocated to a process for memory management 
analysis.

	MEMORY HAS BEEN ALLOCATED TO PROCESS 6
	Total used memory: 300 Mb
	Used memory portions: 1
	Free memory portions: 3
	Total amount of free memory: 700 Mb
	Total amount of free usable memory: 500 Mb

#-----------------------------------------------------------------------------
=======
CREDITS
_______

Thanks to the support of TAs and Instructors during the development of this
application.

#-----------------------------------------------------------------------------
=======
LICENSE
_______

[MIT](https://choosealicense.com/licenses/mit/)

Copyright (c) 2021 Trong Nguyen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
