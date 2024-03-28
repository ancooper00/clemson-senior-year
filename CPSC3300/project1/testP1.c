#define MEM_SIZE_IN_WORDS 32 * 1024

#include <assert.h>
#include <stdio.h>
#include <string.h>
/* struct to help organize source and destination operand handling */
typedef struct ap {
    int mode;   // the lookup you're performing
    int reg;    // the register you're using
    int addr;   // location in memory
    int value;  // the value in the memory location
} phrase_data;

// GLOBALS //
// memory
int mem[MEM_SIZE_IN_WORDS];
// registers
int reg[8] = {0};
// current phrases being worked with
phrase_data src, dst;

// extra bs bein tracked
int instruction_executions;
int memory_reads;
int memory_writes;
int count;
int ogcount;
int branches_executed;
int branches_taken;

// nzvc :)
int n, v, z, c;

// execution style
char execution_style[5];

// GLOABLS //

void printRegister(void) {
    printf(
        "  R0:0%06o  R2:0%06o  R4:0%06o  R6:0%06o\n  R1:0%06o  R3:0%06o  "
        "R5:0%06o  R7:0%06o\n",
        reg[0], reg[2], reg[4], reg[6], reg[1], reg[3], reg[5],
        (reg[7] + 1) * 2);
}

void get_operand(phrase_data *phrase) {
    // direct addressing is on page 31 of 178
    // indirect addressing is on page 37 of 178
    int index_value;
    int temp = 0;
    int testVal = 1;
    int t1, t2;

    assert((phrase->mode >= 0) && (phrase->mode <= 7));
    assert((phrase->reg >= 0) && (phrase->reg <= 7));

    switch (phrase->mode) {
        /* register */
        case 0:
            phrase->value = reg[phrase->reg]; /*value is in the register */
            assert(phrase->value < 0200000);
            phrase->addr = 0; /* unused */

            break;

        /* register indirect */
        case 1:
            phrase->value =
                mem[reg[phrase->reg] / 2];   /* value is in the memory */
            phrase->addr = reg[phrase->reg]; /* address is in the register */

            memory_reads++;

            break;

        /* autoincrement (post reference) */
        case 2:
            phrase->addr = reg[phrase->reg]; /* address is in the register */
            if (phrase->reg == 7) {
                phrase->value = mem[phrase->addr + 1]; /* adjust to word addr */
                reg[phrase->reg]++;
            } else {
                phrase->value =
                    mem[(phrase->addr + 1) >> 1]; /* adjust to word addr */
                reg[phrase->reg] = (reg[phrase->reg] + 2) & 0177777;
            }
            break;

        /* autoincrement indirect */
        case 3:

            phrase->addr = reg[phrase->reg]; /* addr of addr is in reg */
            assert(phrase->addr < 0200000);
            phrase->addr = mem[phrase->addr >> 1]; /* adjust to word addr */
            assert(phrase->addr < 0200000);
            phrase->value = mem[phrase->addr >> 1]; /* adjust to word addr */
            assert(phrase->value < 0200000);
            reg[phrase->reg] = (reg[phrase->reg] + 2) & 0177777;

            if (phrase->reg == 7) {
                memory_reads++;
            } else {
                memory_reads += 2;
            }

            break;

        /* autodecrement */
        case 4:
            reg[phrase->reg] = (reg[phrase->reg] - 2) & 0177777;
            phrase->addr = reg[phrase->reg]; /* address is in the register */
            assert(phrase->addr < 0200000);
            phrase->value = mem[phrase->addr >> 1]; /* adjust to word addr */
            assert(phrase->value < 0200000);

            memory_reads++;

            break;

        /* autodecrement indirect */
        case 5:
            reg[phrase->reg] = (reg[phrase->reg] - 2) & 0177777;
            phrase->addr = reg[phrase->reg];       /* addr of addur is in reg */
            phrase->addr = mem[phrase->addr >> 1]; /* adjust to word addr */
            phrase->value = mem[phrase->addr >> 1]; /* adjust to word addr */

            memory_reads += 2;

            break;
        /* Rn+X is the address of the operand*/
        case 6:
            phrase->addr = reg[phrase->reg]; /* address is in the register */
            t1 = (short)mem[reg[7] + 1];
            t2 = (short)reg[phrase->reg];
            phrase->value =
                mem[(t1 >> 1) + (t2 >> 1)]; /* adjust to word addr */
            reg[7]++;

            memory_reads += 2;

            break;
        /* Rn+X is the address of the address of the operand */
        case 7:
            phrase->addr = reg[phrase->reg]; /* address is in the register */
            t1 = (short)mem[reg[7] + 1];     // modification value
            t2 = (short)reg[phrase->reg];
            phrase->value =
                mem[mem[(t1 >> 1) + (t2 >> 1)] >> 1]; /* adjust to word addr */
            reg[7]++;
            memory_reads += 2;
            break;
        default:
            printf("ERROR: invalid mode\n");
            break;
    }
}

