# COSC1114Assignment2

Assignment 2 of Operating System principles is a small program that takes the input and results of several external process’ within a central process to calculate a final number to be deemed stable (equal upon all external process’).
The assignment is constructed from two files, external.c and central.c which is extended later on in the assignment into several files for different methods of reaching the overall specifications.
The general breakdown of the assignment specifications are as follows:
 
Each external process has an input number which is used as it’s personal temperature. This process and result is then stored as a ID set in the Kernel's message Queue (via user Input) from a defined Central mailbox. This mailbox is used as a reference to store the external process ID’s.
This is important as other process’ (ie. The central process) could take data from an individual external process or the entire collection of external process’ for calculation.
In reference to the assignment, our central program would take a collection temperatures from the various external programs and use this to calculate and change the temperature through all process’ to a stable and even number.

External temperatures are defined in the program as:
new external temp = (10*myTemp + centralTemp) / 11; 

  \* With mytemp being defined as a user input (process argument), and centralTemp being a message grabbed from the Queue via the msgrcv()    function.\*


The Central temperature would then be calculated via:
new central temp = (centralTemp + 1000* temp sum of external processes) / (1000*number of external processes + 1); 

  \* With central temp being defined as a user input (process argument), and Temp Sum of external process’ being the sum of all available   temperatures found in the queue from the msgcrv() function. \*

**Task 1 involved**

One central mailbox with 8 external process'.

**Task 2 involved**

Two central mailbox's calculating seperate temperatures, both with 4 external process'.

**Task 3 involved**

A replication of Task 2, except implementing the solution via multithreading.

# Running the application is simple: 
**_If GCC is installed on your machine, tasks are compiled using the command "make task#" with # being a number of 1-3._**

**_Running each task is done via "make runTask#" with # being a number of 1-3._**
