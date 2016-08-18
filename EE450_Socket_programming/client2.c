/* Sample UDP client */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include<strings.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define MAX_ARY 50
char* given_a_input_give_my_key_token(char str[64])
{

	
    int i=0;
    int numProgs=0;
    char* lines[MAX_ARY];
    char line[40];
    int j;
    char search_key[1000];
    int position=6, length=5, c = 0;
    
    int ret=0;
    char* token;
    char* my_key;
    //strcpy(temp,str);

    FILE *file;
    file = fopen("client1.txt", "r");
    if(file==NULL)
	{
	perror("fopen fail");
	exit(1);
	}

	

    while(fgets(line, sizeof line, file)!=NULL)
    {
     
     lines[i]=malloc(sizeof(line));
     strcpy(lines[i],line);
     i++;
     numProgs++;
    }
	
    //check to be sure going into array correctly 
   /* for (j=0 ;j<numProgs+1;j++)
    {
      //printf("%s", lines[j])
      ;
    }*/
     
   
printf("Please enter your search(USC,UCLA etc.):");
scanf("%s",str);




for(i = 0; i<numProgs; i++)
{
        // Gettng the univ names from the list
        token = strtok(lines[i]," ");
        while( token != NULL )
       {
                ret=strcmp(str, token);
                token = strtok(NULL," ");

                if(ret==0)
                {
                  break;
                }
        }
        if (ret == 0)
                break;
}


if(token == NULL)
	token = "NA";


fclose(file);
return token;

}

#define UDP_PORT 21768


/* Getting the IP address of server 1 and client */


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
char* concat(const char *s1,char *my_required_key)
{
    char *result = malloc(strlen(s1)+strlen(my_required_key)+1);
    strcpy(result, s1);
    strcat(result, my_required_key);
    return result;
}


//Unconcatenate the strings
void unconcatenate(char recvline[1000],char temp[8]) 
{
   char sub[1000];
   int position=5;
   int length=7;
   int c = 0;
 		
   while (c < length+1) {
      sub[c] = recvline[position+c];
	c++;
   }
   sub[c] ='\0';
	strcpy(temp,sub);
	
 
   //printf("Required substring is%s\n", sub);
   //printf("sub=%s\n",sub);
   //return sub;
   
}

int main(int argc, char**argv)
{
   int sockfd,n;	
	struct sockaddr_in servaddr;
	char recvline[1000];
	char* my_required_key;	
	int ret = 0;
	struct sockaddr_in sin;
  	socklen_t len = sizeof(sin);
	char* cliaddr;
	char* univ_name;
	int servaddr_len;
        int getsock_check;
        int dynamic_port_number;
        char *server_s1_name;
   	char *client_name;
   	char ip_client[30];
   	char ip_server[30];
        char str[10]="name";
	char *concat_key;
	const char *s1="GET ";
	char *my_val;
	char temp[8];


   /********************************************************/
        // Getting the ip address for client
	client_name = "localhost";
	//hostname = argv[1];
	hostname_to_ip1(client_name, ip_client);
	//printf("Client IP Address = %s\n", ip_client);
   /********************************************************/
   /********************************************************/
	// Getting the ip address of nunki.usc.edu
	//server_s1_name = "nunki.usc.edu";
	server_s1_name = "localhost";
	//hostname = argv[1];
	hostname_to_ip1(server_s1_name, ip_server);
	//printf("Server S1 IP Address = %s\n", ip_server);
   /********************************************************/

   
/*
   if (argc != 2)
   {
      printf("usage:  udpcli <IP address>\n");
      exit(1);
   }
*/

   /*UDP Connection*/
   sockfd=socket(AF_INET,SOCK_DGRAM,0);
   bzero(&servaddr,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=inet_addr(ip_server);
   servaddr.sin_port=htons(UDP_PORT);

   bind(sockfd,(struct sockaddr *)&servaddr, sizeof servaddr);

	// call your read_file routine here
	my_required_key = given_a_input_give_my_key_token(str);
	if (my_required_key!= NULL)

   	printf("The Client 1 has received a request with search word %s which maps to key %s",str,my_required_key);

	else
	{
		
		exit(0);
	}
	
 	

	//Concatenating
	concat_key=concat(s1,my_required_key);

	
      
	ret = sendto(sockfd,concat_key,strlen(concat_key),0, (struct sockaddr *)&servaddr,sizeof(servaddr));
	
	getsockname(sockfd, (struct sockaddr *)&sin, &len);
	if (ret == strlen(concat_key))
	{
		
 		printf("The Client 1 sends the request %s to the Server 1 with port number %d and IP address %s.\n",concat_key,UDP_PORT,ip_server);

		printf("The Client 1's port number is %d and the IP address is %s\n",ntohs(sin.sin_port),ip_server);
	
	}

	//Here the receive line should have the word POST

        n=recvfrom(sockfd,recvline,50,0,NULL,NULL);
	

	//Unconcatenate POST and value
	unconcatenate(recvline,temp);
	

	printf("The Client 1 received the value %s from the Server 1 with port number %d and IP address %s.\n",temp,UDP_PORT,ip_server);

	printf("The Client 1's port number is %d and IP address is %s.\n",ntohs(sin.sin_port),ip_server);

	
//      recvline[n]=0;
//      fputs(recvline,stdout);
      
   
}


