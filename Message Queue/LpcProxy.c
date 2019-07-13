#include "LpcProxy.h"
#include "Lpc.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <signal.h>

int requestQid;
int responseQid;
int getStringQid;

void* (*call)(char*);
void signalHandler(int signum);

void Init(void)
{
    key_t reqKey = ftok("request", 1);
    key_t resKey = ftok("response", 1);
    key_t strKey = ftok("string", 1);
    
    requestQid = msgget(reqKey, IPC_CREAT | 0644);
    responseQid = msgget(resKey, IPC_CREAT | 0644);
    getStringQid = msgget(strKey, IPC_CREAT | 0644);

    signal(SIGUSR1, signalHandler);
}


int OpenFile(char* path, int flags)
{
    LpcRequest *pLpcRequest;
    LpcResponse *pLpcResponse;
    int *responseData;

    pLpcRequest = (LpcRequest*)malloc(sizeof(LpcRequest));
    pLpcResponse = (LpcResponse*)malloc(sizeof(LpcResponse));
    responseData = (int*)malloc(sizeof(int));
    memset(pLpcRequest, '\0', sizeof(LpcRequest));
    memset(pLpcResponse, '\0', sizeof(LpcResponse));
    memset(responseData, '\0', sizeof(int));

    pLpcRequest->pid = getpid();
    pLpcRequest->service = LPC_OPEN_FILE;
    pLpcRequest->numArg = 2;
    pLpcRequest->lpcArgs[0].argSize = strlen(path);
    strncpy(pLpcRequest->lpcArgs[0].argData, path, strlen(path));
    pLpcRequest->lpcArgs[1].argSize = sizeof(int);
    memcpy(pLpcRequest->lpcArgs[1].argData, &flags, sizeof(int));

    msgsnd(requestQid, pLpcRequest, sizeof(LpcRequest) - sizeof(long), 0);
    msgrcv(responseQid, pLpcResponse, sizeof(LpcResponse) - sizeof(long), getpid(), 0);

    if(pLpcResponse->errorno == 0){
        memcpy(responseData, pLpcResponse->responseData, sizeof(int));
        free(pLpcRequest);
        free(pLpcResponse);

        return *responseData;
    }
    else
        return pLpcResponse->errorno;
}

int ReadFile(int fd, void* pBuf, int size)
{
    LpcRequest *pLpcRequest;
    LpcResponse *pLpcResponse;

    pLpcRequest = (LpcRequest*)malloc(sizeof(LpcRequest));
    pLpcResponse = (LpcResponse*)malloc(sizeof(LpcResponse));
    memset(pLpcRequest, '\0', sizeof(LpcRequest));
    memset(pLpcResponse, '\0', sizeof(LpcResponse));

    memset(pBuf, '\0', 256);

    pLpcRequest->pid = getpid();
    pLpcRequest->service = LPC_READ_FILE;
    pLpcRequest->numArg = 2;
    pLpcRequest->lpcArgs[0].argSize = sizeof(int);
    memcpy(pLpcRequest->lpcArgs[0].argData, &fd, sizeof(int));
    pLpcRequest->lpcArgs[1].argSize = sizeof(int);
    memcpy(pLpcRequest->lpcArgs[1].argData, &size, sizeof(int));

    msgsnd(requestQid, pLpcRequest, sizeof(LpcRequest) - sizeof(long), 0);
    msgrcv(responseQid, pLpcResponse, sizeof(LpcResponse) - sizeof(long), getpid(), 0);

    if(pLpcResponse->errorno == 0){
        memcpy(pBuf, pLpcResponse->responseData, pLpcResponse->responseSize);
        free(pLpcRequest);
        free(pLpcResponse);

        return pLpcResponse->responseSize;
    }
    else
        return pLpcResponse->errorno;
}


int WriteFile(int fd, void* pBuf, int size)
{
    LpcRequest *pLpcRequest;
    LpcResponse *pLpcResponse;
    int responseData;

    pLpcRequest = (LpcRequest*)malloc(sizeof(LpcRequest));
    pLpcResponse = (LpcResponse*)malloc(sizeof(LpcResponse));
    memset(pLpcRequest, '\0', sizeof(LpcRequest));
    memset(pLpcResponse, '\0', sizeof(LpcResponse));

    pLpcRequest->pid = getpid();
    pLpcRequest->service = LPC_WRITE_FILE;
    pLpcRequest->numArg = 3;
    pLpcRequest->lpcArgs[0].argSize = sizeof(int);
    memcpy(pLpcRequest->lpcArgs[0].argData, &fd, sizeof(int));
    pLpcRequest->lpcArgs[1].argSize = sizeof(pBuf);
    strncpy(pLpcRequest->lpcArgs[1].argData, pBuf, strlen(pBuf));
    pLpcRequest->lpcArgs[2].argSize = sizeof(int);
    memcpy(pLpcRequest->lpcArgs[2].argData, &size, sizeof(int));

    msgsnd(requestQid, pLpcRequest, sizeof(LpcRequest) - sizeof(long), 0);
    msgrcv(responseQid, pLpcResponse, sizeof(LpcResponse) - sizeof(long), getpid(), 0);

    if(pLpcResponse->errorno == 0){
        memcpy(&responseData, pLpcResponse->responseData, pLpcResponse->responseSize);
        free(pLpcRequest);
        free(pLpcResponse);

        return responseData;
    }
    else
        return pLpcResponse->errorno;
}


