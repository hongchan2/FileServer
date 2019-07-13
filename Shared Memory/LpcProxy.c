#include "Lpc.h"
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PERMS 0666

void signalHandler(int signum);

int requestShmID_1;
int requestShmID_2;
int requestShmID_3;
int requestShmID_4;
int requestShmID_5;

int responseShmID_1;
int responseShmID_2;
int responseShmID_3;
int responseShmID_4;
int responseShmID_5;

char* requestShmAddr_1;
char* requestShmAddr_2;
char* requestShmAddr_3;
char* requestShmAddr_4;
char* requestShmAddr_5;

char* responseShmAddr_1;
char* responseShmAddr_2;
char* responseShmAddr_3;
char* responseShmAddr_4;
char* responseShmAddr_5;

sem_t* requestSem;

sem_t* responseSem_1;
sem_t* responseSem_2;
sem_t* responseSem_3;
sem_t* responseSem_4;
sem_t* responseSem_5;

int shmId;
int status = 0;

void Init(void)
{
    key_t reqKey1;
    key_t reqKey2;
    key_t reqKey3;
    key_t reqKey4;
    key_t reqKey5;

    key_t resKey1;
    key_t resKey2;
    key_t resKey3;
    key_t resKey4;
    key_t resKey5;

    reqKey1 = ftok("server.c", 1);
    reqKey2 = ftok("server.c", 2);
    reqKey3 = ftok("server.c", 3);
    reqKey4 = ftok("server.c", 4);
    reqKey5 = ftok("server.c", 5);

    resKey1 = ftok("Lpc.h", 1);
    resKey2 = ftok("Lpc.h", 2);
    resKey3 = ftok("Lpc.h", 3);
    resKey4 = ftok("Lpc.h", 4);
    resKey5 = ftok("Lpc.h", 5);

    requestShmID_1 = shmget(reqKey1, sizeof(LpcRequest) * 2, IPC_CREAT);
    requestShmID_2 = shmget(reqKey2, sizeof(LpcRequest) * 2, IPC_CREAT);
    requestShmID_3 = shmget(reqKey3, sizeof(LpcRequest) * 2, IPC_CREAT);
    requestShmID_4 = shmget(reqKey4, sizeof(LpcRequest) * 2, IPC_CREAT);
    requestShmID_5 = shmget(reqKey5, sizeof(LpcRequest) * 2, IPC_CREAT);
    
    responseShmID_1 = shmget(resKey1, sizeof(LpcResponse) * 2, IPC_CREAT);
    responseShmID_2 = shmget(resKey2, sizeof(LpcResponse) * 2, IPC_CREAT);
    responseShmID_3 = shmget(resKey3, sizeof(LpcResponse) * 2, IPC_CREAT);
    responseShmID_4 = shmget(resKey4, sizeof(LpcResponse) * 2, IPC_CREAT);
    responseShmID_5 = shmget(resKey5, sizeof(LpcResponse) * 2, IPC_CREAT);

    requestShmAddr_1 = shmat(requestShmID_1, NULL, 0);
    requestShmAddr_2 = shmat(requestShmID_2, NULL, 0);
    requestShmAddr_3 = shmat(requestShmID_3, NULL, 0);
    requestShmAddr_4 = shmat(requestShmID_4, NULL, 0);
    requestShmAddr_5 = shmat(requestShmID_5, NULL, 0);

    responseShmAddr_1 = shmat(responseShmID_1, NULL, 0);
    responseShmAddr_2 = shmat(responseShmID_2, NULL, 0);
    responseShmAddr_3 = shmat(responseShmID_3, NULL, 0);
    responseShmAddr_4 = shmat(responseShmID_4, NULL, 0);
    responseShmAddr_5 = shmat(responseShmID_5, NULL, 0);

    requestSem = sem_open("RequestSem1", O_CREAT, PERMS, 0);

    responseSem_1 = sem_open("ResponseSem1", O_CREAT, PERMS, 0);
    responseSem_2 = sem_open("ResponseSem2", O_CREAT, PERMS, 0);
    responseSem_3 = sem_open("ResponseSem3", O_CREAT, PERMS, 0);
    responseSem_4 = sem_open("ResponseSem4", O_CREAT, PERMS, 0);
    responseSem_5 = sem_open("ResponseSem5", O_CREAT, PERMS, 0);

    signal(SIGINT, signalHandler);
    signal(SIGSEGV, signalHandler);
}


