/* Sample UDP server */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//char * given_a_key_get_corresponding_value(char * mesg);
#define MAX_ARY 50
#define SRV2 "22768"
#define SRV1 "21768"


void given_a_key_get_corresponding_value(char str[15],char my_required_value[100])
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
	char* my_key;
    	char temp[100];
	strcpy(temp,str);
	FILE *file;
	file = fopen("server1.txt", "r+");

  
	while(fgets(line, sizeof line, file)!=NULL)
	{
		lines[i]=malloc(sizeof(line));
		strcpy(lines[i],line);
		i++;
		numProgs++;
	}
	
	//check to be sure going into array correctly 
	for (j=0 ;j<numProgs+1;j++)
	{
      		;
	}
	strcpy(str,temp);
	

	for(i = 0; i<numProgs; i++)
	{
        	
		token = strtok(lines[i]," ");

        	/* walk through other tokens */
                while( token != NULL )
       		{
                	ret=strcmp(str, token);
                	token = strtok(NULL," ");

                	if(ret==0)
                	{
                	//now token should be my required token, so break here
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
	strcpy(my_required_value,token);
	//return token;
}

/* Getting the IP address of the server 1 and client */

int hostname_to_ip1(char *hostname , char *ip)
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

//Unconcatenate the strings
void unconcatenate(char *mesg,char my_key[6]) 
{
   char sub[1000];
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
  // return sub;
   
}


//Concat 'GET' with key
char* concat_g(const char *s1,char *my_key)
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



int main(int argc, char**argv)
{
   int sockfd,n,rv,streamSockFd;
   int sockfd_tcp; /* TCP */
   struct sockaddr_in servaddr,  servaddr_tcp;
   struct sockaddr_storage cliaddr;
   socklen_t len;
   char mesg[1000];
   char formatted_mesg[1000];
   int ret = 0;
   char  my_required_value[100]="default";
   char* server_ip_addr;
   socklen_t addr_len;
   
   int servaddr_tcp_len;
int counter;
   int getsock_check;
   int dynamic_port_number=0;
   char recvline[1000]; /* TCP */
   char *server_s1_name;
   char *client_name;
   char ip_client[30];
   char ip_server[30];
   char my_key[6]="defal";
   char *concat_key_2;
   const char *s1="GET ";
   char *concat_post_2;
   const char *s2="POST ";
struct addrinfo hints, *servinfo, *p;
struct sockaddr_in sin;

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) 
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; 
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; 
		
	if ((rv = getaddrinfo("127.0.0.1", SRV1, &hints, &servinfo)) != 0) 
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	
	for(p = servinfo; p != NULL; p = p->ai_next) 
	{
		//create socket
		if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) 
		{
			continue;
		}
		//bind the socket
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
		{
			close(sockfd);
			freeaddrinfo(servinfo);
			perror(" bind failed");
			continue;
		}
		break;
	}
	if (p == NULL) 
	{
		fprintf(stderr, "failed to bind socket\n");
		freeaddrinfo(servinfo);
		return 2;
	}
	if (getsockname(sockfd, (struct sockaddr *)&sin, &len) == -1)
	    perror("getsockname error");
	
	
   printf("The Server 1 has UDP port number %s and IP address 127.0.0.1\n",SRV1);

/****************************************/
/* UDP Connection */
//socket, bind- UDP
//socket, bind tcp

	
/* End of TCP connection */
/****************************************/
   counter=0;
   for (;;)
   {
	counter++;
	/**********************************************************************************/
	len = sizeof(cliaddr);
//printf("cliaddr=%s\n",cliaddr);
	n = recvfrom(sockfd,mesg,1000,0,(struct sockaddr *)&cliaddr,&len);
// 	exit(0);
	



	// Unconcatenate GET Key01 and get just key01 and let that be mesg
	unconcatenate(mesg,my_key);
	
	
	printf("The Server 1 has received a request with key %s from client %d with port number %u and IP address 127.0.0.1",my_key,counter,((struct sockaddr_in*)&cliaddr)->sin_port);



	
	mesg[n] = '\0';
	
	//printf("Message received the Client:\n");
	//printf("Message= [%s] with length [%d]\n",mesg, strlen(mesg));
	

	given_a_key_get_corresponding_value(my_key,my_required_value);
//printf("my_required_value=%s\n",my_required_value);
      
	if (!strcmp(my_required_value, "NOT-AVAILABLE"))
	{
		/**********************************************************************************/
		/* TCP Server Communication */
		/* TCP Connection */
		   memset(&hints, 0, sizeof hints);
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;
			if ((rv = getaddrinfo("localhost", SRV2, &hints, &servinfo)) != 0) 
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

			/*Beej Code Reuse - End*/
			//to get the address details of my machine	
			if (getsockname(streamSockFd, (struct sockaddr *)&sin, &len) == -1)
			    perror("getsockname error");
		

	
		printf("The Server 1 sends the request %s to Server 2. The TCP port number is %s and the IP address is 127.0.0.1\n",mesg,SRV2,ip_server); 

        	/* This is going to be a TCP client communication */


		

    		//fgets(sendline, 10000,stdin) ;

		//Concatenating
		concat_key_2=concat_g(s1,my_key);
		printf("s1=%s, my_key=%s, mesg=%s\n",s1,my_key,mesg);
		
      		send(streamSockFd,mesg,strlen(mesg),0);
		memset(recvline,0,sizeof(recvline));
      		n=recv(streamSockFd,recvline,10000,0);
      		recvline[n]=0;
	//		printf("received value %s\n",recvline);
      		fputs(recvline,stdout);


		printf("The Server 1 received the value %s from the server 2 with port number %s and IP address 127.0.0.1\n",recvline,SRV2);
    		printf("The Server 1 closed the TCP connection with Server 2.");
		close(streamSockFd);
		concat_post_2=concat_p(s2,recvline);
				//strcat(s2,my_required_value);
			printf("The Server 1 sends the reply %s to Client %d with port number %u and IP address 127.0.0.1\n",concat_post_2,counter,((struct sockaddr_in*)&cliaddr)->sin_port);
			 	
	
	
	
			ret = sendto(sockfd,concat_post_2,15,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));

	
	
			if (ret == strlen(concat_post_2))
	
			printf("The Server 1, sent reply %s to Client %d with port number %d and IP address 127.0.0.1.",concat_post_2,counter,((struct sockaddr_in*)&cliaddr)->sin_port);

		/**********************************************************************************/
	}
	else
	{
		/* Send the data received back to the Client */


		//Concatenating the value obtained with 'POST' and sending that to the client 
		concat_post_2=concat_p(s2,my_required_value);
			//strcat(s2,my_required_value);
		printf("The Server 1 sends the reply %s to Client %d with port number %u and IP address 127.0.0.1\n",concat_post_2,counter,((struct sockaddr_in*)&cliaddr)->sin_port);
		 	
	
	
	
		ret = sendto(sockfd,concat_post_2,15,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));

	
	
		if (ret == strlen(concat_post_2))
			printf("The Server 1, sent reply %s to Client %d with port number %s and IP address 127.0.0.1",concat_post_2,counter,SRV1);
	}
    }
return 0;
}

