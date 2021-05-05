/** Kernel Simulator - Assignment 2, Part II
 *
 * Carleton University
 * Department of System and Computer Engineering
 * SYSC 4001 - Operating Systems
 * 
 * @authors			Trong Nguyen, 100848232
 * 				Evan Smedley, 101148695
 * @version			1.0
 * @release			March 5, 2021
 *
 * This program generates an executable file that simulates a small
 * OS kernel, which can be used for performance analysis of three
 * scheduling algorithms.
 */



#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>



// Global Time Variable
int time_elapsed = 0;



// Structure implementation
typedef struct pcb {		// Defines the process control block struct
    int pid;
    int arrival_time;
    int remaining_cpu_time;
    int cpu_arrival_time;
    int io_frequency;
    int io_duration;
    int time_until_io;
    int priority;
    int size_of_process;
    int base_memory_location;
    struct pcb *next;
} pcb_t;

typedef struct {      		// Defines the queue struct that will contain pcbs
    pcb_t *front;
    pcb_t *rear;
    int size;
} queue_t;



// Allocating and initializing functions for structs

queue_t *new_queue(void) {
    /*
     * Allocates a queue struct on the heap and returns a pointer
     * to it. Initializes the front and rear pointers to null and the
     * queue size to 0.
     */

    queue_t *queue = malloc(sizeof(queue_t));     // Allocate new queue_t struct on the heap
    assert(queue != NULL);

    queue->front = NULL;				// Initialize elements of queue (the queue_t struct)
    queue->rear = NULL; 
    queue->size = 0;

    return queue;
}

pcb_t *new_pcb(int pid, int arrival_time, int total_cpu_time, int io_freq, int io_dur, int priority, int process_size) {
    /*
     * Takes all of the inputted information about a process and stores it in a pcb_t struct allocated
     * on the heap. Returns a pointer to the pcb struct.
     */

    pcb_t *pcb = malloc(sizeof(pcb_t));
    assert(pcb != NULL);

    pcb->pid = pid;
    pcb->arrival_time = arrival_time;
    pcb->remaining_cpu_time = total_cpu_time;
    pcb->cpu_arrival_time = 0;
    pcb->io_frequency = io_freq;
    pcb->io_duration = io_dur;
    pcb->next = NULL;
    pcb->priority = priority;
    pcb->time_until_io = io_freq;
    pcb->base_memory_location = -1;          // Measured in Mb
    pcb->size_of_process = process_size;     // Measured in Mb

    return pcb;
}



// Queue functions

void enqueue(queue_t *queue, pcb_t *pcb) {
    /* 
     * Adds a previously allocated node to the end of the queue pointed to by queue.
     */

    if (queue->front == NULL) {		// If the queue is empty, add node at the front
        queue->front = pcb;
    } else {
        queue->rear->next = pcb;	//If the queue has other elements, add node at the end
    }

    queue->rear = pcb;     			// Set the rear pointer to point to the new end of the queue
    queue->size += 1;				// Increase queue size
}

void dequeue(queue_t *queue, _Bool deallocate) {
    /*
     * If the queue pointed to by queue is empty, returns false.
     * If the queue has one or more nodes, removes the front node
     * and returns true. If the queue ends up with only one node, 
     * makes sure that the front and rear pointers in the queue 
     * struct both point to the one node in the list.
     */

    if (queue->size == 0) {    					// If the queue is empty, cannot dequeue
        printf("Queue is empty, cannot dequeue.");
        assert(false);
    }
     pcb_t *pcb_to_delete = queue->front;       // Creates a temporary pointer to the front node

    if (deallocate) { 							// Determines whether node is deallocated or removed from list
        queue->front = queue->front->next;		// Remove the node from the queue, set new front
        free(pcb_to_delete);          			// Free the front node
        pcb_to_delete = NULL;
    } else {
        queue->front = queue->front->next;
        pcb_to_delete->next = NULL;
        
    }

    if (queue->front == NULL){					// If list is now empty, set front and rear pointers
        queue->rear = NULL;
    }

    queue->size -= 1;     						// Decrease queue size
}



// Functions for writing to an output file

