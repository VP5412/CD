%{
/*
 * ====================================================================
 * Practical No : 6(c)
 * Title        : WAP to implement yywrap in a LEX program.
 * ====================================================================
 * THEORY & LOGIC
 *  1. yywrap() is a user-definable function that the scanner calls
 *     every time it reaches the END of its current input.
 *  2. If yywrap() returns 1 the scanner finishes and yylex() returns;
 *     this is the normal answer when there is no more input.
 *  3. If yywrap() returns 0 the scanner assumes the program has opened
 *     a NEW input (by assigning a new file to yyin) and continues.
 *  4. This makes it possible to scan several files one after another
 *     as if they were one long file.
 *  5. In this program the file names come from the command line;
 *     main() opens the first one and yywrap() opens each next one.
 *  6. When a file is exhausted, yywrap() closes it, opens the next
 *     name in argv and returns 0; after the last file it returns 1.
 *  7. The rules simply count words and lines across ALL the files.
 *  8. If the program does not define yywrap(), the library -lfl
 *     supplies a default one that always returns 1 (or use
 *     %option noyywrap).
 *  9. Example: ./p06c sample_inputs/file1.txt sample_inputs/file2.txt
 *
 * Compile : flex p06c_yywrap.l
 *           gcc lex.yy.c -o p06c
 * Run     : ./p06c file1.txt file2.txt ...
 * ====================================================================
 */
#include <stdio.h>
int words = 0, lines = 0;
char **files;           /* file names from the command line  */
int nfiles, current;    /* how many files, and which is open */
%}

%%
\n            { lines++; }
[^ \t\n]+     { words++; }
[ \t]+        { /* ignore blanks */ }
%%

/* Called at the end of each file */
int yywrap(void)
{
    printf("-- finished file: %s\n", files[current]);
    current++;
    if (current < nfiles) {
        FILE *next = fopen(files[current], "r");
        if (next == NULL) {
            perror(files[current]);
            return 1;                   /* cannot continue: stop      */
        }
        fclose(yyin);
        yyin = next;                    /* give the scanner a new file */
        return 0;                       /* 0 = keep scanning           */
    }
    return 1;                           /* 1 = no more input           */
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s file1 [file2 ...]\n", argv[0]);
        return 1;
    }
    files  = argv + 1;
    nfiles = argc - 1;
    current = 0;

    yyin = fopen(files[0], "r");
    if (yyin == NULL) {
        perror(files[0]);
        return 1;
    }
    yylex();
    fclose(yyin);

    printf("Total words = %d, total lines = %d (in %d file(s))\n",
           words, lines, nfiles);
    return 0;
}
