#!/usr/bin/env python
import os, os.path
import argparse
import socket
import struct
import hashlib

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
parser = argparse.ArgumentParser(description='PA2 TCP Test Sender')
parser.add_argument('remote', metavar="Empfaengeraddresse",type=str,help="IPv4-Addresse oder Hostname des Empfaenger")
parser.add_argument('port', metavar="Empfaengerport",type=int,help="Port des Empfaenger")
parser.add_argument('infile', metavar="Dateipfad",type=str,help="Pfad zu sendenen Datei")
a = parser.parse_args()
filesize = os.stat(a.infile).st_size
basename = os.path.basename(a.infile)
print filesize_str[0:-1] %filesize
print filename_str[0:-1] %basename
buff = ""
buff+= struct.pack("!H",len(basename))
buff+= basename
buff+= struct.pack("!I",filesize)
sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
sock.connect((a.remote,a.port))
sock.settimeout(10)
sock.send(buff)
f = open(a.infile,"rb")
sha1 = hashlib.sha1()
while True:
    data =  f.read(BUFFERSIZE-1)
    if data == "":
        break;
    sha1.update(data)
    buff = ""
    buff+=data

    sock.send(buff)
sha1sum = sha1.digest()
print sender_sha1[0:-1] %sha1.hexdigest()
buff = ""
buff+=sha1sum
sock.send(buff)
try:
    reply = sock.recv(1)
    if reply == "":
        print "Connection closed too early"
        exit()
except socket.timeout:
    print timeout_error[:-1]
    exit()
finally:
    sock.shutdown(socket.SHUT_RDWR)
    sock.close()
result =  struct.unpack("!B",reply[0])[0]
print sha1_ok[0:-1] if not result else sha1_error[0:-1]