void erase_write_file(char writeFile[]) {
    /*
     * This function erases the contents of the output file, if it exists.
     */

    FILE *f1 = fopen(writeFile, "w");       // Open the file in write mode, deleting any text that was in the file
    if (f1 == NULL) {
        printf("File was not opened");
        assert(false);
    }
    fclose(f1);
    f1 = NULL;
}

void write_transition(int transition_identifier, int pid, char write_file[]) {
    /**
     * Write details about a transition between states to a file. Transition type is determined by 
     * the int transition_type.
     * - 0 is new to ready (admitted)
     * - 1 is ready to running (scheduler dispatch)
     * - 2 is running to waiting (I/O or event wait)
     * - 3 is waiting to ready (I/O or event completion)
     * - 4 is running to ready (interrupt)
     * - 5 is running to terminated (exit)
     */
    
    // Open file specified by writeFile. Print out some transition information.
    FILE *f2 = fopen(write_file, "a");
    if (f2 == NULL) {
        printf("File was not opened");
        assert(false);
    }
    fprintf(f2, "%-14d %-14d ", time_elapsed, pid);
    //fprintf(f2, "%d %d ", time, old_state->front->pid);

    // These if and else statements determine which states are being transitioned between and write
    // the corresponding states to the file pointer to by f. The commented out fprintf statements
    // can be used instead if you want the output file to be less formatted.
    if ((transition_identifier > 5) || (transition_identifier < 0)) {
        printf("Incorrect transition identifier");
        assert(false);
    } else if (transition_identifier == 0) {
        fprintf(f2, "NEW            READY\n\n");  
        //fprintf(f2, "NEW READY\n");
    } else if (transition_identifier == 1) {
        fprintf(f2, "READY          RUNNING\n\n");
        //fprintf(f2, "READY RUNNING\n");
    } else if (transition_identifier == 2) {
        fprintf(f2, "RUNNING        WAITING\n\n");
        //fprintf(f2, "RUNNING WAITING\n");
    } else if (transition_identifier == 3) {
        fprintf(f2, "WAITING        READY\n\n");
        //fprintf(f2, "WAITING READY\n");
    } else if (transition_identifier == 4) {
        fprintf(f2, "RUNNING        READY\n\n");
        //fprintf(f2, "RUNNING READY\n");
    } else {
        fprintf(f2, "RUNNING        TERMINATED\n\n");
        //fprintf(f2, "RUNNING TERMINATED \n");
    }
    fclose(f2);    					// Close the file pointed to by f2.
    f2 = NULL;

    // Display transition details in the terminal (for debugging)
    //printf("Transition Time: %d, ", (time));
    //printf("Pid: %d\n\n", (old_state->front->pid));
}

void transition(int transition_identifier, queue_t *old_state, queue_t *new_state, char write_file[]) {
    /**
     * Perform a transition between the states provided in old_state and 
     * new_state. The transition type is determined by the int transition_type:
     * - 0 is new to ready (admitted)
     * - 1 is ready to running (scheduler dispatch)
     * - 2 is running to waiting (I/O or event wait)
     * - 3 is waiting to ready (I/O or event completion)
     * - 4 is running to ready (interrupt)
     * - 5 is running to terminated (exit)
     * The process is moved from one queue to the next.
     */
    
    write_transition(transition_identifier, old_state->front->pid, write_file);

    // Move the process from the old state to the new state
    enqueue(new_state, old_state->front);
    dequeue(old_state, false);
}



// Schedulers

void fcfs_scheduler(int transition_type, queue_t *old_state, queue_t *new_state, char write_file[]) {
    /**
     * This First-come first-Serve (FCFS) scheduler processes queued processes in order of their arrival.
     */

    // Move the process at the front of the new queue into the ready queue
    transition(transition_type, old_state, new_state, write_file);
}

