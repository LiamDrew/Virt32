/*
 * umlab.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program.
 *  
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.  
 * 
 * Any additional functions and unit tests written for the lab go
 * here. 
 *  
 */


#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>
#include <string.h>


typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc) {
        Um_instruction instruction = 0;
        instruction = Bitpack_newu(instruction, 4, 28, op);
        instruction = Bitpack_newu(instruction, 3, 0, rc);
        instruction = Bitpack_newu(instruction, 3, 3, rb);
        instruction = Bitpack_newu(instruction, 3, 6, ra);
        return instruction;

}

Um_instruction loadval(unsigned ra, unsigned val) {
        Um_instruction instruction = 0;
        instruction = Bitpack_newu(instruction, 4, 28, LV);
        instruction = Bitpack_newu(instruction, 3, 25, ra);
        instruction = Bitpack_newu(instruction, 25, 0, val);
        return instruction;


}

/* helper functions */
static inline void XOR (Seq_T stream, Um_register reg1, Um_register reg2, 
/*                   */ Um_register intermediate);
static inline void saveState(Seq_T stream);
static inline void loadPrevState(Seq_T stream);
static inline void printString(Seq_T stream, Um_register r, char *string);
static inline void addQueryHelper(Seq_T stream);
static inline void subtract(Seq_T stream, Um_register r, 
/*                       */ Um_register int1, Um_register int2, unsigned value);


/* Wrapper functions for each of the instructions */

static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}

static inline Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}

static inline Um_instruction output(Um_register c) {

        return three_register(OUT, 0, 0, c);
}

static inline Um_instruction condMove(Um_register a, 
                                                Um_register b, Um_register c)
{
        return three_register(CMOV, a, b, c);
}

static inline Um_instruction segLoad(Um_register a, 
                                                Um_register b, Um_register c)
{
        return three_register(SLOAD, a, b, c);
}

static inline Um_instruction segStore(Um_register a, 
                                                Um_register b, Um_register c)
{
        return three_register(SSTORE, a, b, c);
}

static inline Um_instruction mult(Um_register a, Um_register b, Um_register c)
{
        return three_register(MUL, a, b, c);
}

static inline Um_instruction div(Um_register a, Um_register b, Um_register c)
{
        return three_register(DIV, a, b, c);
}

static inline Um_instruction nand(Um_register a, Um_register b, Um_register c)
{
        return three_register(NAND, a, b, c);
}

static inline Um_instruction map(Um_register b, Um_register c)
{
        return three_register(ACTIVATE, 0, b, c);
}

static inline Um_instruction unmap(Um_register c)
{
        return three_register(INACTIVATE, 0, 0, c);
}

static inline Um_instruction in(Um_register c)
{
        return three_register(IN, 0, 0, c);
}

static inline Um_instruction loadProg(Um_register b, Um_register c)
{
        return three_register(LOADP, 0, b, c);
}


/* Functions for working with streams */

static inline void append(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {
                Um_instruction inst = (uintptr_t)Seq_remlo(stream);
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }     
}

/* Unit tests for the UM */

void build_halt_test(Seq_T stream)
{
        append(stream, halt());
}

void build_verbose_halt_test(Seq_T stream)
{
        append(stream, halt());
        append(stream, loadval(r1, 'B'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'a'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'd'));
        append(stream, output(r1));
        append(stream, loadval(r1, '!'));
        append(stream, output(r1));
        append(stream, loadval(r1, '\n'));
        append(stream, output(r1));
}

// The following (above the dashed line) are not yet in labwrite
void buildOutputM(Seq_T stream) //output 'M'
{
        append(stream, loadval(r1, 'M'));
        append(stream, output(r1));
        append(stream, halt());
}

void buildOutputString(Seq_T stream) //output the string
{
        char *messagep1 = "Ooh...ooooh...you want to give us an excellent ";
        char *messagep2 = "sooooooooooo bad... ooh... oooooh";
        printString(stream, r0, messagep1);
        printString(stream, r0, messagep2);
        append(stream, halt());
}

void buildLoadValSimple(Seq_T stream)
{
        append(stream, loadval(r1, 7));
        append(stream, halt());
}

void buildOverloadVal(Seq_T stream)
{
        for(unsigned i = 0; i < 10000; i++){
                append(stream, loadval(r1, i));
        }
        append(stream, halt());
}

void buildNandSimple(Seq_T stream) //output 0
{

        append(stream, loadval(r0, 1));
        append(stream, loadval(r1, 0));
        append(stream, nand(r0, r1, r0)); // r0 should hold a value of ala 1's
        append(stream, loadval(r1, 49)); // add 49 to overflow the uint32 and
                                       // get to the ascii value of the char '0'
        append(stream, add(r0, r1, r0));
        append(stream, output(r0));
        append(stream, halt());
}

