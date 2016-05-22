#!/usr/bin/env python
import os, os.path
import argparse
import socket
import struct
import hashlib
import binascii

SHA1_CMP_OK=0
SHA1_CMP_ERROR=-1

sha1_ok 	= "\x1b[32mSHA1 OK \x1b[0m\n";
sha1_error 	= "\x1b[31mSHA1 Error\x1b[0m\n";
port_error 	= "\x1b[31mInvalid Port (%s) \x1b[0m\n";
address_error 	= "\x1b[31mInvalid Address (%s) or Port (%s) \x1b[0m\n";
receiver_sha1 	= "\x1b[34mReceiver SHA1: %s \x1b[0m\n";
sender_sha1 	= "\x1b[34mSender SHA1: %s \x1b[0m\n";
filename_str 	= "\x1b[33mFilename: %s \x1b[0m\n";
filesize_str 	= "\x1b[33mFilesize: %d bytes\x1b[0m\n";

packet_error = "\x1b[31mInvalid Packet received \x1b[0m\n";
order_error = "\x1b[31mInvalid Packet Order: received %d, expected %d \x1b[0m\n";
timeout_error = "\x1b[31mTimeout reached, aborting..\x1b[0m\n";

BUFFERSIZE = 1492

def myrecv(size):
    try:
        reply = sock.recv(size)
    except socket.timeout:
        print timeout_error[:-1]
        sock2.shutdown(socket.SHUT_RDWR)
        sock2.close()
        sock.shutdown(socket.SHUT_RDWR)
        sock.close()
        exit()
    return reply

def checkfilenamelen(filenamelen,reply):
    if filenamelen > struct.unpack("<H",reply)[0]:
        print "Filename very long or Filenamelength not in NBO"

def checkfilesize(filesize,reply):
    if filesize > struct.unpack("<I",reply)[0]:
        print "Very big file or Filesize not in NBO"


def checkfilename(filename):
    if "/" in filename or "\\" in filename:
        print "Filename contains / or \\, could be a Path"

parser = argparse.ArgumentParser(description='PA2 UDP Test Receiver')
parser.add_argument('port', metavar="Empfaengerport",type=int,help="Port des Empfaenger")
a = parser.parse_args()
if not os.path.isdir("received"):
    print "Folder \"./received\" does not exist..."
    exit()
sock2 = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
sock2.bind(('',a.port));
sock2.listen(0)
sock = sock2.accept()[0]
reply = myrecv(2)
sock.settimeout(10)
filenamelen = struct.unpack("!H",reply)[0]
checkfilenamelen(filenamelen,reply)
reply = myrecv(filenamelen)
filename = reply;
checkfilename(filename)
print filename_str[:-1]%filename
reply = myrecv(4)
filesize = struct.unpack("!I",reply)[0]
checkfilesize(filesize,reply)
print filesize_str[:-1]%filesize
f = open("received/"+filename,"w+")
sha1 = hashlib.sha1()
while(True):
    reply = myrecv(BUFFERSIZE if BUFFERSIZE < filesize else filesize)
    f.write(reply)
    sha1.update(reply)
    filesize-=len(reply)
    if not filesize:
        break
reply = myrecv(20)
hexsha1 = binascii.hexlify(reply)
print receiver_sha1[:-1] % hexsha1
sha1sum = sha1.digest()
if reply == sha1sum:
    print sha1_ok[:-1]
    buf =""
    buf+= struct.pack("!B",0)
    sock.send(buf)
else:
    print sha1_error[:-1]
    buf =""
    buf+= struct.pack("!B",-1)
    sock.send(buf)

sock2.shutdown(socket.SHUT_RDWR)
sock2.close()