void external_priorities_scheduler(int transition_type, queue_t *old_state, queue_t *new_state, char write_file[]) {
    /**
     * This priority-based scheduler sorts the processes in the ready queue based on their priority.
     * This makes it so that the CPU is allocated to processes with the highest priority first.
     */

    // If the ready queue is empty, the process being scheduled can just em enqueued to the ready queue
    if (new_state->size == 0) {
        transition(transition_type, old_state, new_state, write_file);

    // If the ready queue contains other processes, the process being scheduled must be inserted into the queue
    // according to its priority.
    } else {
        pcb_t *prev = NULL;
        pcb_t *curr = new_state->front;
        pcb_t *process_to_schedule = old_state->front;
        _Bool scheduled = false;
        for (int i = 0; i < new_state->size; i++) {                 // Iterate through the processes in the ready queue
            if (curr->priority > process_to_schedule->priority) {   // Insert the process at the correct location in the queue
                dequeue(old_state, false);
                process_to_schedule->next = curr;
                scheduled = true;
                new_state->size++;
                write_transition(transition_type, process_to_schedule->pid, write_file);
                if (i == 0) {
                    new_state->front = process_to_schedule;
                } else {
                    prev->next = process_to_schedule;
                }
                break;
            }
            prev = curr;
            curr = curr->next;
        }

        // If the process was not scheduled by this point, scheduled is false.
        // This means that the process is the lowest priority in the queue so it must be enqueued to the ready queue
        if (!scheduled) {
            transition(transition_type, old_state, new_state, write_file);
        }
    }
}

void round_robin_scheduler(int transition_type, queue_t *old_state, queue_t *new_state, char write_file[]) {
    /**
     * Round-robin (RR) scheduling algorithm is similar to FCFS scheduling in that it allocates the CPU to processes
     * in the order that they arrive. The difference is that it interrupts the CPU after a process has been using the
     * CPU for a certain amount of time and moves the process back to the end of the ready queue. The amount of time
     * that each process can have the CPU at one time is specified by the quantum, the quantum is implemented in main.
     */

    transition(transition_type, old_state, new_state, write_file);
}

void scheduler(int transition_type, int scheduler_type, queue_t *old_state, queue_t *new_state, char write_file[]) {
    /**
     * This function determines which scheduler should be used based on a parameter passed into the program in the terminal.
     */

    if (scheduler_type == 0) {
        fcfs_scheduler(transition_type, old_state, new_state, write_file);
    } else if (scheduler_type == 1) {
        external_priorities_scheduler(transition_type, old_state, new_state, write_file);
    } else if (scheduler_type == 2) {
        round_robin_scheduler(transition_type, old_state, new_state, write_file);
    }
}



// Memory Management

_Bool allocate_memory(pcb_t *process, int memory[][2], int num_portions, char write_file[]) {
    /**
     * This function returns true if the memory requirments have been successfully allocated to
	 * memory hole. Otherwise it returns false. This function also prints out information about
     * each process that has memory allocated to it.
     */

    int current_address = 0;
    _Bool allocated = false;

    // Variable initialization for memory metrics
    int total_used_memory = 0;
    int num_used_portions = 0;
    int total_free_memory;
    int free_usable_memory = 0;

    // Iterate through memory portions to see if any is large enought for the process
    for (int i = 0; i < num_portions; i++) {
        // This takes care of allocating the memory portion to the process if possible
        if ((memory[i][1] == 0) && (memory[i][0] >= process->size_of_process) && (!allocated)) {
            memory[i][1] = process->size_of_process;
            process->base_memory_location = current_address;
            allocated = true;
        }
        // This computes the memory metrics
        if (memory[i][1] != 0) {
            total_used_memory += memory[i][1];
            num_used_portions++;
        } else {
            free_usable_memory += memory[i][0];
        }
        // current_address holds the current position in memory
        current_address += memory[i][0];
    }
    // More memory metric computing
    total_free_memory = current_address - total_used_memory;

    // If the process is allocated memory, write the memory metrics to the output file
    if (allocated) {
        FILE *f6 = fopen(write_file, "a");
        assert(f6 != NULL);
        fprintf(f6, "MEMORY HAS BEEN ALLOCATED TO PROCESS %d\n", process->pid);
        fprintf(f6, "Total used memory: %d Mb\n", total_used_memory);
        fprintf(f6, "Used memory portions: %d\nFree memory portions: %d\n", num_used_portions, num_portions - num_used_portions);
        fprintf(f6, "Total amount of free memory: %d Mb\n", total_free_memory);
        fprintf(f6, "Total amount of free usable memory: %d Mb\n\n", free_usable_memory);
        fclose(f6);
    }
    return allocated;
}

