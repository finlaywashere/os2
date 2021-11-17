#include <process/signal.h>
#include <process/process.h>

int register_signal(uint64_t address, uint64_t index, uint64_t flags, uint64_t condition){
	process_t* process = get_process();
	signal_handler_t *handler = &process->signal_handlers[index];
	handler->handler = address;
	handler->flags = flags;
	handler->condition = condition;
	return 0;
}
int get_signal(uint64_t flags){
	process_t* process = get_process();
	for(int i = 0; i < MAX_SIGNALS; i++){
		signal_handler_t *handler = &process->signal_handlers[i];
		if(handler->flags & flags)
			return i;
	}
	return -1;
}
int get_error_signal(){
	return get_signal(HANDLER_ERROR);
}
