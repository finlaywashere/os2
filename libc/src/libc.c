#include <libc.h>

FILE* stdin;
FILE* stdout;

FILE rstdout, rstdin;

void initialize_standard_library(char* argv[], int argc, char* envp[], int envc){
	rstdin.id = 1;
	rstdout.id = 0;
	stdin = &rstdin;
	stdout = &rstdout;
}
