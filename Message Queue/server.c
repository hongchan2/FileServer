#include "LpcStub.h"
#include "Lpc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>

int requestQid;

int main(void){
    LpcRequest *pLpcRequest;
    int returnValue;
    key_t reqKey = ftok("request", 1);

    /* make Message Queue & install Signal*/
    Init();
    requestQid = msgget(reqKey, IPC_CREAT | 0644);

    pLpcRequest = (LpcRequest*)malloc(sizeof(LpcRequest));
    
    while(1){
        memset(pLpcRequest, '\0', sizeof(LpcRequest));

        returnValue = msgrcv(requestQid, pLpcRequest, sizeof(LpcRequest) - sizeof(long), 0, 0);

        if(returnValue == -1)
            continue;

        if(pLpcRequest->service == LPC_OPEN_FILE){
            if(pLpcRequest->lpcArgs[0].argSize == 0)
                continue;
            OpenFile(pLpcRequest);
        }
        else if(pLpcRequest->service == LPC_READ_FILE){
            ReadFile(pLpcRequest);
        }
        else if(pLpcRequest->service == LPC_WRITE_FILE){
            WriteFile(pLpcRequest);
        }
        else if(pLpcRequest->service == LPC_CLOSE_FILE){
            CloseFile(pLpcRequest);
        }
        else if(pLpcRequest->service == LPC_MAKE_DIRECTORY){
            MakeDirectory(pLpcRequest);
        }
        else if(pLpcRequest->service == LPC_GET_STRING){
            GetString(pLpcRequest);
        }
    }
}
