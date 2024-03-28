#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* CACHE.C ADDITION*/

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

    addr_index = (address >> 3) & 0xF; //15 mask 0xF
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

/* END OF CACHE.C ADDITION */


/* PDP 11 SIMULATION */

#define MEM_SIZE_IN_WORDS 32*1024

/*struct to help organize source and destination operand handling*/
typedef struct ap{
    int mode;
    int reg;
    int addr; /*only used for modes 1-7 */
    int value;
} addr_phrase_t;

//global variables for instruction execution
int mem[MEM_SIZE_IN_WORDS];
int reg[8] = {0};
addr_phrase_t src, dst;
int ir;
int currentMemory = 0;
int offset = 0;
int pc = 0;
int result;
int halt = 0;

//condition codes
int n = 0;
int z = 0;
int v = 0;
int c = 0;

//declare statistic variables
int instExecutions = 0;
int instFetches = 0;
int memDataReads = 0;
int memDataWrites = 0;
int branchesExecuted = 0;
int takenBranches = 0;

//flag string
char flag[5];

/*FUNCTION DECLARATIONS*/
void verboseMode();
void traceMode();
void defaultAlg();

void printStats(int instExecuted, int wordsFetched, int dataWordsRead, int wordsWritten, int branchesExecuted, int branchesTaken);
void fourBitInsTracePrint();
void printOctal(int word);
void print20Words();
void printRegisters();

void get_operand( addr_phrase_t *phrase, bool src);
void update_operand( addr_phrase_t *phrase, int result );

void determineInstructions();

void printStats(int instExecuted, int wordsFetched, int dataWordsRead, int wordsWritten, int branchesExecuted, int branchesTaken){
    printf("\nexecution statistics (in decimal):\n");
    printf("  instructions executed     = %d\n", instExecuted);
    printf("  instruction words fetched = %d\n", wordsFetched);
    printf("  data words read           = %d\n", dataWordsRead);
    printf("  data words written        = %d\n", wordsWritten);
    printf("  branches executed         = %d\n", branchesExecuted);
    if(branchesExecuted > 0){
        double percentBranchesTaken = ((double)branchesTaken / branchesExecuted) * 100;
        printf("  branches taken            = %d (%0.1lf%%)\n", branchesTaken, percentBranchesTaken);
    }
    else{
        printf("  branches taken            = %d\n", branchesTaken);
    }
}

void fourBitInsTracePrint(){
    printf("sm %o, sr %o dm %o dr %o\n", src.mode, src.reg, dst.mode, dst.reg);
}

void fourBitVerbosePrint(int result){
    printf("  src.value = 0%06o\n", src.value);
    printf("  dst.value = 0%06o\n", dst.value);
    printf("  result    = 0%06o\n", result);
    printf("  nzvc bits = 4'b%o%o%o%o\n", n, z, v, c);
}

//print words in Octal
void printOctal(int word){
    printf("  0%06o\n", word);
}

//print first 20 words
void print20Words(){
    printf("\nfirst 20 words of memory after execution halts:\n");
    for(int i = 0; i < 20; i++){
        printf("  0%04o: %06o\n", i*2, mem[i]);
    }
}

void printRegisters(){
    printf("  R0:0%06o  R2:0%06o  R4:0%06o  R6:0%06o\n", reg[0], reg[2], reg[4], reg[6]);
    printf("  R1:0%06o  R3:0%06o  R5:0%06o  R7:0%06o\n", reg[1], reg[3], reg[5], (reg[7]+1)*2);
}

//algorithm to run in verbose mode
void verboseMode(){
    //initialize memory
    for( int i = 0; i < MEM_SIZE_IN_WORDS; i++ ){
        mem[i] = 0;
    }

    //read in words 
    printf("reading words in octal from stdin:\n");
    int ir;
    while(scanf("%o", &ir) != EOF){
        printOctal(ir);
        mem[instFetches++] = ir;
    }

    printf("\ninstruction trace:\n");

    //until at halt command:


    //at halt
    printf("at 0%04o, halt instruction\n", reg[7] * 2);
    printRegisters();

    //increment executions
    instExecutions++;

    //print final stats
    printStats(instExecutions, instFetches, memDataReads, memDataWrites, branchesExecuted, takenBranches);
    print20Words();
}

