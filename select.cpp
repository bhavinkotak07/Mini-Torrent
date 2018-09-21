//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <iostream>
#include <unordered_map>
#include <vector>
#define TRUE 1
#define FALSE 0
#define PORT 8888
using namespace std;
void trim(char *str)
{
    int i;
    int begin = 0;
    int end = strlen(str) - 1;

    while (isspace((unsigned char) str[begin]))
        begin++;

    while ((end >= begin) && isspace((unsigned char) str[end]))
        end--;

    // Shift all characters back to the start of the string array.
    for (i = begin; i <= end; i++)
        str[i - begin] = str[i];

    str[i - begin] = '\0'; // Null terminate string.
}
int main(int argc , char *argv[])
{
	int opt = TRUE;
  unordered_map <string, vector<string> > seeder_list;
  string ip = argv[1];
  int index_of_port = ip.find(":");
  string port = ip.substr(index_of_port + 1, ip.size() - index_of_port);
  ip = ip.substr(0, index_of_port);
  cout<<ip<< " "<<port<<endl;
  //struct sockaddr_in address;
	int master_socket , addrlen , new_socket , client_socket[30] ,
		max_clients = 30 , activity, i , valread , sd;
	int max_sd;
	struct sockaddr_in address;

	char buffer[1025]; //data buffer of 1K

	//set of socket descriptors
	fd_set readfds;

	//a message
	char *message = "ECHO Daemon v1.0 \r\n";

	//initialise all client_socket[] to 0 so not checked
	for (i = 0; i < max_clients; i++)
	{
		client_socket[i] = 0;
	}

	//create a master socket
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	//set master socket to allow multiple connections ,
	//this is just a good habit, it will work without this
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
		sizeof(opt)) < 0 )
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	//type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( atoi(port.c_str()) );

	//bind the socket to localhost port 8888
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", atoi(port.c_str()));

	//try to specify maximum of 3 pending connections for the master socket
	if (listen(master_socket, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	//accept the incoming connection
	addrlen = sizeof(address);
	puts("Waiting for connections ...");

	while(TRUE)
	{
		//clear the socket set
		FD_ZERO(&readfds);

		//add master socket to set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;

		//add child sockets to set
		for ( i = 0 ; i < max_clients ; i++)
		{
			//socket descriptor
			sd = client_socket[i];

			//if valid socket descriptor then add to read list
			if(sd > 0)
				FD_SET( sd , &readfds);

			//highest file descriptor number, need it for the select function
			if(sd > max_sd)
				max_sd = sd;
		}

		//wait for an activity on one of the sockets , timeout is NULL ,
		//so wait indefinitely
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

		if ((activity < 0) && (errno!=EINTR))
		{
			printf("select error");
		}

		//If something happened on the master socket ,
		//then its an incoming connection
		if (FD_ISSET(master_socket, &readfds))
		{
			if ((new_socket = accept(master_socket,
					(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}

			//inform user of socket number - used in send and receive commands
			printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));




			for (i = 0; i < max_clients; i++)
			{
				//if position is empty
				if( client_socket[i] == 0 )
				{
					client_socket[i] = new_socket;
					printf("Adding to list of sockets as %d\n" , i);

					break;
				}
			}
		}

		//else its some IO operation on some other socket
		for (i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];
      char buffer[1024];
			if (FD_ISSET( sd , &readfds))
			{
				//Check if it was for closing , and also read the
				//incoming message
				if ((valread = read( sd , buffer, 1024)) == 0)
				{
					//Somebody disconnected , get his details and print
					getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
					printf("Host disconnected , ip %s , port %d \n" ,inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

					//Close the socket and mark as 0 in list for reuse
					close( sd );
					client_socket[i] = 0;
				}

				//Echo back the message that came in
				else
				{
					//set the string terminating NULL byte on the end
					//of the data read
					buffer[valread] = '\0';

          //memset(buff, 0, 1024);
          //int n = read(new_socket, buffer, strlen(buffer) );
          cout<<"Read:"<<valread<<endl;
          if(valread < 0)
            continue;
          for(int i = valread; i < 1024 ;i++){
            buffer[i] = 0;
          }
          //char buffer[1025];
          //valread = read( new_socket , buffer, 1024);
          cout<<"Received:"<<buffer<<endl;
          string received_buffer = buffer;
          received_buffer = received_buffer.substr(received_buffer.find(";")+1, received_buffer.size());
          char *token = strtok(buffer, ";");

          // Keep printing tokens while one of the
          // delimiters present in str[].

          if(strcmp(token, "Share") == 0){
            cout<<"Inside share"<<endl;
            //cout<<"CLient:"<<buffer<<endl;
            string t = token;

            token = strtok(NULL, ";");
            token = strtok(NULL, ";");
            token = strtok(NULL, ";");
            trim(token);
            cout<<"File:"<<token<<endl;
            //token = strtok(NULL, ";");
            //cout<<received_buffer<<endl;
            if(seeder_list.find(token) == seeder_list.end()){
              vector <string > s;
              s.push_back(received_buffer);
              seeder_list[token] = s;
              cout<<"Adding "<<token<<" to map"<<endl;
            }
            else{
              seeder_list[token].push_back(received_buffer);
              cout<<"Updating "<<token<< "  to map"<<endl;
            }

            cout<<"List:"<<seeder_list[token].size()<<endl;
            char buf[100];
            strcpy(buf, "Seed info received\0");
            //snprintf(buf, sizeof buf, "Seed info received\0");
            //printf("Tracker: Host  ip %s , port %d \n" ,inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
            cout<<"Sending "<<buf<<endl;
            send(sd, buf, strlen(buf), 0);
            cout<<"Sent "<<buf<<endl;


          }
          else if(strcmp(token, "Get") == 0){
            cout<<"Inside Get"<<token<<endl;
            token = strtok((char *)received_buffer.c_str(), ";");
            trim(token);
            cout<<"Search:"<<token<<endl;
            cout<<"Len:"<<strlen(token);
            //cout<<received_buffer<<endl;
            string buf = "";
            int i;
            if(seeder_list.find(token) != seeder_list.end()){
              cout<<"Found"<<endl;
              cout<<seeder_list[token].size()<<endl;
              for(i = 0; i < seeder_list[token].size() - 1; i++){
                buf += seeder_list[token][i] + "|";
              }
              buf += seeder_list[token][i] ;
            }
            else{
              buf = "No seeders available";
            }


            unordered_map <string , vector<string>> :: iterator itr;
            for(itr = seeder_list.begin(); itr != seeder_list.end(); itr++){
                cout<<"Map:"<<itr->first<<endl;
              //for(int i = 0; i < itr->second.size(); i++)
                //cout<<itr->second[i]<<endl;
            }
            //strcpy(buf, "Seed info received\0");
            //snprintf(buf, sizeof buf, "Seed info received\0");
            //printf("Tracker: Host  ip %s , port %d \n" ,inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
            cout<<"Sending "<<buf<<endl;
            send(sd, buf.c_str(), strlen(buf.c_str()), 0);
            cout<<"Sent "<<buf<<endl;
          }
					//send(sd , buffer , strlen(buffer) , 0 );
				}
			}
		}
	}

	return 0;
}
