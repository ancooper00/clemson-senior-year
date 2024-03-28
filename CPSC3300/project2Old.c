#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define MEM_SIZE_IN_WORDS 32*1024

/*struct to help organize source and destination operand handling*/
typedef struct ap{
    int mode;
    int reg;
    int addr; /*only used for modes 1-7 */
    int value;
} addr_phrase_t;

int mem[MEM_SIZE_IN_WORDS];
int reg[8] = {0};
addr_phrase_t src, dst;

//condition codes
int n, z, v, c;

//declare statistic variables
int instExecutions;
int instFetches;
int memDataReads;
int memDataWrites;
int branchInstructions;
int takenBranches;

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

void get_operand( addr_phrase_t *phrase);
void update_operand( addr_phrase_t *phrase, int result );

void determineInstructions();
void tenBitOp(int instruction);
void eightBitOp(int instruction);
void sevenBitOp(int instruction);
void fourBitOp(int instruction);

/* CACHE VARIABLES */
#define LINES_PER_BANK 16

unsigned int
  plru_state[LINES_PER_BANK],  /* current state for each set */
  valid[4][LINES_PER_BANK],    /* valid bit for each line    */
  dirty[4][LINES_PER_BANK],    /* dirty bit for each line    */
  tag[4][LINES_PER_BANK],      /* tag bits for each line     */

  plru_bank[8] /* table for bank replacement choice based on state */

                 = { 0, 0, 1, 1, 2, 3, 2, 3 },

  next_state[32] /* table for next state based on state and bank ref */
                 /* index by 5-bit (4*state)+bank [=(state<<2)|bank] */

                                    /*  bank ref  */
                                    /* 0  1  2  3 */

                 /*         0 */  = {  6, 4, 1, 0,
                 /*         1 */       7, 5, 1, 0,
                 /*         2 */       6, 4, 3, 2,
                 /* current 3 */       7, 5, 3, 2,
                 /*  state  4 */       6, 4, 1, 0,
                 /*         5 */       7, 5, 1, 0,
                 /*         6 */       6, 4, 3, 2,
                 /*         7 */       7, 5, 3, 2  };

unsigned int
    cache_reads,  /* counter */
    cache_writes, /* counter */
    hits,         /* counter */
    misses,       /* counter */
    write_backs;  /* counter */

void cache_init( void ){
  int i;
  for( i=0; i<LINES_PER_BANK; i++ ){
    plru_state[i] = 0;
    valid[0][i] = dirty[0][i] = tag[0][i] = 0;
    valid[1][i] = dirty[1][i] = tag[1][i] = 0;
    valid[2][i] = dirty[2][i] = tag[2][i] = 0;
    valid[3][i] = dirty[3][i] = tag[3][i] = 0;
  }
  cache_reads = cache_writes = hits = misses = write_backs = 0;
}

void cache_stats( void ){
  printf( "cache statistics (in decimal):\n" );
  printf( "  cache reads       = %d\n", cache_reads );
  printf( "  cache writes      = %d\n", cache_writes );
  printf( "  cache hits        = %d\n", hits );
  printf( "  cache misses      = %d\n", misses );
  printf( "  cache write backs = %d\n", write_backs );
}


/* address is byte address, type is read (=0) or write (=1) */

