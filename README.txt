Submitter name: Sagalpreet Singh
Roll No.: 2019csb1113
Course: CS303
=================================
1. What does this program do
The program simulates the deadlock scenario in cases where resources (multiple instances) are acquired by processes (threads in the case of program) in such a way that it becomes impossible for all the processes to make progress. The program handles deadlocks caused in such a manner by:

a) Detecting that deadlock has occurred
b) Identifying the processes(simulated as threads) involved in the deadlock
c) Terminating some of these processes to release resources bound to them so that other processes can progress and the deadlock is broken
d) For each of the deadlock, we need to keep a track of the duration between consecutive deadlocks while using different heuristics in selecting the process to be terminated first
e) The number of resources of different types that are available have to be taken as command line arguments
f) A thread is used to simulate a process. A thread continuously puts forward a set of resources (generated randomly) required by it. It tries to get those resources (with some random delay in between acquiring different resources) and the moment it has all the required resources, it waits for some time to simulate the process execution. After the wait is over, the thread releases all the resources acquired by it and starts off with a new set.
g) Whenever a process is terminated, essentially a thread is killed in simulation and a new thread is spawned in its place i.e the number of threads remains constant throughout the execution of the program.
h) Checking and handling of deadlocks is done on a dedicated separate thread which calls the function to detect deadlocks every "d" microseconds, where d is takes as a command line argument.
i) The program runs indefinitely until terminated forcefully by an interrupt

FILE STRUCTURE
- bin
  - run
- log
  - deadlock.txt
  - log.txt
  - main.txt
- obj
  - deadlock.o
  - log.o
  - main.o
  - resource.o
  - worker.o
- scripts
  - run.sh
- src
  - deadlock.c
  - deadlock.h
  - log.c
  - log.h
  - main.c
  - main.h
  - resources.c
  - resources.h
  - worker.c
  - worker.h
- unit-tests
  - bin
    - deadlock
    - resource
  - deadlock.c
  - deadlock.h
  - resource.c
  - resource.h
  - run_test.sh
- Design.pdf
- README.txt

*.git and .vscode folders are also present

=================================

2. A description of how this program works (i.e. its logic)

a) Simulating Processes: Using Threads. After taking the number of threads as command line arguments, corresponding number of threads are spawned each associated with a thread function.
This thread function randomly generates a set of required resources (with corresponding quantity). It tries to acquire these resources as and when available by constantly looking out for them. Each thread constantly keeps a check on if it is instructed by the deadlock handler to kill itself. The communication happends through shared memory.
After getting all the resources, it sleeps for some random time between 0.7d to 1.5d microseconds where d is the delay argument taken as a command. This simulates the process execution.
After this, the thread releases all the acquired resources. (it increments the number of freely available resources accordingly to simulate this)

b) Simulating resources: Resources are simulated using structures that contain the name of the resource, its total quantity available and the number of instances that are freely available to be acquired at present

c) Simulatiion of deadlock detection: The routine runs after every d microseconds where d is the delay argument taken as a command. To simulate this, it sleeps for d microseconds in every iteration of the while loop. To detect deadlock, the following algorithm is used:
- Try to go through all the available processes and check if any of these can complete successfully with the resources it has already acquired and the resources available freely to it.
- If such a resource is found, add the resources it has already acquired to the pool of free resources and mark it to be not involved in a deadlock. (Note that this step is performed on the copy of the available resources and the copy of resources acquired and not the original data structures as the process is still under progress while deadlock is being detected)
- Repeat the above two steps until no such process is found which can proceed further with given availability.
- At this point, all the processes which are not marked as deadlock free are involved in deadlock and can be reported.
- Now through some heuristic approach (which is discussed in detail in the Design document), it is decided as to which process to kill (i.e which thread to terminate), that thread is asked to terminate itself. The communication happens through shared memory. (Communication via signals is not a good idea for threads)
- Now all the above steps are repeated again until the deadlock is resolved.

