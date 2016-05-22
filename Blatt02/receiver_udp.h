#ifndef _RECEIVER_UDP_H
#define _RECEIVER_UDP_H

/**
 * @file sender_udp.h
 *
 * @brief A receiver using UDP protocol.
 *
 * @author Dennis Altenhoff (daltenhoff@uni-osnabrueck.de)
 * @author Till Grenzdörffer(tgrenzdoerff@uni-osnabrueck.de)
 *
 */

#include "Aufgabe2.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <libgen.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <openssl/sha.h>


// Maximum package size
int MAX = 1464;
int debug = 0;

struct filedata{
    unsigned short namelen;
    char* filename;
    unsigned int fsize;
};

struct filedata* receiveIdentificationPackage(struct sockaddr_in receiver, int fd);
FILE* createFile(char* filename);
SHA_CTX initSHA1();
void writeDataToFile(char* package, struct filedata* fileInfo, FILE* fp, SHA_CTX ctx, int seqNr);
void checkSHAAndSendReply(struct sockaddr_in receiver, int fd, char* package, SHA_CTX ctx);
int receiveOtherPackages(struct sockaddr_in receiver, int fd, struct filedata* fileInfo, FILE* fp, SHA_CTX ctx, int seqNr);
void receiveFile(struct sockaddr_in receiver, int fd);
in_port_t checkPort(int portNum);

/**
 * main-method
 */
int main(int argc, char **argv);


#endif //_RECEIVER_UDP_H