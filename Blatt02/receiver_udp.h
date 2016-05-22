#ifndef _RECEIVER_UDP_H
#define _RECEIVER_UDP_H

/**
 * @file sender_udp.h
 *
 * @brief A receiver using UDP protocol.
 *
 * @author Dennis Altenhoff (daltenhoff@uni-osnabrueck.de)
 * @author Till Grenzdoerffer(tgrenzdoerff@uni-osnabrueck.de)
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
#include <unistd.h>
#include <openssl/sha.h>


// Maximum package size
int MAX = 1464;

// Filedata struct. Needed to be able to neatly split up behaviour into functions
struct filedata{
    unsigned short namelen;
    char* filename;
    unsigned int fsize;
};

/** 
 * @brief Print filedata (as in the python script)
 * 
 * @param fileInfo fileInfo to be printed
 * 
 */
void printFileInfo(struct filedata* fileInfo);

/**
 * @brief Receive identification-package.
 * 
 * @param client socketaddress of client
 * @param fd socket of server
 * 
 * @return struct filedata* struct containing information on the file to be received
 *
 */
struct filedata* receiveIdentificationPackage(struct sockaddr_in client, int fd);

/**
 * @brief Creates file to write data into
 * 
 * @param fileInfo fileInfo to construct file from
 * 
 * @return File pointer to created file
 *
 */
FILE* createFile(struct filedata* fileInfo);

/**
 * @brief Write received data to file
 * 
 * @param package received data
 * @param fileInfo info about the file to write data into
 * @param fp file-pointer
 * @param *ctx pointer to SHA1-ctx-variable
 * @param seqNr sequenceNumber of next package which should be the next number to be received
 * 
 */
void writeDataToFile(char* package, struct filedata* fileInfo, FILE* fp, SHA_CTX *ctx, int seqNr);

/**
 * @brief Checks received SHA and replies whether matches received data or not
 * 
 * @param client socketaddress of client
 * @param fd socket of server
 * @param package received data
 * @param *ctx pointer to SHA1-ctx-variable
 * 
 */
void checkSHAAndSendReply(struct sockaddr_in client, int fd, char* package, SHA_CTX *ctx);

/**
 * @brief Receives other packages and delegates to next function depending on package type
 * 
 * @param client socketaddress of client
 * @param fd socket of server
 * @param fp file-pointer
 * @param *ctx pointer to SHA1-ctx-variable
 * @param seqNr sequenceNumber of next package which should be the next number to be received
 * 
 * @return int 0 if received package was a DataPackage, 1 if it was the SHAPackage
 *
 */
int receiveOtherPackages(struct sockaddr_in client, int fd, struct filedata* fileInfo, FILE* fp, SHA_CTX *ctx, int seqNr);

/**
 * @brief Frees fileInfo's allocated space
 * 
 * @param fileInfo struct whose space is to be freed
 * 
 */
void freeFileInfo(struct filedata* fileInfo);

/**
 * @brief Start receiving file and delegate necessary steps to other methods
 * 
 * @param client socketaddress of client
 * @param  fd socket of server
 * 
 * @return 
 *
 */
void receiveFile(struct sockaddr_in client, int fd);

/**
 * @brief Check port number
 * 
 * @param portNum oort number to check
 * 
 * @return Converted port number
 *
 */
in_port_t checkPort(int portNum);

/**
 * main-method
 */
int main(int argc, char **argv);


#endif //_RECEIVER_UDP_H