void deallocate_memory(pcb_t *process, int memory[][2], int num_portions) {
    /**
     * This function deallocates memory allocated by the allocate_memory function.
     */
    
    int current_address = 0;

    // Iterate through memory portions
    for (int i = 0; i < num_portions; i++) {
        // If the memory portion is the one allocated to the process stored in process, deallocate it
        if ((current_address == process->base_memory_location) && (memory[i][1] != 0)) {
            process->base_memory_location = -1;
            memory[i][1] = 0;
            break;
        }
        current_address += memory[i][0];
    }
}



// Error handling

void error_handling(pcb_t *processes[], int num_processes, int memory[], int num_memory_portions, _Bool use_memory_management) {
    /*
     * This function is designed to catch any problematic values in the input text file and print descriptive
     * error statements. It does not prevent any errors caused by an incorrectly formatted input file, only
     * accounts for situations where an input value could caused the program to enter an infinite loop or other
     * problematic situation.
     */

    // Find the biggest memory portion
    int biggest_portion = 0;
    if (use_memory_management) {
        for (int i = 0; i < num_memory_portions; i++) {
            if (memory[i] > biggest_portion) {
            biggest_portion = memory[i];
            }
        }
    }


    _Bool exit = false;

    // Iterate through all of the processes in the input file
    for (int i = 0; i < num_processes; i++) {

        // Make sure that no process requires more memory than the largest memory portion
        if (((processes[i]->size_of_process) > biggest_portion) && use_memory_management) {
            printf("\nERROR >>> Process %d requires %d Mb of memory\n", processes[i]->pid, processes[i]->size_of_process);
            printf("There is no memory portion large enough to hold the process as the largest portion is %d Mb\n", biggest_portion);
            exit = true;
        }

        // Make sure that all process inputs except pid and priority are positive
        if ((processes[i]->arrival_time < 0) || (processes[i]->remaining_cpu_time < 0) || (processes[i]->io_frequency < 0) || 
            (processes[i]->io_duration < 0) || (processes[i]->size_of_process < 0)) {
                printf("\nERROR >>> Process %d has an invalid negative input\n", processes[i]->pid);
                exit = true;
            }

        // Make sure that CPU burst time is not 0
        if (processes[i]->remaining_cpu_time == 0) {
            printf("\nERROR >>> Process %d has an invalid CPU burst time of 0\n", processes[i]->pid);
            exit = true;
        }

        // If I/O frequency for a process is 0, set it to be greater than the CPU burst time so I/O never occurs
        if (processes[i]->io_frequency == 0) {
            processes[i]->time_until_io = processes[i]->remaining_cpu_time;
            if (processes[i]->io_duration == 0) {
                processes[i]->io_duration == 1;
            } 
        }
    }
    // Exit if there is an error
    if (exit) {
        assert(false);
    }
}



// Main Script

