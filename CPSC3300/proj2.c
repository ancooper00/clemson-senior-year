#include <assert.h>
#include <stdio.h>
#include <string.h>

#define LINES_PER_BANK 16

unsigned int plru_state[LINES_PER_BANK], /* current state for each set */
    valid[4][LINES_PER_BANK],            /* valid bit for each line    */
    dirty[4][LINES_PER_BANK],            /* dirty bit for each line    */
    tag[4][LINES_PER_BANK],              /* tag bits for each line     */

    plru_bank[8]

    = {0, 0, 1, 1, 2, 3, 2, 3},

    next_state[32] = {6, 4, 1, 0, 7, 5, 1, 0, 6, 4, 3, 2, 7, 5, 3, 2,
                      6, 4, 1, 0, 7, 5, 1, 0, 6, 4, 3, 2, 7, 5, 3, 2};

unsigned int cache_reads, /* counter */
    cache_writes,         /* counter */
    hits,                 /* counter */
    misses,               /* counter */
    write_backs;          /* counter */

void cache_init(void)
{
    int i;
    for (i = 0; i < LINES_PER_BANK; i++)
    {
        plru_state[i] = 0;
        valid[0][i] = dirty[0][i] = tag[0][i] = 0;
        valid[1][i] = dirty[1][i] = tag[1][i] = 0;
        valid[2][i] = dirty[2][i] = tag[2][i] = 0;
        valid[3][i] = dirty[3][i] = tag[3][i] = 0;
    }
    cache_reads = cache_writes = hits = misses = write_backs = 0;
}

void cache_stats(void)
{
    printf("cache statistics (in decimal):\n");
    printf("  cache reads       = %d\n", cache_reads);
    printf("  cache writes      = %d\n", cache_writes);
    printf("  cache hits        = %d\n", hits);
    printf("  cache misses      = %d\n", misses);
    printf("  cache write backs = %d\n", write_backs);
}

/* address is byte address, type is read (=0) or write (=1) */

void cache_access(unsigned int address, unsigned int type)
{
    unsigned int addr_tag, /* tag bits of address     */
        addr_index,        /* index bits of address   */
        bank;              /* bank that hit, or bank chosen for replacement */

    if (type == 0)
    {
        cache_reads++;
    }
    else
    {
        cache_writes++;
    }

    addr_index = (address >> 3) & (LINES_PER_BANK - 1); // TODO: possible failure point
    addr_tag = address >> 7;

    /* check bank 0 hit */

    if (valid[0][addr_index] && (addr_tag == tag[0][addr_index]))
    {
        hits++;
        bank = 0;

        /* check bank 1 hit */
    }
    else if (valid[1][addr_index] && (addr_tag == tag[1][addr_index]))
    {
        hits++;
        bank = 1;

        /* check bank 2 hit */
    }
    else if (valid[2][addr_index] && (addr_tag == tag[2][addr_index]))
    {
        hits++;
        bank = 2;

        /* check bank 3 hit */
    }
    else if (valid[3][addr_index] && (addr_tag == tag[3][addr_index]))
    {
        hits++;
        bank = 3;

        /* miss - choose replacement bank */
    }
    else
    {
        misses++;

        if (!valid[0][addr_index])
            bank = 0;
        else if (!valid[1][addr_index])
            bank = 1;
        else if (!valid[2][addr_index])
            bank = 2;
        else if (!valid[3][addr_index])
            bank = 3;
        else
            bank = plru_bank[plru_state[addr_index]];

        if (valid[bank][addr_index] && dirty[bank][addr_index])
        {
            write_backs++;
        }

        valid[bank][addr_index] = 1;
        dirty[bank][addr_index] = 0;
        tag[bank][addr_index] = addr_tag;
    }

    /* update replacement state for this set (i.e., index value) */

    plru_state[addr_index] = next_state[(plru_state[addr_index] << 2) | bank];

    /* update dirty bit on a write */

    if (type == 1)
        dirty[bank][addr_index] = 1;
}

#define MEM_SIZE_IN_WORDS 32 * 1024

typedef struct ap
{
    int mode;
    int reg;
    int addr;
    int value;
} addr_phrase_t;

