part2: part2.S
	as --64 -o part2.o part2.S
	ld -m elf_x86_64 part2.o -o part2.out

part3: part3.S
	as --64 -o part3.o part3.S
	ld -m elf_x86_64 part3.o -o part3.out

part4: part4.S
	as --64 -o part4.o part4.S
	ld -m elf_x86_64 part4.o -o part4.out

part5: part5.c
	gcc -O0 -o part5.out part5.c

clean: 
	rm *.out *.o
