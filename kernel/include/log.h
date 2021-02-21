#ifndef LOG_H
#define LOG_H

#include <stdint.h>
#include <stddef.h>
#include <arch/x86_64/tty.h>
#include <utils.h>

#define OUTPUT_TTY 0

void panic(char* message);
void log_error(char* message);
void log_error_num(uint64_t num, uint8_t base);
void log_warn(char* message);
void log_debug(char* message);
void log_verb(char* message);
void log(char* message, uint8_t foreground, uint8_t background);

void panicl(char* message, uint64_t len);
void log_errorl(char* message, uint64_t len);
void log_warnl(char* message, uint64_t len);
void log_debugl(char* message, uint64_t len);
void log_verbl(char* message, uint64_t len);
void logl(char* message, uint8_t foreground, uint8_t background, uint64_t len);

#endif
