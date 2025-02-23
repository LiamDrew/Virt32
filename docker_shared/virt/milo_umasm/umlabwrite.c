#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "fmt.h"
#include "seq.h"

extern void Um_write_sequence(FILE *output, Seq_T instructions);

extern void build_halt_test(Seq_T instructions);
extern void build_verbose_halt_test(Seq_T instructions);
extern void build_add_r2_r3(Seq_T instructions);
extern void build_print_digit(Seq_T instructions);
extern void build_milo_forLoop(Seq_T stream);
extern void build_print_six_and_newline(Seq_T stream);
extern void UREbuild_unmap_nonmapped_segment(Seq_T stream);
extern void buildOut(Seq_T stream);
extern void UREbuild_nand_and_add_no_halt(Seq_T stream);
extern void UREbuildSLOADUnmapped(Seq_T stream);
extern void UREbuildDivideByZero(Seq_T stream);
extern void UREbuildDoubleUnmap(Seq_T stream);
extern void UREbuildUnmapSegZero(Seq_T stream);
extern void UREoutputGT255(Seq_T stream);
extern void SmallEqualityComparison(Seq_T stream);
extern void UREbuildSSTOREUnmapped(Seq_T stream);
extern void UREbuildSLOADOutOfBounds(Seq_T stream);
extern void UREbuildSSTOREOutOfBounds(Seq_T stream);
extern void URELoadProgFromNonSegment(Seq_T stream);
//extern void resourceExhaustion(Seq_T stream);
extern void queryLoopAdd(Seq_T stream);
extern void buildOutputM(Seq_T stream);
extern void buildOutputString(Seq_T stream);
extern void buildLoadValSimple(Seq_T stream);
extern void buildOverloadVal(Seq_T stream);
extern void buildNandSimple(Seq_T stream);
extern void buildXOR(Seq_T stream);
extern void buildSimpleInput(Seq_T stream); 
extern void buildSimpleInput(Seq_T stream);
extern void buildSimpleAdd(Seq_T stream);
extern void buildSubtract(Seq_T stream);
extern void buildExponential(Seq_T stream);
extern void buildMultiplySimple(Seq_T stream);
extern void buildMultiplyWrapAround(Seq_T stream);
extern void buildDivide(Seq_T stream);
extern void buildDivideFloor(Seq_T stream);
extern void buildTimesTables(Seq_T stream);
extern void buildMapSimple(Seq_T stream);
extern void buildUnmapSimple(Seq_T stream);
extern void buildMapUnmapVerbose(Seq_T stream);
extern void buildSegStore(Seq_T stream);
extern void buildSegLoad(Seq_T stream);
extern void buildLoadProgramOutput(Seq_T stream);
extern void buildFiftyMillyNands(Seq_T stream);
extern void buildFiftyMillyAdds(Seq_T stream);
extern void buildFiftyMillyMaps(Seq_T stream);
extern void buildFiftyMillyMapUnmaps(Seq_T stream);

/* The array `tests` contains all unit tests for the lab. */