It is worth noting here that deadlock may resolve in a lot of different manners, depending upon how randomness proceeds. Say some thread if terminated resolves the deadlock completely and by heuristics, that thread is selected, still it may so happen that before the next check for deadlock existence, that thread has not terminated, so another thread will be instructed to terminate itself, although even if it wouldn't have done so, the deadlock would have been resolved in some time.

d) Spawning new threads when older ones die: Threads are spawned only by the main thread in the implementation for obvious reasons of avoiding complicacies. Whenever a thread terminates itself, it communicates to the main thread (parent thread essentially) to spawn a new thread in its place. The communication again takes place through shared memory space. The main thread continuosly looks for such messages.

e) Inter thread communication: The communication between threads happens through shared memory via status bits (technically bytes, since char is used to store them). For example: To indicate to the main thread that the worker thread is exiting, it will unset the 'active' bit i.e active[i] = 0 where i is the mapping to id corresponding to that thread. Main thread observes this and spawns a new thread, storing the new thread id in place of the previous thread.

f) Tracking time between deadlocks: To track the time between deadlocks, all details related to the deadlocks are logged into a file with timestamp of when and because of which threads, the deadlock occured. Which threads had to be killed to resolve the deadlock etc.

NOTE: In the implementation, proper care is taken to keep the program thread safe by using mutex locks as and when required.

=================================

3. How to compile and run this program

-- Run this program "preferrably" in linux, although any POSIX based OS should do.

a) Easy method:
   - execute shell script in scripts folder by the command: ./run.sh OR sh ./run.sh (make sure you run this command from inside the script folder)
   - The command line arguments can be altered in the last line of execution

b) Full method:
   - Compile the entire code, link it to generate binaries and then run the binary using following commands
	gcc -w -c ./src/deadlock.c -o ./obj/deadlock.o -pthread
	gcc -w -c ./src/main.c -o ./obj/main.o -pthread
  	gcc -w -c ./src/resource.c -o ./obj/resource.o -pthread
	gcc -w -c ./src/worker.c -o ./obj/worker.o -pthread
	gcc -w -c ./src/log.c -o ./obj/log.o -pthread
	gcc -o ./bin/run ./obj/deadlock.o ./obj/main.o ./obj/resource.o ./obj/worker.o ./obj/log.o -pthread
   - Execute binary names "run" from bin folder using the command
        ./bin/run A 1 B 2 C 3 2 1000000

Command Line Arguments: Specify the list of avilable resources with their name and quantity specified as space separated strings and numbers as pairs. The above command corresponds to 1 A, 2 B, 3 C resources. The second last integer argument specifies the number of different threads to run. The last argument specifies the delay in microseconds.

By default, a constant int FORCE_DEADLOCK is set to 1 in worker.c file in the src folder. You may change it to 0 if you don't want to force deadlocks and keep randomness in resource requests but then it might become difficult for you to track the deadlocks and no deadlocks may occur if number of threads are very less (like, say 2)

The heuristic function can be changed in deadlock.c file by changing the function used in detect_deadlock function on line 107. By default, heaviest_thread_heuristic is being used.

The execution results are stored as log files in log folder.
log.txt contains time-stamped information about which thread requested which resource, which resource has been acquired in how much quantity by which thread etc.
deadlock.txt contains time-stamped information about whether deadlock has occurred. If yes, which all threads are involved in it.
main.txt is less useful and simply contains time stamps of when a new thread is spawned in place of the previous one.

=================================

4. Provide a snapshot of a sample run

a) STDOUT: simply indicates if some error/warning has occurred while execution otherwise no important output is written to STDOUT.

b) log.txt

Wed Oct 13 01:51:32 2021 : Thread 0 (140305928537856): Resource Requests made:
Wed Oct 13 01:51:32 2021 : Resource 0: 1
Wed Oct 13 01:51:32 2021 : Resource 1: 2
Wed Oct 13 01:51:32 2021 : Resource 2: 3


Wed Oct 13 01:51:32 2021 : Thread 1 (140305920145152): Resource Requests made:
Wed Oct 13 01:51:32 2021 : Resource 0: 1
Wed Oct 13 01:51:32 2021 : Resource 1: 2
Wed Oct 13 01:51:32 2021 : Resource 2: 3