int OpenFile(char* path, int flags)
{
    LpcRequest *pLpcRequest;
    LpcResponse *pLpcResponse;
    int responseData;

    if(status != 1){
        shmId = (getpid() % 5) + 1;
        status = 1;
    }

    pLpcRequest = (LpcRequest*)malloc(sizeof(LpcRequest));
    pLpcResponse = (LpcResponse*)malloc(sizeof(LpcResponse));

    memset(pLpcRequest, '\0', sizeof(LpcRequest));
    memset(pLpcResponse, '\0', sizeof(LpcResponse));

    pLpcRequest->valid = shmId;
    pLpcRequest->pid = getpid();
    pLpcRequest->service = LPC_OPEN_FILE;
    pLpcRequest->numArg = 2;
    pLpcRequest->lpcArgs[0].argSize = strlen(path);
    strncpy(pLpcRequest->lpcArgs[0].argData, path, strlen(path));
    pLpcRequest->lpcArgs[1].argSize = sizeof(int);
    memcpy(pLpcRequest->lpcArgs[1].argData, &flags, sizeof(int));

//printf("OPEN %d] Send : %s / %d\n", getpid(), pLpcRequest->lpcArgs[0].argData, flags);

    if(shmId == 1){
        memcpy(requestShmAddr_1, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_1);
        memcpy(pLpcResponse, responseShmAddr_1, sizeof(LpcResponse));
    }
    else if(shmId == 2){
        memcpy(requestShmAddr_2, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_2);
        memcpy(pLpcResponse, responseShmAddr_2, sizeof(LpcResponse));
    }
    else if(shmId == 3){
        memcpy(requestShmAddr_3, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_3);
        memcpy(pLpcResponse, responseShmAddr_3, sizeof(LpcResponse));
    }
    else if(shmId == 4){
        memcpy(requestShmAddr_4, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_4);
        memcpy(pLpcResponse, responseShmAddr_4, sizeof(LpcResponse));
    }
    else if(shmId == 5){
        memcpy(requestShmAddr_5, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_5);
        memcpy(pLpcResponse, responseShmAddr_5, sizeof(LpcResponse));
    }

    if(pLpcResponse->errorno == 0){
        memcpy(&responseData, pLpcResponse->responseData, sizeof(int));
//printf("OPEN %d] Recieve : %d\n", getpid(), responseData);

        free(pLpcRequest);
        free(pLpcResponse);

        return responseData;
    }
    else{
//printf("OPEN %d] Recieve : ERROR %d\n", getpid(), pLpcResponse->errorno);

        free(pLpcRequest);
        free(pLpcResponse);

        return pLpcResponse->errorno;
    }
}

int ReadFile(int fd, void* pBuf, int size)
{
    LpcRequest *pLpcRequest;
    LpcResponse *pLpcResponse;

    if(status != 1){
        shmId = (getpid() % 5) + 1;
        status = 1;
    }

    pLpcRequest = (LpcRequest*)malloc(sizeof(LpcRequest));
    pLpcResponse = (LpcResponse*)malloc(sizeof(LpcResponse));
    memset(pLpcRequest, '\0', sizeof(LpcRequest));
    memset(pLpcResponse, '\0', sizeof(LpcResponse));

    memset(pBuf, '\0', 256);

    pLpcRequest->valid = shmId;
    pLpcRequest->pid = getpid();
    pLpcRequest->service = LPC_READ_FILE;
    pLpcRequest->numArg = 2;
    pLpcRequest->lpcArgs[0].argSize = sizeof(int);
    memcpy(pLpcRequest->lpcArgs[0].argData, &fd, sizeof(int));
    pLpcRequest->lpcArgs[1].argSize = sizeof(int);
    memcpy(pLpcRequest->lpcArgs[1].argData, &size, sizeof(int));
//printf("READ %d] Send : %d / %d\n", getpid(), fd, size);

    if(shmId == 1){
        memcpy(requestShmAddr_1, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_1);
        memcpy(pLpcResponse, responseShmAddr_1, sizeof(LpcResponse));
    }
    else if(shmId == 2){
        memcpy(requestShmAddr_2, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_2);
        memcpy(pLpcResponse, responseShmAddr_2, sizeof(LpcResponse));
    }
    else if(shmId == 3){
        memcpy(requestShmAddr_3, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_3);
        memcpy(pLpcResponse, responseShmAddr_3, sizeof(LpcResponse));
    }
    else if(shmId == 4){
        memcpy(requestShmAddr_4, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_4);
        memcpy(pLpcResponse, responseShmAddr_4, sizeof(LpcResponse));
    }
    else if(shmId == 5){
        memcpy(requestShmAddr_5, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_5);
        memcpy(pLpcResponse, responseShmAddr_5, sizeof(LpcResponse));
    }

    if(pLpcResponse->errorno == 0){
        memcpy(pBuf, pLpcResponse->responseData, pLpcResponse->responseSize);
//printf("READ %d] Recieve : %d\n", getpid(), pLpcResponse->responseSize);

        free(pLpcRequest);
        free(pLpcResponse);

        return pLpcResponse->responseSize;
    }
    else{
//printf("READ %d] Recieve : ERROR %d\n", getpid(), pLpcResponse->errorno);
        free(pLpcRequest);
        free(pLpcResponse);

        return pLpcResponse->errorno;
    }
}


