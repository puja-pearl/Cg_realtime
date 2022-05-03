
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

#define PRIMARY_SERVER_PORT 8080
#define ALTERNATIVE_SERVER_PORT 8000
#define LOCAL_IP_ADDR "127.0.0.1"

// --------------------MACROS----------------------------------
#define NUMBER_OF_LINEES_PER_PAGE 3
// -----------record mantinance functions----------------------
int fileRecordGenerator(char **dataBuffer){
    FILE *fileBuffer;
    if((fileBuffer=fopen("clientFileBuffer","a+"))!=NULL){
        for(int i=0;i<NUMBER_OF_LINEES_PER_PAGE;i++){
        fputs(dataBuffer[i],fileBuffer);
        }
        fclose(fileBuffer);
        return 0;
    } else{
        perror("ERR! [opening record file.]");
        // fatal level log
        return -1;
    }
    

}

// ------------client dataFactory functions---------------

int randomNumberGenerator(int seed){
    // initializaion of time seed
    if(!seed){
        srand(time(0));
    } else{
        srand(seed);
    }
    return (rand()%900)+100; //gentrating 3 digit number my mod 1000 ie. pow(10,3);
}
char** createDataBuffer(int size_x){
    int size_y=50;
    char **dataBuffer=(char **)malloc(sizeof(char*)*size_x);
    for (int i=0;i<size_x;i++){
        dataBuffer[i]=(char *)malloc(size_y*sizeof(char));
    }
    return dataBuffer;
}

char** generateDataBlock(int socked_fd){
    char** dataBuffer=createDataBuffer(NUMBER_OF_LINEES_PER_PAGE);
    time_t ltime;
    struct tm res;
    char TIMESTAMP[32];
    int randNum;
    for(int i=0;i<NUMBER_OF_LINEES_PER_PAGE;i++){
        srand(time(NULL));
        int seed;
        if(i==0){
            seed = 0;
        } else {
            seed = randNum;
        }
        randNum=randomNumberGenerator(seed);
        ltime = time(NULL); // creating timestamps as per local system time
        localtime_r(&ltime,&res);
        asctime_r(&res,TIMESTAMP); // conv timestamp into ascii (human readable format)
        char* currTime=strtok(TIMESTAMP,"\n");
        sprintf(dataBuffer[i],"[%s - ( %d )] \n",currTime,randNum);
        sendDataToServer(socked_fd,dataBuffer[i]);
        sleep(1);
    }
    
    return dataBuffer;
}


void sendDataToServer(int socket_fd,char* line){
    send(socket_fd,line,50,0);
    return;
}

int main(int argc, char const* argv[])
{
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	char buffer[1024] = { 0 },yn;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PRIMARY_SERVER_PORT);
	if (inet_pton(AF_INET, LOCAL_IP_ADDR, &serv_addr.sin_addr)
		<= 0) {
		printf(
			"\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(sock, (struct sockaddr*)&serv_addr,
				sizeof(serv_addr))
		< 0) {
		printf("\nConnection Failed \n");
		return -1;
	}

    char** dataBuffer= generateDataBlock(sock);
    while(1){
        for(int i=0;i<NUMBER_OF_LINEES_PER_PAGE;i++){
        printf("%s",dataBuffer[i]);
        }
        if(fileRecordGenerator(dataBuffer)>=0){
            printf("record holder file written successfully!\n");
        } else{
            printf("unable to write record file\n");
        }
        // freeing up allocated heap memory
        for(int i=0;i<NUMBER_OF_LINEES_PER_PAGE;i++){
            free(dataBuffer[i]);
        }
        printf("wanna continue ? ");
        scanf("%c",&yn);
        printf("response : %c",yn);
        if(yn=='n'){
            send(sock,"killServer\0",10,0);
            return 0;
        }
    }
    return 0;

}
