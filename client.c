#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<unistd.h>

// ----------MACROS----------
#define NUMBER_OF_LINEES_PER_PAGE 20
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
char** generateDataBlock(){
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
        // sleep(1);
    }
    
    return dataBuffer;
}



// ---------driver function-----------------

int main(int argc , char* argv[]){
    
    // creating data buffer in heap memory for read/write purposes

    char** dataBuffer= generateDataBlock();
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
    return 0;
}