int CloseFile(int fd)
{
    LpcRequest *pLpcRequest;
    LpcResponse *pLpcResponse;
    int responseData;

    pLpcRequest = (LpcRequest*)malloc(sizeof(LpcRequest));
    pLpcResponse = (LpcResponse*)malloc(sizeof(LpcResponse));
    memset(pLpcRequest, '\0', sizeof(LpcRequest));
    memset(pLpcResponse, '\0', sizeof(LpcResponse));

    pLpcRequest->pid = getpid();
    pLpcRequest->service = LPC_CLOSE_FILE;
    pLpcRequest->numArg = 1;
    pLpcRequest->lpcArgs[0].argSize = sizeof(int);
    memcpy(pLpcRequest->lpcArgs[0].argData, &fd, sizeof(int));

    msgsnd(requestQid, pLpcRequest, sizeof(LpcRequest) - sizeof(long), 0);
    msgrcv(responseQid, pLpcResponse, sizeof(LpcResponse) - sizeof(long), getpid(), 0);

    if(pLpcResponse->errorno == 0){
        memcpy(&responseData, pLpcResponse->responseData, pLpcResponse->responseSize);
        free(pLpcRequest);
        free(pLpcResponse);

        return responseData;
    }
    else
        return pLpcResponse->errorno;
}

int MakeDirectory(char* path, int mode)
{
    LpcRequest *pLpcRequest;
    LpcResponse *pLpcResponse;
    int responseData;
    
    pLpcRequest = (LpcRequest*)malloc(sizeof(LpcRequest));
    pLpcResponse = (LpcResponse*)malloc(sizeof(LpcResponse));
    memset(pLpcRequest, '\0', sizeof(LpcRequest));
    memset(pLpcResponse, '\0', sizeof(LpcResponse));

    pLpcRequest->pid = getpid();
    pLpcRequest->service = LPC_MAKE_DIRECTORY;
    pLpcRequest->numArg = 2;
    pLpcRequest->lpcArgs[0].argSize = strlen(path);
    strncpy(pLpcRequest->lpcArgs[0].argData, path, strlen(path));
    pLpcRequest->lpcArgs[1].argSize = sizeof(int);
    memcpy(pLpcRequest->lpcArgs[1].argData, &mode, sizeof(int));

    msgsnd(requestQid, pLpcRequest, sizeof(LpcRequest) - sizeof(long), 0);
    msgrcv(responseQid, pLpcResponse, sizeof(LpcResponse) - sizeof(long), getpid(), 0);

    if(pLpcResponse->errorno == 0){
        memcpy(&responseData, pLpcResponse->responseData, pLpcResponse->responseSize);
        free(pLpcRequest);
        free(pLpcResponse);

        return responseData;
    }
    else
        return pLpcResponse->errorno;
}

int GetString(void* (*callback)(char*))
{
    LpcRequest *pLpcRequest;
    call = callback;

    pLpcRequest = (LpcRequest*)malloc(sizeof(LpcRequest));
    memset(pLpcRequest, '\0', sizeof(LpcRequest));

    pLpcRequest->pid = getpid();
    pLpcRequest->service = LPC_GET_STRING;
    pLpcRequest->numArg = 0;

    msgsnd(requestQid, pLpcRequest, sizeof(LpcRequest) - sizeof(long), 0);
}

void signalHandler(int signum){
    LpcResponse *pLpcResponse;
    char *userStr;

    pLpcResponse = (LpcResponse*)malloc(sizeof(LpcResponse));
    userStr = (char*)malloc(LPC_DATA_MAX);
    memset(pLpcResponse, '\0', sizeof(LpcResponse));
    memset(userStr, '\0', LPC_DATA_MAX);

    msgrcv(getStringQid, pLpcResponse, sizeof(LpcResponse) - sizeof(long), getpid(), 0);
    strncpy(userStr, pLpcResponse->responseData, pLpcResponse->responseSize);
    call(userStr);
}