int WriteFile(int fd, void* pBuf, int size)
{
    LpcRequest *pLpcRequest;
    LpcResponse *pLpcResponse;
    int responseData;

    if(status != 1){
        shmId = (getpid() % 5) + 1;
        status = 1;
    }

    pLpcRequest = (LpcRequest*)malloc(sizeof(LpcRequest));
    pLpcResponse = (LpcResponse*)malloc(sizeof(LpcResponse));
    memset(pLpcRequest, '\0', sizeof(LpcRequest));
    memset(pLpcResponse, '\0', sizeof(LpcResponse));

    pLpcRequest->valid = shmId;
    pLpcRequest->pid = getpid();
    pLpcRequest->service = LPC_WRITE_FILE;
    pLpcRequest->numArg = 3;
    pLpcRequest->lpcArgs[0].argSize = sizeof(int);
    memcpy(pLpcRequest->lpcArgs[0].argData, &fd, sizeof(int));
    pLpcRequest->lpcArgs[1].argSize = sizeof(pBuf);
    strncpy(pLpcRequest->lpcArgs[1].argData, pBuf, strlen(pBuf));
    pLpcRequest->lpcArgs[2].argSize = sizeof(int);
    memcpy(pLpcRequest->lpcArgs[2].argData, &size, sizeof(int));

//printf("WRITE %d] Send : %d / %s / %d\n", getpid(), fd, pLpcRequest->lpcArgs[1].argData, size);

    if(shmId == 1){
        memcpy(requestShmAddr_1, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_1);
        memcpy(pLpcResponse, responseShmAddr_1, sizeof(LpcResponse));
    }
    else if(shmId == 2){
        memcpy(requestShmAddr_2, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_2);
        memcpy(pLpcResponse, responseShmAddr_2, sizeof(LpcResponse));
    }
    else if(shmId == 3){
        memcpy(requestShmAddr_3, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_3);
        memcpy(pLpcResponse, responseShmAddr_3, sizeof(LpcResponse));
    }
    else if(shmId == 4){
        memcpy(requestShmAddr_4, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_4);
        memcpy(pLpcResponse, responseShmAddr_4, sizeof(LpcResponse));
    }
    else if(shmId == 5){
        memcpy(requestShmAddr_5, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_5);
        memcpy(pLpcResponse, responseShmAddr_5, sizeof(LpcResponse));
    }

    if(pLpcResponse->errorno == 0){
        memcpy(&responseData, pLpcResponse->responseData, pLpcResponse->responseSize);

//printf("WRITE %d] Recieve : %d\n", getpid(), responseData);
        free(pLpcRequest);
        free(pLpcResponse);

        return responseData;
    }
    else{
//printf("WRITE %d] Recieve : ERROR %d\n", getpid(), pLpcResponse->errorno);
        free(pLpcRequest);
        free(pLpcResponse);

        return pLpcResponse->errorno;
    }
}


