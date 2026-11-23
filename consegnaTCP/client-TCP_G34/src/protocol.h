#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#define PROTOPORT 27015
#define BUFFERSIZE 512
#define QLEN 6

typedef struct {
    int n1;
    int n2;
    int n3;
} msg_numbers_t;

#endif /* PROTOCOL_H_ */