void buildXOR(Seq_T stream) //output 0
{       
        append(stream, loadval(r1, 10000));
        append(stream, loadval(r2, 10000));
        XOR(stream, r1, r2, r3);
        append(stream, loadval(r0, 48));
        append(stream, add(r1, r1, r0));
        append(stream, output(r1));
        append(stream, halt());
}

void buildSimpleInput(Seq_T stream) //output whatever we put in (make it 0)
{
        append(stream, in(r1));
        append(stream, output(r1));
        append(stream, halt());
}

void buildSimpleAdd(Seq_T stream) //output 0
{
        append(stream, loadval(r0, 24));
        append(stream, loadval(r1, 24));
        append(stream, add(r0, r0, r1));
        append(stream, output(r0));
        append(stream, halt());
}

void buildSubtract(Seq_T stream) //output 0
{       
        append(stream, loadval(r1, 96));
        subtract(stream, r1, r0, r2, 48);
        append(stream, output(r1));
        append(stream, halt());
}

void buildExponential(Seq_T stream) //output 0
{
        append(stream, loadval(r0, 1));
        for(int i = 0; i < 10; i++) {
                append(stream, add(r0, r0, r0));
        }
        append(stream, loadval(r1, 1024));
        XOR(stream, r0, r1, r2);
        append(stream, loadval(r1, 48));
        append(stream, add(r0, r0, r1));
        append(stream, output(r0));
        append(stream, halt());
}

void buildMultiplySimple(Seq_T stream) //output 0
{
        append(stream, loadval(r0, 6));
        append(stream, loadval(r1, 8));
        append(stream, mult(r0, r0, r1));
        append(stream, output(r0));
        append(stream, halt());
}

void buildMultiplyWrapAround(Seq_T stream) // output 0
{
        append(stream, loadval(r0, 256));
        append(stream, loadval(r1, 4096));
        append(stream, mult(r0, r0, r1));
        append(stream, mult(r0, r0, r1));
        append(stream, loadval(r1, 48));
        append(stream, add(r0, r0, r1));
        append(stream, output(r0));
        append(stream, halt());

}

void buildDivide(Seq_T stream) // output 0
{
        append(stream, loadval(r0, 9699690));
        append(stream, loadval(r1, 2));
        append(stream, div(r0, r0, r1));
        append(stream, loadval(r1, 3));
        append(stream, div(r0, r0, r1));
        append(stream, loadval(r1, 5));
        append(stream, div(r0, r0, r1));
        append(stream, loadval(r1, 7));
        append(stream, div(r0, r0, r1));
        append(stream, loadval(r1, 11));
        append(stream, div(r0, r0, r1));
        append(stream, loadval(r1, 13));
        append(stream, div(r0, r0, r1));
        append(stream, loadval(r1, 17));
        append(stream, div(r0, r0, r1));
        append(stream, loadval(r1, 19));
        append(stream, div(r0, r0, r1));
        append(stream, loadval(r1, 48));
        append(stream, add(r0, r0, r1));
        append(stream, output(r0));
        append(stream, halt());

}

void buildDivideFloor(Seq_T stream) //output 0
{
        append(stream, loadval(r0, 3));
        append(stream, loadval(r1, 2));
        append(stream, div(r0, r0, r1));
        append(stream, loadval(r1, 1));
        XOR(stream, r0, r1, r2);
        append(stream, loadval(r1, 48));
        append(stream, add(r0, r0, r1));
        append(stream, output(r0));
        append(stream, halt());

}