void cache_access( unsigned int address, unsigned int type ){

  unsigned int
    addr_tag,    /* tag bits of address     */
    addr_index,  /* index bits of address   */
    bank;        /* bank that hit, or bank chosen for replacement */

  if( type == 0 ){
    cache_reads++;
  }else{
    cache_writes++;
  }

  addr_index = (address >> 3) & 0xF; //15 bit mask (LINES_PER_BANK - 1 )
  addr_tag = address >> 7;

  /* check bank 0 hit */

  if( valid[0][addr_index] && (addr_tag==tag[0][addr_index]) ){
    hits++;
    bank = 0;

  /* check bank 1 hit */

  }else if( valid[1][addr_index] && (addr_tag==tag[1][addr_index]) ){
    hits++;
    bank = 1;

  /* check bank 2 hit */

  }else if( valid[2][addr_index] && (addr_tag==tag[2][addr_index]) ){
    hits++;
    bank = 2;

  /* check bank 3 hit */

  }else if( valid[3][addr_index] && (addr_tag==tag[3][addr_index]) ){
    hits++;
    bank = 3;

  /* miss - choose replacement bank */

  }else{
    misses++;

         if( !valid[0][addr_index] ) bank = 0;
    else if( !valid[1][addr_index] ) bank = 1;
    else if( !valid[2][addr_index] ) bank = 2;
    else if( !valid[3][addr_index] ) bank = 3;
    else bank = plru_bank[ plru_state[addr_index] ];

    if( valid[bank][addr_index] && dirty[bank][addr_index] ){
      write_backs++;
    }

    valid[bank][addr_index] = 1;
    dirty[bank][addr_index] = 0;
    tag[bank][addr_index] = addr_tag;
  }

  /* update replacement state for this set (i.e., index value) */

  plru_state[addr_index] = next_state[ (plru_state[addr_index]<<2) | bank ];

  /* update dirty bit on a write */

  if( type == 1 ) dirty[bank][addr_index] = 1;
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

/*HELPER FUNCTIONS*/

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
    for(reg[7] = 0; mem[reg[7]] != 0 && reg[7] < 1000; reg[7]++){
        printf("at 0%04o, ", reg[7]*2);
        determineInstructions();
    }

    //at halt
    printf("at 0%04o, halt instruction\n", reg[7] * 2);
    printRegisters();

    //increment executions
    instExecutions++;

    //print final stats
    printStats(instExecutions, instFetches, memDataReads, memDataWrites, branchInstructions, takenBranches);
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
    for(reg[7] = 0; mem[reg[7]] != 0 && reg[7] < 1000; reg[7]++){
        printf("at 0%04o, ", reg[7]*2);
        determineInstructions();
    }

    //at halt
    printf("at 0%04o, halt instruction\n", reg[7] * 2);

    //increment executions
    instExecutions++;

    //print final stats
    printStats(instExecutions, instFetches, memDataReads, memDataWrites, branchInstructions, takenBranches);
}

//default algorithm
void defaultAlg(){
    //initialize memory
    for( int i = 0; i < MEM_SIZE_IN_WORDS; i++ ){
        mem[i] = 0;
    }

   //read in words 
    int ir;
    while(scanf("%o", &ir) != EOF){
        mem[instFetches++] = ir;
    }

    //until at halt command:
    for(reg[7] = 0; mem[reg[7]] != 0 && reg[7] < 1000; reg[7]++){
        determineInstructions();
    }

    //increment executions
    instExecutions++;
    

    //print final stats
    printStats(instExecutions, instFetches, memDataReads, memDataWrites, branchInstructions, takenBranches);
}

/* PRINTING HELPER FUNCTIONS*/
//print execution statistics
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

