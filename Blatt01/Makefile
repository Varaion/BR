default: compile

compile: DLLTest.c
	gcc -o DLLTest DLLTest.c
	
clean:
	rm DLLTest

valgrind: DLLTest
	valgrind --leak-check=yes ./DLLTest 3 4 2 1
    
test: DLLTest
	./DLLTest 3 4 2 1