#include <log.h>

void panic(char* message){
	panicl(message,strlen(message));
}
void log_error(char* message){
	log_errorl(message,strlen(message));
}
void log_error_num(uint64_t num, uint8_t base){
	uint64_t len = numlen(num,base);
        char* buf = (char*) kmalloc_p(len);
        int_to_str(buf,num,base);
        log_error(buf);
        kfree_p((void*) buf,len);
}
void log_debug(char* message){
	log_debugl(message,strlen(message));
}
void log_warn(char* message){
	log_warnl(message,strlen(message));
}
void log_verb(char* message){
	log_verbl(message,strlen(message));
}
void log(char* message, uint8_t foreground, uint8_t background){
	logl(message,foreground,background,strlen(message));
}

void panicl(char* message,uint64_t len){
	char* m2 = "Kernel Panic: ";
	log_errorl(m2, strlen(m2));
	log_errorl(message, len);
	asm volatile ("cli"); // Disable interrupts
	while(1){}
}
void log_errorl(char* message, uint64_t len){
	logl(message, VGA_COLOUR_WHITE, VGA_COLOUR_RED, len);
	
}
void log_warnl(char* message, uint64_t len){
	logl(message, VGA_COLOUR_WHITE, VGA_COLOUR_BLUE, len);
}
void log_debugl(char* message, uint64_t len){
	logl(message, VGA_COLOUR_YELLOW, VGA_COLOUR_BLACK, len);
}
void log_verbl(char* message, uint64_t len){
	logl(message, VGA_COLOUR_WHITE, VGA_COLOUR_BLACK, len);
}
void logl(char* message, uint8_t foreground, uint8_t background, uint64_t len){
	uint16_t start = tty_getcolour(OUTPUT_TTY);
        tty_setcolour(OUTPUT_TTY,foreground,background);
	tty_putchars(OUTPUT_TTY,message,len);
	tty_setcolour(OUTPUT_TTY,(uint8_t)start,(uint8_t)(start>>8));
}
