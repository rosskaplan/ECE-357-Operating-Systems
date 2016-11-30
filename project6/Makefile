tas.exe: tas.c
	gcc tas.c -o tas.out

temp.o : temp.c
	gcc -c temp.c 

sem.o: sem.c sem.h
	gcc -c sem.c

fifo.o: fifo.c fifo.h sem.c sem.h
	gcc -c fifo.c

testfifo.o: fifo.c fifo.h testfifo.c
	gcc -c testfifo.c

runfifo.exe: testfifo.o fifo.o sem.o tas64.S
	gcc -o testfifo.o fifo.o sem.o tas64.S

temp.exe: fifo.o sem.o temp.o tas64.S
	gcc -o fifo.o sem.o temp.o tas64.S