void update_operand(phrase_data *phrase, int result) {
    // direct addressing is on page 31 of 178
    // indirect addressing is on page 37 of 178
    int address;
    switch (phrase->mode) {
        /* register */
        case 0:
        case 4:
        case 6:
            reg[phrase->reg] = result;
            break;

        /* register indirect */
        case 1:
        case 2:
        case 3:
        case 5:
        case 7:
            address = reg[phrase->reg];
            mem[(address / 2) - 1] = result;
            memory_writes++;
            break;
        default:
            printf("error\n");
    }
}

void performOperation(int instruction, int size) {
    int result;
    if (size == 4) {
        switch (instruction) {
            case 01: {  // MOV
                result = src.value;
                result = result & 0177777;
                n = src.value << 16 < 0 ? 1 : 0;
                z = src.value == 0 ? 1 : 0;
                v = 0;
                update_operand(&dst, result);
                if (!strcmp(execution_style, "-t") ||
                    !strcmp(execution_style, "-v")) {
                    printf("mov instruction ");
                    printf("sm %o, sr %o dm %o dr %o\n", src.mode, src.reg,
                           dst.mode, dst.reg);
                    if (!strcmp(execution_style, "-v")) {
                        printf("  src.value = 0%06o\n", src.value);
                        printf("  nzvc bits = 4'b%o%o%o%o\n", n, z, v, c);
                        if (dst.addr > 7) {
                            printf("  value 0%06o is written to 0%06o\n",
                                   result, dst.addr);
                        }
                        printRegister();
                    }
                }
                break;
            }
            case 06: {  // ADD
                result = dst.value + src.value;
                c = (result & 0200000) >> 16;
                result = result & 0177777;
                int sign_bit = result & 0100000;
                n = 0;
                if (sign_bit) n = 1;
                z = 0;
                if (result == 0) z = 1;
                v = 0;
                if ((dst.value & 0100000) == (src.value & 0100000) &&
                    (dst.value & 0100000) != (result & 0100000)) {
                    v = 1;
                }
                update_operand(&dst, result);
                if (!strcmp(execution_style, "-t") ||
                    !strcmp(execution_style, "-v")) {
                    printf("add instruction ");
                    printf("sm %o, sr %o dm %o dr %o\n", src.mode, src.reg,
                           dst.mode, dst.reg);
                    if (!strcmp(execution_style, "-v")) {
                        printf("  src.value = 0%06o\n", src.value);
                        printf("  dst.value = 0%06o\n", dst.value);
                        printf("  result    = 0%06o\n", result);
                        printf("  nzvc bits = 4'b%o%o%o%o\n", n, z, v, c);
                        printRegister();
                    }
                }
                break;
            }
            case 016: {  // SUB
                result = dst.value - src.value;
                c = (result & 0200000) >> 16;
                result = result & 0177777;
                int sign_bit = result & 0100000;
                n = 0;
                if (sign_bit) n = 1;
                z = 0;
                z = !result;
                v = 0;
                if (((src.value & 0100000) != (dst.value & 0100000)) &&
                    ((src.value & 0100000) == (result & 0100000))) {
                    v = 1;
                }
                update_operand(&dst, result);
                if (!strcmp(execution_style, "-t") ||
                    !strcmp(execution_style, "-v")) {
                    printf("sub instruction ");
                    printf("sm %o, sr %o dm %o dr %o\n", src.mode, src.reg,
                           dst.mode, dst.reg);
                    if (!strcmp(execution_style, "-v")) {
                        printf("  src.value = 0%06o\n", src.value);
                        printf("  dst.value = 0%06o\n", dst.value);
                        printf("  result    = 0%06o\n", result);
                        printf("  nzvc bits = 4'b%o%o%o%o\n", n, z, v, c);
                        printRegister();
                    }
                }
                break;
            }
            case 02: {  // CMP
                if (src.value < dst.value) {
                    result = (~src.value - (dst.value - 1)) & 0177777;
                } else
                    result = src.value - dst.value;
                n = (result & (1 << 15)) ? 1 : 0;
                z = result == 0 ? 1 : 0;
                v = 0;
                if ((dst.value & 0100000) != (src.value & 0100000) &&
                    (dst.value & 0100000) == (result & 0100000)) {
                    v = 1;
                }
                c = (result & (1 << 15)) >> 15;
                if (!strcmp(execution_style, "-t") ||
                    !strcmp(execution_style, "-v")) {
                    printf("cmp instruction ");
                    printf("sm %o, sr %o dm %o dr %o\n", src.mode, src.reg,
                           dst.mode, dst.reg);
                    if (!strcmp(execution_style, "-v")) {
                        printf("  src.value = 0%06o\n", src.value);
                        printf("  dst.value = 0%06o\n", dst.value);
                        printf("  result    = 0%06o\n", result);
                        printf("  nzvc bits = 4'b%o%o%o%o\n", n, z, v, c);
                        printRegister();
                    }
                }
            }
        }
    } else if (size == 7) {  // SOB
        if (!strcmp(execution_style, "-t") || !strcmp(execution_style, "-v")) {
            printf("sob instruction reg %o ", src.reg);
            printf("with offset %03o\n", src.value);
        }
        reg[src.reg]--;
        branches_executed++;
        if (reg[src.reg] != 0) {
            reg[7] -= src.value;
            count += src.value;
            branches_taken++;
        }
        if (!strcmp(execution_style, "-v")) printRegister();

    } else if (size == 8) {
        switch (instruction) {
            case 01:  // BR
                if (!strcmp(execution_style, "-t") ||
                    !strcmp(execution_style, "-v")) {
                    printf("br instruction ");
                    printf("with offset 0%03o\n", src.value);
                }

                branches_executed++;
                int offset = src.value;
                if (offset & (1 << 7)) {
                    offset = (~offset & 0377) + 1;
                    result = reg[7] - offset;
                    count += offset;
                } else {
                    result = reg[7] + offset;
                    count -= offset;
                }
                reg[7] = result;
                branches_taken++;
                if (!strcmp(execution_style, "-v")) {
                    printRegister();
                }
                break;
            case 02:  // BNE
                if (!strcmp(execution_style, "-t") ||
                    !strcmp(execution_style, "-v")) {
                    printf("bne instruction ");
                    printf("with offset 0%03o\n", src.value);
                }
                branches_executed++;
                if (!z) {
                    int offset = src.value;
                    if (offset & (1 << 7)) {
                        offset = (~offset & 0377) + 1;
                        result = reg[7] - offset;
                        count += offset;
                    } else {
                        result = reg[7] + offset;
                        count -= offset;
                    }
                    reg[7] = result;
                    branches_taken++;
                }
                if (!strcmp(execution_style, "-v")) {
                    printRegister();
                }
                break;
            case 03:  // BEQ
                if (!strcmp(execution_style, "-t") ||
                    !strcmp(execution_style, "-v")) {
                    printf("beq instruction ");
                    printf("with offset 0%03o\n", src.value);
                }
                branches_executed++;
                if (z) {
                    int offset = src.value;
                    if (offset & (1 << 7)) {
                        offset = (~offset & 0377) + 1;
                        result = reg[7] - offset;
                        count += offset;
                    } else {
                        result = reg[7] + offset;
                        count -= offset;
                    }
                    reg[7] = result;
                    branches_taken++;
                }
                if (!strcmp(execution_style, "-v")) {
                    printRegister();
                }
                break;
            default:
                printf("unknown instruction\n");
                assert(0 == 1);
                break;
        }
    } else if (size == 10) {
        switch (instruction) {
            case 0063: {
                result = src.value << 1;
                result = result & 0177777;
                n = result << 16 < 0 ? 1 : 0;
                z = result == 0 ? 1 : 0;
                c = (src.value & (1 << 15)) >> 15;
                v = n ^ c;
                update_operand(&src, result);
                if (!strcmp(execution_style, "-t") ||
                    !strcmp(execution_style, "-v")) {
                    printf("asl instruction ");

                    printf("dm %o dr %o\n", src.mode, src.reg);
                    if (!strcmp(execution_style, "-v")) {
                        printf("  dst.value = 0%06o\n", src.value);
                        printf("  result    = 0%06o\n", result);
                        printf("  nzvc bits = 4'b%o%o%o%o\n", n, z, v, c);
                        printRegister();
                    }
                }
                break;
            }
            case 0062: {
                result = src.value >> 1;
                result = result & 0177777;
                result = result | (1 << 15);
                c = src.value & 1;
                n = result << 16 < 0 ? 1 : 0;
                z = result == 0 ? 1 : 0;
                v = n ^ c;
                update_operand(&src, result);
                if (!strcmp(execution_style, "-t") ||
                    !strcmp(execution_style, "-v")) {
                    printf("asr instruction ");
                    printf("dm %o dr %o\n", src.mode, src.reg);
                    if (!strcmp(execution_style, "-v")) {
                        printf("  dst.value = 0%06o\n", src.value);
                        printf("  result    = 0%06o\n", result);
                        printf("  nzvc bits = 4'b%o%o%o%o\n", n, z, v, c);
                        printRegister();
                    }
                }
                break;
            }
            default: {
                printf("unknown action\n");
                assert(1 == 0);
                break;
            }
        }
    }
}

