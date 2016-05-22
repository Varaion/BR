#include "receiver_udp.h"

struct filedata* receiveIdentificationPackage(struct sockaddr_in receiver, int fd){
     // Allocate space for package
    unsigned char* package = (unsigned char*)malloc(MAX);
    // And check for right allocation
    if(package == NULL){
        printf(malloc_error);
        exit(EXIT_FAILURE);
    }

	struct sockaddr_in from;
    // Receive IdentificationPackage
    int received = recvfrom(fd, package, MAX, 0, NULL, NULL);
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

        // Extract filename from IdentificationPackage
        char* filename = malloc(namelen);
        // Check for right allocation
        if(filename == NULL){
            printf(malloc_error);
            exit(EXIT_FAILURE);
        }
        int i;
        for(i = 0; i<namelen; i++){
            filename[i] = package[i+3];
        }

        // Extract filesize from IdentificationPackage
        unsigned int fsize;
        memcpy(&fsize, package+3+namelen, sizeof(unsigned int));
        fsize = ntohl(fsize);

        free(package);

        struct filedata* fileInfo = malloc(sizeof(struct filedata));
        fileInfo->namelen = namelen;
        fileInfo->filename = filename;
        fileInfo->fsize = fsize;

        return fileInfo;
    }else{
        printf(package_type_error);
        exit(EXIT_FAILURE);
    }
}

FILE* createFile(char* filename){

    // Add received-directory to filename to get the right path to save the file to
    char* recPath = malloc((strlen(filename)+9+1));
    sprintf(recPath, "%s", "received/");
    strcat(recPath, filename);

    FILE* fp = fopen(recPath, "wb");
    if(fp == NULL){
        printf(file_creation_error);
        exit(EXIT_FAILURE);
    }

    // Return file-pointer
    return fp;
}

void writeDataToFile(char* package, struct filedata* fileInfo, FILE* fp, SHA_CTX *ctx, int seqNr){

    int toWrite;
    if(fileInfo->fsize>MAX){
        toWrite = MAX;
    }else{
        toWrite = fileInfo->fsize;
    }

    int receivedSeqNr;
    memcpy(&receivedSeqNr, package+1, sizeof(int));
    receivedSeqNr = ntohl(receivedSeqNr);
    if(receivedSeqNr!=seqNr){
        printf(order_error,receivedSeqNr,seqNr);
        exit(EXIT_FAILURE);
    }

    unsigned char* buffer = (unsigned char*)malloc(toWrite);
    // Check for right allocation
    if(buffer == NULL){
        printf(malloc_error);
        exit(EXIT_FAILURE);
    }

    // debug
    int i;
    for(i = 0; i<toWrite; i++){
        buffer[i] = package[i+1+sizeof(int)];
    }

    fwrite(buffer, sizeof(char), toWrite, fp);

    SHA1_Update(ctx, buffer, toWrite);

}

void checkSHAAndSendReply(struct sockaddr_in receiver, int fd, char* package, SHA_CTX *ctx){

    // Allocate space for checksum
    unsigned char* checksum = malloc(160);
    // Check for right allocation
    if(checksum == NULL){
        printf(malloc_error);
        exit(EXIT_FAILURE);
    }
    SHA1_Final(checksum, ctx);
    char* checksum_string;
    checksum_string = create_sha1_string(checksum);

    // Allocate space for sent checksum
    unsigned char* received_checksum = malloc(20);
    // Check for right allocation
    if(received_checksum == NULL){
        printf(malloc_error);
        exit(EXIT_FAILURE);
    }
    int i;
    for(i = 0; i < 20; i++){
        received_checksum[i] = package[i+1];
    }
    char* received_checksum_string;
    received_checksum_string = create_sha1_string(received_checksum);

    printf(checksum_string);printf("\n");
    printf(received_checksum_string);printf("\n");


    char* sha1Result = malloc(2);
    sha1Result[0] = SHA1_CMP_T;
    // Check for right allocation
    if(received_checksum == NULL){
        printf(malloc_error);
        exit(EXIT_FAILURE);
    }

    sha1Result[1] = (strcmp(checksum_string, received_checksum_string)==0)?SHA1_CMP_OK:SHA1_CMP_ERROR;

    //struct sockaddr_in from;
    int err = sendto(fd, sha1Result, 2, 0, (struct sockaddr*) &receiver, sizeof(struct sockaddr_in));
    if(err<0){
        printf(sendto_error);
        exit(EXIT_FAILURE);
    }

    free(received_checksum);
    free(checksum);
    free(sha1Result);
    free(checksum_string);
    free(received_checksum_string);
}

int receiveOtherPackages(struct sockaddr_in receiver, int fd, struct filedata* fileInfo, FILE* fp, SHA_CTX *ctx, int seqNr){
     // Allocate space for package
    unsigned char* package = (unsigned char*)malloc(MAX);
    // And check for right allocation
    if(package == NULL){
        printf(malloc_error);
        exit(EXIT_FAILURE);
    }

    int test = (sizeof(struct sockaddr_in));
    // Receive DataPackage
    int received = recvfrom(fd, package, MAX, 0, (struct sockaddr*) &receiver, &test);
    // And check for validity
    if (received < 0){
        printf(package_error);
        exit(EXIT_FAILURE);
    }

    // Check if DataPackage
    if(package[0] == DATA_T){
        writeDataToFile(package, fileInfo, fp, ctx, seqNr);
        return 0;
    }else{
        if(package[0] == SHA1_T){
            checkSHAAndSendReply(receiver, fd, package, ctx);
            return 1;
        }else{
            printf(package_type_error);
            exit(EXIT_FAILURE);
        }
    }
}

void receiveFile(struct sockaddr_in receiver, int fd){

    // Receive IdentificationPackage
    struct filedata* fileInfo = receiveIdentificationPackage(receiver, fd);

    FILE* fp = createFile(fileInfo->filename);

    // Set timeout to 10s
    fd_set waiting_for_IO;
    FD_ZERO (&waiting_for_IO);
    FD_SET (fd, &waiting_for_IO);

    struct timeval time_out;
    time_out.tv_usec = 0;
    time_out.tv_sec = 10;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&time_out, sizeof(struct timeval));

    SHA_CTX ctx;
    SHA1_Init(&ctx);

    // Receive DataPackages and write them to File and receive and check SHA1Package
    for (;;){
        int seqNr = 0;
        int sent = receiveOtherPackages(receiver, fd, fileInfo, fp, &ctx, seqNr);
        if(sent == 1){
            break;
        }
    }
}

in_port_t checkPort(int portNum){
    // Check port number
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

	struct sockaddr_in receiver;

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        printf(socket_error);
        exit(EXIT_FAILURE);
    }

    receiver.sin_family = AF_INET;
    receiver.sin_port = htons(rec_port);
    receiver.sin_addr.s_addr = htonl(INADDR_ANY);

    int binderr;
    binderr = bind(fd, (struct sockaddr*)&receiver, sizeof(struct sockaddr_in));
    if(binderr<0){
        printf(socket_bind_error);
        exit(EXIT_FAILURE);
    }

	struct sockaddr_in newReceiver;

    receiveFile(newReceiver, fd);

    int closed = -1;
    closed = close(fd);
    if (closed!=0){
        printf(socket_close_error);
        exit(EXIT_FAILURE);
    }

}
