#include <sys/stat.h>
#include <stdio.h>

int main(int argc, char **argv){
	char *name = "test";
	mkdir(name,0);
	return 0;
}
