#Problem 4;
.global _start

.text
_start:
    movq $5, %rax # Wrong params for open
    movq $hello, %rbx # Messed up arguments to produce error
    syscall

    movq $60, %rax #Exit system call into %rax
    movq $0, %rdi #Exit status (first param)
    syscall

.data

hello:
    .ascii "Hello world\n"