Wed Oct 13 01:51:34 2021 : Thread 0 (140305928537856): 1 units of Resource 0 acquired
Wed Oct 13 01:51:36 2021 : Thread 0 (140305928537856): 3 units of Resource 2 acquired
Wed Oct 13 01:51:42 2021 : Thread 0 (140305928537856): 2 units of Resource 1 acquired
Wed Oct 13 01:51:42 2021 : Thread 0 (140305928537856): All the requirements for the process fulfilled. Initiating Process (sleep time)
Wed Oct 13 01:51:42 2021 : Successfully completed the request and deallocated all the resources


Wed Oct 13 01:51:42 2021 : Thread 0 (140305928537856): Resource Requests made:
Wed Oct 13 01:51:42 2021 : Resource 0: 1
Wed Oct 13 01:51:42 2021 : Resource 1: 2
Wed Oct 13 01:51:42 2021 : Resource 2: 3

Wed Oct 13 01:51:44 2021 : Thread 1 (140305920145152): 3 units of Resource 2 acquired
Wed Oct 13 01:51:46 2021 : Thread 1 (140305920145152): 1 units of Resource 0 acquired
Wed Oct 13 01:51:48 2021 : Thread 0 (140305928537856): 2 units of Resource 1 acquired
Wed Oct 13 01:51:50 2021 : Thread 1 (140305920145152): killed and all the resources acquired by it released

Wed Oct 13 01:51:50 2021 : Thread 1 (140305835747072): Resource Requests made:
Wed Oct 13 01:51:50 2021 : Resource 0: 1
Wed Oct 13 01:51:50 2021 : Resource 1: 2
Wed Oct 13 01:51:50 2021 : Resource 2: 3

Wed Oct 13 01:51:52 2021 : Thread 1 (140305835747072): 1 units of Resource 0 acquired
Wed Oct 13 01:51:52 2021 : Thread 0 (140305928537856): killed and all the resources acquired by it released

Wed Oct 13 01:51:52 2021 : Thread 0 (140305827354368): Resource Requests made:
Wed Oct 13 01:51:52 2021 : Resource 0: 1
Wed Oct 13 01:51:52 2021 : Resource 1: 2
Wed Oct 13 01:51:52 2021 : Resource 2: 3

Wed Oct 13 01:51:54 2021 : Thread 1 (140305835747072): 3 units of Resource 2 acquired
Wed Oct 13 01:51:56 2021 : Thread 1 (140305835747072): 2 units of Resource 1 acquired
Wed Oct 13 01:51:56 2021 : Thread 1 (140305835747072): All the requirements for the process fulfilled. Initiating Process (sleep time)
Wed Oct 13 01:51:57 2021 : Successfully completed the request and deallocated all the resources


Wed Oct 13 01:51:57 2021 : Thread 1 (140305835747072): Resource Requests made:
Wed Oct 13 01:51:57 2021 : Resource 0: 1
Wed Oct 13 01:51:57 2021 : Resource 1: 2
Wed Oct 13 01:51:57 2021 : Resource 2: 3

Wed Oct 13 01:51:58 2021 : Thread 0 (140305827354368): 2 units of Resource 1 acquired
Wed Oct 13 01:52:00 2021 : Thread 0 (140305827354368): 3 units of Resource 2 acquired
Wed Oct 13 01:52:06 2021 : Thread 0 (140305827354368): 1 units of Resource 0 acquired
Wed Oct 13 01:52:06 2021 : Thread 0 (140305827354368): All the requirements for the process fulfilled. Initiating Process (sleep time)
Wed Oct 13 01:52:08 2021 : Successfully completed the request and deallocated all the resources


Wed Oct 13 01:52:08 2021 : Thread 0 (140305827354368): Resource Requests made:
Wed Oct 13 01:52:08 2021 : Resource 0: 1
Wed Oct 13 01:52:08 2021 : Resource 1: 2
Wed Oct 13 01:52:08 2021 : Resource 2: 3

