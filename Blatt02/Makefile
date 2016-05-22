default: compile

compile: sender_udp.c receiver_udp.c
	gcc -o sender_udp sender_udp.c -lcrypto -lssl
	gcc -o receiver_udp receiver_udp.c -lcrypto -lssl
    
clean:
	rm sender_udp
	rm receiver_udp

valgrindSender:
	valgrind --leak-check=yes --track-origins=yes ./sender_udp 127.0.0.1 1024 test
    
testSender:
	./sender_udp 127.0.0.1 1024 test

valgrindReceiver:
	valgrind --leak-check=yes --track-origins=yes ./receiver_udp 1024

testReceiver:
	./receiver_udp 1024