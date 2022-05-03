#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<stdbool.h>

#define PORT 8080
typedef struct serverInfo{
    char serverIp[16];
    int serverPort;
    int maxClientLimit;
} serverInfo;

int createServerSocket(void){
    printf("[+] server socket created!\n");
    return socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
}
serviceLogger(char* name,char* msg){
    char logFileName[]="serviceLogs";
    FILE* log;
    time_t ltime;
    struct tm res;
    char TIMESTAMP[32];
    char buffer[50];
    ltime = time(NULL);
    localtime_r(&ltime,&res);
    asctime_r(&res,TIMESTAMP);
    char* currTime=strtok(TIMESTAMP,"\n");
    if((log=fopen(logFileName,"a+"))==NULL){
        // unable to open logfile
    }
    sprintf(buffer,"[%s\t@%s]=\t%s\n",name,currTime,msg);
    fputs(buffer,log);
    fclose(log);
}
checkServerStatus(char* serverIp,int serverPort,int userBit){
    // userBit 1 for slave
    // userbit 1 for master
    int socket_fd;
    struct sockaddr_in server_addr;
    if((socket_fd = socket(AF_INET,SOCK_STREAM,0))<0){
        // printf("error in creating socket\n")
        return -1;
    } 
    server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(serverPort);
	if (inet_pton(AF_INET, serverIp, &server_addr.sin_addr)
		<= 0) {
		printf(
			"\nInvalid address/ Address not supported \n");
		return -1;
	}
    if (connect(socket_fd, (struct sockaddr*)&server_addr,
				sizeof(server_addr))
		< 0) {
		if(userBit){
            // log needed slave is dead
            serviceLogger("slave","master is dead");
            
        } else {
            serviceLogger("master","slave is dead");
        }
		return -1;
	} else{
        // log needed both are working fine
        serviceLogger("master","master is working fine");
        close(socket_fd);
        // clean memory
        return 0;
    }

}


void serverInitiation(void){
    int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = { 0 };
	
	if ((server_fd =createServerSocket())
		== 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 
	if (setsockopt(server_fd, SOL_SOCKET,
				SO_REUSEADDR | SO_REUSEPORT, &opt,
				sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// Forcefully attaching socket to the port 
	if (bind(server_fd, (struct sockaddr*)&address,
			sizeof(address))
		< 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
    printf("[+]server is ready to work on PORT=%d\n",PORT);
    if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
    }
    printf("[+]server listening ...\n");

    // ------------------- mainline fork pont -----------------------
    int p_id = fork();
    if(p_id==0){
        // execution line one 
            if ((new_socket
        = accept(server_fd, (struct sockaddr*)&address,
                (socklen_t*)&addrlen))
        < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
        }   else {
            printf("[+]client connected to the server...\n");
        }
        return 0;
    } else {
        // execution line two
        checkServerStatus("127.0.0.1",5000,0);
    }

    
}
int main(int argc, char* argv[]){
    
    serverInitiation();
    
}



