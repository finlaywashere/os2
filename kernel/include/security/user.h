#ifndef USER_H
#define USER_H

#include <stdint.h>
#include <stddef.h>

uint64_t get_uid();
uint64_t get_gid();
int is_superuser();

#endif