Wed Oct 13 01:52:09 2021 : Thread 1 (140305835747072): 2 units of Resource 1 acquired
Wed Oct 13 01:52:10 2021 : Thread 0 (140305827354368): 1 units of Resource 0 acquired
Wed Oct 13 01:52:11 2021 : Thread 1 (140305835747072): 3 units of Resource 2 acquired
Wed Oct 13 01:52:13 2021 : Thread 1 (140305835747072): killed and all the resources acquired by it released

Wed Oct 13 01:52:13 2021 : Thread 1 (140305818961664): Resource Requests made:
Wed Oct 13 01:52:13 2021 : Resource 0: 1
Wed Oct 13 01:52:13 2021 : Resource 1: 2
Wed Oct 13 01:52:13 2021 : Resource 2: 3

Wed Oct 13 01:52:14 2021 : Thread 0 (140305827354368): 3 units of Resource 2 acquired
Wed Oct 13 01:52:16 2021 : Thread 0 (140305827354368): 2 units of Resource 1 acquired
Wed Oct 13 01:52:16 2021 : Thread 0 (140305827354368): All the requirements for the process fulfilled. Initiating Process (sleep time)
Wed Oct 13 01:52:17 2021 : Successfully completed the request and deallocated all the resources


Wed Oct 13 01:52:17 2021 : Thread 0 (140305827354368): Resource Requests made:
Wed Oct 13 01:52:17 2021 : Resource 0: 1
Wed Oct 13 01:52:17 2021 : Resource 1: 2
Wed Oct 13 01:52:17 2021 : Resource 2: 3

Wed Oct 13 01:52:19 2021 : Thread 1 (140305818961664): 1 units of Resource 0 acquired
Wed Oct 13 01:52:21 2021 : Thread 1 (140305818961664): 2 units of Resource 1 acquired
Wed Oct 13 01:52:25 2021 : Thread 0 (140305827354368): 3 units of Resource 2 acquired
Wed Oct 13 01:52:27 2021 : Thread 1 (140305818961664): killed and all the resources acquired by it released

Wed Oct 13 01:52:27 2021 : Thread 1 (140305810568960): Resource Requests made:
Wed Oct 13 01:52:27 2021 : Resource 0: 1
Wed Oct 13 01:52:27 2021 : Resource 1: 2
Wed Oct 13 01:52:27 2021 : Resource 2: 3

Wed Oct 13 01:52:29 2021 : Thread 1 (140305810568960): 2 units of Resource 1 acquired
Wed Oct 13 01:52:29 2021 : Thread 0 (140305827354368): killed and all the resources acquired by it released

Wed Oct 13 01:52:29 2021 : Thread 0 (140305802176256): Resource Requests made:
Wed Oct 13 01:52:29 2021 : Resource 0: 1
Wed Oct 13 01:52:29 2021 : Resource 1: 2
Wed Oct 13 01:52:29 2021 : Resource 2: 3

Wed Oct 13 01:52:31 2021 : Thread 1 (140305810568960): 1 units of Resource 0 acquired
Wed Oct 13 01:52:33 2021 : Thread 1 (140305810568960): 3 units of Resource 2 acquired
Wed Oct 13 01:52:33 2021 : Thread 1 (140305810568960): All the requirements for the process fulfilled. Initiating Process (sleep time)
Wed Oct 13 01:52:33 2021 : Successfully completed the request and deallocated all the resources


Wed Oct 13 01:52:33 2021 : Thread 1 (140305810568960): Resource Requests made:
Wed Oct 13 01:52:33 2021 : Resource 0: 1
Wed Oct 13 01:52:33 2021 : Resource 1: 2
Wed Oct 13 01:52:33 2021 : Resource 2: 3

Wed Oct 13 01:52:35 2021 : Thread 0 (140305802176256): 2 units of Resource 1 acquired
Wed Oct 13 01:52:35 2021 : Thread 1 (140305810568960): 1 units of Resource 0 acquired
Wed Oct 13 01:52:37 2021 : Thread 0 (140305802176256): killed and all the resources acquired by it released

