#include "Lpc.h"
#include "LpcStub.h"
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

int requestQid;
int responseQid;
int getStringQid;
int pid;

void signalHandler(int signum);
void* thread_GetString(void* arg);

void Init(void)
{
    key_t reqKey = ftok("request", 1);
    key_t resKey = ftok("response", 1);
    key_t strKey = ftok("string", 1);

    requestQid = msgget(reqKey, IPC_CREAT | 0644);
    responseQid = msgget(resKey, IPC_CREAT | 0644);
    getStringQid = msgget(strKey, IPC_CREAT | 0644);

    signal(SIGINT, signalHandler);
    signal(SIGSEGV, signalHandler);
}

int OpenFile(LpcRequest* pRequest)
{
    LpcResponse *pLpcResponse;
    char *arg1;
    int argInt1;
    int fd = 3;


    pLpcResponse = (LpcResponse*)malloc(sizeof(LpcResponse));
    arg1 = (char*)malloc(LPC_DATA_MAX);

    memcpy(arg1, pRequest->lpcArgs[0].argData, pRequest->lpcArgs[0].argSize);
    memcpy(&argInt1, pRequest->lpcArgs[1].argData, pRequest->lpcArgs[1].argSize);

    fd = open(arg1, argInt1, 0644);
    if(fd == -1)
        pLpcResponse->errorno = -1;
    else
        pLpcResponse->errorno = 0;

    pLpcResponse->pid = pRequest->pid;
    pLpcResponse->responseSize = sizeof(int);
    memcpy(pLpcResponse->responseData, &fd, sizeof(int));

    msgsnd(responseQid, pLpcResponse, sizeof(LpcResponse) - sizeof(long), 0);
}

int ReadFile(LpcRequest* pRequest)
{
    LpcResponse *pLpcResponse;
    int argInt1;
    int argInt2;
    ssize_t rsize;

    pLpcResponse = (LpcResponse*)malloc(sizeof(LpcResponse));

    memcpy(&argInt1, pRequest->lpcArgs[0].argData, pRequest->lpcArgs[0].argSize);
    memcpy(&argInt2, pRequest->lpcArgs[1].argData, pRequest->lpcArgs[1].argSize);
    
    rsize = read(argInt1, pLpcResponse->responseData, argInt2);
    if(rsize == -1)
        pLpcResponse->errorno = -1;
    else
        pLpcResponse->errorno = 0;
    
    pLpcResponse->pid = pRequest->pid;
    pLpcResponse->responseSize = (int)rsize;

    msgsnd(responseQid, pLpcResponse, sizeof(LpcResponse) - sizeof(long), 0);
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
    arg1 = (char*)malloc(LPC_DATA_MAX);

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
    msgsnd(responseQid, pLpcResponse, sizeof(LpcResponse) - sizeof(long), 0);
}

int CloseFile(LpcRequest* pRequest)
{
    LpcResponse *pLpcResponse;
    int argInt1;
    int returnValue;

    pLpcResponse = (LpcResponse*)malloc(sizeof(LpcResponse));
    memcpy(&argInt1, pRequest->lpcArgs[0].argData, pRequest->lpcArgs[0].argSize);

    returnValue = close(argInt1);

    if(returnValue == -1)
        pLpcResponse->errorno = -1;
    else
        pLpcResponse->errorno = 0;

    pLpcResponse->pid = pRequest->pid;
    pLpcResponse->responseSize = sizeof(int);
    memcpy(pLpcResponse->responseData, &returnValue, sizeof(int));
    msgsnd(responseQid, pLpcResponse, sizeof(LpcResponse) - sizeof(long), 0);
}

int MakeDirectory(LpcRequest* pRequest)
{
    LpcResponse *pLpcResponse;
    char *arg1;
    int argInt1;
    int fd;

    pLpcResponse = (LpcResponse*)malloc(sizeof(LpcResponse));
    arg1 = (char*)malloc(LPC_DATA_MAX);

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

    msgsnd(responseQid, pLpcResponse, sizeof(LpcResponse) - sizeof(long), 0);
}

int GetString(LpcRequest* pRequest){
    pthread_t tid;
    pid = pRequest->pid;

    if(pthread_create(&tid, NULL, thread_GetString, NULL)){
        printf("pthread_create error!\n");
    }
    
    if(pthread_detach(tid)){
        printf("pthread_detach error!\n");
    }
    /*
    if(pthread_join(tid, NULL)){
        printf("pthread_detach error!\n");
    }
    */
    return 0;
}

void* thread_GetString(void* arg){
    LpcResponse *pLpcResponse;
    char *data;
    int thread_exit = 1;
    int returnValue;

    pLpcResponse = (LpcResponse*)malloc(sizeof(LpcResponse));
    data = (char*)malloc(LPC_DATA_MAX);
    memset(pLpcResponse, '\0', sizeof(LpcResponse));
    memset(data, '\0', LPC_DATA_MAX);

    printf("Input the text : ");
    fgets(data, LPC_DATA_MAX, stdin);
    pLpcResponse->errorno = 0;
    pLpcResponse->pid = pid;
    pLpcResponse->responseSize = strlen(data) - 1;
    strncpy(pLpcResponse->responseData, data, strlen(data));
    msgsnd(getStringQid, pLpcResponse, sizeof(LpcResponse) - sizeof(long), 0);
    kill((pid_t)pid, SIGUSR1);

    free(data);
    free(pLpcResponse);
    pthread_exit(&thread_exit);
    return NULL;
}

void signalHandler(int signum){
    if(signum == SIGINT || signum == SIGSEGV){
        msgctl(requestQid, IPC_RMID, 0);
        msgctl(responseQid, IPC_RMID, 0);
        msgctl(getStringQid, IPC_RMID, 0);
        exit(-1);
    }
}