//algorithm to run instruction trace
void traceMode(){
    //initialize memory
    for( int i = 0; i < MEM_SIZE_IN_WORDS; i++ ){
        mem[i] = 0;
    }

    //read in words 
    int ir;
    while(scanf("%o", &ir) != EOF){
        mem[instFetches++] = ir;
    }

    printf("instruction trace:\n");

    //until at halt command:


    //at halt
    printf("at 0%04o, halt instruction\n", reg[7] * 2);

    //increment executions
    instExecutions++;

    //print final stats
    printStats(instExecutions, instFetches, memDataReads, memDataWrites, branchesExecuted, takenBranches);
}

//default algorithm
void defaultAlg(){

    pc = 0;
    offset = 0;
   //read in words to memory array
    while(scanf("%o", &mem[pc]) != EOF){
        pc += 2;
    }

    //until at halt command:
    while(!halt){
        //get instruction from memory
        ir = mem[currentMemory];

        //set source and destination modes and registers
        src.mode = (ir >> 9) & 07;
        src.reg = (ir >> 6) & 07;
        dst.mode = (ir >> 3) & 07;
        dst.reg = ir & 07;

        //determine which instruction to execute and execute instruction
        determineInstructions();

        //increment executions
        instExecutions++;
    }

     //print final stats
    printStats(instExecutions, instFetches, memDataReads, memDataWrites, branchesExecuted, takenBranches);
}