void parseCode() {
    instruction_executions++;
    if (!strcmp(execution_style, "-t") || !strcmp(execution_style, "-v")) {
        printf("at 0%04o, ", reg[7] * 2);
    }
    int instruction10b = mem[reg[7]] >> 6;
    int instruction8b = mem[reg[7]] >> 8;
    int instruction7b = mem[reg[7]] >> 9;
    int instruction4b = mem[reg[7]] >> 12;

    if (instruction10b == 0062 || instruction10b == 0063) {
        src.mode = (mem[reg[7]] >> 3) & 07;
        src.reg = mem[reg[7]] & 07;
        get_operand(&src);
        performOperation(instruction10b, 10);
    } else if (instruction7b == 077) {
        src.reg = (mem[reg[7]] >> 6) & 07;
        src.value = (mem[reg[7]] & 077);
        performOperation(instruction7b, 7);
    } else if (instruction8b == 001 || instruction8b == 002 ||
               instruction8b == 003) {
        src.value = (mem[reg[7]] & 0377);
        performOperation(instruction8b, 8);

    } else {
        src.mode = (mem[reg[7]] >> 9) & 07;
        src.reg = (mem[reg[7]] >> 6) & 07;
        dst.mode = (mem[reg[7]] >> 3) & 07;
        dst.reg = mem[reg[7]] & 07;
        get_operand(&src);
        get_operand(&dst);
        performOperation(instruction4b, 4);
    }
}

