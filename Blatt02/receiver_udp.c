#include "receiver_udp.h"

void printFileInfo(struct filedata* fileInfo){
    // Print filedata (as in the python script)
    printf(filename_str, fileInfo->filename);
    printf(filesize_str, fileInfo->fsize);
}

struct filedata* receiveIdentificationPackage(struct sockaddr_in client, int fd){
     // Allocate space for package
    unsigned char* package = (unsigned char*)malloc(MAX);
    // And check for right allocation
    if(package == NULL){
        printf(malloc_error);
        exit(EXIT_FAILURE);
    }

    // Receive IdentificationPackage
    int sockaddr_inSize = (sizeof(struct sockaddr_in));
    int received = recvfrom(fd, package, MAX, 0, (struct sockaddr*) &client, &sockaddr_inSize);
    // And check for validity
    if (received < 0){
        printf(package_error);
        exit(EXIT_FAILURE);
    }

    // Check if IdentificationPackage
    if(package[0] == HEADER_T){
        // Extract namelength from IdentificationPackage
        unsigned short namelen;
        memcpy(&namelen, package+1, sizeof(unsigned short));
        namelen = ntohs(namelen);

        // Extract filename from IdentificationPackage (+1 for terminating 0)
        char* filename = malloc(namelen+1);
        // Check for right allocation
        if(filename == NULL){
            printf(malloc_error);
            exit(EXIT_FAILURE);
        }
        // Fill filename
        int i;
        for(i = 0; i<namelen; i++){
            filename[i] = package[i+3];
        }
        // Add terminating 0
        filename[namelen] = '\0';

        // Extract filesize from IdentificationPackage
        unsigned int fsize;
        memcpy(&fsize, package+3+namelen, sizeof(unsigned int));
        // Turn it into right Byte-Order
        fsize = ntohl(fsize);

        // Fill fileInfo
        struct filedata* fileInfo = malloc(sizeof(struct filedata));
        fileInfo->namelen = namelen;
        fileInfo->filename = filename;
        fileInfo->fsize = fsize;

        // Print filedata (as in the python script)
        printf("- File to be received: -\n");
        printFileInfo(fileInfo);
        
        // Free!
        free(package);
        
        return fileInfo;
    }else{
        printf(package_type_error);
        exit(EXIT_FAILURE);
    }
}

FILE* createFile(struct filedata* fileInfo){

    // Add received-directory to filename to get the right path to save the file to
    char* recPath = malloc((strlen(fileInfo->filename)+9+1));
    // Check for right allocation
    if(recPath == NULL){
        printf(malloc_error);
        exit(EXIT_FAILURE);
    }
    
    // Add directory name before filename to get the right path
    sprintf(recPath, "%s", "received/");
    strcat(recPath, fileInfo->filename);

    // Try to open file
    FILE* fp = fopen(recPath, "wb");
    if(fp == NULL){
        printf(file_creation_error);
        exit(EXIT_FAILURE);
    }

    // Free path space
    free(recPath);
    // Return file-pointer
    return fp;
}

void writeDataToFile(char* package, struct filedata* fileInfo, FILE* fp, SHA_CTX *ctx, int seqNr){

    // Check sequenceNumber
    int receivedSeqNr;
    memcpy(&receivedSeqNr, package+1, sizeof(int));
    receivedSeqNr = ntohl(receivedSeqNr);
    if(receivedSeqNr!=seqNr){
        printf(order_error,receivedSeqNr,seqNr);
        exit(EXIT_FAILURE);
    }

    // Calculate how much to write
    int toWrite;
    if(fileInfo->fsize>MAX){
        toWrite = MAX;
    }else{
        toWrite = fileInfo->fsize;
    }
    
    // Create buffer-variable
    unsigned char* buffer = (unsigned char*)malloc(toWrite);
    // Check for right allocation
    if(buffer == NULL){
        printf(malloc_error);
        exit(EXIT_FAILURE);
    }
    // Fill buffer
    int i;
    for(i = 0; i<toWrite; i++){
        buffer[i] = package[i+1+sizeof(int)];
    }

    // Write buffer data to file
    fwrite(buffer, sizeof(char), toWrite, fp);
    
    // Update SHA1-hash
    SHA1_Update(ctx, buffer, toWrite);
    
    // Free buffer
    free(buffer);
}

