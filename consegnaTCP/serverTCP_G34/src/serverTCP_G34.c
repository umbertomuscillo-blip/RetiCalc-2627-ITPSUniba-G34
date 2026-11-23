#if defined WIN32
#include <winsock.h>
typedef int socklen_t;
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protocol.h"

void ErrorHandler(const char *errorMessage) {
    printf("%s", errorMessage);
}

void ClearWinSock(void) {
#if defined WIN32
    WSACleanup();
#endif
}

int main(int argc, char *argv[]) {
    int port;
    if (argc > 1) {
        port = atoi(argv[1]);
    } else {
        port = PROTOPORT;
    }

    if (port <= 0) {
        printf("bad port number %s\n", argv[1]);
        return 0;
    }

#if defined WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        ErrorHandler("Error at WSAStartup()\n");
        return 0;
    }
#endif

    int mySocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mySocket < 0) {
        ErrorHandler("socket creation failed.\n");
        ClearWinSock();
        return -1;
    }

    int opt = 1;
    setsockopt(mySocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));

    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = htonl(INADDR_ANY);
    sad.sin_port = htons(port);

    if (bind(mySocket, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
        ErrorHandler("bind() failed.\n");
        closesocket(mySocket);
        ClearWinSock();
        return -1;
    }

    if (listen(mySocket, QLEN) < 0) {
        ErrorHandler("listen() failed.\n");
        closesocket(mySocket);
        ClearWinSock();
        return -1;
    }

    printf("Server TCP avviato sulla porta %d. In attesa di connessioni...\n", port);
    fflush(stdout);

    struct sockaddr_in cad;
    int clientSocket;
    socklen_t clientLen;

    while (1) {
        clientLen = sizeof(cad);
        clientSocket = accept(mySocket, (struct sockaddr *)&cad, &clientLen);
        if (clientSocket < 0) {
            ErrorHandler("accept() failed.\n");
            continue;
        }

        printf("connesso con il client il cui nome IP: %s\n", inet_ntoa(cad.sin_addr));
        fflush(stdout);

        const char *welcomeMsg = "connessione avvenuta";
        int welcomeLen = (int)strlen(welcomeMsg) + 1;
        if (send(clientSocket, welcomeMsg, welcomeLen, 0) != welcomeLen) {
            ErrorHandler("send() welcome failed.\n");
            closesocket(clientSocket);
            continue;
        }

        while (1) {
            msg_numbers_t netNums;
            int bytesRcvd = recv(clientSocket, (char *)&netNums, sizeof(msg_numbers_t), 0);
            if (bytesRcvd <= 0) {
                break;
            }

            int n1 = ntohl(netNums.n1);
            int n2 = ntohl(netNums.n2);
            int n3 = ntohl(netNums.n3);
            int res = n1 + n2 - n3;

            printf("la somma di %d+%d-%d=%d\n", n1, n2, n3, res);
            fflush(stdout);

            char replyBuf[BUFFERSIZE];
            memset(replyBuf, 0, sizeof(replyBuf));
            if (res == 100) {
                snprintf(replyBuf, sizeof(replyBuf), "QUIT");
            } else {
                snprintf(replyBuf, sizeof(replyBuf), "%d", res);
            }

            int replyLen = (int)strlen(replyBuf) + 1;
            if (send(clientSocket, replyBuf, replyLen, 0) != replyLen) {
                ErrorHandler("send() reply failed.\n");
                break;
            }

            if (res == 100) {
                break;
            }
        }

        closesocket(clientSocket);
    }

    closesocket(mySocket);
    ClearWinSock();
    return 0;
}
