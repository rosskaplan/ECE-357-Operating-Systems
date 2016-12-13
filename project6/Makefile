test.exe: fifo.o sem.o test.o tas64.S
	gcc -o fifotest.exe fifo.o sem.o tas64.S test.o
	make clean

runfifo.exe: testfifo.o fifo.o sem.o tas64.S
	gcc -o testfifo.o fifo.o sem.o tas64.S

testfifo.o: fifo.c fifo.h
	gcc -c testfifo.c

fifo.o: fifo.c fifo.h
	gcc -c fifo.c

sem.o: sem.c sem.h
	gcc -c sem.c

test.o: test.c
	gcc -c test.c 

tas.exe: tas.c
	gcc tas.c -o tas.out

clean:
	rm *.o
