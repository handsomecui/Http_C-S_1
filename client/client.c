#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define N 1024000

typedef struct sockaddr SA;


void PrintHelp()
{
    printf("help : display help info\n");
    printf("list : get file list of server\n");
    printf("get  : get <file>\n");
    printf("put  : put <file>\n");
    printf("quit : quit the client\n");

    return;
}


void ProcessList(struct sockaddr_in server_addr)
{
     int sockfd, nbyte;
    char buf[N];

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
         printf("fail to list\n");
        return;
    }

    if (connect(sockfd, (SA *)&server_addr, sizeof(server_addr)) < 0)
    {
      printf("fail to connect server\n");
        goto ERROR_1;
    }

    strcpy(buf, "L");
    send(sockfd, buf, N, 0);
    
    while ((nbyte = recv(sockfd, buf, N, 0)) != 0)
    {
        printf("%s\n", buf);
    }

ERROR_1:
    close(sockfd);
   
    return;
}


void ProcessGet(struct sockaddr_in server_addr, char command[])
{
    int sockfd, nbyte, fd;
    char buf[N];

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
       printf("fail to get\n");
        return;
    }

    if (connect(sockfd, (SA *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("fail to connect server\n");
        goto ERROR_2;
       
   }

    sprintf(buf, "G%s", command+4);
    send(sockfd, buf, N, 0);
    recv(sockfd, buf, N, 0);
    
    if (buf[0] == 'N') // no such file
    {
        printf("No such file on server\n");
        goto ERROR_2;
    }

    if ((fd = open(command+4, O_WRONLY|O_CREAT|O_TRUNC, 0666)) < 0)
    {
       printf("fail to create local file %s\n", command+4);
        goto ERROR_2;
    }
    
    while ((nbyte = recv(sockfd, buf, N, 0)) > 0)
    {
        write(fd, buf, nbyte);
    }
    close(fd);

ERROR_2:
    close(sockfd);

    return;
}


void ProcessPut(struct sockaddr_in server_addr, char command[])
{
    int sockfd, nbyte, fp;
    char buf[N];

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
       printf("fail to get\n");
        return;
    }

    if (connect(sockfd, (SA *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("fail to connect server\n");
        goto ERROR_3;
       
    }

    if ((fp = open(command+4, O_RDONLY)) < 0)
    {
        printf("fail to open %s\n", command+4);
        goto ERROR_3;
    }      
    int File_length;
    //fseek(fp, 0, 2);//2 is FILE END
    //File_length = ftell(fp);
    if((File_length = lseek(fp, 0, SEEK_END)) < 0){
	perror("lseek file failure!");
    }
    lseek(fp, 0, SEEK_SET);
    sprintf(buf, "P%d:%s", File_length, command+4);
    send(sockfd, buf, strlen(buf), 0);
    while (1)
    {
	nbyte = read(fp, buf, 1024);
	//printf("nbyte = %d\n", nbyte);
	if(nbyte <= 0)break;
        send(sockfd, buf, nbyte, 0);
    }
    close(fp);

ERROR_3:
    close(sockfd);

    return;
}


int main(int argc, char *argv[])
{
    int sockfd, fd, nbyte;
    char command[32];
    struct sockaddr_in server_addr;

   if (argc < 3)
    {
        printf("Usage : %s <server_ip> : <port>\n", argv[0]);
        exit(-1);
    }    
 
    // XXX:step 1  int socket(int domain, int type, int protocol);
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "fail to socket : %s\n", strerror(errno));
        exit(-1);
    }

#ifdef _DEBUG_
   printf("socket is %d\n", sockfd);
#endif  

  // XXX:step 2  int connect(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
   //server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
 
    
 
    while ( 1 )
    {
        printf("<client> ");
	memset(command, 0, sizeof(command));
        fgets(command, 32, stdin);
        command[strlen(command)-1] = '\0';  // overwrite the '\n'
        
        if (strcmp(command, "help") == 0)
        {
            PrintHelp();
        }
        else if (strcmp(command, "list") == 0)
        {
            ProcessList(server_addr);
        }
        else if (strncmp(command, "get ", 4) == 0)
        {
            ProcessGet(server_addr, command);
        }
        else if (strncmp(command, "put ", 4) == 0)
        {
            ProcessPut(server_addr, command);
        }
        else if (strcmp(command, "quit") == 0)
        {
            printf("Bye\n");
            break;
        }
        else
        {
            printf("wrong command, 'help' for command list\n");
        }
    }

   return 0;
}