void buildTimesTables(Seq_T stream) // output times tables (see file)
{
        char *firstLine = "  0  1  2  3  4  5  6  7  8  9  \n";
        printString(stream, r0, firstLine);
        for(unsigned i = 0; i < 10; i++) {

                append(stream, loadval(r0, i + 48));
                append(stream, output(r0));
                append(stream, loadval(r0, ' '));
                append(stream, output(r0));
                for(unsigned j = 0; j < 10; j++) {
                        // all values where i * j < 10;
                        if((j < 2) || (i < 2) || (i == 2 && j < 5) 
                                   || (j == 2 && i < 5) || (i == 3 && j == 3)) {
                                append(stream, loadval(r0, i));
                                append(stream, loadval(r1, j));
                                append(stream, mult(r0, r0, r1));
                                append(stream, loadval(r1, 48));
                                append(stream, add(r0, r0, r1));
                                append(stream, output(r0));
                                append(stream, loadval(r0, ' '));
                                append(stream, output(r0));
                        } else {
                                append(stream, loadval(r0, i));
                                append(stream, loadval(r1, j));
                                append(stream, mult(r0, r0, r1)); // r0 holds the output number
                                // we now have a 2-digit number
                                // get the first digit
                                append(stream, loadval(r1, 10));
                                append(stream, div(r1, r0, r1));
                                append(stream, loadval(r2, 48));
                                append(stream, add(r1, r2, r1));
                                append(stream, output(r1)); // r1 holds the tens place digit as a character value, not as its real value. 
                                //get the second digit 
                                //first subtract the 48 from r1 that we added in order to display the ascii value of its digit
                                append(stream, loadval(r3, 1));
                                append(stream, nand(r2, r2, r2));
                                append(stream, add(r2, r2, r3));
                                append(stream, add(r1, r1, r2));
                                // now r1 holds the value of the digit printed
                                append(stream, loadval(r2, 10));
                                append(stream, mult(r1, r1, r2)); // r1 now holds the number we want to subtract from r0
                                append(stream, nand(r1, r1, r1));
                                append(stream, loadval(r2, 1));
                                append(stream, add(r1, r2, r1)); // r1 holds its own 2's comp
                                append(stream, add(r0, r0, r1)); // r0 now holds the ones place digit
                                append(stream, loadval(r1, 48));
                                append(stream, add(r0, r0, r1));
                                append(stream, output(r0));

                        }
                        append(stream, loadval(r0, ' '));
                        append(stream, output(r0));
                }
                append(stream, loadval(r0, '\n'));
                append(stream, output(r0));
        }
        append(stream, halt());
}

void buildMapSimple(Seq_T stream) // output 0
{
        append(stream, loadval(r0, 100));
        append(stream, map(r1, r0));
        append(stream, loadval(r0, 1)); /* test if our maping works how we 
                                         * think it does (r1 should be 1) */
        XOR(stream, r0, r1, r2);
        append(stream, loadval(r1, 48));
        append(stream, add(r0, r0, r1));
        append(stream, output(r0));
        append(stream, halt());
        
}

void buildUnmapSimple(Seq_T stream) {
        append(stream, loadval(r0, 100));
        append(stream, map(r1, r0));
        append(stream, unmap(r1));
        append(stream, halt());
}

void buildMapUnmapVerbose(Seq_T stream) 
{
        for(unsigned i = 0; i < 100; i++) {
                append(stream, loadval(r0, i));
                append(stream, map(r1, r0));
                for(unsigned j = 0; j < 100; j++) {
                        append(stream, loadval(r0, j));
                        append(stream, map(r2, r0));
                        append(stream, unmap(r2));
                } 
                for(unsigned j = 0; j < 100; j++) {
                        append(stream, loadval(r0, i * j));
                        append(stream, map(r2, r0));
                        append(stream, unmap(r2));                
                }
                append(stream, unmap(r1));
        }
        append(stream, halt());
}

void buildSegStore(Seq_T stream) {
        append(stream, loadval(r0, 8));
        append(stream, map(r7, r0));
        for(unsigned i = 0; i < 8; i++) { // fill the new seg with 8
                append(stream, loadval(r1, i));
                append(stream, segStore(r7, r1, r0));
        }
        append(stream, unmap(r7));
        append(stream, halt());
}

void buildSegLoad(Seq_T stream) { //output 0
        append(stream, loadval(r0, 8));
        append(stream, loadval(r2, 0));
        append(stream, loadval(r3, 64));
        append(stream, map(r7, r0));
        for(unsigned i = 0; i < 8; i++) { // fill the new seg with 8
                append(stream, loadval(r1, i));
                append(stream, segStore(r7, r1, r0));
        }
        for(unsigned i = 0; i < 8; i++) { // fill the new seg with 8
                append(stream, loadval(r1, i));
                append(stream, segLoad(r0, r7, r1));
                append(stream, add(r2, r0, r2));
        }
        append(stream, unmap(r7));
        XOR(stream, r2, r3, r1);
        append(stream, loadval(r0, 48));
        append(stream, add(r0, r2, r0));
        append(stream, output(r0));
        append(stream, halt());
}

