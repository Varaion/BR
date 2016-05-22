#ifndef AUFGABE_2_H_
#define AUFGABE_2_H_

#include <stdlib.h>
#include <stdio.h>

static const char SHA1_CMP_OK = 0;
static const char SHA1_CMP_ERROR = -1;

static const unsigned char HEADER_T = 0;
static const unsigned char DATA_T = 1;
static const unsigned char SHA1_T = 2;
static const unsigned char SHA1_CMP_T = 3;

static const char* const SHA1_OK 	= "\x1b[32mSHA1 OK \x1b[0m\n";
static const char* const SHA1_ERROR 	= "\x1b[31mSHA1 Error\x1b[0m\n";
static const char* const command_error 	= "\x1b[33mInvalid number of parameters!\x1b[0m\n";
static const char* const port_error 	= "\x1b[31mInvalid Port (%s) \x1b[0m\n";
static const char* const address_error 	= "\x1b[31mInvalid Address (%s) or Port (%s) \x1b[0m\n";
static const char* const package_error = "\x1b[31mInvalid package received \x1b[0m\n";
static const char* const package_type_error = "\x1b[31mPackage of invaild type received \x1b[0m\n";
static const char* const order_error = "\x1b[31mInvalid Package Order: received %d, expected %d \x1b[0m\n";
static const char* const timeout_error = "\x1b[31mTimeout reached, aborting..\x1b[0m\n";
static const char* const path_error 	= "\x1b[33mInvalid filepath!\x1b[0m\n";
static const char* const read_error 	= "\x1b[33mFile cant be read!\x1b[0m\n";
static const char* const file_creation_error 	= "\x1b[33mFile could not be created!\x1b[0m\n";
static const char* const socket_close_error 	= "\x1b[33mCouldn't close Socket!\x1b[0m\n";
static const char* const socket_bind_error 	= "\x1b[33mCouldn't bind Socket!\x1b[0m\n";
static const char* const socket_error 	= "\x1b[33mCouldn't create Socket!\x1b[0m\n";
static const char* const recCon_error 	= "\x1b[33mCould not receive Confirmation!failed!\x1b[0m\n";
static const char* const malloc_error 	= "\x1b[33mMalloc failed!\x1b[0m\n";
static const char* const recConData_error 	= "\x1b[33mReceived data does not match what was expected!\x1b[0m\n";
static const char* const sendto_error 	= "\x1b[33mSending of a package failed!\x1b[0m\n";
static const char* const receiver_sha1 	= "\x1b[34mReceiver SHA1: %s \x1b[0m\n";
static const char* const sender_sha1 	= "\x1b[34mSender SHA1: %s \x1b[0m\n";
static const char* const filename_str 	= "\x1b[33mFilename: %s \x1b[0m\n";
static const char* const filesize_str 	= "\x1b[33mFilesize: %d bytes\x1b[0m\n";

static char* create_sha1_string(unsigned char* sha1){
  char* result = (char*) malloc(41);
  int i;
  for(i = 0; i < 20; i++){
    sprintf(result+2*i,"%02x",*(sha1+i));
  }
  return result;
}

#endif
