#include "sender_udp.h"

void printFileInfo(char* filename, unsigned int fsize){
    // Print filedata (as in the python script)
    printf(filename_str, filename);
    printf(filesize_str, fsize);
}

void sendIdentificationPackage(int sender, struct sockaddr_in to, char* filename, unsigned short namelen, unsigned int fsize){
    
    // Calculate space needed for IdenticationPackage and allocate space for it
    int len = 7+namelen;
	unsigned char* identPackage = (unsigned char*)malloc(len);
    // And check for right allocation
    if(identPackage == NULL){
        printf(malloc_error);
        exit(EXIT_FAILURE);
    }
    
    // Add type to package
    identPackage[0] = HEADER_T;
    
    // Add converted length of filename to package
    unsigned short convLen = htons(namelen);
	memcpy((identPackage+1),&convLen,sizeof(unsigned short));
    
    // Add filename to package
	int i = 0;
	while(i<namelen){
		identPackage[i+3]=filename[i];
		i++;
	}
    
    // Add converted filesize to package
    unsigned int convFsize = htonl(fsize);
    memcpy(identPackage+3+namelen,&convFsize,sizeof(unsigned int));
    
    // Print filedata (as in the python script)
    printf("- File to be sent: -\n");
    printFileInfo(filename, fsize);
    
    // Send identification package
	int err = sendto(sender,identPackage,len,0,(struct sockaddr*) &to,sizeof(struct sockaddr_in));
    // And react to outcome
    if(err<0){
        printf(sendto_error);
        exit(EXIT_FAILURE);
    }
    
    // Free allocated space
    free(identPackage);
}

void sendDataPackage(int sender, struct sockaddr_in to, unsigned int fsize, FILE* fp, int seqNr){
    // Get variable to check how much is to be added to the package (less redundant coding)
    int toRead;
    if(fsize>MAX){
        toRead = MAX;
    }else{
        toRead = fsize;
    }
    
    // Allocate space for type(1), sequenceNumber (4) and filedata (toRead)!
    unsigned char* package = (unsigned char*)malloc(toRead+5);
    // And check for right allocation
    if(package == NULL){
        printf(malloc_error);
        exit(EXIT_FAILURE);
    }
    
    // Add type to package
    package[0] = DATA_T;
    
    // Add sequence number to package
    int convSeqNr = htonl(seqNr);
	memcpy((package+1), &convSeqNr, sizeof(int));
    
    // Allocate buffer to read data from file into
    unsigned char* buffer = (unsigned char*)malloc(toRead);
    // And check for right allocation
    if(buffer == NULL){
        printf(malloc_error);
        exit(EXIT_FAILURE);
    }
    
    // Read data from file to buffer (file was opened in sendFile method)
    fread(buffer, sizeof(char), toRead, fp);
    
    // Initialise counter
    int i;
    // Add filedata to package
    for(i=0; i<toRead; i++){
        package[i+5] = buffer[i];
    }
    
    // Send package
    int err = sendto(sender, package, toRead+5, 0,  (struct sockaddr*) &to, sizeof(struct sockaddr_in));
    // And react to outcome
    if(err<0){
        printf(sendto_error);
        exit(EXIT_FAILURE);
    }
    
    // Free allocated space
    free(package);
    free(buffer);
}

void sendFile(int sender, struct sockaddr_in to, char* filename, unsigned int fsize){
    // Create some variables
    int package_count;
    int i;
    
    // Get number of packages to send
    package_count = (fsize/MAX)+1;
    
    // Open file and see ifit worked out
    FILE *fp = fopen(filename, "rb");
    if(fp == NULL){
        printf(path_error);
        exit(EXIT_FAILURE);
    }
    // Send the necessary number of packages
    for (i=0; i<package_count; i++){
        sendDataPackage(sender, to, fsize, fp, i);
        // sleep for 1ms (to make the right order of packages as probable as possible)
         usleep(1*1000);
        if(fsize > MAX){
          fsize = fsize-MAX;
        }
    }
    // Close file
    fclose(fp);
}

unsigned char* calculateSHA1(unsigned int fsize, char* filename){
    
    // Allocate buffer to read data from file into
    unsigned char* buffer = (unsigned char*)malloc(MAX);
    // And check for right allocation
    if(buffer == NULL){
        printf(malloc_error);
        exit(EXIT_FAILURE);
    }
    // 
    int bufferLength = 0;
    // Allocate buffer for SHA1 sum
    unsigned char* SHA1Sum = malloc(20);
    // And check for right allocation
    if(SHA1Sum == NULL){
        printf(malloc_error);
        exit(EXIT_FAILURE);
    }
    
    // Calculate SHA sum
    SHA_CTX ctx;
    SHA1_Init(&ctx);
    // Open file and check ifit worked out
    FILE *fp = fopen(filename, "rb");
    if(fp == NULL){
        printf(path_error);
        exit(EXIT_FAILURE);
    }
    while (bufferLength = fread (buffer, sizeof(char), MAX, fp)){
        SHA1_Update(&ctx, buffer, bufferLength);
    }
    // Close file
    fclose(fp);
    SHA1_Final(SHA1Sum, &ctx);

    // Free allocated space
    // SHA1Sum will be free'd in main method
    free(buffer);
    
    return SHA1Sum;
}