void buildLoadProgramOutput(Seq_T stream) // output 'M'
{
        append(stream, loadval(r0, 3));
        append(stream, loadval(r7, r0)); //r7 = address register
        append(stream, loadval(r0, 208)); // first 7 bits of first loadval instruction
        append(stream, loadval(r1, 16777216)); // 2^25
        append(stream, mult(r0, r0, r1)); // shift r0 by 25 bits
        append(stream, loadval(r1, 'M'));
        append(stream, add(r0, r0, r1)); // r0 contains the first instruction!
        append(stream, loadval(r1, 0));
        append(stream, segStore(r7, r1, r0));
        append(stream, loadval(r0, 160)); // first 7 bits of output command
        append(stream, loadval(r1, 16777216)); // 2^25
        append(stream, mult(r0, r0, r1)); // shift r0 by 25 bits no need for second addition bc we want register 0 to be acted on
        append(stream, loadval(r1, 1));
        append(stream, segStore(r7, r1, r0));
        append(stream, loadval(r0, 112)); // first 7 bits of output command -- 
        append(stream, loadval(r1, 16777216)); // 2^25
        append(stream, mult(r0, r0, r1)); // shift r0 by 25
        append(stream, loadval(r1, 2));
        append(stream, segStore(r7, r1, r0));
        // now we have a program that outputs M  loaded into segment m$r7
        append(stream, loadval(r0, 0));
        append(stream, loadProg(r7, r0));
}

void buildFiftyMillyNands(Seq_T stream)
{
        for(int i = 1; i < 50000000; i++) {
                append(stream, nand(r0, r0, r0));
        }
        append(stream, halt());
}

void buildFiftyMillyAdds(Seq_T stream)
{
        for(int i = 1; i < 50000000; i++) {
                append(stream, add(r0, r0, r0));
        }
        append(stream, halt());
}

void buildFiftyMillyMaps(Seq_T stream)
{       
        append(stream, loadval(r1, 1));
        for(int i = 2; i < 50000000; i++) {
                append(stream, map(r0, r1));
        }
        append(stream, halt());
}

void buildFiftyMillyMapUnmaps(Seq_T stream)
{       
        append(stream, loadval(r1, 1));
        for(int i = 0; i < 25000000; i++) {
                append(stream, map(r0, r1));
                append(stream, unmap(r0));
        }
        append(stream, halt());
}



/*----------------------------------------------------------------------------*/



void build_add_r2_r3(Seq_T stream) 
{
        append(stream, add(r1, r2, r3));
        append(stream, halt());

}

void build_print_digit(Seq_T stream)
{
        append(stream, loadval(r1, 48));
        append(stream, loadval(r2, 6));
        append(stream, add(r3, r1, r2));
        append(stream, output(r3));
        append(stream, halt());
}

void build_print_six_and_newline(Seq_T stream)
{
        append(stream, loadval(r1, 48));
        append(stream, loadval(r2, 6));
        append(stream, add(r3, r1, r2));
        append(stream, output(r3));
        append(stream, loadval(r3, '\n'));
        append(stream, output(r3));
        append(stream, halt());
}

void UREbuild_unmap_nonmapped_segment(Seq_T stream)
{       
        append(stream, loadval(r3, 3));
        append(stream, unmap(r3));
        // append(stream, loadval(r2, 2));
        // append(stream, map(r3, r2));
        append(stream, halt());
}

void UREbuild_nand_and_add_no_halt(Seq_T stream)
{
        append(stream, nand(r0, r6, r0));
        append(stream, nand(r0, r6, r0));
        append(stream, add(r0, r6, r0));
}

void UREbuildDivideByZero(Seq_T stream)
{
        append(stream, loadval(r3, 1));
        append(stream, loadval(r2, 0));
        append(stream, div(r4, r3, r2));
        append(stream, halt());
}

void UREbuildDoubleUnmap(Seq_T stream)
{
        append(stream, loadval(r2, 3));
        append(stream, map(r3, r2));
        append(stream, unmap(r3));
        append(stream, unmap(r3));
        append(stream, halt());
}

void UREbuildUnmapSegZero(Seq_T stream)
{       
        /* some other working code for testing */
        append(stream, loadval(r2, 0));
        append(stream, unmap(r2));
        append(stream, halt());
}

void UREoutputGT255(Seq_T stream)
{       
        /* some other working code for testing */
        append(stream, loadval(r2, 256));
        append(stream, output(r2));
        append(stream, halt());
}

void UREbuildSLOADUnmapped(Seq_T stream)
{
        append(stream, loadval(r3, 3));
        append(stream, loadval(r2, 3));
        append(stream, map(r3, r2));
        append(stream, unmap(r3));
        append(stream, loadval(r2, 1));
        append(stream, segLoad(r3, r3, r2));
        append(stream, halt());
}

void UREbuildSSTOREUnmapped(Seq_T stream)
{
        append(stream, loadval(r3, 3));
        append(stream, loadval(r2, 3));
        append(stream, map(r3, r2));
        append(stream, unmap(r3));
        append(stream, loadval(r2, 1));
        append(stream, segStore(r3, r3, r2));
        append(stream, halt());
}

