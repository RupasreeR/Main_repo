#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define SRVPORT "22768" 
#define SRV1 "21768"
#define SRV3 "23768"
#define BUFFSIZE 100    
#define BACKLOG 10 	
#define MAX_ARY 50

char* given_a_key_get_corresponding_value_from_server2(char * my_key)
{
	int i=0;
	int numProgs=0;
	char* lines[MAX_ARY];
	char line[40];
	int j;
	char search_key[1000];
	int position=6, length=5, c = 0;
	//char str[10];
	int ret=0;
	char* token;
	//char* my_key;
    
	FILE *file;
	file = fopen("server2.txt", "r+");
	
	if (file == NULL) {
                       fprintf(stderr, "Can't open input file in.list!\n");
                       exit(1);
              }

	
    
	while(fgets(line, sizeof line, file)!=NULL)
	{
		lines[i]=malloc(sizeof(line));
		strcpy(lines[i],line);
		i++;
		numProgs++;
	}
/*
	//check to be sure going into array correctly 
	for (j=0 ;j<numProgs+1;j++)
	{
      		;
	}
*/
	

	for(i = 0; i<numProgs; i++)
	{
        	// Gettng the key from the list
		token = strtok(lines[i]," ");

        	/* walk through other tokens */
                while( token != NULL )
       		{
                	ret=strcmp(my_key, token);
                	//printf("str=%s\t token=%s ret=%d\n", str, token, ret);
                	token = strtok(NULL," ");

                	if(ret==0)
                	{
                	//now token should be my required value, so break here
	                	break;
                	}
        	}
        	if (ret == 0)
                	break;
	}

	if (token == NULL)
        	token = "NOT-AVAILABLE";
	//printf("Required substring is: %s\n", token);
	fclose(file);

	return token;
}

/* Getting the IP address of the server 1,server 2 and server 3 */
int hostname_to_ip1(char * hostname , char* ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    if ( (he = gethostbyname( hostname ) ) == NULL)
    {
       // get the host info
               herror("gethostbyname");
        return 1;
    }

    addr_list = (struct in_addr **) he->h_addr_list;

    for(i = 0; addr_list[i] != NULL; i++)
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }

    return 1;

}


//Concat 'GET' with key
char* concat(const char *s1,char *my_key)
{
    char *result = malloc(strlen(s1)+strlen(my_key)+1);
    strcpy(result, s1);
    strcat(result, my_key);
    return result;
}

//Concat 'POST' with value
char* concat_p(const char *s2,char *my_required_value)
{
    char *result = malloc(strlen(s2)+strlen(my_required_value)+1);
    strcpy(result, s2);
    strcat(result, my_required_value);
    return result;
}





//Unconcatenate the strings
void unconcatenate(char *mesg,char *my_key) 
{
 char sub[100];
   int position=4;
   int length=5;
   int c = 0;
 
   while (c < length) {
      sub[c] = mesg[position+c];
      c++;
   }
   sub[c] ='\0';
 strcpy(my_key,sub);
   //printf("Required substring is%s\n", sub);
   //printf("sub=%s\n",sub);

}


