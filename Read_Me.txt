Readers Writer Problem -->
To solve this, I have taken a hypothetical case of 50 reader threads and 2 writer threads. Every reader thread will have its own functionality but it will read (and to make them a bit different I have used usleep with a random number generator so that the sleeping time for all the reader threads vary) the value of the given shared global variable. I am printing the value of the global variable accessed by both reader and writer thread. That variable (accesses by both the threads) is global_data_RW. I have used mutex and condition variables to solve this problem.

I have used sleep as reader threads will be coming from different places and will not be the same they will just be calling this synchronization function of read. SO sleep makes every reader thread a bit different by making them sleep for different time.

The writer and reader threads have their own condition variable queues and they sleep on them until someone signals/broadcasts them to wake up. A writer thread can only work if no other writer thread and no reader thread is accessing the common (global) data structure (variable in this case). 

to compile and run the program use --> 
a)gcc -o q2 Read_write.c -lpthread and then b) ./q2

