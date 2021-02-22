#include <libc.h>

FILE* stdin;
FILE* stdout;

void initialize_standard_library(char* argv[], int argc, char* envp[], int envc){
	stdin->id = 1;
	stdout->id = 0;
}