int main(int argc, char *argv[]) {

    // Asserts the correct number of arguments is passed when running the program from command prompt
    if ((argc < 6) || (argc > 6)) {
        printf("Incorrect number of arguments");
        assert(false);
    }

    // Manage inputs
    int scheduler_type = atoi(argv[3]);
    int quantum;                    // The quantum is hardcoded to be 1000000 for all schedulers except the round robin scheduler
    if (scheduler_type == 2) {      // This is so that the quantum doesn't interfere in any other process
        quantum = atoi(argv[4]);    // The quantum can be specified in an input parameter for the round robin scheduler
    } else {
        quantum = 1000000;
    }

    _Bool use_memory_management;
    if (atoi(argv[5]) != 0) {           // Translates input parameter in command prompt to indicate if memory management
        use_memory_management = true;   // is being used
    } else {
        use_memory_management = false;
    }

    // Allocate an empty queue on the heap for each state in the diagram
    queue_t *new = new_queue();
    queue_t *ready = new_queue();
    queue_t *running = new_queue();
    queue_t *waiting = new_queue();
    queue_t *terminated = new_queue();

    // At this point, the memory portion used is one of two as selected by the user
    // This system was designed so that you only need to modify the following array to change memory for the whole program
    int memory_portion_sizes[] = {0, 0, 0, 0};
    if (atoi(argv[5]) == 1) {
        memory_portion_sizes[0] = 500;
        memory_portion_sizes[1] = 250;
        memory_portion_sizes[2] = 150;
        memory_portion_sizes[3] = 100;
    } else if (atoi(argv[5]) == 2) {
        memory_portion_sizes[0] = 300;
        memory_portion_sizes[1] = 300;
        memory_portion_sizes[2] = 350;
        memory_portion_sizes[3] = 50;
    }

    // Set up actual main_memory 2D array based on memory_portion_sizes array
    int num_portions = sizeof(memory_portion_sizes) / sizeof(int);
    int main_memory[num_portions][2];
    for (int i = 0; i < num_portions; i++) {
        main_memory[i][0] = memory_portion_sizes[i];
        main_memory[i][1] = 0;
    }

    // Remove all previous writing in the output file
    erase_write_file(argv[2]);

    // Add header to the output file
    FILE *f5 = fopen(argv[2], "a");
    assert(f5 != NULL);
    fprintf(f5, "%-14s %-14s %-14s %-14s\n\n", "Time", "PID", "Old State", "New State");
    fclose(f5);

    // Variable initialization for metrics
    int total_cpu_burst_time = 0;
    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    int total_time_between_io = 0;
    int num_io = 0;
    
    // Variable initialization for input file parsing
    int pid;
    int arrival_time;
    int total_cpu_time;
    int io_freq;
    int io_dur;
    int priority;
    int process_size;
    int num_processes = 0;

    // Input file parsing
    FILE *f3 = fopen(argv[1], "r");
    assert(f3 != NULL);
    while (!feof(f3)) {
        fscanf(f3, "%d %d %d %d %d %d %d\n", &pid, &arrival_time, &total_cpu_time, &io_freq, &io_dur, &priority, &process_size);
        num_processes++;
    }

    pcb_t *inputted_processes[num_processes];       // Create an array to hold the pcb's of all processes

    rewind(f3);

    for (int i = 0; i < num_processes; i++) {
        fscanf(f3, "%d %d %d %d %d %d %d\n", &pid, &arrival_time, &total_cpu_time, &io_freq, &io_dur, &priority, &process_size);
        inputted_processes[i] = new_pcb(pid, arrival_time, total_cpu_time, io_freq, io_dur, priority, process_size);
        total_cpu_burst_time += inputted_processes[i]->remaining_cpu_time;
    }
    fclose(f3);

    // Error handling function to check inputs
    error_handling(inputted_processes, num_processes, memory_portion_sizes, num_portions, use_memory_management);

    // Variable initialization for processing
    int num_terminated = 0;
    int wait_until;
    queue_t *temp = new_queue();
    pcb_t *curr;
    pcb_t *prev;
    int num_filled_memory_portions = 0;
    _Bool first_loop;

    // Beginning of processing
    while (num_processes > num_terminated) {

        // Metrics calculations
        total_waiting_time += ready->size;
        if (waiting->size == 0) { total_time_between_io++; }

        // Add processes to new at their arrival time
        int count = 0;
        for (int i = 0; i < num_processes; i++) {
            if (inputted_processes[i]->arrival_time == time_elapsed) {
                enqueue(new, inputted_processes[i]);
            }
        }
        
        // Move processes from new to ready (admit processes), option to use memory management or not
        if (use_memory_management) {
            if (num_filled_memory_portions < num_portions) {
                curr = new->front;
                prev = NULL;
                first_loop = true;
                while (curr != NULL) {
                    if (allocate_memory(curr, main_memory, num_portions, argv[2])) {
                        if (first_loop) {
                            scheduler(0, scheduler_type, new, ready, argv[2]);
                            curr = new->front;
                        } else {
                            if (curr == new->rear) {
                                new->rear = prev;
                            }
                            prev->next = curr->next;
                            curr->next = NULL;
                            new->size--;
                            enqueue(temp, curr);
                            scheduler(0, scheduler_type, temp, ready, argv[2]);
                            curr = prev->next;
                        }
                        num_filled_memory_portions++;
                    } else {
                        prev = curr;
                        curr = curr->next;
                        first_loop = false;
                    }
                }
            }
        } else {       // This else statement is used if memory management is not to be used
            while (new->size != 0) {
                scheduler(0, scheduler_type, new, ready, argv[2]);
            }
        }

        // If there is a process in ready and running is empty, transition
        if ((ready->size > 0) && (running->size == 0)) {
            transition(1, ready, running, argv[2]);
            running->front->cpu_arrival_time = time_elapsed;
        }

        // If there is a process in running, check if it needs to transition and update its
        // remaining CPU time.
        if (running->size > 0) {

            // If the process in running is done executing, move it from running to terminated
            if (running->front->remaining_cpu_time == 0) {
                total_turnaround_time += time_elapsed - running->front->arrival_time;
                transition(5, running, terminated, argv[2]);
                if (use_memory_management) {
                    deallocate_memory(terminated->front, main_memory, num_portions);
                    num_filled_memory_portions--;
                }
                dequeue(terminated, true);
                num_terminated++;
            
            // If the process in running needs I/O, transition from running to waiting
            } else if (running->front->time_until_io == 0) {
                num_io++;
                running->front->time_until_io = running->front->io_frequency;
                transition(2, running, waiting, argv[2]);
                if (waiting->size == 1) {
                    wait_until = time_elapsed + waiting->front->io_duration;
                }
            
            // If an interrupt is triggered because a process has been in running for too long,
			// transition to ready.
            } else if ((time_elapsed - running->front->cpu_arrival_time) > quantum)  {
                scheduler(4, scheduler_type, running, ready, argv[2]);
            } else {
                running->front->remaining_cpu_time -= 1;
                running->front->time_until_io -= 1;
            }
        }

        // If there is a process in waiting and it is done, transition to ready and 
		// set wait_until for the new process.
        if ((time_elapsed == wait_until) && (waiting->size > 0)) {
            scheduler(3, scheduler_type, waiting, ready, argv[2]);
            if (waiting->size > 0) {
                wait_until = time_elapsed + waiting->front->io_duration;
            }
        }
        time_elapsed++;
    }

    // Free queues, no need to free pcb's because they have already been deallocated by dequeue
    free(new);
    free(ready);
    free(running);
    free(waiting);
    free(terminated);
    free(temp);

    // Display metrics and write them to the output file
    FILE *f4 = fopen(argv[2], "a");
    assert(f4 != NULL);

    fprintf(f4, "\n\nNUMBER OF PROCESSES >>> %d\n\n", num_processes);

    fprintf(f4, "THROUGHPUT >>> %.2lf ms/process\n\n", (double) time_elapsed / num_processes);
    printf("THROUGHPUT >>> %.2lf ms/process\n\n", (double) time_elapsed / num_processes);

    fprintf(f4, "AVERAGE TURNAROUND TIME >>> %.2lf ms/process\n\n", (double) total_turnaround_time / num_processes);
    printf("AVERAGE TURNAROUND TIME >>> %.2lf ms/process\n\n", (double) total_turnaround_time / num_processes);

    fprintf(f4, "TOTAL WAIT TIME >>> %d ms\n\n", total_waiting_time);
    printf("TOTAL WAIT TIME >>> %d ms\n\n", total_waiting_time);

    fprintf(f4, "AVERAGE WAIT TIME >>> %.2lf ms/process\n\n", (double) total_waiting_time / num_processes);
    printf("AVERAGE WAIT TIME >>> %.2lf ms/process\n\n", (double) total_waiting_time / num_processes);

    fprintf(f4, "AVERAGE CPU BURST TIME >>> %.2lf ms/process\n\n", (double) total_cpu_burst_time / num_processes);
    printf("AVERAGE CPU BURST TIME >>> %.2lf ms/process\n\n", (double) total_cpu_burst_time / num_processes);

    if (num_io > 0) {
        fprintf(f4, "AVERAGE RESPONSE TIME >>> %.2lf ms", (double) total_time_between_io / num_io);
        printf("AVERAGE RESPONSE TIME >>> %.2lf ms", (double) total_time_between_io / num_io);
    }

    fclose(f4);

    // DONE!
    return 0;
}