void UREbuildSLOADOutOfBounds(Seq_T stream)
{
        append(stream, loadval(r2, 3));
        append(stream, map(r3, r2));
        append(stream, loadval(r2, 25));
        append(stream, segLoad(r4, r3, r2));
        /* print statement */
        append(stream, loadval(r2, 'y')); 
        append(stream, output(r2));
        append(stream, loadval(r2, '\n'));
        append(stream, output(r2));

        append(stream, halt());
}

void UREbuildSSTOREOutOfBounds(Seq_T stream)
{
        append(stream, loadval(r2, 3));
        append(stream, map(r3, r2));
        append(stream, loadval(r2, 25));
        append(stream, segStore(r3, r2, r3));
        /* print statement */
        append(stream, loadval(r2, 'y')); 
        append(stream, output(r2));
        append(stream, loadval(r2, '\n'));
        append(stream, output(r2));

        append(stream, halt());
}

void URELoadProgFromNonSegment(Seq_T stream)
{
        append(stream, loadval(r2, 3));
        append(stream, loadProg(r2, r2));
        /* print statement */
        append(stream, loadval(r2, 'y')); 
        append(stream, output(r2));
        append(stream, loadval(r2, '\n'));
        append(stream, output(r2));

        append(stream, halt());
}

void resourceExhaustion(Seq_T stream)
{
        for(int i = 0; i < 20000000; i++) {
                append(stream, loadval(r1, 'M'));
                append(stream, output(r1));
                append(stream, loadval(r1, 'M'));
                append(stream, output(r1));
        }
}

void queryLoopAdd(Seq_T stream)
{
        // PROBLEMS: quitting just reprompts the query every time
        char *query = "Enter a query: ";
        char *help1 = "Commands:\n";
        char *helpa = "a - add: takes in 2 ascii characters and adds them\n";
        char *helph = "h - help: display help commands\n";
        char *helpq = "q - quit: quits the program\n";
        char *notReckognize = "Query not recognized, try again.\n";
        int queryStart = Seq_length(stream);

        printString(stream, r0, query);

        append(stream, in(r2)); /* input first char into r1 */
        append(stream, in(r0)); /* input the garbage enter value into r0 */

        /* add Test condition */
        append(stream, loadval(r0, 1));
        append(stream, condMove(r1, r2, r0)); /* r1->user value to compare */
        append(stream, loadval(r0, 'a'));
        XOR(stream, r0, r1, r5);

        append(stream, loadval(r1, queryStart + 44)); /* HELPINSTRUCTION */
        append(stream, loadval(r5, queryStart + 68)); /* ADDJUMP */
        append(stream, condMove(r5, r1, r0)); /* go to the next instruction
                                               * check if user didn't enter a */
        append(stream, loadval(r1, 0));
        append(stream, loadProg(r1, r5));

        /* help Test condition */
        // int HELPINSTRUCTION = Seq_length(stream);
        // printf("\nHELPINSTRUCTION: %d\n", HELPINSTRUCTION);
        append(stream, loadval(r0, 1));
        append(stream, condMove(r1, r2, r0)); /* r1->user value to compare */
        append(stream, loadval(r0, 'h'));
        XOR(stream, r0, r1, r5);

        append(stream, loadval(r1, queryStart + 56)); /* QUITINSTRUCTION */
        append(stream, loadval(r5, queryStart + 215)); /* HELPJUMP */
        append(stream, condMove(r5, r1, r0)); /* go to the next instruction
                                               * check if user didn't enter h */
        append(stream, loadval(r1, 0));
        append(stream, loadProg(r1, r5));

        /* quit Test condition */
        // int QUITINSTRUCTION = Seq_length(stream);
        // printf("QUITINSTRUCTION: %d\n", QUITINSTRUCTION);
        append(stream, loadval(r0, 1));
        append(stream, condMove(r1, r2, r0)); /* r1->user value to compare */
        append(stream, loadval(r0, 'q'));
        XOR(stream, r0, r1, r5);

        append(stream, loadval(r1, queryStart + 463)); /* ENTER NOTRECOGNIZE JUMP*/
        append(stream, loadval(r5, queryStart + 460)); /* ENTER QUITJUMP */
        append(stream, condMove(r5, r1, r0)); /* go to the next instruction
                                               * check if user didn't enter h */
        append(stream, loadval(r1, 0));
        append(stream, loadProg(r1, r5));

        /* add */
        // int ADDJUMP = Seq_length(stream);
        // printf("ADDJUMP: %d\n", ADDJUMP);
        addQueryHelper(stream);
        append(stream, loadval(r1, 0));         /* jump back to the beggining */
        append(stream, loadval(r5, queryStart));/* of the query */
        append(stream, loadProg(r1, r5));

        /* help */
        // int HELPJUMP = Seq_length(stream);
        // printf("HELPJUMP: %d\n", HELPJUMP);
        printString(stream, r0, help1);
        printString(stream, r0, helpa);
        printString(stream, r0, helph);
        printString(stream, r0, helpq);
        append(stream, loadval(r1, 0));         /* jump back to the beggining */
        append(stream, loadval(r5, queryStart));/* of the query */
        append(stream, loadProg(r1, r5));

        /* quit */
        // int QUITJUMP = Seq_length(stream);
        // printf("QUITJUMP: %d\n", QUITJUMP);
        append(stream, loadval(r1, 0));
        append(stream, loadval(r5, queryStart + 532)); /* ENTER ENDJUMP */
        append(stream, loadProg(r1, r5));
        
        /* not recognized */
        // int NOTRECKOGNIZEDJUMP = Seq_length(stream);
        // printf("NOTRECKOGNIZED: %d\n", NOTRECKOGNIZEDJUMP);
        printString(stream, r0, notReckognize);

        append(stream, loadval(r1, 0));         /* jump back to the beggining */
        append(stream, loadval(r5, queryStart));/* of the query */
        append(stream, loadProg(r1, r5));

        /* end of the query */
        // int ENDJUMP = Seq_length(stream);
        // printf("ENDJUMP: %d\n", ENDJUMP);
        append(stream, halt()); 

}

