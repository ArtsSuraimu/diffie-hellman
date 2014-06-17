/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


/*
 * TCP RC4 Encrypted Communication Client
 * Author: Clemens Jonischkeit, Dai Yang
 * Chair of Network Architechtures and Network Services
 * Faculty of Computer Science
 * Technical University Munich
 */



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdint.h>
#include <string.h>

#include "rc4.h"
#include "fastexp.h"


#define _VERSION_ "2.3"
/* This is the maximum message length  - inrease if necessary */
#define MAXMSGLEN 1024

/* Global variable to control command line options. */
struct {
 	char 	 	*target;
   	unsigned short 	port;
  	int  	 	af;
} options;

/* Prints the supplied error message to stderr and terminates the program. */
static void
error(char *msg)
{
   	fprintf(stderr, "%s\n", msg);
  	exit(-errno);
}

/* Prints the help output and terminates the program. */
static void
printUsage(char *name)
{
 	fprintf(stderr, "Usage: %s -t <target> -p <port> -<4|6>\n\n", name);
 	fprintf(stderr, "       -t: DNS name of the server\n");
 	fprintf(stderr, "       -p: Port the server is listening on\n");
   	fprintf(stderr, "       -4: Use IPv4\n");
  	fprintf(stderr, "       -6: Use IPv6\n\n");
  	exit(-EINVAL);
}

/* Draw a 64bit random unsigned integer from the system's entropy pool. */
static uint64_t
getRandom()
{
 	uint64_t rand;
   	FILE *f;
    
  	if (!(f = fopen("/dev/urandom", "rb")))
 	 	error("Cannot open \"/dev/urandom\".");
 	if (fread(&rand, sizeof(uint64_t), 1, f) < 1)
   	  	error("Cannot read from \"/dev/urandom\".");
  	fclose(f);
 	return rand;
}

/* Parse the command line arguments. Returns 0 on success and -1 on failure. */
static int
parse_args(int argc, char **argv)
{
   	char c;
    
  	memset(&options,0,sizeof(options));
    
 	while (-1 != (c = getopt(argc,argv,"t:p:46h"))) {
 	 	switch(c) {
            case 't':
                options.target = optarg;
                break;
            case 'p':
                options.port = atoi(optarg);
                break;
            case '4':
                options.af = AF_INET;
                break;
            case '6':
                options.af = AF_INET6;
                break;
            case 'h':
                return -1;
            default:
                abort();
 	 	}
 	}
    
   	if (NULL == options.target || options.port == 0 ||
  	  	(options.af != AF_INET && options.af != AF_INET6))
 	   	return -1;
    
  	return 0;
}

/* Converts the target's DNS name given as character array pointed to by target
 into a struct in_addr binary representation in network byte order pointed to
 by addr. Returns 0 on succes and -1 on error. */
static int
getTargetIp(const char *target, void *dst, int af)
{
 	struct hostent *he;
 	char **addr_list;
 	int i;
    
   	if (NULL == (he=gethostbyname2(target,af))) {
  	  	herror("gethostbyname2() failed");
 	   	return 1;
  	}
    
 	addr_list = he->h_addr_list;
    
 	/* may be the target has multiple ips, return the first one */
 	for (i=0; addr_list[i]!=NULL; i++) {
   	  	memcpy(dst,addr_list[i],he->h_length);
  	 	return 0;
   	}
    
  	return -1;
}

