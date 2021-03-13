#include <arch/x86_64/driver/keyboard.h>
#include <log.h>

ps2_device_t* ps2_devices;
int keyboard = 0;

char* keyboard_buffer;
int keyboard_offset = 0;

void send_command_noresp(uint16_t command){
	outb(COMMAND_PORT,command);
    if(command >> 8 != 0)
        outb(DATA_PORT, command >> 8);
}

uint8_t send_command(uint16_t command){
	outb(COMMAND_PORT,command);
	if(command >> 8 != 0)
		outb(DATA_PORT, command >> 8);
	while(1){
        uint8_t status = inb(COMMAND_PORT);
        if(status & 1)
            break;
    }
    uint8_t response = inb(DATA_PORT);
	return response;
}
void send_device_command_noresp(uint8_t command, uint8_t device){
	if(device == 2){
		send_command_noresp(0xD4);
	}
	outb(DATA_PORT,command);
}
uint8_t send_device_command(uint8_t command, uint8_t device){
    if(device == 2){
        send_command_noresp(0xD4);
    }
    outb(DATA_PORT,command);
	if(command >> 8 != 0)
        outb(DATA_PORT, command >> 8);
	while(1){
        uint8_t status = inb(COMMAND_PORT);
        if(status & 1 || status & (1 << 5))
            break;
    }
    uint8_t response = inb(DATA_PORT);
    return response;
}
uint16_t ps2_identify(uint8_t device){
	send_device_command(0xF2, device); // Identify
    uint64_t timeout = 0;
    uint16_t response2 = 0;
    int count = 0;
    while(1){
        if(timeout > 1000000)
            break;
        uint8_t status = inb(COMMAND_PORT);
        if(status & 1){
            if(count == 0){
                response2 = inb(DATA_PORT) << 8;
                timeout = 0;
            }else{
                response2 |= inb(DATA_PORT);
                count++;
                break;
            }
            count++;
        }
        timeout++;
    }
	return response2;
}
uint8_t scancode_to_key(uint8_t scancode){
    if(scancode > 0x67)
        return 0;
	if(scancode >= 2 && scancode <= 0x0A)
		return scancode - 2 + '1';
	switch(scancode){
		case 0x0B:
			return '0';
		case 0xC:
			return '-';
		case 0x0d:
			return '=';
		case 0x0e:
			return 1;
		case 0x0f:
			return '\t';
		case 0x10:
			return 'q';
		case 0x11:
			return 'w';
		case 0x12:
			return 'e';
		case 0x13:
			return 'r';
		case 0x14:
			return 't';
		case 0x15:
			return 'y';
		case 0x16:
			return 'u';
		case 0x17:
			return 'i';
		case 0x18:
			return 'o';
		case 0x19:
			return 'p';
		case 0x1a:
			return '[';
		case 0x1b:
			return ']';
		case 0x1c:
			return '\n';
		case 0x1e:
			return 'a';
		case 0x1f:
			return 's';
		case 0x20:
			return 'd';
		case 0x21:
			return 'f';
		case 0x22:
			return 'g';
		case 0x23:
			return 'h';
		case 0x24:
			return 'j';
		case 0x25:
			return 'k';
		case 0x26:
			return 'l';
		case 0x27:
			return ';';
		case 0x28:
			return '\'';
		case 0x29:
			return '`';
		case 0x2b:
			return '\\';
		case 0x2c:
			return 'z';
		case 0x2d:
			return 'x';
		case 0x2e:
			return 'c';
		case 0x2f:
			return 'v';
		case 0x30:
			return 'b';
		case 0x31:
			return 'n';
		case 0x32:
			return 'm';
		case 0x33:
			return ',';
		case 0x34:
			return '.';
		case 0x35:
			return '/';
	}
	return 0;
}

void keyboard_callback(registers_t* regs){
	uint8_t scan = inb(DATA_PORT);
	uint8_t key = scancode_to_key(scan);
	if(key == 0)
		return;
	if(key == 1){
		if(keyboard_offset == 0)
			return;
		// Backspace
		keyboard_buffer[keyboard_offset-1] = 0;
		keyboard_offset--;
	}
	keyboard_buffer[keyboard_offset] = key;
	keyboard_offset++;
}
void init_keyboard(){
	keyboard_buffer = (char*) kmalloc_p(4096);
	keyboard = 1;
	/*ps2_devices = (ps2_device_t*) kmalloc_p(sizeof(ps2_device_t)*2);
	send_command_noresp(0xAD); // Disable 1st PS/2 port
	send_command_noresp(0xA7); // Disable 2nd PS/2 port
	
	// Flush buffer
	while(inb(COMMAND_PORT) & 1){
		inb(DATA_PORT);
	}

	uint8_t configuration_byte = send_command(0x20); // Read config byte
	configuration_byte &= 0b10111100; // Clear bits 0, 1, and 6
	send_command_noresp(0x60 | (configuration_byte << 8)); // Write config byte

	// Perform PS/2 controller self tests
	uint8_t response = send_command(0xAA);
	if(response != 0x55){
		panic("PS/2 Keyboard failed self check!");
	}
	response = send_command(0xAB);
	if(response != 0x0){
		panic("PS/2 Port #1 failed self test!");
	}
	if(configuration_byte & (1 << 5)){
		response = send_command(0xA9);
		if(response != 0x0){
			panic("PS/2 Port #2 failed self test!");
		}
	}

	send_command_noresp(0xAE); // Enable 1st PS/2 port
	send_device_command(0xFF, 1);
	
	send_device_command(0xF5, 1); // Disable scanning
	uint16_t identity = ps2_identify(1); // Identify
	if(identity == 0xAB83){
		ps2_devices[0].type = PS2_KEYBOARD;
		keyboard = 1;
		send_device_command(0xF4,1); // Enable scanning
	}
	ps2_devices[0].id = 1;

	if(configuration_byte & (1 << 5)){
		// 2nd device is supported
		send_command_noresp(0xA8); // Enable 2nd PS/2 port
	    send_device_command(0xFF, 2);
		identity = ps2_identify(2); // Identify
        if(identity == 0xAB83){
            ps2_devices[1].type = PS2_KEYBOARD;
			if(keyboard == 0){
				keyboard = 2;
				send_device_command(0xF4,2); // Enable scanning
			}
        }
		ps2_devices[1].id = 2;
	}*/
	isr_register_handler(33,&keyboard_callback);
	//configuration_byte &= 0b00000011; // Set bits 0, 1
    //send_command_noresp(0x60 | (configuration_byte << 8)); // Write config byte
}
uint8_t read_ps2_keyboard(){
	if(keyboard == 0)
		panic("No keyboard found!");
	uint8_t key = keyboard_buffer[0];
	if(key == 0){
		return key;
	}
	for(uint64_t i = 1; i < keyboard_offset+1; i++){
		keyboard_buffer[i-1] = keyboard_buffer[i];
	}
	keyboard_offset--;
	return key;
}
