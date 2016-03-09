import socket
import thread
from datetime import datetime
import time
import json

class Server:
	PORT	=	1201
	SIZE	=	4096
	users 	= 	{}

	def Recieve(self):
		chat_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		# host = socket.gethostname() 
		
		chat_socket.bind(('', Server.PORT))        
		# print "Bind Complete"
		
		chat_socket.listen(Server.SIZE)                 
		# print "Listen set"
		while True:
			# print "Waiting for a connection"
			client, addr = chat_socket.accept()     
			
			msg = client.recv(Server.SIZE).replace("#", "")
			
			# print "Server: Message received: " + msg
			
			splits = msg.split("|")
			
			if splits[0] == "MSG":
				msg += "#";
				
				client.send ("THANK_YOU#")
				client.close()
				
				output = time.strftime ("%H:%M:%S") + " - " + splits[2] + ": " + splits[3]
				print output
				
				with open ("ChatApp_" + time.strftime ("%Y-%m-%d") + ".txt", "a") as file:
					file.write (output)
				
				# forward the message to everyone
				# print "Users are: " + str(Server.users.keys())
				for user in Server.users:
					# print user
					temp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
					# print "Forwarding message to - " + Server.users[user]["ip"] + ", " + str(Server.PORT)
					temp_socket.connect((Server.users[user]["ip"], Server.PORT))
					temp_socket.send(msg)
					
					ty = temp_socket.recv(Server.SIZE)
					if (ty != "THANK_YOU#"):
						print Server.users[user]["ip"] + " did not receive the message"
					
					temp_socket.close()
				
				
			elif splits[0] == "NEW":
				# Do Something
				key = splits[1] + "_" + splits[2]
				# print "New Connection - " + key
				
				Server.users[key] = {}
				Server.users[key]["ip"] = splits[1]
				Server.users[key]["Name"] = splits[2]
				# Server.users[key]["Alive"] = True
				
				# print "Added new user - " + str (Server.users.keys())
				# print json.dumps(Server.users)
				
				# keys = Server.users.keys()
				# keys.sort()
				# all_users = ", ".join (keys)
				
				client.send ("WELCOME#")
				client.close()
			else:
				# Do Something
				print "Invalid Message - " + splits[0]
			
			

	def IsAlive(self):
		print "Starting Server Thread"
		
		while True:
			deadusers = []
			time.sleep (5)
			# print "Sending ISALIVE messages"
			
			for user in Server.users:
				keys = Server.users.keys()
				keys.sort()
				all_users = ", ".join (keys)
			
				temp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
				try:
					temp_socket.connect((Server.users[user]["ip"], Server.PORT))
					temp_socket.send("ARE_YOU_ALIVE|" + all_users + "#")
					
					alive = temp_socket.recv(Server.SIZE)
					if (alive != "I_AM_ALIVE#"):
						print user["ip"] + " did not receive the message"
					
					temp_socket.close()
				except:
					# client's dead
					print user + " is dead!"
					deadusers.append(user)
					
			for deaduser in deadusers:
				del Server.users[deaduser]
			
		
		
def main ():
	receive = Server()

	print "Starting Server.. Active on port ", receive.PORT
	thread.start_new_thread(receive.IsAlive, ())
	receive.Recieve()

if __name__ == "__main__":
	main()