void sendSHA1(int sender, struct sockaddr_in to, char* SHA1Sum){
    // Allocate space for SHA1 package (1 for type and 20 for SHA1 sum)
    unsigned char* package = (unsigned char*)malloc(21);
    // And check for right allocation
    if(package == NULL){
        printf(malloc_error);
        exit(EXIT_FAILURE);
    }
    
    // Counter-variable
    int i;
    
    // Add type to package
    package[0] = (unsigned char)SHA1_T;
    
    // Add SHA1 sum to package
    for(i=0; i<20; i++){
        package[i+1] = SHA1Sum[i];
    }
    
    // Print SHA1-hash (as in the python script)
    unsigned char* SHA1SumString = create_sha1_string(SHA1Sum);
    printf(sender_sha1, SHA1SumString);
    
    // Send package
    int err = sendto(sender, package, 21, 0,  (struct sockaddr*) &to, sizeof(struct sockaddr_in));
    // And react to outcome
    if(err<0){
        printf(sendto_error);
        exit(EXIT_FAILURE);
    }
    
    // Free allocated space
    free(SHA1SumString);
    free(package);
}

void recConfirmation(int sender, struct sockaddr_in to){
    // Create some necessary variables
    int len = -1;
    int flen = sizeof(struct sockaddr_in);
    unsigned char type;
    char sha1_result;
    // Allocate space for the confirmation package
    char package[1500];
    // And check for right allocation
    if(package == NULL){
        printf(malloc_error);
        exit(EXIT_FAILURE);
    }

    // Set up 10s timeout
    fd_set waiting_for_IO;
    FD_ZERO (&waiting_for_IO);
    FD_SET (sender, &waiting_for_IO);
    struct timeval time_out;
    time_out.tv_usec = 0;
    time_out.tv_sec = 10;
    setsockopt(sender, SOL_SOCKET, SO_RCVTIMEO, (char *)&time_out, sizeof(struct timeval));

    // Receive package
    len = recvfrom(sender, package, sizeof(package), 0, (struct sockaddr*) &to, &flen);
    // And check ifit has the right length
    if(len<0){
        printf(recCon_error);
        exit(EXIT_FAILURE);
    }
    
    // Add terminating 0
    package[len] = '\0';

    // Read package type
    type = package[0];

    // And check ifit is the right package type
    if(type == SHA1_CMP_T){
        // Read SHA1 check value
        sha1_result = package[1];

        // And check whether its right or not
        if(sha1_result == SHA1_CMP_OK){
            // Print SHA1OK (as in the python script)
            printf(SHA1_OK);
        }else{
            if(sha1_result == SHA1_CMP_ERROR){
                // Print SHA1ERROR (as in the python script)
                printf(SHA1_ERROR);
            }
        }
    }else{
        printf(recConData_error);
        exit(EXIT_FAILURE);
    }
}

in_addr_t checkAddr(char* addr){
    // Try to create a connection to see ifaddress is valid
    struct sockaddr_in sockaddr;
    if(inet_pton(AF_INET, addr, &(sockaddr)) < 1){
        printf(address_error);
        exit(EXIT_FAILURE);
    }else{
        return inet_addr(addr);
    }
}

in_port_t checkPort(char* portNum){
    // Check port number
	if(*portNum < 0 || *portNum > 65535){
		printf(port_error);
        exit(EXIT_FAILURE);
	}
    return atoi(portNum);
}

char* checkFile(char *filename)
{
    // Cut of path
    char* file = basename(filename);
    // Check iffile exists
    if(access(file, F_OK)){
        printf(path_error);
        exit(EXIT_FAILURE);
    } else{
        // Check iffile readable
        if(access(file, R_OK)){
            printf(path_error);
        exit(EXIT_FAILURE);
        }else{
            // return filename
            return file;
        }
    }
    
}

int main(int argc, char* argv[]){
    // Check number of parameters
    if(argc!=4){
        printf(command_error);
        exit(EXIT_FAILURE);
    }    
    
    // Create receiver data
	struct sockaddr_in to;
	in_port_t port = -1;
	in_addr_t inaddr = -1;
    
    // Read and check incoming address
    inaddr = checkAddr(argv[1]);
    
    // Read an check incoming port
	port = checkPort(argv[2]);

    // Set receiver data
	to.sin_family = AF_INET;
	to.sin_port = htons(port);
	to.sin_addr.s_addr=inaddr;

    // Create sender socket
	int sender;
	sender=socket(AF_INET,SOCK_DGRAM,0);	
	if(sender<0){
		printf(socket_error);
        exit(EXIT_FAILURE);
	}

    // check and set filename and length
	char* filename;
	filename = checkFile(argv[3]);
	unsigned short namelen = strlen(filename);
	
    // Get filesize
	unsigned int fsize;
	FILE *fp = fopen(filename,"rb");
    if(fp == NULL){
        printf(path_error);
        exit(EXIT_FAILURE);
    }
    // By looking for end
	fseek(fp,0,SEEK_END);
    // Checking position
	fsize=ftell(fp);
    // And reset position in file
	fseek(fp,0,SEEK_SET);
	fclose(fp);
    
    // Send IdenticationPackage
    sendIdentificationPackage(sender, to, filename, namelen, fsize);
    
    // Send File
    sendFile(sender, to, filename, fsize);
    
    // Create and send SHA1-Sum
    unsigned char* SHA1Sum = calculateSHA1(fsize, filename);
    sendSHA1(sender, to, SHA1Sum);
    
    // Receive confirmation package
    recConfirmation(sender, to);
    
    // Try to close sender socket
    if(close(sender) != 0){
        printf(socket_close_error);
        exit(EXIT_FAILURE);
    }
    
    // Free allocated space
    free(SHA1Sum);
    
    // Done
    return 0;
}