Wed Oct 13 01:52:37 2021 : Thread 0 (140305793783552): Resource Requests made:
Wed Oct 13 01:52:37 2021 : Resource 0: 1
Wed Oct 13 01:52:37 2021 : Resource 1: 2
Wed Oct 13 01:52:37 2021 : Resource 2: 3

Wed Oct 13 01:52:37 2021 : Thread 1 (140305810568960): 3 units of Resource 2 acquired
Wed Oct 13 01:52:41 2021 : Thread 1 (140305810568960): 2 units of Resource 1 acquired
Wed Oct 13 01:52:41 2021 : Thread 1 (140305810568960): All the requirements for the process fulfilled. Initiating Process (sleep time)
Wed Oct 13 01:52:43 2021 : Successfully completed the request and deallocated all the resources


Wed Oct 13 01:52:43 2021 : Thread 1 (140305810568960): Resource Requests made:
Wed Oct 13 01:52:43 2021 : Resource 0: 1
Wed Oct 13 01:52:43 2021 : Resource 1: 2
Wed Oct 13 01:52:43 2021 : Resource 2: 3

Wed Oct 13 01:52:43 2021 : Thread 0 (140305793783552): 3 units of Resource 2 acquired
Wed Oct 13 01:52:45 2021 : Thread 0 (140305793783552): 1 units of Resource 0 acquired
Wed Oct 13 01:52:47 2021 : Thread 0 (140305793783552): 2 units of Resource 1 acquired
Wed Oct 13 01:52:47 2021 : Thread 0 (140305793783552): All the requirements for the process fulfilled. Initiating Process (sleep time)
Wed Oct 13 01:52:48 2021 : Successfully completed the request and deallocated all the resources


Wed Oct 13 01:52:48 2021 : Thread 0 (140305793783552): Resource Requests made:
Wed Oct 13 01:52:48 2021 : Resource 0: 1
Wed Oct 13 01:52:48 2021 : Resource 1: 2
Wed Oct 13 01:52:48 2021 : Resource 2: 3

Wed Oct 13 01:52:49 2021 : Thread 1 (140305810568960): 3 units of Resource 2 acquired
Wed Oct 13 01:52:51 2021 : Thread 1 (140305810568960): 1 units of Resource 0 acquired
Wed Oct 13 01:52:52 2021 : Thread 0 (140305793783552): 2 units of Resource 1 acquired
Wed Oct 13 01:52:53 2021 : Thread 1 (140305810568960): killed and all the resources acquired by it released

Wed Oct 13 01:52:53 2021 : Thread 1 (140305709922048): Resource Requests made:
Wed Oct 13 01:52:53 2021 : Resource 0: 1
Wed Oct 13 01:52:53 2021 : Resource 1: 2
Wed Oct 13 01:52:53 2021 : Resource 2: 3

c) deadlock.txt