typedef struct in
{
    char instruction[4];
    int source_mode;
    int source_register;
    int destination_mode;
    int destination_register;
} Input;

void get_operand(addr_phrase_t *phrase, int is_src);
void update_operand(addr_phrase_t *phrase, int val);
void print_registers();

int mem[MEM_SIZE_IN_WORDS];
int reg[8] = {0};
int ir;
int total_inputs = 0;
int current_mem = 0;
int n = 0, z = 0, v = 0, c = 0;
int read_memory = 0;
int write_memory = 0;
int branches_executed = 0;
int branches_taken = 0;
int instructions_executed = 0;
int instructions_fetched = 0;
char input_flag[3];
addr_phrase_t src, dst;

int main(int argc, char *argv[])
{
    if (argv[1] != NULL)
        strcpy(input_flag, argv[1]);
    int halt = 0;
    int result;
    Input input;
    if (!strcmp(input_flag, "-v"))
        printf("reading words in octal from stdin:\n");
    int counter = 0;
    int offset = 0;
    while (scanf("%o", &mem[counter]) != EOF)
    {
        if (!strcmp(input_flag, "-v"))
            printf("  0%06o\n", mem[counter]);
        total_inputs += 1;
        counter += 2;
    }

    if (!strcmp(input_flag, "-v") || !strcmp(input_flag, "-t"))
        printf("\ninstruction trace:\n");
    while (!halt)
    {
        ir = mem[current_mem];
        instructions_executed++;
        src.mode = (ir >> 9) & 07;
        src.reg = (ir >> 6) & 07;
        dst.mode = (ir >> 3) & 07;
        dst.reg = ir & 07;

        if ((ir >> 12) == 01)
        {
            if (!strcmp(input_flag, "-v") || !strcmp(input_flag, "-t"))
                printf("at 0%04o, mov instruction sm %i, sr %i dm %i dr %i\n",
                       current_mem, src.mode, src.reg, dst.mode, dst.reg);
            get_operand(&src, 1);
            get_operand(&dst, 0);
            if (!strcmp(input_flag, "-v"))
                printf("  src.value = 0%06o\n", src.value);
            result = src.value & 0177777;
            n = src.value << 16 < 0;
            z = !src.value;
            v = 0;
            update_operand(&dst, result);
            reg[7] += 2;
            current_mem += 2;
            if (!strcmp(input_flag, "-v"))
                printf("  nzvc bits = 4'b%i%i%i%i\n", n, z, v, c);
            if (dst.addr > 7)
            {
                if (!strcmp(input_flag, "-v"))
                    printf("  value 0%06o is written to 0%06o\n", result,
                           dst.addr);
            }
            if (!strcmp(input_flag, "-v"))
                print_registers();
        }
        else if ((ir >> 12) == 02)
        {
            if (!strcmp(input_flag, "-v") || !strcmp(input_flag, "-t"))
                printf("at 0%04o, cmp instruction sm %i, sr %i dm %i dr %i\n",
                       current_mem, src.mode, src.reg, dst.mode, dst.reg);
            get_operand(&src, 1);
            get_operand(&dst, 0);
            if (!strcmp(input_flag, "-v"))
            {
                printf("  src.value = 0%06o\n", src.value);
                printf("  dst.value = 0%06o\n", dst.value);
            }
            if (src.value >= dst.value)
                result = src.value - dst.value;
            else
                result = (~src.value - (dst.value - 1)) & 0177777;
            if (!strcmp(input_flag, "-v"))
                printf("  result    = 0%06o\n", result);
            n = result << 16 < 0;
            z = !result;
            v = ((src.value > 0 && dst.value < 0 && result < 0) ||
                 (src.value < 0 && dst.value > 0 && result > 0));
            c = (result & (1 << 15)) >> 15;

            reg[7] += 2;
            current_mem += 2;
            if (!strcmp(input_flag, "-v"))
            {
                printf("  nzvc bits = 4'b%i%i%i%i\n", n, z, v, c);
                print_registers();
            }
        }
        else if ((ir >> 12) == 06)
        {
            if (!strcmp(input_flag, "-v") || !strcmp(input_flag, "-t"))
                printf("at 0%04o, add instruction sm %i, sr %i dm %i dr %i\n",
                       current_mem, src.mode, src.reg, dst.mode, dst.reg);
            get_operand(&src, 1);
            get_operand(&dst, 0);
            if (!strcmp(input_flag, "-v"))
            {
                printf("  src.value = 0%06o\n", src.value);
                printf("  dst.value = 0%06o\n", dst.value);
            }
            result = src.value + dst.value;
            v = ((dst.value & 0100000) == (src.value & 0100000) &&
                 (dst.value & 0100000) != (result & 0100000));
            c = (result & 0200000) >> 16;
            result = result & 0177777;
            if (!strcmp(input_flag, "-v"))
                printf("  result    = 0%06o\n", result);
            n = (result << 16 < 0);
            z = !result;

            update_operand(&dst, result);
            reg[7] += 2;
            current_mem += 2;
            if (!strcmp(input_flag, "-v"))
            {
                printf("  nzvc bits = 4'b%i%i%i%i\n", n, z, v, c);
                print_registers();
            }
        }
        else if ((ir >> 12) == 016)
        {
            if (!strcmp(input_flag, "-v") || !strcmp(input_flag, "-t"))
                printf("at 0%04o, sub instruction sm %i, sr %i dm %i dr %i\n",
                       current_mem, src.mode, src.reg, dst.mode, dst.reg);
            get_operand(&src, 1);
            get_operand(&dst, 0);
            if (!strcmp(input_flag, "-v"))
            {
                printf("  src.value = 0%06o\n", src.value);
                printf("  dst.value = 0%06o\n", dst.value);
            }
            result = dst.value - src.value;
            c = (result & (1 << 15)) >> 15;
            result = result & 0177777;
            if (!strcmp(input_flag, "-v"))
                printf("  result    = 0%06o\n", result);
            n = (result << 16 < 0);
            z = !result;
            v = ((src.value > 0 && dst.value < 0 && result < 0) ||
                 (src.value < 0 && dst.value > 0 && result > 0));

            update_operand(&dst, result);
            reg[7] += 2;
            current_mem += 2;
            if (!strcmp(input_flag, "-v"))
            {
                printf("  nzvc bits = 4'b%i%i%i%i\n", n, z, v, c);
                print_registers();
            }
        }
        else if ((ir >> 9) == 077)
        {
            offset = ir & 037; /* 8-bit signed offset */
            if (!strcmp(input_flag, "-v") || !strcmp(input_flag, "-t"))
                printf("at 0%04o, sob instruction reg %i with offset %03o\n",
                       current_mem, dst.reg, offset);
            offset = offset << 24; /* sign extend to 32 bits */
            offset = offset >> 24;
            // get_operand(&src);
            reg[7] += 2;
            reg[src.reg]--;
            branches_executed++;
            if (reg[src.reg] != 0)
            {
                reg[7] -= offset * 2;
                branches_taken++;
            }
            current_mem = reg[7];
            if (!strcmp(input_flag, "-v"))
                print_registers();
        }
        else if ((ir >> 8) == 01)
        {
            offset = ir & 0377; /* 8-bit signed offset */
            if (!strcmp(input_flag, "-v") || !strcmp(input_flag, "-t"))
                printf("at 0%04o, br instruction with offset 0%03o\n",
                       current_mem, offset);
            offset = offset << 24; /* sign extend to 32 bits */
            offset = offset >> 24;
            branches_executed++;
            reg[7] = (reg[7] + (offset << 1)) & 0177777;
            branches_taken++;

            reg[7] += 2;
            current_mem = reg[7];
            if (!strcmp(input_flag, "-v"))
                print_registers();
        }
        else if ((ir >> 8) == 02)
        {
            offset = ir & 0377; /* 8-bit signed offset */
            if (!strcmp(input_flag, "-v") || !strcmp(input_flag, "-t"))
                printf("at 0%04o, bne instruction with offset 0%03o\n",
                       current_mem, offset);

            branches_executed++;
            if (!z)
            {
                offset = offset << 24; /* sign extend to 32 bits */
                offset = offset >> 24;
                reg[7] = (reg[7] + (offset << 1)) & 0177777;
                branches_taken++;
            }
            reg[7] += 2;
            current_mem = reg[7];
            if (!strcmp(input_flag, "-v"))
                print_registers();
        }
        else if ((ir >> 8) == 03)
        {
            if (!strcmp(input_flag, "-v") || !strcmp(input_flag, "-t"))
                printf("at 0%04o, beq instruction with offset 0%03o\n",
                       current_mem, offset);

            branches_executed++;
            if (z)
            {
                offset = offset << 24; /* sign extend to 32 bits */
                offset = offset >> 24;
                reg[7] = (reg[7] + (offset << 1)) & 0177777;
                branches_taken++;
            }
            reg[7] += 2;
            current_mem = reg[7];
            if (!strcmp(input_flag, "-v"))
                print_registers();
        }
        else if ((ir >> 6) == 062)
        {
            if (!strcmp(input_flag, "-v") || !strcmp(input_flag, "-t"))
                printf("at 0%04o, asr instruction dm %i dr %i\n", current_mem,
                       dst.mode, dst.reg);
            get_operand(&dst, 0);
            if (!strcmp(input_flag, "-v"))
                printf("  dst.value = 0%06o\n", dst.value);
            result = ((dst.value >> 1) & 0177777) | (1 << 15);
            if (!strcmp(input_flag, "-v"))
                printf("  result    = 0%06o\n", result);
            n = (result << 16 < 0);
            z = !result;
            c = dst.value & 1;
            v = n ^ c;

            update_operand(&dst, result);
            reg[7] += 2;
            current_mem += 2;
            if (!strcmp(input_flag, "-v"))
            {
                printf("  nzvc bits = 4'b%i%i%i%i\n", n, z, v, c);
                print_registers();
            }
        }
        else if ((ir >> 6) == 063)
        {
            if (!strcmp(input_flag, "-v") || !strcmp(input_flag, "-t"))
                printf("at 0%04o, asl instruction dm %i dr %i\n", current_mem,
                       dst.mode, dst.reg);
            get_operand(&dst, 0);
            if (!strcmp(input_flag, "-v"))
                printf("  dst.value = 0%06o\n", dst.value);
            result = (dst.value << 1) & 0177777;
            if (!strcmp(input_flag, "-v"))
                printf("  result    = 0%06o\n", result);
            n = (result << 16 < 0);
            z = !result;
            c = (dst.value & (1 << 15)) >> 15;
            v = n ^ c;

            update_operand(&dst, result);
            reg[7] += 2;
            current_mem += 2;
            if (!strcmp(input_flag, "-v"))
            {
                printf("  nzvc bits = 4'b%i%i%i%i\n", n, z, v, c);
                print_registers();
            }
        }
        else if (ir == 0)
        {
            if (!strcmp(input_flag, "-v") || !strcmp(input_flag, "-t"))
                printf("at 0%04o, halt instruction\n", current_mem);
            reg[7] += 2;
            current_mem += 2;
            if (!strcmp(input_flag, "-v"))
                print_registers();
            halt = 1;
        }
        else
        {
            printf("no match\n");
        }
        instructions_fetched++;
        cache_access(current_mem, 0);
    }

    printf("\nexecution statistics (in decimal):\n");
    printf("  instructions executed     = %d\n", instructions_executed);
    printf("  instruction words fetched = %d\n", instructions_fetched);
    printf("  data words read           = %d\n", read_memory);
    printf("  data words written        = %d\n", write_memory);
    printf("  branches executed         = %d\n", branches_executed);
    if (branches_executed > 0)
    {
        printf("  branches taken            = %d (%.1f%%)\n", branches_taken,
               ((float)branches_taken / (float)branches_executed) * (float)100);
    }
    else
        printf("  branches taken            = %d\n", branches_taken);

    if (!strcmp(input_flag, "-v"))
    {
        printf("\nfirst 20 words of memory after execution halts:\n");
        for (int i = 0; i < 40; i += 2)
        {
            printf("  0%04o: %06o\n", i, mem[i]);
        }
    }
    cache_stats();
}