/* OPERAND HELPER FUNCTIONS*/
void get_operand( addr_phrase_t *phrase){
    //helper variables
    int temp1, temp2;

    assert( (phrase->mode >= 0) && (phrase->mode <= 7) );
    assert( (phrase->reg >= 0) && (phrase->reg <= 7) );

    switch( phrase->mode ){
        /* register */
        //value is stored in the register
        case 0:
            phrase->value = reg[ phrase->reg ];
            assert( phrase->value < 0200000 );
            phrase->addr = 0; /* unused */
            break;
        //register deferred
        //Rn contains the address of the operand
        case 1:
            phrase->addr = reg[phrase->reg]; //address is in register
            assert( phrase->addr < 0200000 );
            //access to memory
            cache_access(reg[phrase->reg], 0); //removed division
            phrase->value = mem[reg[phrase->reg]/2];
            memDataReads++;
            break;
        /* autoincrement (post reference) */
        //immediate values
        case 2:
            phrase->addr = reg[ phrase->reg ]; //register stores address
            //access to memory 
            //check register number
            if(phrase->reg == 7){
                cache_access(phrase->addr, 0);
                phrase->value = mem[ phrase->addr + 1 ]; /* adjust to word addr */ 
                //increment
                reg[phrase->reg]++;
            }
            else{
                cache_access(phrase->addr, 0); //added cache access 
                phrase->value = mem[phrase->addr + 1]; 
                reg[ phrase->reg ] = ( reg[ phrase->reg ] + 2 ) & 0177777;
            }
            break;
        //autoincrement deferred
        //Rn contains the address of the address of the operand, then increment Rn by 2
        case 3:
            //address of address is in register
            phrase->addr = reg[phrase->reg];
            assert(phrase->addr < 0200000); //check address
            //get word address from mem
            cache_access(phrase->addr, 0);
            phrase->addr = mem[phrase->addr >> 1];
            assert(phrase->addr < 0200000); //check address
            //get value
            cache_access(phrase->addr, 0);
            phrase->value = mem[phrase->addr >> 1];
            assert(phrase->value < 0200000); //check value
            //set register
            reg[ phrase->reg ] = ( reg[ phrase->reg ] + 2 ) & 0177777;

            //set statistics
            if (phrase->reg == 7){
                memDataReads++;
            }
            else{
                memDataReads+=2;
            }

            break;
        //autodecrement
        //decrement Rn, then use the result as the address of the operand
        case 4:
            //decrement
            reg[ phrase->reg ] = ( reg[ phrase->reg ] - 2 ) & 0177777;
            //address stored in register
            phrase->addr = reg[phrase->reg];
            assert(phrase->addr < 0200000);
            //access to memory
            cache_access(phrase->addr, 0);
            phrase->value = mem[phrase->addr >> 1];
            assert(phrase->value < 0200000);
            //update stats
            memDataReads++;
            break;
        //autodecrement deferred
        //Decrement Rn by 2, then use the result as the adress of the address of the operand
        case 5:
            //decrement
            reg[ phrase->reg ] = ( reg[ phrase->reg ] - 2 ) & 0177777;
            //set address
            phrase->addr = reg[phrase->reg]; //reg holds address of the address

            cache_access(phrase->addr, 0);
            phrase->addr = mem[phrase->addr >> 1]; //get word address from mem
            memDataReads++;

            cache_access(phrase->addr, 0);
            phrase->value = mem[phrase->addr >> 1]; //set value
            memDataReads++;

            break;
        //index
        //Rn + X is the address of the operand
        //index values
        case 6:
            phrase->addr = reg[phrase->reg]; //address is stored in register
            //set helpers
            cache_access(reg[7] + 1, 0); 
            temp1 = (short)mem[reg[7] + 1];  
            
            memDataReads++;
            temp2 = (short)reg[phrase->reg];
            //set value 
            cache_access((temp1) + (temp2), 0); //removed right shifts 
            phrase->value = mem[(temp1 >> 1) + (temp2 >> 1)];
            
            memDataReads++;
            //increment register 7
            reg[7]++; //clamp to 16 bits 
            break;
        //index deferred
        //Rn+X is the address of the address of the operand
        //index values
        case 7: 
            phrase->addr = reg[phrase->reg]; //address is stored in register
            //set helpers 
            cache_access(reg[7] + 1, 0); 
            temp1 = (short)mem[reg[7] + 1];
            
            memDataReads++;
            temp2 = (short)reg[phrase->reg];
            //set value
            cache_access((temp1 >> 1) + (temp2 >> 1), 0);
            cache_access(mem[(temp1 >> 1) + (temp2 >> 1)] >> 1, 0);
            phrase->value = mem[mem[(temp1 >> 1) + (temp2 >> 1)] >> 1];
            
            memDataReads++;
            
            //increment register 7
            reg[7]++;
            break;
        default:
            printf("unimplemented address mode %o\n", phrase->mode);
    }
}

void update_operand( addr_phrase_t *phrase, int result ){
    int tempAddr;
    int mode = phrase->mode;

    //register modes (direct)
    if(mode == 0 || mode == 4 || mode == 6){
        reg[phrase->reg] = result;
    }
    //indirect register modes
    else if(mode == 1 || mode == 2 || mode == 3 || mode == 5 || mode == 7){
        tempAddr = reg[phrase->reg];
        //write to memory
        cache_access(reg[phrase->reg], 1);
        mem[(tempAddr / 2) - 1] = result; 
        
        memDataWrites++;
        
    }
    else{
        printf("Error updating operand\n");
    }
}