static inline void addQueryHelper(Seq_T stream)
{
        char *intro = "Enter a single digit: ";
        char *exitMessage = "Sum: ";
        int helperStart = Seq_length(stream);
        printString(stream, r0, intro);
        
        append(stream, in(r1)); /* input first char into r1 */
        append(stream, in(r0)); /* input the garbage enter value into r0 */
        
        printString(stream, r0, intro);
        append(stream, in(r2)); /* input first char into r2 */
        append(stream, in(r0)); /* input the garbage enter value into r0 */
        //print the message before any conditional logic
        printString(stream, r0, exitMessage);
        // condition the next print
        append(stream, add(r1, r1, r2));
        subtract(stream, r1, r0, r2, 96);
        //now r1 holds the TRUE value of the calculation, r2, r0 are free
        append(stream, loadval(r0, 10));
        append(stream, div(r0, r1, r0));
        append(stream, loadval(r2, 0));
        XOR(stream, r0, r2, r3);

        // if r0 = 0, this is a single digit number. else r0 > 0 and this is a multi-digit number
        append(stream, loadval(r5, helperStart + 120)); /* ONEDIGIT */
        append(stream, loadval(r6, helperStart + 126)); /* TWODIGIT */
        append(stream, condMove(r5, r6, r0));
        append(stream, loadval(r6, 0));
        append(stream, loadProg(r6, r5));


        /* oneDigit logic */
        // int ONEDIGIT = Seq_length(stream);
        // printf("ONEDIGIT: %d\n", ONEDIGIT - helperStart);
        append(stream, loadval(r6, 48));
        append(stream, add(r1, r1, r6));
        append(stream, output(r1));
        append(stream, loadval(r6, 0));
        append(stream, loadval(r1, helperStart + 142)); /* ENDHELPER */
        append(stream, loadProg(r6, r1));

        /* twoDigit logic */
        // int TWODIGIT = Seq_length(stream);
        // printf("TWODIGIT: %d\n", TWODIGIT - helperStart);
        append(stream, loadval(r0, 10));
        append(stream, div(r0, r1, r0));
        append(stream, loadval(r6, 48));
        append(stream, add(r0, r0, r6));
        append(stream, output(r0)); /* output 10's place */
        append(stream, loadval(r0, 10));
        append(stream, div(r0, r1, r0)); /* get 10's place again */
        append(stream, loadval(r6, 10));
        append(stream, mult(r0, r0, r6)); /* r0 holds the number we want to subtract from r1 */
        append(stream, loadval(r6, 1));
        append(stream, nand(r0, r0, r0));
        append(stream, add(r0, r0, r6)); /* twos comp of r0 */
        append(stream, add(r1, r1, r0)); /* r1 holds the true value of the ones place */
        append(stream, loadval(r6, 48));
        append(stream, add(r1, r1, r6));
        append(stream, output(r1));

        /* end helper VVV*/
        // int ENDHELPER = Seq_length(stream);
        // printf("ENDHELPER: %d\n", ENDHELPER - helperStart);
        append(stream, loadval(r0, '\n'));
        append(stream, output(r0));
        
}

