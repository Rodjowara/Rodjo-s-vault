#include <stdio.h> // decl. of FILE, __stdin, __stdout
#include <rt_sys.h> // declaration of _sys_open, etc.
#include <string.h> // strncmp, etc.
#include "morse.h"
extern void sendchar_USART2(int);

__asm(".global __use_no_semihosting");

#define FH_STDIN 0x8001
#define FH_STDOUT 0x8002
#define FH_STDERR 0x8003
#define MORSE 0x1234

FILEHANDLE _sys_open(const char * name, int openmode){
	(void)openmode;
	if (!strncmp(name, ":STDIN",6)) {
		return(FH_STDIN); // 0x8001
	} else if (!strncmp(name, ":STDOUT",7)) {
		return(FH_STDOUT); // 0x8002
	} else if (!strncmp(name, ":STDERR",7)) {
		return(FH_STDERR); // 0x8003
	}else if( !strncmp(name, "Morse", 5)){
		return(MORSE);	//0x1234
	} else {
		return (-1); // ostalo nije implementirano
	}
}

int _sys_istty(FILEHANDLE fh) {
	if (fh==FH_STDIN) {
		return (1); // tty
	}else if (fh==FH_STDOUT) {
		return (1); // tty
	}else if (fh==FH_STDERR) {
		return (1); // tty
	}else if(fh== MORSE){
		return (1);
	}else {
		return (-1); // ostalo nije implementirano
	}
}

int _sys_write(FILEHANDLE fh, const unsigned char * buf, unsigned len, int mode) {
	(void)mode;
	if (fh==FH_STDOUT) { // konzola
		while(len != 0) {
			sendchar_USART2(*buf);
			buf++;
			len--;
		}
		return (0);
	}else if(fh == MORSE){
		while(len != 0) {
			send_char_morse(*buf);
			buf++;
			len--;
		}
		return(0);
	}else {
		return (-1); // ostalo nije implementirano
 }
}
 
void _ttywrch(int ch){
	sendchar_USART2(ch);
}

int stdout_putchar(int ch){
	return ch;
}

int _sys_close(FILEHANDLE fh){
	return 0;
}

__attribute__((noreturn)) void _sys_exit(int return_code){
	(void)return_code;
	while(1);
}