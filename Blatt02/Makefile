default: compile

compile: sender_udp.c receiver_udp.c
	gcc -o sender_udp sender_udp.c -lcrypto -lssl
	gcc -o receiver_udp receiver_udp.c -lcrypto -lssl
    
clean:
	rm sender_udp
	rm receiver_udp

valgrindSender:
	valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./sender_udp 127.0.0.1 1024 test
    
testSender:
	./sender_udp 127.0.0.1 1024 test
    
testSenderBig:
	./sender_udp 127.0.0.1 1024 test5

valgrindReceiver:
	valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./receiver_udp 1024

testReceiver:
	./receiver_udp 1024