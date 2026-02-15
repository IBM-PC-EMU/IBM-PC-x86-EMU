// The x86 emulator
// Currently targets the 8088

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

// Registers
uint16_t ax, bx, cx, dx;
uint16_t si, di, bp, sp;
uint16_t flags;
uint16_t cs, ds, es, ss;
uint16_t ip;

bool running = true;

// Memory map
char memory[1048576]; // 1 MB of memory

// Read a byte from memory
uint8_t read_byte(uint16_t segment, uint16_t offset) {
    uint32_t address = (segment << 4) + offset;
    return memory[address];
}

// Read a word from memory
uint16_t read_word(uint16_t segment, uint16_t offset) {
    uint32_t address = (segment << 4) + offset;
    return memory[address] | (memory[address + 1] << 8);
}

// Write a byte to memory
void write_byte(uint16_t segment, uint16_t offset, uint8_t value) {
    uint32_t address = (segment << 4) + offset;
    memory[address] = value;
}

// Write a word to memory
void write_word(uint16_t segment, uint16_t offset, uint16_t value) {
    uint32_t address = (segment << 4) + offset;
    memory[address] = value & 0xFF;
    memory[address + 1] = (value >> 8) & 0xFF;
}

// Execute an instruction at the current IP
void execute_instruction() {
    uint8_t opcode = read_byte(cs, ip);
    ip++;

    switch (opcode) {
        case 0xCD: // INT imm8
            {
                uint8_t int_num = read_byte(cs, ip);
                ip++;
                printf("Interrupt: %02X\n", int_num);
            }
            break;
        case 0x90: // NOP
            break;
        case 0xF4: // HLT
            printf("HLT encountered. Halting execution.\n");
            // Stops execution
            running = false;
            break;
        case 0xFA: // CLI
            printf("CLI encountered. Disabling interrupts.\n");
            flags &= ~0x200; // Clear the interrupt flag (IF)
            break;
        default:
            printf("Unknown opcode: %02X\n", opcode);
            break;
    }
}

void main() 
{
    // Load a simple program into memory (for testing)
    memory[0x0000] = 0xCD; // INT 0x10
    memory[0x0001] = 0x10;
    memory[0x0002] = 0xFA; // CLI
    memory[0x0003] = 0xF4; // HLT

    cs = 0x0000; // Code segment starts at 0
    ip = 0x0000; // Instruction pointer starts at 0

    flags = 0x200 ; // Set the interrupt flag (IF) to enable interrupts

    while (running) {
        execute_instruction();
    }

    if (!(flags & 0x200))
    {
        // Check if the interrupt flag is not set
        printf("Machine freeze due to halting with disabled interrupts.\n");
    }
}