/* INSTRUCTION HELPER FUNCTIONS*/
void determineInstructions(){
    //MOV instruction
    if ((ir >> 12) == 01){
        //get operands
        get_operand(&src, true);
        get_operand(&dst, false);

        //determine result
        result = src.value & 0177777;

        //set condtion codes
        n= src.value << 16 < 0;
        z = !src.value;
        v = 0;

        //update operand
        update_operand(&dst, result);

        //update registers
        reg[7] += 2;
        currentMemory +=2;
    }
    //CMP instruction
    else if((ir >> 12) == 02){
        //get operands
        get_operand(&src, true);
        get_operand(&dst, false);

        //set result
        if (src.value < dst.value) {
                result = (~src.value - (dst.value - 1)) & 0177777;
        }
        else{
            result = src.value - dst.value;
        } 
        //set condition codes
        n = result << 16 < 0;
        z = !result;
        v = ((src.value > 0 && dst.value < 0 && result < 0) || (src.value < 0 && dst.value > 0 && result > 0));
        c = (result & (1 << 15)) >> 15;

        //update registers
        reg[7] += 2;
        currentMemory += 2;
    }
    //ADD instruction
    else if((ir >> 12) == 06){
        //get operands
        get_operand(&src, true);
        get_operand(&dst, false);

        //set result
        result = dst.value + src.value;
        //check carry over
        c = (result & 0200000) >> 16;
        v = ((dst.value & 0100000) == (src.value & 0100000) && (dst.value & 0100000) != (result & 0100000));
        //set result with mask
        result = result & 0177777;

        //set remaining condition codes
        n = (result << 16 < 0);
        z = !result;

        //update operand
        update_operand(&dst, result);

        //update registers and memory
        reg[7] += 2;
        currentMemory += 2;
    }
    //SUB instruction
    else if ((ir >> 12) == 016){
        //get operands
        get_operand(&src, true);
        get_operand(&dst, false);

        //set result
        result = dst.value - src.value;
        //check carry
        c = (result & (1 << 15)) >> 15;
        //set result with mask
        result = result & 0177777;

        //set remaining condition codes
        n = (result << 16 < 0);
        z = !result;
        v = ((src.value > 0 && dst.value < 0 && result < 0) || (src.value < 0 && dst.value > 0 && result > 0));

        //update operand
        update_operand(&dst, result);

        //set register and current memory
        reg[7] += 2;
        currentMemory += 2;
    }
    //SOB instruction
    else if ((ir >> 9) == 077){
        // set 8-bit signed offset
        offset = ir & 037;

        //sign extend offset
        offset = offset << 24;
        offset = offset >> 24;

        //update registers
        reg[7] += 2;
        reg[src.reg]--;

        branchesExecuted++;

        //update registers and taken branches
        if (reg[src.reg] != 0) {
            reg[7] -= offset * 2;
            takenBranches++;
        }

        //update current memory
        currentMemory = reg[7];
    }
    //BR instruction
    else if ((ir >> 8) == 01){
        //set 8-bit signed offset
        offset = ir & 0377;
        //sign extend offset
        offset = offset << 24;
        offset = offset >> 24;
        branchesExecuted++;

        //update register and taken branches
        reg[7] = (reg[7] + (offset << 1)) & 0177777;
        takenBranches++;

        //update registers and current memory
        reg[7] += 2;
        currentMemory = reg[7];
    }
    //BNE instruction
    else if ((ir >> 8) == 02){
        //set 8-bit signed offset
        offset = ir & 0377;

        branchesExecuted++;

        //sign extend offset
        if (!z){
            offset = offset << 24;
            offset = offset >> 24;
            reg[7] = (reg[7] + (offset << 1)) & 0177777;
            takenBranches++;
        }
        //update registers and memory
        reg[7] += 2;
        currentMemory = reg[7];
    }
    //BEQ instruction
    else if ((ir >> 8) == 03){
        branchesExecuted++;

        //sign extend offset
        if (z) {
            offset = offset << 24;
            offset = offset >> 24;
            reg[7] = (reg[7] + (offset << 1)) & 0177777;
            takenBranches++;
        }
        //update register and memory
        reg[7] += 2;
        currentMemory = reg[7];
    }
    //ASR instruction
    else if ((ir >> 6) == 062){
        //get operand
        get_operand(&dst, false);
        //set result
        result = dst.value >> 1;
        result = result & 0177777;
        result = result | (1 << 15);
        //set condition codes
        n = (result << 16 < 0);
        c = dst.value & 1;
        z = !result;
        v = n ^ c;

        //update operand
        update_operand(&dst, result);
        //update register and current memory
        reg[7] += 2;
        currentMemory += 2;
    }
    //ASL instruction
    else if ((ir >> 6) == 063){
        //get operand
        get_operand(&dst, false);
        //set result
        result = (dst.value << 1) & 0177777;
        //set condition codes
        n = (result << 16 < 0);
        z = !result;
        c = (dst.value & (1 << 15)) >> 15;
        v = n ^ c;
        //update operand
        update_operand(&dst, result);
        //update register and current memory
        reg[7] += 2;
        currentMemory += 2;
    }
    //HALT
    else if (ir == 0){
        //update register and memory
        reg[7] += 2;
        currentMemory += 2;
        halt = 1;
    }

    //update fetches
    instFetches++;
    //make cache access call
    cache_access(currentMemory, 0);
}

