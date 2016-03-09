ChatApp - A simple broadcast based, group chat application.
==
Project Coded in C++, Python. OS - Unix
--

#### Please follow the steps for execution:


**Step 1**: To compile, download the code/clone the repository on your local server/client(s) machine(s).

**Step 2**: Compile - Client only

	>*location_for_all_the_files_listed*
	> g++ --std=c++11 -Wl,-rpath=/usr/local/gcc491/lib64 -pthread client.cpp -o client

Server - You need to have Python 2.7 or any above version installed to run.
    	   
**Step 6**: Execution

	Client:
	> ./client [Username] [User IP] [User's Port] [Server IP] [Server Port]


	Server:
	> python server.py # The port is hard coded to be 1201 - just for simplicity.

**Step 7**: Output -- The Output can be seen on screen - The live chatting. A local file under the name of "GroupChat_YYYY-MM-DD.txt" will be created locally for both the client and the server. Technically, all the files should have the same data.

#################################################################################
**Note:**

1. As it is visible, the Server has been programmed in Python and client in C++. This was done with an intention to demonstrate cross-platform interaction over the network. 
2. TCP/IP socket programming is the base for this application.
3. Has been developed and tested extensively in Unix (Ubuntu) - g++491 and python 2.7. Cannot guarantee accurate results of cygwin. 
4. Users can be added or deleted dynamically. The server needs to be running.
5. Limitation 1: No 2 users can be on the same machine with same name.
6. Limitation 2: This is a CLI application, programmed for the better understanding of TCP/IP programming across different platforms. Hence, the view or interaction is not as good as GUI.

These limitations will be taken care of.
#################################################################################

*Please contact me on any kind of concerns/questions*

**By,** 
### Iswarya S Gabbita 
**Email: igabbita@gmail.com**
