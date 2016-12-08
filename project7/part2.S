#Problem 2;
.global _start

.text
_start:
    movq    $1, %rax # Puts write in %rax
    movq    $1, %rdi # First param to write is standard output
    movq    $hello, %rsi # Next param is message contents
    movq    $13, %rdx # Final param is message length
    syscall

.data
hello:
    .ascii "Hello world\n"