static struct test_info {
        const char *name;
        const char *test_input;          /* NULL means no input needed */
        const char *expected_output;
        /* writes instructions into sequence */
        void (*build_test)(Seq_T stream);
} tests[] = {
        { "halt",          NULL,  "", build_halt_test },
        { "halt-verbose",  NULL,  "", build_verbose_halt_test },
        { "add",           NULL,  "", build_add_r2_r3 },
        { "print-six",     NULL, "6", build_print_digit },
        { "print-six-nl",  NULL,  "", build_print_six_and_newline },
        { "Milo-for-loop", NULL,  "", build_milo_forLoop },
        { "unmap-non-mapped", NULL, "", UREbuild_unmap_nonmapped_segment },
        { "Output",        NULL,  "", buildOut },
        { "nand-and-add-no-halt",  NULL,  "", UREbuild_nand_and_add_no_halt },
        { "load-into-unmapped", NULL, "", UREbuildSLOADUnmapped },
        { "store-into-unmapped", NULL, "", UREbuildSSTOREUnmapped },
        { "load-out-of-bounds", NULL, "", UREbuildSLOADOutOfBounds },
        { "store-out-of-bounds", NULL, "", UREbuildSSTOREOutOfBounds },
        { "div-by-zero",   NULL, "", UREbuildDivideByZero },
        { "double-unmap", NULL, "", UREbuildDoubleUnmap },
        { "unmap-seg-zero", NULL, "", UREbuildUnmapSegZero},
        { "outputIs>255", NULL, "", UREoutputGT255},
        { "simpleEqComp", NULL, "", SmallEqualityComparison},
        { "prog-load-non-segment", NULL, "", URELoadProgFromNonSegment},
        { "queryAdd", NULL, "", queryLoopAdd },
        { "outputM", NULL, "M", buildOutputM},
        { "outputString", NULL, "", buildOutputString},
        { "loadValSimple", NULL, "", buildLoadValSimple},
        { "overLoadVal", NULL, "", buildOverloadVal},
        { "nandSimple", NULL, "1", buildNandSimple},
        { "xor", NULL, "0", buildXOR},
        { "inputSimple", "0", "0", buildSimpleInput},
        { "addSimple", NULL, "0", buildSimpleAdd },
        { "subtract", NULL, "0", buildSubtract},
        { "exponential", NULL, "0", buildExponential},
        { "multiplySimple", NULL, "0", buildMultiplySimple},
        { "multiplyWrapAround", NULL, "0", buildMultiplyWrapAround},
        { "divide", NULL, "0", buildDivide},
        { "divideFloor", NULL, "0", buildDivideFloor},
        { "timesTable", NULL, "", buildTimesTables},
        { "mapSimple", NULL, "0", buildMapSimple},
        { "unmapSimple", NULL, "", buildUnmapSimple},
        { "mapUnmapVerbose", NULL, "", buildMapUnmapVerbose},
        { "segmentedStore", NULL, "", buildSegStore},
        { "segmentedLoad",  NULL, "0", buildSegLoad},
        { "loadProgToOutputM", NULL, "M", buildLoadProgramOutput}//,
        //{ "fiftyMillyNands", NULL, "", buildFiftyMillyNands},
        //{ "fiftyMillyAdds", NULL, "", buildFiftyMillyAdds},
        //{ "fiftyMillyMaps", NULL, "", buildFiftyMillyMaps},
        //{ "fiftyMillyMapUnmaps", NULL, "", buildFiftyMillyMapUnmaps}

        //{ "exhaust", NULL, "", resourceExhaustion}
};

  
#define NTESTS (sizeof(tests)/sizeof(tests[0]))

/*
 * open file 'path' for writing, then free the pathname;
 * if anything fails, checked runtime error
 */
static FILE *open_and_free_pathname(char *path);

/*
 * if contents is NULL or empty, remove the given 'path', 
 * otherwise write 'contents' into 'path'.  Either way, free 'path'.
 */
static void write_or_remove_file(char *path, const char *contents);

static void write_test_files(struct test_info *test);


int main (int argc, char *argv[])
{
        bool failed = false;
        if (argc == 1)
                for (unsigned i = 0; i < NTESTS; i++) {
                        printf("***** Writing test '%s'.\n", tests[i].name);
                        write_test_files(&tests[i]);
                }
        else
                for (int j = 1; j < argc; j++) {
                        bool tested = false;
                        for (unsigned i = 0; i < NTESTS; i++)
                                if (!strcmp(tests[i].name, argv[j])) {
                                        tested = true;
                                        write_test_files(&tests[i]);
                                }
                        if (!tested) {
                                failed = true;
                                fprintf(stderr,
                                        "***** No test named %s *****\n",
                                        argv[j]);
                        }
                }
        return failed; /* failed nonzero == exit nonzero == failure */
}


static void write_test_files(struct test_info *test)
{
        FILE *binary = open_and_free_pathname(Fmt_string("%s.um", test->name));
        Seq_T instructions = Seq_new(0);
        test->build_test(instructions);
        Um_write_sequence(binary, instructions);
        Seq_free(&instructions);
        fclose(binary);

        write_or_remove_file(Fmt_string("%s.0", test->name),
                             test->test_input);
        write_or_remove_file(Fmt_string("%s.1", test->name),
                             test->expected_output);
}


static void write_or_remove_file(char *path, const char *contents)
{
        if (contents == NULL || *contents == '\0') {
                remove(path);
        } else {
                FILE *input = fopen(path, "wb");
                assert(input != NULL);

                fputs(contents, input);
                fclose(input);
        }
        free(path);
}


static FILE *open_and_free_pathname(char *path)
{
        FILE *fp = fopen(path, "wb");
        assert(fp != NULL);

        free(path);
        return fp;
}

