#include "Lpc.h"
#include "LpcStub.h"
#include <stdio.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main(void){
    key_t reqKey1;
    key_t reqKey2;
    key_t reqKey3;
    key_t reqKey4;
    key_t reqKey5;

    int requestShmID_1;
    int requestShmID_2;
    int requestShmID_3;
    int requestShmID_4;
    int requestShmID_5;

    char* requestShmAddr_1;
    char* requestShmAddr_2;
    char* requestShmAddr_3;
    char* requestShmAddr_4;
    char* requestShmAddr_5;

    sem_t* requestSem;

    LpcRequest* pLpcRequest;
    LpcRequest* reset;

    Init();

    reqKey1 = ftok("server.c", 1);
    reqKey2 = ftok("server.c", 2);
    reqKey3 = ftok("server.c", 3);
    reqKey4 = ftok("server.c", 4);
    reqKey5 = ftok("server.c", 5);

    requestShmID_1 = shmget(reqKey1, sizeof(LpcRequest) * 2, IPC_CREAT);
    requestShmID_2 = shmget(reqKey2, sizeof(LpcRequest) * 2, IPC_CREAT);
    requestShmID_3 = shmget(reqKey3, sizeof(LpcRequest) * 2, IPC_CREAT);
    requestShmID_4 = shmget(reqKey4, sizeof(LpcRequest) * 2, IPC_CREAT);
    requestShmID_5 = shmget(reqKey5, sizeof(LpcRequest) * 2, IPC_CREAT);
    
    requestShmAddr_1 = shmat(requestShmID_1, NULL, 0);
    requestShmAddr_2 = shmat(requestShmID_2, NULL, 0);
    requestShmAddr_3 = shmat(requestShmID_3, NULL, 0);
    requestShmAddr_4 = shmat(requestShmID_4, NULL, 0);
    requestShmAddr_5 = shmat(requestShmID_5, NULL, 0);

    requestSem = sem_open("RequestSem1", O_CREAT, 0666, 1);

    pLpcRequest = (LpcRequest*)malloc(sizeof(LpcRequest));
    reset = (LpcRequest*)malloc(sizeof(LpcRequest));

    while(1){
        sem_wait(requestSem);
//puts("<< Get Request");
        // GET REQUEST

        memset(pLpcRequest, 0x00, sizeof(LpcRequest));
        memcpy(pLpcRequest, requestShmAddr_1, sizeof(LpcRequest));

        if(pLpcRequest->valid == 1){
//puts("[Segment 1] has data");
            // have msg

            // reset shm
            memset(reset, 0x00, sizeof(LpcRequest));
            reset->valid = 0;
            memcpy(requestShmAddr_1, reset, sizeof(LpcRequest));
            
            if(pLpcRequest->service == LPC_OPEN_FILE){
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
        }

        memset(pLpcRequest, 0x00, sizeof(LpcRequest));
        memcpy(pLpcRequest, requestShmAddr_2, sizeof(LpcRequest));

        if(pLpcRequest->valid == 2){
//puts("[Segment 2] has data");
            // have msg

            // reset shm
            memset(reset, 0x00, sizeof(LpcRequest));
            reset->valid = 0;
            memcpy(requestShmAddr_2, reset, sizeof(LpcRequest));

            if(pLpcRequest->service == LPC_OPEN_FILE){
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
        }

        memset(pLpcRequest, 0x00, sizeof(LpcRequest));
        memcpy(pLpcRequest, requestShmAddr_3, sizeof(LpcRequest));

        if(pLpcRequest->valid == 3){
//puts("[Segment 3] has data");
            // have msg

            // reset shm
            memset(reset, 0x00, sizeof(LpcRequest));
            reset->valid = 0;
            memcpy(requestShmAddr_3, reset, sizeof(LpcRequest));

            if(pLpcRequest->service == LPC_OPEN_FILE){
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
        }

        memset(pLpcRequest, 0x00, sizeof(LpcRequest));
        memcpy(pLpcRequest, requestShmAddr_4, sizeof(LpcRequest));

        if(pLpcRequest->valid == 4){
//puts("[Segment 4] has data");
            // have msg

            // reset shm
            memset(reset, 0x00, sizeof(LpcRequest));
            reset->valid = 0;
            memcpy(requestShmAddr_4, reset, sizeof(LpcRequest));

            if(pLpcRequest->service == LPC_OPEN_FILE){
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
        }

        memset(pLpcRequest, 0x00, sizeof(LpcRequest));
        memcpy(pLpcRequest, requestShmAddr_5, sizeof(LpcRequest));

        if(pLpcRequest->valid == 5){
//puts("[Segment 5] has data");
            // have msg

            // reset shm
            memset(reset, 0x00, sizeof(LpcRequest));
            reset->valid = 0;
            memcpy(requestShmAddr_5, reset, sizeof(LpcRequest));

            if(pLpcRequest->service == LPC_OPEN_FILE){
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
        }
    }

    return 0;
}