/* INSTRUCTION HELPER FUNCTIONS*/
void determineInstructions(){
    //increment instructions
    instExecutions++;

    cache_access(reg[7], 0);
    int ir = mem[reg[7]]; 
    

    //set instruction from mem
    int inst10b = ir >> 6;
    int inst8b = ir >> 8;
    int inst7b = ir >> 9;
    int inst4b = ir >> 12;

    //10 bit instruction
    if(inst10b == 0062 || inst10b == 0063){
        //set mode and register 
        src.mode = (ir >> 3) & 07;
        src.reg = ir & 07;
        //get operand
        get_operand(&src);
        //do operation
        tenBitOp(inst10b);
    }
    //7 bit instruction
    else if(inst7b == 077){
        src.reg = (ir >> 6) & 07;
        src.value = (ir) & 077;
        sevenBitOp(inst7b);
    }
    //8 bit instruction 
    else if(inst8b == 001 || inst8b == 002 || inst8b == 003){
        src.value = (ir & 0377);
        eightBitOp(inst8b);
    }
    //4 bit instruction 
    else{
        //extract fields for the addressing modes
        src.mode = (ir >> 9) & 07; //three one bits in the mask
        src.reg = (ir >> 6) & 07; 
        dst.mode = (ir >> 3) & 07;
        dst.reg = ir & 07;

        //get operand for src and dst
        get_operand(&src);
        get_operand(&dst);

        fourBitOp(inst4b);
    }
    
}

void tenBitOp(int instruction){
    int result;
    //call right instruction 
    switch(instruction){
        case 0062:
            //asr
            result = src.value >> 1;
            result = result & 0177777;
            result = result | (1 << 15);
            //set condition codes
            c = src.value & 1;
            if((result << 16) < 0){
                n = 1;
            }else{
                n = 0;
            }
            if(result == 0){
                z = 1;
            }else{
                z = 0;
            }
            v = n ^ c;

            update_operand(&src, result);

            //print results if in certain mode
            if(!strcmp(flag, "-t") || !strcmp(flag, "-v")){
                printf("asr instruction ");
                printf("dm %o dr %o\n", src.mode, src.reg);
            }
            if(!strcmp(flag, "-v")){
                printf("  dst.value = 0%06o\n", src.value);
                printf("  result    = 0%06o\n", result);
                printf("  nzvc bits = 4'b%o%o%o%o\n", n, z, v, c);
                printRegisters();
            }
            
            break;
        case 0063:
            //asl 
            result = src.value << 1;
            result = result & 0177777;
            //set condition codes
            if((result << 16) < 0){
                n = 1;
            }else{
                n = 0;
            }
            if(result == 0){
                z = 1;
            }else{
                z = 0;
            }
            c = (src.value & (1 << 15)) >> 15;
            v = n ^ c;

            update_operand(&src, result);

            //print results if in certain mode
            if(!strcmp(flag, "-t") || !strcmp(flag, "-v")){
                printf("asl instruction ");
                printf("dm %o dr %o\n", src.mode, src.reg);
            }
            if(!strcmp(flag, "-v")){
                printf("  dst.value = 0%06o\n", src.value);
                printf("  result    = 0%06o\n", result);
                printf("  nzvc bits = 4'b%o%o%o%o\n", n, z, v, c);
                printRegisters();
            }
            break;
        default:
            printf("Error. Unknown Instruction\n");
            assert(1==0);
            break;
    }
}

void eightBitOp(int instruction){
    int result;
    int offset;
    switch(instruction){
        case 01:
            //br

            //print results if in certain mode
            if(!strcmp(flag, "-t") || !strcmp(flag, "-v")){
                printf("br instruction ");
                printf("with offset 0%03o\n", src.value);
            }

            branchInstructions++;

            offset = src.value;
            //check offset
            if (offset & (1 << 7)) {
                offset = (~offset & 0377) + 1;
                result = reg[7] - offset;
                instFetches += offset;
            } else {
                result = reg[7] + offset;
                instFetches -= offset;
            }
            reg[7] = result;
            takenBranches++;

            if(!strcmp(flag, "-v")){
                printRegisters();
            }
            break;
        case 02:
            //bne
            //print results if in certain mode
            if(!strcmp(flag, "-t") || !strcmp(flag, "-v")){
                printf("bne instruction ");
                printf("with offset 0%03o\n", src.value);
            }

            branchInstructions++;

            if (!z) {
                offset = src.value;
                if (offset & (1 << 7)) {
                    offset = (~offset & 0377) + 1;
                    result = reg[7] - offset;
                    instFetches += offset;
                } else {
                    result = reg[7] + offset;
                    instFetches -= offset;
                }
                reg[7] = result;
                takenBranches++;
            }

            if(!strcmp(flag, "-v")){
                printRegisters();
            }
            break;
        case 03:
            //beq
            //print results if in certain mode
            if(!strcmp(flag, "-t") || !strcmp(flag, "-v")){
                printf("beq instruction ");
                printf("with offset 0%03o\n", src.value);
            }

            branchInstructions++;

            if (z) {
                offset = src.value;
                if (offset & (1 << 7)) {
                    offset = (~offset & 0377) + 1;
                    result = reg[7] - offset;
                    instFetches += offset;
                } else {
                    result = reg[7] + offset;
                    instFetches -= offset;
                }
                reg[7] = result;
                takenBranches++;
            }

            if(!strcmp(flag, "-v")){
                printRegisters();
            }
            break;
        default:
            break;
    }
}

