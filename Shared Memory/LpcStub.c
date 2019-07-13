#include "Lpc.h"
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PERMS 0666

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

void signalHandler(int signum);

void Init(void)
{
    // create shm, sem
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

    requestShmID_1 = shmget(reqKey1, sizeof(LpcRequest) * 2, IPC_CREAT | PERMS);
    requestShmID_2 = shmget(reqKey2, sizeof(LpcRequest) * 2, IPC_CREAT | PERMS);
    requestShmID_3 = shmget(reqKey3, sizeof(LpcRequest) * 2, IPC_CREAT | PERMS);
    requestShmID_4 = shmget(reqKey4, sizeof(LpcRequest) * 2, IPC_CREAT | PERMS);
    requestShmID_5 = shmget(reqKey5, sizeof(LpcRequest) * 2, IPC_CREAT | PERMS);
    
    responseShmID_1 = shmget(resKey1, sizeof(LpcResponse) * 2, IPC_CREAT | PERMS);
    responseShmID_2 = shmget(resKey2, sizeof(LpcResponse) * 2, IPC_CREAT | PERMS);
    responseShmID_3 = shmget(resKey3, sizeof(LpcResponse) * 2, IPC_CREAT | PERMS);
    responseShmID_4 = shmget(resKey4, sizeof(LpcResponse) * 2, IPC_CREAT | PERMS);
    responseShmID_5 = shmget(resKey5, sizeof(LpcResponse) * 2, IPC_CREAT | PERMS);

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


int OpenFile(LpcRequest* pRequest)
{
    LpcResponse *pLpcResponse;
    char *arg1;
    int argInt1;
    int fd;

    pLpcResponse = (LpcResponse*)malloc(sizeof(LpcResponse));
    arg1 = (char*)malloc(LPC_DATA_MAX);

    memset(pLpcResponse, 0x00, sizeof(LpcResponse));
    memset(arg1, 0x00, LPC_DATA_MAX);

    memcpy(arg1, pRequest->lpcArgs[0].argData, pRequest->lpcArgs[0].argSize);
    memcpy(&argInt1, pRequest->lpcArgs[1].argData, pRequest->lpcArgs[1].argSize);

    fd = open(arg1, argInt1, 0666);
    if(fd == -1)
        pLpcResponse->errorno = -1;
    else
        pLpcResponse->errorno = 0;

    pLpcResponse->pid = pRequest->pid;
    pLpcResponse->responseSize = sizeof(int);
    memcpy(pLpcResponse->responseData, &fd, sizeof(int));

    if(pRequest->valid == 1){
        memcpy(responseShmAddr_1, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_1);
    }
    else if(pRequest->valid == 2){
        memcpy(responseShmAddr_2, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_2);
    }
    else if(pRequest->valid == 3){
        memcpy(responseShmAddr_3, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_3);
    }
    else if(pRequest->valid == 4){
        memcpy(responseShmAddr_4, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_4);
    }
    else if(pRequest->valid == 5){
        memcpy(responseShmAddr_5, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_5);
    }

    free(pLpcResponse);
    free(arg1);
}

int ReadFile(LpcRequest* pRequest)
{
    LpcResponse *pLpcResponse;
    int argInt1;
    int argInt2;
    ssize_t rsize;

    pLpcResponse = (LpcResponse*)malloc(sizeof(LpcResponse));

    memset(pLpcResponse, 0x00, sizeof(LpcResponse));

    memcpy(&argInt1, pRequest->lpcArgs[0].argData, pRequest->lpcArgs[0].argSize);
    memcpy(&argInt2, pRequest->lpcArgs[1].argData, pRequest->lpcArgs[1].argSize);
    
    rsize = read(argInt1, pLpcResponse->responseData, argInt2);

    if(rsize == -1)
        pLpcResponse->errorno = -1;
    else
        pLpcResponse->errorno = 0;
    
    pLpcResponse->pid = pRequest->pid;
    pLpcResponse->responseSize = (int)rsize;

    if(pRequest->valid == 1){
        memcpy(responseShmAddr_1, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_1);
    }
    else if(pRequest->valid == 2){
        memcpy(responseShmAddr_2, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_2);
    }
    else if(pRequest->valid == 3){
        memcpy(responseShmAddr_3, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_3);
    }
    else if(pRequest->valid == 4){
        memcpy(responseShmAddr_4, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_4);
    }
    else if(pRequest->valid == 5){
        memcpy(responseShmAddr_5, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_5);
    }

    free(pLpcResponse);
}


int WriteFile(LpcRequest* pRequest)
{
    LpcResponse *pLpcResponse;
    int argInt1;
    int argInt2;
    char* arg1;
    ssize_t wsize;
    int int_wsize;

    pLpcResponse = (LpcResponse*)malloc(sizeof(LpcResponse));
    arg1 = (char*)malloc(LPC_DATA_MAX); // set mem

    memset(pLpcResponse, 0x00, sizeof(LpcResponse));
    memset(arg1, 0x00, LPC_DATA_MAX);

    memcpy(&argInt1, pRequest->lpcArgs[0].argData, pRequest->lpcArgs[0].argSize);
    strncpy(arg1, pRequest->lpcArgs[1].argData, strlen(pRequest->lpcArgs[1].argData));
    memcpy(&argInt2, pRequest->lpcArgs[2].argData, pRequest->lpcArgs[2].argSize);

    wsize = write(argInt1, (char*)arg1, strlen(arg1));
    if(wsize == -1)
        pLpcResponse->errorno = -1;
    else
        pLpcResponse->errorno = 0;

    pLpcResponse->pid = pRequest->pid;
    int_wsize = (int)wsize;
    pLpcResponse->responseSize = sizeof(int);
    memcpy(pLpcResponse->responseData, &int_wsize, sizeof(int));

    if(pRequest->valid == 1){
        memcpy(responseShmAddr_1, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_1);
    }
    else if(pRequest->valid == 2){
        memcpy(responseShmAddr_2, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_2);
    }
    else if(pRequest->valid == 3){
        memcpy(responseShmAddr_3, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_3);
    }
    else if(pRequest->valid == 4){
        memcpy(responseShmAddr_4, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_4);
    }
    else if(pRequest->valid == 5){
        memcpy(responseShmAddr_5, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_5);
    }

    free(pLpcResponse);
    free(arg1);
}


int CloseFile(LpcRequest* pRequest)
{
    LpcResponse *pLpcResponse;
    int argInt1;
    int returnValue;

    pLpcResponse = (LpcResponse*)malloc(sizeof(LpcResponse));

    memset(pLpcResponse, 0x00, sizeof(LpcResponse));

    memcpy(&argInt1, pRequest->lpcArgs[0].argData, pRequest->lpcArgs[0].argSize);

    returnValue = close(argInt1);

    if(returnValue == -1)
        pLpcResponse->errorno = -1;
    else
        pLpcResponse->errorno = 0;

    pLpcResponse->pid = pRequest->pid;
    pLpcResponse->responseSize = sizeof(int);
    memcpy(pLpcResponse->responseData, &returnValue, sizeof(int));

    if(pRequest->valid == 1){
        memcpy(responseShmAddr_1, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_1);
    }
    else if(pRequest->valid == 2){
        memcpy(responseShmAddr_2, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_2);
    }
    else if(pRequest->valid == 3){
        memcpy(responseShmAddr_3, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_3);
    }
    else if(pRequest->valid == 4){
        memcpy(responseShmAddr_4, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_4);
    }
    else if(pRequest->valid == 5){
        memcpy(responseShmAddr_5, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_5);
    }

    free(pLpcResponse);
}

int MakeDirectory(LpcRequest* pRequest)
{
    LpcResponse *pLpcResponse;
    char *arg1;
    int argInt1;
    int fd;
    int flag;

    pLpcResponse = (LpcResponse*)malloc(sizeof(LpcResponse));
    arg1 = (char*)malloc(LPC_DATA_MAX);

    memset(pLpcResponse, 0x00, sizeof(LpcResponse));
    memset(arg1, 0x00, LPC_DATA_MAX);

    memcpy(arg1, pRequest->lpcArgs[0].argData, pRequest->lpcArgs[0].argSize);
    memcpy(&argInt1, pRequest->lpcArgs[1].argData, pRequest->lpcArgs[1].argSize);

    fd = mkdir(arg1, argInt1);
    if(fd == -1)
        pLpcResponse->errorno = -1;
    else
        pLpcResponse->errorno = 0;

    pLpcResponse->pid = pRequest->pid;
    pLpcResponse->responseSize = sizeof(int);
    memcpy(pLpcResponse->responseData, &fd, sizeof(int));

    if(pRequest->valid == 1){
        memcpy(responseShmAddr_1, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_1);
    }
    else if(pRequest->valid == 2){
        memcpy(responseShmAddr_2, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_2);
    }
    else if(pRequest->valid == 3){
        memcpy(responseShmAddr_3, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_3);
    }
    else if(pRequest->valid == 4){
        memcpy(responseShmAddr_4, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_4);
    }
    else if(pRequest->valid == 5){
        memcpy(responseShmAddr_5, pLpcResponse, sizeof(LpcResponse));
        sem_post(responseSem_5);
    }

    free(pLpcResponse);
    free(arg1);
}


int GetString(LpcRequest* pRequest)
{
}

void signalHandler(int signum){
    if(signum == SIGINT || signum == SIGSEGV){
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

        shmctl(requestShmID_1, IPC_RMID, NULL);
        shmctl(requestShmID_2, IPC_RMID, NULL);
        shmctl(requestShmID_3, IPC_RMID, NULL);
        shmctl(requestShmID_4, IPC_RMID, NULL);
        shmctl(requestShmID_5, IPC_RMID, NULL);

        shmctl(responseShmID_1, IPC_RMID, NULL);
        shmctl(responseShmID_2, IPC_RMID, NULL);
        shmctl(responseShmID_3, IPC_RMID, NULL);
        shmctl(responseShmID_4, IPC_RMID, NULL);
        shmctl(responseShmID_5, IPC_RMID, NULL);

        sem_close(requestSem);

        sem_close(responseSem_1);
        sem_close(responseSem_2);
        sem_close(responseSem_3);
        sem_close(responseSem_4);
        sem_close(responseSem_5);

        sem_unlink("RequestSem1");

        sem_unlink("ResponseSem1");
        sem_unlink("ResponseSem2");
        sem_unlink("ResponseSem3");
        sem_unlink("ResponseSem4");
        sem_unlink("ResponseSem5");
        
        exit(-1);
    }
}
