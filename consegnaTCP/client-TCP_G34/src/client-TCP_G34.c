#if defined WIN32
#include <winsock.h>
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

int main(void) {
#if defined WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        ErrorHandler("error at WSAStartup\n");
        return -1;
    }
#endif

    /* 2) Il client legge da tastiera l'IP dell'host e il numero di porta */
    char serverIP[64];
    int serverPort = PROTOPORT;

    printf("Inserisci l'indirizzo IP del server (es. 127.0.0.1): ");
    if (scanf("%63s", serverIP) != 1) {
        ErrorHandler("Errore nella lettura dell'indirizzo IP.\n");
        ClearWinSock();
        return -1;
    }

    printf("Inserisci il numero di porta del server (es. %d): ", PROTOPORT);
    if (scanf("%d", &serverPort) != 1 || serverPort <= 0) {
        ErrorHandler("Errore nella lettura della porta.\n");
        ClearWinSock();
        return -1;
    }

    /* Creazione della socket */
    int cSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (cSocket < 0) {
        ErrorHandler("socket creation failed.\n");
        ClearWinSock();
        return -1;
    }

    /* Costruzione dell'indirizzo del server */
    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = inet_addr(serverIP);
    sad.sin_port = htons(serverPort);

    /* 3) Il client richiede la connessione al server */
    if (connect(cSocket, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
        ErrorHandler("Failed to connect.\n");
        closesocket(cSocket);
        ClearWinSock();
        return -1;
    }

    /* 6) Il client riceve la stringa "connessione avvenuta" e la stampa */
    char buf[BUFFERSIZE];
    memset(buf, 0, sizeof(buf));
    int bytesRcvd = recv(cSocket, buf, BUFFERSIZE - 1, 0);
    if (bytesRcvd <= 0) {
        ErrorHandler("recv() failed or connection closed prematurely\n");
        closesocket(cSocket);
        ClearWinSock();
        return -1;
    }
    buf[bytesRcvd] = '\0';
    printf("%s\n", buf);

    /* 7) -> 10) Ciclo di lettura n1, n2, n3 e ricezione risultato o QUIT */
    while (1) {
        int n1, n2, n3;
        printf("Inserisci tre numeri interi (n1 n2 n3): ");
        if (scanf("%d %d %d", &n1, &n2, &n3) != 3) {
            ErrorHandler("Input non valido.\n");
            break;
        }

        msg_numbers_t netNums;
        netNums.n1 = htonl(n1);
        netNums.n2 = htonl(n2);
        netNums.n3 = htonl(n3);

        if (send(cSocket, (const char *)&netNums, sizeof(msg_numbers_t), 0) != sizeof(msg_numbers_t)) {
            ErrorHandler("send() sent a different number of bytes than expected\n");
            break;
        }

        /* 10) Il client riceve la stringa dal server e la visualizza sullo STD output */
        memset(buf, 0, sizeof(buf));
        bytesRcvd = recv(cSocket, buf, BUFFERSIZE - 1, 0);
        if (bytesRcvd <= 0) {
            ErrorHandler("recv() failed or connection closed prematurely\n");
            break;
        }
        buf[bytesRcvd] = '\0';
        printf("%s\n", buf);

        /* Se la stringa corrisponde a "QUIT" chiude la connessione e termina */
        if (strcmp(buf, "QUIT") == 0) {
            break;
        }
    }

    closesocket(cSocket);
    ClearWinSock();
    return 0;
}