static inline void subtract(Seq_T stream, Um_register r, 
                            Um_register int1, Um_register int2, unsigned value)
{
        append(stream, loadval(int1, value));
        append(stream, loadval(int2, 1));
        append(stream, nand(int1, int1, int1)); /* not the value */
        append(stream, add(int1, int1, int2)); /* add 1 -> 2's comp*/
        append(stream, add(r, r, int1));

}

// To know which line we're on, test the stream length before a given 
// instruction
void SmallEqualityComparison(Seq_T stream) 
{
        /* setup vars */
        char *introp1 = "This program takes two single characters or digits";
        char *introp2 = " and compares their ascii values.\n";
        char *enter = "Enter a character or single digit: ";
        char *yes = "Yes, these entries are the same!\n";
        char *no  = "No, these entries are different!\n";

        /* intro */
        printString(stream, r0, introp1);
        printString(stream, r0, introp2);

        /* enter 1 */
        printString(stream, r0, enter);

        append(stream, in(r1)); /* input first char into r1 */
        append(stream, in(r0)); /* input the garbage enter value into r0 */

        /* enter 2 */
        printString(stream, r0, enter);

        append(stream, in(r2)); /* input second char into r2 */
        append(stream, in(r0)); /* input the garbage enter value into r0 */

        /* simple xor gate made with nands */
        XOR(stream, r1, r2, r3);

        /* conditionally print the correct message */
        for(int i = 0; i < 33; i++) {
                append(stream, loadval(r2, yes[i]));
                append(stream, loadval(r3, no[i]));
                append(stream, condMove(r2, r3, r1));
                append(stream, output(r2));
        }

        /* print a new line before exting */
        printString(stream, r0, "\n");

        append(stream, halt());

}

/* r is the register used for loading and output, 
 * string is the string to be printed to stdout
 * line counter represents where we are currently in our um program */
static inline void printString(Seq_T stream, Um_register r, char *string)
{
        int length = strlen(string);
        for(int i = 0; i < length; i++) {
                append(stream, loadval(r, string[i]));
                append(stream, output(r));
        }
}

/* r1 := r1 XOR r2 intermediate must be a register free for computation 
 * quick look usage: I can use this for equality comparisons */
 /* line counter represents the current instruction the UM program is on so that
  * we can program load to previous, known states (if we want) */
static inline void XOR(Seq_T stream, Um_register reg1, Um_register reg2, 
                       Um_register intermediate)
{
        append(stream, nand(intermediate, reg1, reg2));
        append(stream, nand(reg1, reg1, intermediate));
        append(stream, nand(reg2, reg2, intermediate));
        append(stream, nand(reg1, reg1, reg2));
}

/* to save the state of a program, the two intermediary registers, r5 and r6
 * Must be free for use. This limits most computations that utilize this 
 * program feature to storing information and calculations in only the
 * first five registers */
/* NOTE: im sure theres a much more elegant solution requiring fewer 
 * calculations and registers, but I don't have that time right now lol */
static inline void saveState(Seq_T stream)
{
        append(stream, loadval(r5, 8)); /* load 8->r5 to indicate size */
        append(stream, map(r6, r5)); /* create a new segment, store ID->r6 */
        append(stream, loadval(r5, 0));
        append(stream, segStore(r6, r5, r0)); /* save r0 */
        append(stream, loadval(r5, 1));
        append(stream, segStore(r6, r5, r1)); /* save r1 */
        append(stream, loadval(r5, 2));
        append(stream, segStore(r6, r5, r2)); /* save r2 */
        append(stream, loadval(r5, 3));
        append(stream, segStore(r6, r5, r3)); /* save r3 */
        append(stream, loadval(r5, 4));
        append(stream, segStore(r6, r5, r4)); /* save r4 */
        append(stream, loadval(r5, 7));
        append(stream, segStore(r6, r5, r7)); /* save last save state's pos */
        append(stream, condMove(r7, r6, r5)); /* new save state pos->r7 */

}

/* to load the previous register state, all current register values will be 
 * overridden. in addition, r6 must be a free register The old save state
 * register will be unmapped. r5 holds the ID of that unmapped state */
static inline void loadPrevState(Seq_T stream)
{
        append(stream, loadval(r6, 0));
        append(stream, segLoad(r0, r7, r6));
        append(stream, loadval(r6, 1));
        append(stream, segLoad(r1, r7, r6));
        append(stream, loadval(r6, 2));
        append(stream, segLoad(r2, r7, r6));
        append(stream, loadval(r6, 3));
        append(stream, segLoad(r3, r7, r6));
        append(stream, loadval(r6, 4));
        append(stream, segLoad(r4, r7, r6));
        append(stream, condMove(r5, r7, r6)); /* r5 holds the curr position */
        append(stream, loadval(r6, 7));
        append(stream, segLoad(r7, r7, r6)); /* r7->prev state position */
        append(stream, unmap(r5));

}