int main(int argc, char *argv[]) {
    /* initialize memory with the word index for this test */
    /*   => will equal half the value of the address       */
    if (argv[1] != NULL) strcpy(execution_style, argv[1]);

    for (int i = 0; i < MEM_SIZE_IN_WORDS; i++) mem[i] = 0;

    if (!strcmp(execution_style, "-v"))
        printf("reading words in octal from stdin:\n");
    int ir;
    while (scanf("%o", &ir) != EOF) {
        if (!strcmp(execution_style, "-v")) printf("  0%06o\n", ir);
        mem[count++] = ir;
    }
    ogcount = count;
    if (!strcmp(execution_style, "-t") || !strcmp(execution_style, "-v"))
        printf("\ninstruction trace:\n");
    for (reg[7] = 0; mem[reg[7]] != 0 && reg[7] < 1000; reg[7]++) {
        parseCode();
    }

    if (!strcmp(execution_style, "-t") || !strcmp(execution_style, "-v")) {
        printf("at 0%04o, halt instruction\n", reg[7] * 2);
        if (!strcmp(execution_style, "-v")) printRegister();
        printf("\n");
    }
    instruction_executions++;

    printf("execution statistics (in decimal):\n");
    printf("  instructions executed     = %d\n", instruction_executions);
    printf("  instruction words fetched = %d\n", count);
    printf("  data words read           = %d\n", memory_reads);
    printf("  data words written        = %d\n", memory_writes);
    printf("  branches executed         = %d\n", branches_executed);
    if (branches_executed > 0)
        printf("  branches taken            = %d (%0.1lf%%)\n", branches_taken,
               ((double)branches_taken / branches_executed) * 100);
    else
        printf("  branches taken            = %d\n", branches_taken);
    if (!strcmp(execution_style, "-v")) {
        printf("\nfirst 20 words of memory after execution halts:\n");
        for (int i = 0; i < 20; i++) printf("  0%04o: %06o\n", i * 2, mem[i]);
    }
    return 0;
}