/* This is the function most of the work is done. */
static void
communicate(int sock)
{
 	
    char *hello = "HELLO";
    char buffer[MAXMSGLEN];
    int recieved;
    
    uint64_t g;
    uint64_t p;
    uint64_t c;
    c = getRandom();
    
    
    send(sock, hello, strlen(hello)+1,0);
    memset(buffer,0,sizeof(buffer));
    recv(sock, buffer, MAXMSGLEN-1,0);
    sscanf(buffer,"HELLO %llu,%llu",&g,&p);
    printf("Client received: g: %llu, p: %llu\n",g,p);
    //Greetings, we recevied here Information about g, p.
    
    uint64_t C ;
    C = fastexp(g,c,p);
    memset(buffer,0,sizeof(buffer));
    sprintf(buffer,"%llu",C);
    send(sock, buffer, strlen(buffer)+1,0);
    printf("Client sent: C: %llu\n",C);
    //Send Information about C.
    
    memset(buffer,0,sizeof(buffer));
    recv(sock, buffer, MAXMSGLEN-1,0);
    uint64_t S;
    sscanf(buffer, "%llu", &S);
    printf("Client received: S: %llu\n",S);
    //Received information about S
    
    uint64_t K ;
    K= fastexp(S,c,p);
    if(rc4_init(K)!=0){
        printf("Cannot init RC4 Encryption");
        exit(1);
    }
    //Initialize the encryption
    unsigned char msg[] = "EncOK";
    rc4_crypt(&msg, strlen(msg));
    send(sock, &msg,strlen(msg),0);
    printf("Client sent (encrypted): EncOK\n");
    //Send EncOK
    
    memset(buffer,0,sizeof(buffer));
    recieved= recv(sock, buffer,MAXMSGLEN-1 ,0);
    rc4_crypt(&buffer,recieved);
    printf("Client recived (decrypted): %s\n", buffer);
    //Receive EncOk
    
    
    memset(buffer,0,sizeof(buffer));
    char msg2[] = "team128, Please notice that this notice is not worth to notice!";
    printf("Client sending: %s\n", msg2);
    rc4_crypt(&msg2,strlen(msg2));
    send(sock, &msg2, strlen(msg2),0);
    //Send final message
    
    memset(buffer,0,sizeof(buffer));
    recieved = recv(sock, buffer,MAXMSGLEN-1 ,0);
    rc4_crypt(&buffer,recieved);
    printf("Client recived (decrypted): %s\n", buffer);
    //Recieve last Message and clean up
    
    rc4_clean();
    
}

int main(int argc, char **argv)
{
    if (argc!=6)
    {
        printUsage(argv[0]);
        exit(1);
    }
    
    if (parse_args(argc, argv)==-1)
    {
        printUsage(argv[0]);
        exit(1);
    }
    //Get arguments
    
    
    if (options.af==AF_INET){
        struct sockaddr_in sa_srv;
        if (getTargetIp(options.target,&sa_srv.sin_addr.s_addr,options.af)!=0){
            printf("Cannot get the IP address. \n");
            exit(1);
        }
        sa_srv.sin_family = options.af;
        sa_srv.sin_port = htons(options.port);
        int sd;
        if(0>(sd=socket(options.af,SOCK_STREAM,IPPROTO_TCP))){
            perror("socket() failed");
            exit(1);
        }
        
        if(0>connect(sd,(struct sockaddr *)&sa_srv,sizeof(sa_srv))){
            perror("connect() failed");
            exit(1);
        }
        
        
        communicate(sd);
        
        close(sd);
        
    }   //This Part start a standard IPv4 TCP Connection;
    else
    {
        struct sockaddr_in6 sa_srv;
        if (getTargetIp(options.target,&sa_srv.sin6_addr.s6_addr,options.af)!=0){
            printf("Cannot get the IP address. \n");
            exit(1);
        }
        sa_srv.sin6_family = options.af;
        sa_srv.sin6_port = htons(options.port);
        int sd;
        if(0>(sd=socket(options.af,SOCK_STREAM,IPPROTO_TCP))){
            perror("socket() failed");
            exit(1);
        }
        
        if(0>connect(sd,(struct sockaddr *)&sa_srv,sizeof(sa_srv))){
            perror("connect() failed");
            exit(1);
        }
        
        
        communicate(sd);
        
        close(sd);
    } //This part uses a IPv6 Connection (TCP) to communicate.
    
  	return 0;
}
