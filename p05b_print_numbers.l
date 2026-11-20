%{
/*
 * ====================================================================
 * Practical No : 5(b)
 * Title        : Write a Lex program to print out all numbers from the
 *                given file.
 * ====================================================================
 * THEORY & LOGIC
 *  1. A number is a lexical pattern, so it can be described by a
 *     regular expression and recognised automatically by LEX.
 *  2. Integer part   : [0-9]+            (one or more digits)
 *  3. Fraction part  : (\.[0-9]+)?       (optional dot and digits)
 *  4. Exponent part  : ([eE][-+]?[0-9]+)? (optional, e.g. 1e5, 2E-3)
 *  5. The three parts joined form one rule; thanks to the longest-
 *     match principle  3.14  is printed as one number, not as 3 and 14.
 *  6. The input is read from the file given on the command line by
 *     assigning it to yyin.
 *  7. Each match is printed through yytext and counted.
 *  8. Everything that is not a number (letters, symbols, newlines) is
 *     consumed silently by the rule  .|\n  so only numbers appear in
 *     the output.
 *  9. Signs are treated as separate characters, so "-5" prints 5.
 *
 * Compile : flex p05b_print_numbers.l
 *           gcc lex.yy.c -o p05b
 * Run     : ./p05b sample_inputs/numbers.txt
 * ====================================================================
 */
#include <stdio.h>
int count = 0;
%}

DIGIT   [0-9]

%%
{DIGIT}+(\.{DIGIT}+)?([eE][-+]?{DIGIT}+)?   { printf("%s\n", yytext); count++; }
.|\n                                         { /* skip non-numbers */ }
%%

int yywrap(void) { return 1; }

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }
    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("Cannot open file");
        return 1;
    }
    printf("Numbers found in %s:\n", argv[1]);
    yylex();
    fclose(yyin);
    printf("Total numbers = %d\n", count);
    return 0;
}