void get_operand(addr_phrase_t *phrase, int is_src)
{
    assert((phrase->mode >= 0) && (phrase->mode <= 7));
    assert((phrase->reg >= 0) && (phrase->reg <= 7));
    int x;
    int new_address;

    switch (phrase->mode)
    {
    // register
    case 0:
        phrase->value = reg[phrase->reg];
        phrase->addr = 0; /* unused */
        break;

    // register deferred
    case 1:
        phrase->addr = reg[phrase->reg];
        phrase->value = mem[phrase->addr]; // TODO: maybe divide by 2
        if (is_src)
        {
            read_memory++;
            cache_access(phrase->addr, 0);
        }
        break;

    // autoincrement (post reference)
    case 2:
        phrase->addr = reg[phrase->reg];
        if (phrase->reg == 7)
        {
            current_mem += 2;
            phrase->value = mem[current_mem];
            reg[7] += 2;
            cache_access(current_mem, 0);
            instructions_fetched++;
            // printf("====================================================
            // FETCHING 1\n");
        }
        else
        {
            phrase->value = mem[phrase->addr];
            /* adjust to word addr */ // TODO: maybe divide by 2
            reg[phrase->reg] = (reg[phrase->reg] + 2) & 0177777;
        }
        break;

    // autoincrement deferred
    case 3:
        phrase->addr = reg[phrase->reg];
        phrase->addr = mem[phrase->addr];  // TODO: maybe divide by 2
        phrase->value = mem[phrase->addr]; // TODO: maybe divide by 2
        reg[phrase->reg] = (reg[phrase->reg] + 2) & 0177777;
        if (phrase->reg == 7)
        {
            read_memory++;
            cache_access(phrase->addr, 0);
        }
        else
        {
            read_memory += 2;
            cache_access(phrase->addr, 0);
        }
        break;

    // autodecrement
    case 4:
        reg[phrase->reg] = (reg[phrase->reg] - 2) & 0177777;
        phrase->addr = reg[phrase->reg];
        phrase->value = mem[phrase->addr]; // TODO: maybe divide by 2
        read_memory++;
        cache_access(phrase->addr, 0);
        break;

    // autodecrement deferred
    case 5:
        reg[phrase->reg] = (reg[phrase->reg] - 2) & 0177777;
        phrase->addr = reg[phrase->reg];
        phrase->addr = mem[phrase->addr];  // TODO: maybe divide by 2
        phrase->value = mem[phrase->addr]; // TODO: maybe divide by 2
        read_memory += 2;
        cache_access(phrase->addr, 0);
        break;

    // index
    case 6:
        current_mem += 2;
        x = mem[current_mem];
        new_address = reg[phrase->reg] + x;
        phrase->value = mem[new_address]; // TODO: maybe divide by 2
        reg[7] += 2;
        read_memory += 2;
        instructions_fetched++;
        cache_access(mem[new_address], 0); // TODO: maybe 2?
        // printf("====================================================
        // FETCHING 1\n");
        break;

    // index deferred
    case 7:
        current_mem += 2;
        x = (short)mem[current_mem];
        new_address = (short)reg[phrase->reg] + x;
        new_address = mem[new_address];
        phrase->value = mem[new_address]; // TODO: maybe divide by 2
        read_memory += 2;
        reg[7] += 2;
        instructions_fetched++;
        cache_access(mem[new_address], 0);
        break;

    default:
        printf("unimplemented address mode %o\n", phrase->mode);
    }
}

void update_operand(addr_phrase_t *phrase, int val)
{
    if (phrase->mode == 0 || phrase->mode == 4 ||
        phrase->mode == 6)
    { // direct
        reg[phrase->reg] = val;
    }
    else
    {                                // indirect
        mem[reg[phrase->reg]] = val; // TODO: maybe subtract 1
        write_memory++;
        cache_access((reg[phrase->reg]), 1);
    }
}

void print_registers()
{
    printf("  R0:0%06o  R2:0%06o  R4:0%06o  R6:0%06o\n", reg[0], reg[2], reg[4],
           reg[6]);
    printf("  R1:0%06o  R3:0%06o  R5:0%06o  R7:0%06o\n", reg[1], reg[3], reg[5],
           reg[7]);
}
