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
#include <netdb.h>

#include "client.h"

Self self;

Self::Self () {
	ip 		=	"";
	name 	= 	"";
	port 	= 	-1;
}

void Self::SetIP (string ip) {
	this->ip = ip;
}

void Self::SetName (string name) {
	this->name = name;
}

void Self::SetPort (int port) {
	this->port = port;
}

string Self::GetIP() {
	return ip;
}

string Self::GetName() {
	return name;
}

int Self::GetPort() {
	return port;
}

Connection::Connection (string ip, int port) {
	char *		str;
	char 		buffer[SIZE + 1]; 					
	int 		length, chat_socket;
	struct 		sockaddr_in 	dest; 
	struct 		hostent *		server;
	string		init;
	
	memset (&dest, 0, sizeof(dest));             	
	strcpy (buffer, ip.c_str());
	
	str						=	NULL;
	this->ip				=	ip;
	this->port				= 	port;
	
	/* gethostbyname: get the server's DNS entry */
    server = gethostbyname(buffer);
    if (server == NULL) {
        cout << "ERROR, no such host as " << buffer << endl;
        exit(1);
    }

    /* build the server's Internet address */
    bzero((char *) &dest, sizeof(dest));
    dest.sin_family 	= 		AF_INET;
    bcopy((char *)server->h_addr,(char *)&dest.sin_addr.s_addr, server->h_length);
    dest.sin_port 		= 		htons(port);
	
	chat_socket = socket (AF_INET, SOCK_STREAM, 0);
	// cout << "Connection Constructor: chat_socket - " << chat_socket << endl;
	// cout << "Connection Constructor: IP Connecting to - " << ip.c_str() << " and port " << port << endl;
	if (connect (chat_socket, (struct sockaddr *)&dest, sizeof (struct sockaddr_in)) == SOCKET_ERROR) {
		cout << "Connection Constructor: Error in establishing connection with " << ip << endl;
		exit (1);
	}
	
	// Send initiation messages
	init = "NEW|" + self.GetIP() + "|" + self.GetName() + "#";
	strcpy (buffer, init.c_str());
	write (chat_socket, buffer, strlen (buffer));
	
	memset (buffer, 0, SIZE);
	read (chat_socket, buffer, SIZE);
	
	str = strtok (buffer, "#");
	
	if (strcmp (str, "WELCOME")) {
		cout << "User failed to register with the server" << endl;
		exit (2);
	}
	
	// cout << str << endl;
	
	close (chat_socket);
}

void Connection::Send (string msg) {
	char * 		str;
	string 		packed;
	char 		buffer[SIZE];
	
	int 		length, chat_socket;
	struct 		sockaddr_in 	dest; 
	struct 		hostent *		server;
	
    str		=	NULL;
	packed	= 	"";
	memset (buffer, 0, SIZE);
	
	packed 	= 	Pack(msg);		
	
	strcpy (buffer, packed.c_str ());
	memset (&dest, 0, sizeof(dest));     

	dest.sin_family 		= 	AF_INET;
	chat_socket = socket (AF_INET, SOCK_STREAM, 0);
	
	/* gethostbyname: get the server's DNS entry */
    server = gethostbyname((this->ip).c_str());
    if (server == NULL) {
        cout << "ERROR, no such host as " << buffer << endl;
        exit(1);
    }

    /* build the server's Internet address */
    bzero((char *) &dest, sizeof(dest));
    dest.sin_family 	= 		AF_INET;
    bcopy((char *)server->h_addr,(char *)&dest.sin_addr.s_addr, server->h_length);
    dest.sin_port 		= 		htons(this->port);

	if (connect (chat_socket, (struct sockaddr *)&dest, sizeof(struct sockaddr)) == SOCKET_ERROR) {
		cout << "Error in establishing connection with " << ip << endl;
		exit (1);
	}
	
	write (chat_socket, buffer, strlen(buffer));
	
	// Read Thank You
	read (chat_socket, buffer, SIZE);
	
	// validate
	str = strtok (buffer, "#");
	if (strcmp (str, "THANK_YOU")) {
		cout << "Message not Acknowledged" << endl;
	}
	// cout << str << endl;
	
	close(chat_socket);
}

string Connection::Pack (string msg) {
	string final_str = "MSG|" + self.GetIP() + "|" + self.GetName() + "|" + msg + "#";
	return final_str;
}

Receive::Receive() {
	struct 		sockaddr_in dest; 
	struct 		sockaddr_in serv; 
	int 		chat_socket, sSocket;           
	
	socksize = sizeof(struct sockaddr_in);
}

void Receive::Write (string s) {
	time_t 			rawtime;
	struct tm * 	timeinfo;
	char 			buffer[SIZE/4];
	ofstream		ofile;

	time (&rawtime);
	timeinfo = localtime(&rawtime);

	strftime (buffer, SIZE/4, "GroupChat_%Y-%m-%d.txt", timeinfo);
	string fName (buffer);
	
	cout << s << "# ";
	
	ofile.open (fName.c_str(), ofstream::app);
	ofile << s;
	ofile.close ();
}

