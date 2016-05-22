#ifndef _SENDER_UDP_H
#define _SENDER_UDP_H

/**
 * @file sender_udp.h
 *
 * @brief A sender using UDP protocol.
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
int MAX = 1459;


/** 
 * @brief Print filedata (as in the python script)
 * 
 * @param fileInfo fileInfo to be printed
 * 
 */
void printFileInfo(char* filename, unsigned int fsize);

/**
 * @brief Sends identification package according to protocol
 *
 * @param sender sender
 * @param to receiver
 * @param filename filename
 * @param namelen length of filename
 * @param fsize filesize
 *
 */
void sendIdentificationPackage(int sender, struct sockaddr_in to, char* filename, unsigned short namelen, unsigned int fsize);

/**
 * @brief Sends data packages according to protocol
 *
 * @param sender sender
 * @param to receiver
 * @param fsize filesize
 * @param fp pointer to file
 * @param seqNr next sequence number
 *
 */
void sendDataPackage(int sender, struct sockaddr_in to, unsigned int fsize, FILE* fp, int seqNr);

/**
 * @brief Sends filedata according to protocol
 *
 * @param sender sender
 * @param to receiver
 * @param filename filename
 * @param fsize filesize
 *
 */
void sendFile(int sender, struct sockaddr_in to, char* filename, unsigned int fsize);

/**
 * @brief calculates SHA1Sum
 *
 * @param fsize filesize
 * @param filename filename
 * 
 * @return unsigned char* calculated SHA1Sum
 *
 */
unsigned char* calculateSHA1(unsigned int fsize, char* filename);

/**
 * @brief send SHA1 package according to protocol
 *
 * @param sender sender
 * @param to receiver
 * @param SHA1Sum the SHA1Sum to send
 *
 */
void sendSHA1(int sender, struct sockaddr_in to, char* SHA1Sum);

/**
 * @brief receives conformation from receiver with a timeout of 10s
 *
 * @param sender sender
 * @param to receiver to get confirmation from
 *
 */
void recConfirmation(int sender, struct sockaddr_in to);

/**
 * @brief checks address for validity
 *
 * @param addr given address to check
 *
 * @return in_addr_t converted address
 *
 */
in_addr_t checkAddr(char* addr);

/**
 * @brief checks port for validity
 *
 * @param portNum port to check
 * 
 * @return in_port_t converted port
 *
 */
in_port_t checkPort(char* portNum);

/**
 * @brief check file for validity
 *
 * @param filename filename to check validity of
 *
 * @return char* checked filename without path
 *
 */
char* checkFile(char *filename);

/**
 * main-method
 */
int main(int argc, char **argv);


#endif //_SENDER_UDP_H