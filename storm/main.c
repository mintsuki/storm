#include <stdio.h>
#include <stdlib.h>

#include "literals.h"
#include "tokeniser.h"
#include "lexer.h"
#include "codegen.h"
#include "machine.h"

FILE *sourcefd;
FILE *outputasm;
FILE *tmp0;

//#define LOGGING

#ifdef LOGGING
    #define logging stderr
#else
    #define logging devnull
#endif

int main(int argc, char **argv) {
    FILE *devnull = fopen("/dev/null", "w");

    if (argc < 2) {
        fprintf(stderr, "No source specified.\n");
        return 1;
    }

    if (argc < 3) {
        fprintf(stderr, "No output name specified.\n");
        return 1;
    }

    if (!(sourcefd = fopen(argv[1], "rb"))) {
        perror("Couldn't open source file");
        return 1;
    }

    if (!(outputasm = fopen(argv[2], "wb"))) {
        fclose(sourcefd);
        perror("Couldn't open output file");
        return 1;
    }

    if (!(tmp0 = tmpfile())) {
        fclose(sourcefd);
        fclose(outputasm);
        perror("Couldn't open tmp0");
        return 1;
    }

    literals_convert(sourcefd, outputasm, tmp0);

    fclose(sourcefd);

    fprintf(outputasm, MACHINE_SECTION_TEXT);
    fprintf(outputasm, MACHINE_CDECL);

    rewind(tmp0);

    token_t *tokens = malloc(32768 * sizeof(token_t));

    tokenise(tmp0, tokens, logging);

    lex_t *lexes = malloc(32768 * sizeof(lex_t));

    lex(lexes, tokens, logging);

    codegen(outputasm, lexes);

    free(tokens);
    free(lexes);

    fclose(outputasm);
    fclose(tmp0);

    return 0;
}