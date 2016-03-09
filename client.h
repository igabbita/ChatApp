#include <iostream>
#include <ctime>
#include <fstream>
using namespace std;
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <pthread.h>
 
#define 	SIZE 				4096
#define		SOCKET_ERROR		-1

class Self {
	string 	ip;
	string 	name;
	int 	port;
	
	public:
	Self ();
	
	void 	SetIP (string);
	void 	SetName (string);
	void 	SetPort (int);
	string 	GetIP();	
	string 	GetName();	
	int 	GetPort();
}; 

class Connection {
	string 		ip;
	int			port;
	
	public:
		Connection (string, int);
		void 			Send (string);
		string 			Pack (string);
};

class Receive {
	socklen_t 	socksize;
	
	public:
		Receive();
		void 			Write (string);
		string 			GetTimeStamp ();
		static void * 	Listener (void *);
};

