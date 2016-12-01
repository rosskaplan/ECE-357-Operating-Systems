temp.exe: fifo.o sem.o temp.o tas64.S
	gcc -o fifotest.exe fifo.o sem.o tas64.S temp.o

runfifo.exe: testfifo.o fifo.o sem.o tas64.S
	gcc -o testfifo.o fifo.o sem.o tas64.S

testfifo.o: fifo.c fifo.h
	gcc -c testfifo.c

fifo.o: fifo.c fifo.h
	gcc -c fifo.c

sem.o: sem.c sem.h
	gcc -c sem.c

temp.o: temp.c
	gcc -c temp.c 

tas.exe: tas.c
	gcc tas.c -o tas.out