void sevenBitOp(int instruction){
    //SOB instruction
    //print results if in certain mode
    if(!strcmp(flag, "-t") || !strcmp(flag, "-v")){
        printf("sob instruction reg %o", src.reg);
        printf(" with offset %03o\n", src.value);
    }

    reg[src.reg]--;

    branchInstructions++;

    if (reg[src.reg] != 0) {
        reg[7] -= src.value;
        instFetches += src.value;
        takenBranches++;
    }

    if(!strcmp(flag, "-v")){
        printRegisters();
    }
}

void fourBitOp(int instruction){
    int result;
    int sign_bit;

    switch(instruction){
        case 01:
            //mov
            result = src.value;
            result = result & 0177777;
            //set condtion codes
            if((src.value << 16) < 0){
                n = 1;
            }else{
                n = 0;
            }
            if(src.value == 0){
                z = 1;
            }else{
                z = 0;
            }
            v = 0;
            //update operand 
            update_operand(&dst, result);

             //print results if in certain mode
            if(!strcmp(flag, "-t") || !strcmp(flag, "-v")){
                printf("mov instruction ");
                fourBitInsTracePrint();
            }
            if(!strcmp(flag, "-v")){
                printf("  src.value = 0%06o\n", src.value);
                printf("  nzvc bits = 4'b%o%o%o%o\n", n, z, v, c);
                if (dst.addr > 7) {
                    printf("  value 0%06o is written to 0%06o\n",
                            result, dst.addr);
                }
                printRegisters();
            }
            break;
        case 02:
            //cmp
            if (src.value < dst.value) {
                result = (~src.value - (dst.value - 1)) & 0177777;
            }
            else{
                result = src.value - dst.value;
            }
            //set condition codes
            if(result & (1 << 15)){
                n = 1;
            }else{
                n = 0;
            }
            if(result == 0){
                z = 1;
            }else{
                z = 0;
            }
            v = 0;
            if (((dst.value & 0100000) != (src.value & 0100000)) && ((dst.value & 0100000) == (result & 0100000))) {
                v = 1;
            }
            c = (result & (1 << 15)) >> 15;

            if(!strcmp(flag, "-t") || !strcmp(flag, "-v")){
                printf("cmp instruction ");
                fourBitInsTracePrint();
            }
            if(!strcmp(flag, "-v")){
                fourBitVerbosePrint(result);
                printRegisters();
            }
            break;
        case 06:
            //add
            result = dst.value + src.value;
            //check carry over
            c = (result & 0200000) >> 16;
            result = result & 0177777;
            //set sign 
            sign_bit = result & 0100000;
            n = 0;
            if (sign_bit){
                n = 1;
            } 
            z = 0;
            if (result == 0){
                 z = 1;
            }
            v = 0;
            if ((dst.value & 0100000) == (src.value & 0100000) && (dst.value & 0100000) != (result & 0100000)) {
                v = 1;
            }
            update_operand(&dst, result);

            if(!strcmp(flag, "-t") || !strcmp(flag, "-v")){
                printf("add instruction ");
                fourBitInsTracePrint();
            }
            if(!strcmp(flag, "-v")){
                fourBitVerbosePrint(result);
                printRegisters();
            }
            break;
        case 016:
            //sub
            result = dst.value - src.value;
            //check carry 
            c = (result & 0200000) >> 16;
            result = result & 0177777;
            //check sign 
            sign_bit = result & 0100000;
            n = 0;
            if (sign_bit){
                n = 1;
            }
            z = 0;
            z = !result;
            v = 0;
            if (((src.value & 0100000) != (dst.value & 0100000)) && ((src.value & 0100000) == (result & 0100000))) {
                v = 1;
            }
            update_operand(&dst, result);

            if(!strcmp(flag, "-t") || !strcmp(flag, "-v")){
                printf("sub instruction ");
                fourBitInsTracePrint();
            }
            if(!strcmp(flag, "-v")){
                fourBitVerbosePrint(result);
                printRegisters();
            }
            break;
        default:
            break;
    }
}