void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) 
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
		return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(void)
{
	int sockfd, new_fd,streamSockFd; 					// listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p; 			// to get the address info to create socket.
	struct sockaddr_storage their_addr; 			// connector's address information
	socklen_t sin_size;
char * my_post_val_s2;
int yes=1,n;

  char *concat_key;
  const char *s1="GET ";
  const char *s2="POST ";

	char s[INET6_ADDRSTRLEN],ipstr[INET6_ADDRSTRLEN],srvip[INET6_ADDRSTRLEN];
	int rv,bytesRecvd;
	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);
	void *addr;
	char buf[BUFFSIZE];
	   char recvline[1000];
	int reserved[6]={0},i,j,idxint;
	char mesg[1000];
	  char  my_key[1000];
  char * my_required_value;

   	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP
	
	// to get the address information of server itself.
	if ((rv = getaddrinfo("127.0.0.1", SRVPORT, &hints, &servinfo)) != 0) 
	{
		fprintf(stderr, "ERROR!! getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	//Get the first match of ip address and create socket and then bind the socket to server address.
	for(p = servinfo; p != NULL; p = p->ai_next) 
	{
		//creating a socket
		if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) 
		{
			perror("server: socket()failed");
			continue;
		}
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) 
		{
			perror("setsockopt() failed");
			close(sockfd);
			freeaddrinfo(servinfo);
			exit(1);
		}
		//binding the socket
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
		{
			close(sockfd);
			perror("server: bind");
			continue;
		}
		break;
	}
	if (p == NULL) 
	{
		fprintf(stderr, "server: failed to bind\n");
		freeaddrinfo(servinfo);
		return 2;
	}
	
	//get the socket details of server and print the same
	if (getsockname(sockfd, (struct sockaddr *)&sin, &len) == -1)
	    perror("getsockname error");
	else
	{	
		addr = &(sin.sin_addr);
		inet_ntop(p->ai_family,addr,ipstr,sizeof ipstr);
		strcpy(srvip,ipstr);
		printf("The Server 2 has the TCP port number %d and IP address %s.\n",ntohs(sin.sin_port),ipstr);
	
	}

	if (listen(sockfd, BACKLOG) == -1) 
	{
		perror("listen() failed");
		freeaddrinfo(servinfo);
		close(sockfd);
		exit(1);
	}
	//signal handler to reap dead processes. Code reused from Beej Tutorial
	while(1) 
	{ 
		//Accept the client
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) 
		{
			if(errno==EINTR)			
			continue;
			perror("accept");
		}
		inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);
		if ((bytesRecvd = recv(new_fd, buf, BUFFSIZE-1, 0)) == -1)
			{
				perror("first recv error!!!");
				exit(1);
			}
			buf[bytesRecvd] = '\0';
			
			unconcatenate(buf,my_key);
			printf("my_key=%s\n",my_key);

   		printf("The Server 2 has received a request with key %s from the Server 1 with port number %s and IP address 127.0.0.1.\n",my_key,SRV1); 


 		my_required_value = given_a_key_get_corresponding_value_from_server2(my_key);

		if (!strcmp(my_required_value, "NOT-AVAILABLE"))
		{
			memset(&hints, 0, sizeof hints);
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;
			if ((rv = getaddrinfo("localhost", SRV3, &hints, &servinfo)) != 0) 
			{
				fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
				return 1;
			}
	
			// loop through all the results and connect to the first we can
			for(p = servinfo; p != NULL; p = p->ai_next) 
			{	
				//create a TCP Socket
				if ((streamSockFd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1)
				{
					perror("client: socket error");
					continue;
				}
				//Connect to the server
				if (connect(streamSockFd, p->ai_addr, p->ai_addrlen) == -1) 
				{
					close(streamSockFd);
					perror("client: connect error");
					continue;
				}
				break;
			}//end of for loop
			if (p == NULL) 
			{
				fprintf(stderr, "client: failed to connect\n");
				freeaddrinfo(servinfo); 
				return 2;
			}
			//add the code to send request
			send(streamSockFd,buf,strlen(buf),0);
			memset(recvline,0,sizeof(recvline));
      			n=recv(streamSockFd,recvline,10000,0);
	      		recvline[n]='\0';
			printf("received value %s\n",recvline);
	      		fputs(recvline,stdout);


			printf("The Server 2 received the value %s from the server 3 with port number %s and IP address 127.0.0.1\n",recvline,SRV3);
	    		printf("The Server 2 closed the TCP connection with Server 3.");
		
			//concat_post_2=concat_p(s2,recvline);
					//strcat(s2,my_required_value);
				printf("The Server 2 sends the reply %s to server 1 with port number %s and IP address 127.0.0.1\n",recvline,SRV1);
			 				
	
			n = send(new_fd,recvline,sizeof(recvline),0);
			
		}	
		
		else 
		{
            	
            	
            		 n = send(new_fd,my_required_value,sizeof(my_required_value),0);


		// Concatenate POST with value
		 my_post_val_s2=concat_p(s2,my_required_value);



			 printf("The Server 2,sent reply %s to the Server 1 with port number %s and IP address 127.0.0.1\n",my_post_val_s2,SRV1);


		}	

	
		close(new_fd);	
		//break;
	}
close(sockfd);
}