int CloseFile(int fd)
{
    LpcRequest *pLpcRequest;
    LpcResponse *pLpcResponse;
    int responseData;

    if(status != 1){
        shmId = (getpid() % 5) + 1;
        status = 1;
    }

    pLpcRequest = (LpcRequest*)malloc(sizeof(LpcRequest));
    pLpcResponse = (LpcResponse*)malloc(sizeof(LpcResponse));
    memset(pLpcRequest, '\0', sizeof(LpcRequest));
    memset(pLpcResponse, '\0', sizeof(LpcResponse));

    pLpcRequest->valid = shmId;
    pLpcRequest->pid = getpid();
    pLpcRequest->service = LPC_CLOSE_FILE;
    pLpcRequest->numArg = 1;
    pLpcRequest->lpcArgs[0].argSize = sizeof(int);
    memcpy(pLpcRequest->lpcArgs[0].argData, &fd, sizeof(int));

//printf("CLOSE %d] Send : %d\n", getpid(), fd);
    if(shmId == 1){
        memcpy(requestShmAddr_1, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_1);
        memcpy(pLpcResponse, responseShmAddr_1, sizeof(LpcResponse));
    }
    else if(shmId == 2){
        memcpy(requestShmAddr_2, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_2);
        memcpy(pLpcResponse, responseShmAddr_2, sizeof(LpcResponse));
    }
    else if(shmId == 3){
        memcpy(requestShmAddr_3, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_3);
        memcpy(pLpcResponse, responseShmAddr_3, sizeof(LpcResponse));
    }
    else if(shmId == 4){
        memcpy(requestShmAddr_4, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_4);
        memcpy(pLpcResponse, responseShmAddr_4, sizeof(LpcResponse));
    }
    else if(shmId == 5){
        memcpy(requestShmAddr_5, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_5);
        memcpy(pLpcResponse, responseShmAddr_5, sizeof(LpcResponse));
    }

    if(pLpcResponse->errorno == 0){
        memcpy(&responseData, pLpcResponse->responseData, pLpcResponse->responseSize);
//printf("CLOSE %d] Recieve : %d\n", getpid(), responseData);

        free(pLpcRequest);
        free(pLpcResponse);

        return responseData;
    }
    else{
        free(pLpcRequest);
        free(pLpcResponse);
//printf("CLOSE %d] Recieve : ERROR %d\n", getpid(), pLpcResponse->errorno);

        return pLpcResponse->errorno;
    }
}

int MakeDirectory(char* path, int mode)
{   
    LpcRequest *pLpcRequest;
    LpcResponse *pLpcResponse;
    int responseData;

    if(status != 1){
        shmId = (getpid() % 5) + 1;
        status = 1;
    }
    
    pLpcRequest = (LpcRequest*)malloc(sizeof(LpcRequest));
    pLpcResponse = (LpcResponse*)malloc(sizeof(LpcResponse));
    memset(pLpcRequest, '\0', sizeof(LpcRequest));
    memset(pLpcResponse, '\0', sizeof(LpcResponse));

    pLpcRequest->valid = shmId;
    pLpcRequest->pid = getpid();
    pLpcRequest->service = LPC_MAKE_DIRECTORY;
    pLpcRequest->numArg = 2;
    pLpcRequest->lpcArgs[0].argSize = strlen(path);
    strncpy(pLpcRequest->lpcArgs[0].argData, path, strlen(path));
    pLpcRequest->lpcArgs[1].argSize = sizeof(int);
    memcpy(pLpcRequest->lpcArgs[1].argData, &mode, sizeof(int));

    if(shmId == 1){
        memcpy(requestShmAddr_1, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_1);
        memcpy(pLpcResponse, responseShmAddr_1, sizeof(LpcResponse));
    }
    else if(shmId == 2){
        memcpy(requestShmAddr_2, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_2);
        memcpy(pLpcResponse, responseShmAddr_2, sizeof(LpcResponse));
    }
    else if(shmId == 3){
        memcpy(requestShmAddr_3, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_3);
        memcpy(pLpcResponse, responseShmAddr_3, sizeof(LpcResponse));
    }
    else if(shmId == 4){
        memcpy(requestShmAddr_4, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_4);
        memcpy(pLpcResponse, responseShmAddr_4, sizeof(LpcResponse));
    }
    else if(shmId == 5){
        memcpy(requestShmAddr_5, pLpcRequest, sizeof(LpcRequest));
        sem_post(requestSem);

        sem_wait(responseSem_5);
        memcpy(pLpcResponse, responseShmAddr_5, sizeof(LpcResponse));
    }

    if(pLpcResponse->errorno == 0){
        memcpy(&responseData, pLpcResponse->responseData, pLpcResponse->responseSize);

        free(pLpcRequest);
        free(pLpcResponse);

        return responseData;
    }
    else{
        free(pLpcRequest);
        free(pLpcResponse);

        return pLpcResponse->errorno;
    }
}

int GetString(void* (*callback)(char*))
{
}

void signalHandler(int signum){
    shmdt(requestShmAddr_1);
    shmdt(requestShmAddr_2);
    shmdt(requestShmAddr_3);
    shmdt(requestShmAddr_4);
    shmdt(requestShmAddr_5);

    shmdt(responseShmAddr_1);
    shmdt(responseShmAddr_2);
    shmdt(responseShmAddr_3);
    shmdt(responseShmAddr_4);
    shmdt(responseShmAddr_5);

    sem_close(requestSem);

    sem_close(responseSem_1);
    sem_close(responseSem_2);
    sem_close(responseSem_3);
    sem_close(responseSem_4);
    sem_close(responseSem_5);

    exit(-1);
}