Wed Oct 13 01:51:33 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:33 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:33 2021 : 
Wed Oct 13 01:51:33 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:34 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:34 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:34 2021 : 
Wed Oct 13 01:51:34 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:35 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:35 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:35 2021 : 
Wed Oct 13 01:51:35 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:36 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:36 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:36 2021 : 
Wed Oct 13 01:51:36 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:37 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:37 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:37 2021 : 
Wed Oct 13 01:51:37 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:38 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:38 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:38 2021 : 
Wed Oct 13 01:51:38 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:39 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:39 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:39 2021 : 
Wed Oct 13 01:51:39 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:40 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:40 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:40 2021 : 
Wed Oct 13 01:51:40 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:41 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:41 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:41 2021 : 
Wed Oct 13 01:51:41 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:42 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:42 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:42 2021 : 
Wed Oct 13 01:51:42 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:43 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:43 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:43 2021 : 
Wed Oct 13 01:51:43 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:44 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:44 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:44 2021 : 
Wed Oct 13 01:51:44 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:45 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:45 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:45 2021 : 
Wed Oct 13 01:51:45 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:46 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:46 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:46 2021 : 
Wed Oct 13 01:51:46 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:47 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:47 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:47 2021 : 
Wed Oct 13 01:51:47 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:48 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:48 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:48 2021 : 
Wed Oct 13 01:51:48 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:49 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:49 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:49 2021 : 0 (140305928537856)
Wed Oct 13 01:51:49 2021 : 1 (140305920145152)
Wed Oct 13 01:51:49 2021 : 
Wed Oct 13 01:51:49 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:50 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:50 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:50 2021 : 
Wed Oct 13 01:51:50 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:51 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:51 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:51 2021 : 
Wed Oct 13 01:51:51 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:52 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:52 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:52 2021 : 0 (140305928537856)
Wed Oct 13 01:51:52 2021 : 1 (140305835747072)
Wed Oct 13 01:51:52 2021 : 
Wed Oct 13 01:51:52 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:53 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:53 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:53 2021 : 
Wed Oct 13 01:51:53 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:54 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:54 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:54 2021 : 
Wed Oct 13 01:51:54 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:55 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:55 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:55 2021 : 
Wed Oct 13 01:51:55 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:56 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:56 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:56 2021 : 
Wed Oct 13 01:51:56 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:57 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:57 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:57 2021 : 
Wed Oct 13 01:51:57 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:58 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:58 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:58 2021 : 
Wed Oct 13 01:51:58 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:51:59 2021 : Deadlock Checking Begins
Wed Oct 13 01:51:59 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:51:59 2021 : 
Wed Oct 13 01:51:59 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:00 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:00 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:00 2021 : 
Wed Oct 13 01:52:00 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:01 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:01 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:01 2021 : 
Wed Oct 13 01:52:01 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:02 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:02 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:02 2021 : 
Wed Oct 13 01:52:02 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:03 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:03 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:03 2021 : 
Wed Oct 13 01:52:03 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:04 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:04 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:04 2021 : 
Wed Oct 13 01:52:04 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:05 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:05 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:05 2021 : 
Wed Oct 13 01:52:05 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:06 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:06 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:06 2021 : 
Wed Oct 13 01:52:06 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:07 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:07 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:07 2021 : 
Wed Oct 13 01:52:07 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:08 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:08 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:08 2021 : 
Wed Oct 13 01:52:08 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:09 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:09 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:09 2021 : 
Wed Oct 13 01:52:09 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:10 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:10 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:10 2021 : 
Wed Oct 13 01:52:10 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:11 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:11 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:11 2021 : 0 (140305827354368)
Wed Oct 13 01:52:11 2021 : 1 (140305835747072)
Wed Oct 13 01:52:11 2021 : 
Wed Oct 13 01:52:11 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:12 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:12 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:12 2021 : 0 (140305827354368)
Wed Oct 13 01:52:12 2021 : 1 (140305835747072)
Wed Oct 13 01:52:12 2021 : 
Wed Oct 13 01:52:12 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:13 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:13 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:13 2021 : 
Wed Oct 13 01:52:13 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:14 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:14 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:14 2021 : 
Wed Oct 13 01:52:14 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:15 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:15 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:15 2021 : 
Wed Oct 13 01:52:15 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:16 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:16 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:16 2021 : 
Wed Oct 13 01:52:16 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:17 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:17 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:17 2021 : 
Wed Oct 13 01:52:17 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:18 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:18 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:18 2021 : 
Wed Oct 13 01:52:18 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:19 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:19 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:19 2021 : 
Wed Oct 13 01:52:19 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:20 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:20 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:20 2021 : 
Wed Oct 13 01:52:20 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:21 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:21 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:21 2021 : 
Wed Oct 13 01:52:21 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:22 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:22 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:22 2021 : 
Wed Oct 13 01:52:22 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:23 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:23 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:23 2021 : 
Wed Oct 13 01:52:23 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:24 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:24 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:24 2021 : 
Wed Oct 13 01:52:24 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:25 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:25 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:25 2021 : 
Wed Oct 13 01:52:25 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:26 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:26 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:26 2021 : 0 (140305827354368)
Wed Oct 13 01:52:26 2021 : 1 (140305818961664)
Wed Oct 13 01:52:26 2021 : 
Wed Oct 13 01:52:26 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:27 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:27 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:27 2021 : 
Wed Oct 13 01:52:27 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:28 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:28 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:28 2021 : 
Wed Oct 13 01:52:28 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:29 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:29 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:29 2021 : 0 (140305827354368)
Wed Oct 13 01:52:29 2021 : 1 (140305810568960)
Wed Oct 13 01:52:29 2021 : 
Wed Oct 13 01:52:29 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:30 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:30 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:30 2021 : 
Wed Oct 13 01:52:30 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:31 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:31 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:31 2021 : 
Wed Oct 13 01:52:31 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:32 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:32 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:32 2021 : 
Wed Oct 13 01:52:32 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:33 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:33 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:33 2021 : 
Wed Oct 13 01:52:33 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:34 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:34 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:34 2021 : 
Wed Oct 13 01:52:34 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:35 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:35 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:35 2021 : 
Wed Oct 13 01:52:35 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:36 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:36 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:36 2021 : 0 (140305802176256)
Wed Oct 13 01:52:36 2021 : 1 (140305810568960)
Wed Oct 13 01:52:36 2021 : 
Wed Oct 13 01:52:36 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:37 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:37 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:37 2021 : 0 (140305802176256)
Wed Oct 13 01:52:37 2021 : 1 (140305810568960)
Wed Oct 13 01:52:37 2021 : 
Wed Oct 13 01:52:37 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:38 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:38 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:38 2021 : 
Wed Oct 13 01:52:38 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:39 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:39 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:39 2021 : 
Wed Oct 13 01:52:39 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:40 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:40 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:40 2021 : 
Wed Oct 13 01:52:40 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:41 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:41 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:41 2021 : 
Wed Oct 13 01:52:41 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:42 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:42 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:42 2021 : 
Wed Oct 13 01:52:42 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:43 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:43 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:43 2021 : 
Wed Oct 13 01:52:43 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:44 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:44 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:44 2021 : 
Wed Oct 13 01:52:44 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:45 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:45 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:45 2021 : 
Wed Oct 13 01:52:45 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:46 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:46 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:46 2021 : 
Wed Oct 13 01:52:46 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:47 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:47 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:47 2021 : 
Wed Oct 13 01:52:47 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:48 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:48 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:48 2021 : 
Wed Oct 13 01:52:48 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:49 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:49 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:49 2021 : 
Wed Oct 13 01:52:49 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:50 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:50 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:50 2021 : 
Wed Oct 13 01:52:50 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:51 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:51 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:51 2021 : 
Wed Oct 13 01:52:51 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:52 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:52 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:52 2021 : 
Wed Oct 13 01:52:52 2021 : 
-----------Deadlock Checking Ends------------


Wed Oct 13 01:52:53 2021 : Deadlock Checking Begins
Wed Oct 13 01:52:53 2021 : The processes involved in deadlock reside on the following threads:
Wed Oct 13 01:52:53 2021 : 0 (140305793783552)
Wed Oct 13 01:52:53 2021 : 1 (140305810568960)
Wed Oct 13 01:52:53 2021 : 
Wed Oct 13 01:52:53 2021 : 
-----------Deadlock Checking Ends------------


d) main.txt

Wed Oct 13 01:51:50 2021 : Thread 1 is being restarted...
Wed Oct 13 01:51:52 2021 : Thread 0 is being restarted...
Wed Oct 13 01:52:13 2021 : Thread 1 is being restarted...
Wed Oct 13 01:52:27 2021 : Thread 1 is being restarted...
Wed Oct 13 01:52:29 2021 : Thread 0 is being restarted...
Wed Oct 13 01:52:37 2021 : Thread 0 is being restarted...
Wed Oct 13 01:52:53 2021 : Thread 1 is being restarted...


NOTE: deadlock.txt tracks the deadlock handler completely. So, even if no deadlock occurred in some check of deadlock handler, it still prints stuff to indicate that deadlock handler got activated although no thread presence indicates that no deadlock actually occurred.