string Receive::GetTimeStamp () {
	time_t 			rawtime;
	struct tm * 	timeinfo;
	char 			buffer[SIZE/4];

	time (&rawtime);
	timeinfo = localtime(&rawtime);

	strftime (buffer, SIZE/4, "%I:%M:%S\t", timeinfo);
	string str (buffer);
	
	return str;
}

void * Receive::Listener (void * ptr) {
	char		buffer [SIZE];
	char * 		str;
	char 		copy [SIZE];
	char 		msg [SIZE];
	char 		msg_type[SIZE/4], ip[SIZE], oName[SIZE];;
	string		output;
	Receive	*	receiver;
	string		old_users, new_users;
	// struct 		hostent *		server;
	
	struct 		sockaddr_in dest; 
	struct 		sockaddr_in serv; 
	int 		chat_socket, sSocket;           

	memset (buffer, 0, SIZE);
	memset (copy, 0, SIZE);
	memset (msg, 0, SIZE);
	memset (msg_type, 0, SIZE/4);
	memset (ip, 0, SIZE);
	memset (oName, 0, SIZE);
	memset (&serv, 0, sizeof(serv));
	output					=		"";
	old_users				=		"";
	new_users				=		"";
	str						=		NULL;
	
	serv.sin_family 		= 		AF_INET;    
	serv.sin_addr.s_addr 	= 		htonl(INADDR_ANY); // doesn't matter
	serv.sin_port 			= 		htons(self.GetPort());           
	
	receiver				=		(Receive *) ptr;
	
	cout << "Starting Client Listener Thread.." << endl;
	// sleep (3);
	
	chat_socket = socket (AF_INET, SOCK_STREAM, 0);
	if (chat_socket == SOCKET_ERROR) {
		cout << "Error in creating chat_socket" << endl;
		exit (1);
	}
  
	if (bind (chat_socket, (struct sockaddr *)&serv, sizeof(struct sockaddr)) == SOCKET_ERROR) {
		cout << "Error in Binding" << endl;
		close (chat_socket);
		exit (1);
	}

	if (listen (chat_socket, SIZE) == SOCKET_ERROR)    {
		cout << "Failed to listen" << endl;
		close (chat_socket);
		exit (1);
	}
	
	while (true) {
		sSocket = accept(chat_socket, (struct sockaddr *)&dest, &receiver->socksize);
		if (sSocket == SOCKET_ERROR) {
			cout << "Error in chat_socket connection" << endl;
			continue;
		}
		
		memset (buffer, 0, SIZE);
		read (sSocket, buffer, SIZE);
		
		// cout << "Listener Thread - " << buffer << endl;
		
		// Read and Write message to file and console.
		str = NULL;
		buffer [SIZE - 1] = '\0';
		strcpy (copy, buffer);
		
		str = strtok (buffer, "|");
		strcpy (msg_type, str);
		
		// cout << "Message Type: " << msg_type << endl;
		
		if (!strcmp (msg_type, "ARE_YOU_ALIVE")) {
			// Say Yes
			str = strtok (copy, "|");
			str = strtok (NULL, "#");
			strcpy (ip, str);
			
			memset (buffer, 0, SIZE);
			strcpy (buffer, "I_AM_ALIVE#");
			write (sSocket, buffer, strlen (buffer));
			close (sSocket);
			
			// New User list
			new_users = string (ip);
			if (new_users != old_users) {
				cout << "List of Users: " << new_users << endl << "# ";
				old_users = new_users;
			}
			
			continue;
		}
		else if (!strcmp (msg_type, "MSG")) {
			str = strtok (copy, "|");
			str = strtok (NULL, "|");
			strcpy (ip, str);
			
			// cout << "IP: " << ip << endl;
			
			str = strtok (NULL, "|");
			strcpy (oName, str);
			
			// cout << "Name: " << oName << endl;
			
			str = strtok (NULL, "#");
			strcpy (msg, str);
			
			// cout << "Message: " << msg << endl;
		
			output = receiver->GetTimeStamp() + string (oName) + ": " + string (msg) + "\n";
			receiver->Write (output);
			
			// Say thank you
			memset (buffer, 0, SIZE);
			strcpy (buffer, "THANK_YOU#");
			write (sSocket, buffer, strlen (buffer));
		}
		
		close (sSocket);
	}
}

int main(int argc, char *argv[]) {
	string 		message;
	pthread_t	hello, receive;
	
	if (argc != 6) {
		cout << "Usage: " << argv[0] << " [Username] [User IP] [User's Port] [Server IP] [Server Port]" << endl;
		return 0;
	}
	
	self.SetIP (string (argv[2]));
	self.SetName (string (argv[1]));
	self.SetPort (atoi (argv[3]));
	
	Receive 		receiver;
	cout << "Creating a connection...." << endl;
	pthread_create (&receive, NULL, &Receive::Listener,  (void *) &receiver);
	Connection 		conn(string (argv[4]), atoi (argv[5]));
	
	cout << "Type your message and hit enter. Your message will be broadcasted." << endl;
	
	while (true ) {
		cout << "# ";
		getline (cin, message);
		
		conn.Send(message);
	}
	
	return 0;
}