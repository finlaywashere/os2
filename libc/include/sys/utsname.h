#ifndef UTSNAME_H
#define UTSNAME_H

#include <stdint.h>
#include <stddef.h>
#include <syscall.h>

struct utsname{
	char sysname[32];
};

int uname(struct utsname *buf);

#endif