void checkSHAAndSendReply(struct sockaddr_in client, int fd, char* package, SHA_CTX *ctx){

    // Allocate space for checksum
    unsigned char* checksum = malloc(20);
    // Check for right allocation
    if(checksum == NULL){
        printf(malloc_error);
        exit(EXIT_FAILURE);
    }
    // Finalise SHA1-hash
    SHA1_Final(checksum, ctx);
    // Create string for checksum
    char* checksumString;
    checksumString = create_sha1_string(checksum);
    
    // Print receiverSha1 (other than in the python script, but makes more sense to us)
    printf(receiver_sha1, checksumString);

    // Allocate space for sent checksum
    unsigned char* receivedChecksum = malloc(20);
    // Check for right allocation
    if(receivedChecksum == NULL){
        printf(malloc_error);
        exit(EXIT_FAILURE);
    }
    
    // Put received checksum into variable
    int i;
    for(i = 0; i < 20; i++){
        receivedChecksum[i] = package[i+1];
    }
    // Create string for receivedChecksum
    char* receivedChecksumString;
    receivedChecksumString = create_sha1_string(receivedChecksum);

    // Create variable for result to send
    char* sha1Result = malloc(2);
    // Check for right allocation
    if(sha1Result == NULL){
        printf(malloc_error);
        exit(EXIT_FAILURE);
    }

    // Fill result
    sha1Result[0] = SHA1_CMP_T;
    // Send OK if strings match, ERROR otherwise
    sha1Result[1] = (strcmp(checksumString, receivedChecksumString)==0)?SHA1_CMP_OK:SHA1_CMP_ERROR;
    
    // Print SHA1OK or SHA1ERROR (as in the python script)
    printf((strcmp(checksumString, receivedChecksumString)==0)?SHA1_OK:SHA1_ERROR);

    // Send result to client!
    int err = sendto(fd, sha1Result, 2, 0, (struct sockaddr*) &client, sizeof(struct sockaddr_in));
    if(err<0){
        printf(sendto_error);
        exit(EXIT_FAILURE);
    }

    // Free!
    free(checksum);
    free(checksumString);
    free(receivedChecksum);
    free(receivedChecksumString);
    free(sha1Result);
}

int receiveOtherPackages(struct sockaddr_in client, int fd, struct filedata* fileInfo, FILE* fp, SHA_CTX *ctx, int seqNr){
    
    // Allocate space for package
    unsigned char* package = (unsigned char*)malloc(MAX);
    // And check for right allocation
    if(package == NULL){
        printf(malloc_error);
        exit(EXIT_FAILURE);
    }

    // Variable to make recvfrom possible
    int sockaddr_inSize = (sizeof(struct sockaddr_in));
    // Receive DataPackage
    int received = recvfrom(fd, package, MAX, 0, (struct sockaddr*) &client, &sockaddr_inSize);
    // And check packagesize for validity
    if (received < 0){
        printf(package_error);
        exit(EXIT_FAILURE);
    }

    // Check if DataPackage
    if(package[0] == DATA_T){
        // Write received data to file
        writeDataToFile(package, fileInfo, fp, ctx, seqNr);
        // Remember to free package!
        free(package);
        return 0;
    }else{
        // Or SHAPackage
        if(package[0] == SHA1_T){
            // Work with SHAPackage
            checkSHAAndSendReply(client, fd, package, ctx);
            // Remember to free package!
            free(package);
            return 1;
        }else{
            printf(package_type_error);
            exit(EXIT_FAILURE);
        }
    }
}

void freeFileInfo(struct filedata* fileInfo){
    // First free allocated space of filename, then free fileInfo struct
    free(fileInfo->filename);
    free(fileInfo);
}

void receiveFile(struct sockaddr_in client, int fd){

    // Receive IdentificationPackage
    struct filedata* fileInfo = receiveIdentificationPackage(client, fd);

    // Create file to write data into
    FILE* fp = createFile(fileInfo);

    // Set timeout to 10s
    fd_set waiting_for_IO;
    FD_ZERO (&waiting_for_IO);
    FD_SET (fd, &waiting_for_IO);
    struct timeval time_out;
    time_out.tv_usec = 0;
    time_out.tv_sec = 10;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&time_out, sizeof(struct timeval));

    // Initialize SHA_CTX
    SHA_CTX ctx;
    SHA1_Init(&ctx);

    int seqNr = 0;
    // Receive DataPackages and write them to File and receive and check SHA1Package
    for (;;seqNr++){
        // Receive other packages
        // And increment sequenceNumber after each package
        int sent = receiveOtherPackages(client, fd, fileInfo, fp, &ctx, seqNr);
        // If SHA1Package was received and reply sent, stop
        if(sent == 1){
            break;
        }
    }
    
    // Close file
    fclose(fp);
    // free fileInfo (duh)
    freeFileInfo(fileInfo);
}

in_port_t checkPort(int portNum){
    // Check port number for validity
	if(portNum < 0 || portNum > 65535){
		printf(port_error);
        exit(EXIT_FAILURE);
	}
    return portNum;
}

int main(int argc, char **argv)
{
    // Check number of parameters
    if(argc!=2){
        printf(command_error);
        exit(EXIT_FAILURE);
    }

    // Check and set Port
    in_port_t rec_port = checkPort(atoi(argv[1]));


    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        printf(socket_error);
        exit(EXIT_FAILURE);
    }

    // Create sockaddr_in struct for receiver
	struct sockaddr_in receiver;
    receiver.sin_family = AF_INET;
    receiver.sin_port = htons(rec_port);
    receiver.sin_addr.s_addr = htonl(INADDR_ANY);

    // Try to bind it
    int binderr;
    binderr = bind(fd, (struct sockaddr*)&receiver, sizeof(struct sockaddr_in));
    if(binderr<0){
        printf(socket_bind_error);
        exit(EXIT_FAILURE);
    }

    // Create sockaddr_in struct for receiver
	struct sockaddr_in client;

    // Start to receive file
    receiveFile(client, fd);

    // Close socket before end
    int closed = -1;
    closed = close(fd);
    if (closed!=0){
        printf(socket_close_error);
        exit(EXIT_FAILURE);
    }

    return 0;
}