void build_milo_forLoop(Seq_T stream)
{       
        uint32_t limit = 10000;
        uint32_t loopStart = 16;
        uint32_t onePastCompare = 43;
        uint32_t compareStart = 31;
        //uint32_t testLength = 0;
        
        /* info segment set up */
        /* The segment SHOULD now hold the following information for reference:
         *      0      |       1       |       2       |       3       |
         *   currVal   |     limit     |   loopStart   |onePastCompare |
         *   of i (0)  |               |               |               |
        */
        
        append(stream, loadval(r1, 4));   /* give r1 4 for info segment setup */
        /* create the info segment (size = 4) storing its tag in r7 */
        append(stream, map(r7, r1)); 
        append(stream, loadval(r0, 1)); /* r0 gets the index value 1 */
        /* r1 gets the value of limit (to be stored) */
        append(stream, loadval(r1, limit));
        /* store the limit information in m[r7][r0] */
        append(stream, segStore(r7, r0, r1));   
        append(stream, loadval(r0, 2));
        append(stream, loadval(r1, loopStart));
        append(stream, segStore(r7, r0, r1));
        append(stream, loadval(r0, 3));
        append(stream, loadval(r1, onePastCompare));
        append(stream, segStore(r7, r0, r1));
        
        /* initial comparson */
        append(stream, loadval(r0, 0));
        append(stream, loadval(r3, compareStart));
        append(stream, segLoad(r1, r7, r0));
        append(stream, loadval(r2, 1));
        append(stream, loadProg(r0, r3)); /*Should always jump to comparestart*/

        /* loop -- simple logic, should output a series of Milo's*/
        append(stream, loadval(r1, 'M')); /* LOOP START */
        append(stream, output(r1));
        append(stream, loadval(r1, 'i'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'l'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'o'));
        append(stream, output(r1));
        append(stream, loadval(r1, '\n'));
        append(stream, output(r1));

        /* add 1 to i */
        append(stream, loadval(r3, 0));
        append(stream, loadval(r2, 1));
        append(stream, segLoad(r1, r7, r3));
        append(stream, add(r1, r1, r2));
        append(stream, segStore(r7, r3, r1));
        
        /* compare and conditionally jump */
        append(stream, segLoad(r2, r7, r2)); /* COMPARE START */
        // got here, then we fucked up.
        append(stream, nand(r3, r1, r2));
        append(stream, nand(r1, r1, r3));
        append(stream, nand(r2, r2, r3));
        append(stream, nand(r1, r1, r2));
        append(stream, loadval(r2, 2));
        append(stream, segLoad(r2, r7, r2));
        append(stream, loadval(r3, 3));
        append(stream, segLoad(r3, r7, r3));
        append(stream, condMove(r2, r3, r1));
        append(stream, loadval(r3, 0));
        append(stream, loadProg(r3, r2));
        append(stream, unmap(r7)); /* ONE PAST COMPARE */
        append(stream, halt());

}

        /*
        
                char *gotToCompareStart = "Got to compare start\n";
        for (int i = 0; i < 21; i++) {
                append(stream, loadval(r1, gotToCompareStart[i]));
                append(stream, output(r1));
        }

        TESTING
        char *comparisonOver = "Done with the first comparison!\n";
        for (int i = 0; i < 32; i++) {
                append(stream, loadval(r1, comparisonOver[i]));
                append(stream, output(r1));
                testLength++;
        }
        

        TESTING
        char *initialCompare = "initializing info for the first comparison\n";
        for (int i = 0; i < 43; i++) {
                append(stream, loadval(r1, initialCompare[i]));
                append(stream, output(r1));
                testLength++;
        }
        
        TESTING PRINT STATEMENT
        char *infoSetupDone = "Info is all Set\n"; // 16 long
        for (int i = 0; i < 16; i++) {
                append(stream, loadval(r1, infoSetupDone[i]));
                append(stream, output(r1));
                testLength++;
        }

        TESTING PRINT STATEMENT
        char *infoSetupString = "Starting Info Setup\n"; // 20 long
        for (int i = 0; i < 20; i++) {
                append(stream, loadval(r1, infoSetupString[i]));
                append(stream, output(r1));
                testLength++;
        } 
        
        
        */
void buildOut(Seq_T stream)
{
        for(int i = 0; i < 256; i++) {
                append(stream, loadval(r1, i));
                append(stream, output(r1));
        }
        append(stream, halt());
}