/* OPERAND HELPER FUNCTIONS*/
void get_operand( addr_phrase_t *phrase, bool src){
    //helper variables
    int temp1, temp2;

    assert( (phrase->mode >= 0) && (phrase->mode <= 7) );
    assert( (phrase->reg >= 0) && (phrase->reg <= 7) );

    switch( phrase->mode ){
        /* register */
        //value is stored in the register
        case 0:
            phrase->value = reg[ phrase->reg ];
            phrase->addr = 0; /* unused */
            break;
        //register deferred
        //Rn contains the address of the operand
        case 1:
            phrase->addr = reg[phrase->reg]; //address is in register
            //access to memory
            phrase->value = mem[phrase->addr];
            if(src){
              memDataReads++;
              cache_access(phrase->addr, 0);
            }
            break;
        /* autoincrement (post reference) */
        //immediate values
        case 2:
            phrase->addr = reg[ phrase->reg ]; //register stores address
            //access to memory
            //check register number
            if(phrase->reg == 7){
                currentMemory += 2;
                phrase->value = mem[ currentMemory ];
                //increment register
                reg[7] += 2;
                //cache access
                cache_access(currentMemory, 0);
                instFetches++;
            }
            else{
                phrase->value = mem[phrase->addr];
                reg[ phrase->reg ] = ( reg[ phrase->reg ] + 2 ) & 0177777;
            }
            break;
        //autoincrement deferred
        //Rn contains the address of the address of the operand, then increment Rn by 2
        case 3:
            //address of address is in register
            phrase->addr = reg[phrase->reg];
            phrase->addr = mem[phrase->addr];

            //get value
            phrase->value = mem[phrase->addr];

            //set register
            reg[ phrase->reg ] = ( reg[ phrase->reg ] + 2 ) & 0177777;

            //set statistics
            if (phrase->reg == 7){
                memDataReads++;
                cache_access(phrase->addr, 0);
            }
            else{
                memDataReads+=2;
                cache_access(phrase->addr, 0);
            }

            break;
        //autodecrement
        //decrement Rn, then use the result as the address of the operand
        case 4:
            //decrement
            reg[ phrase->reg ] = ( reg[ phrase->reg ] - 2 ) & 0177777;
            //address stored in register
            phrase->addr = reg[phrase->reg];
            //set value
            phrase->value = mem[phrase->addr];
            //update stats
            memDataReads++;
            cache_access(phrase->addr, 0);
            break;
        //autodecrement deferred
        //Decrement Rn by 2, then use the result as the adress of the address of the operand
        case 5:
            //decrement
            reg[ phrase->reg ] = ( reg[ phrase->reg ] - 2 ) & 0177777;
            //set address
            phrase->addr = reg[phrase->reg]; //reg holds address of the address
            phrase->addr = mem[phrase->addr]; //get address from mem
            phrase->value = mem[phrase->addr]; //set value
            memDataReads+=2;
            cache_access(phrase->addr, 0);
            break;
        //index
        //Rn + X is the address of the operand
        //index values
        case 6:
            //increment current memoru
            currentMemory += 2;

            temp1 = mem[currentMemory];
            temp2 = reg[phrase->reg] + temp1;

            //set value
            phrase->value = mem[temp2];

            //increment vals
            reg[7] += 2;
            memDataReads+=2;
            instFetches++;
            //call to cache
            cache_access(mem[temp2], 0);

            break;
        //index deferred
        //Rn+X is the address of the address of the operand
        //index values
        case 7:
            //increment current memoru
            currentMemory += 2;

            temp1 = (short)mem[currentMemory];
            temp2 = (short)reg[phrase->reg] + temp1;
            temp2 = mem[temp2];

            //set value
            phrase->value = mem[temp2];

            //increment vals
            reg[7] += 2;
            memDataReads+=2;
            instFetches++;
            //call to cache
            cache_access(mem[temp2], 0);
            break;
        default:
            printf("unimplemented address mode %o\n", phrase->mode);
    }
}

void update_operand( addr_phrase_t *phrase, int resultVal ){
    int mode = phrase->mode;

    //register modes (direct)
    if(mode == 0 || mode == 4 || mode == 6){
        reg[phrase->reg] = resultVal;
    }
    //indirect register modes
    else {
        mem[reg[phrase->reg]] = resultVal;
        //write to memory
        memDataWrites++;
        cache_access((reg[phrase->reg]), 1);
    }
}

/*MAIN FUNCTION*/
int main(int argc, char *argv[]){

    //read command line arguments to know what command flag
    if(argv[1] != NULL){
        strcpy(flag, argv[1]);
    }

    //initialize cache
    cache_init();

    //if -t call instruction trace
    if(!strcmp(flag, "-t")){
        traceMode();
    }
    //else if -v go to verbose mode
    else if(!strcmp(flag, "-v")){
        verboseMode();
    }
    //else only display execution statistics
    else{
        defaultAlg();
    }

    //call to cache stats
    cache_stats();

    return 0;

}
