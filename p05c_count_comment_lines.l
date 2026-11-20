%{
/*
 * ====================================================================
 * Practical No : 5(c)
 * Title        : Write a Lex program to count the number of comment
 *                lines in a given C program.
 * ====================================================================
 * THEORY & LOGIC
 *  1. C has single-line comments  // ...  and block comments that may
 *     span many lines; every physical line of a comment is counted.
 *  2. A single-line comment is matched by the regular expression
 *     "//".*  (the dot stops at the end of the line); one match = one
 *     comment line.
 *  3. A block comment needs memory of "being inside a comment", so an
 *     exclusive start condition BLOCK is used (BEGIN switches to it).
 *  4. When "/ *" is seen the first comment line is counted and the
 *     scanner enters BLOCK.
 *  5. Inside BLOCK every newline means that the comment continues on
 *     one more line, so the counter is incremented again.
 *  6. "* /" leaves BLOCK; any other character inside is skipped.
 *  7. Code lines are skipped by the last rule (.|\n) in the INITIAL
 *     state, so only comment lines are counted.
 *  8. Example: a block comment on lines 3-5 adds 3, and a trailing
 *     // comment adds 1.
 *  9. Limitation: a "/ *" appearing inside a string literal is treated
 *     as a comment start (a full scanner would also track strings).
 *
 * Compile : flex p05c_count_comment_lines.l
 *           gcc lex.yy.c -o p05c
 * Run     : ./p05c sample_inputs/with_comments.c
 * ====================================================================
 */
#include <stdio.h>
int comment_lines = 0;
%}

%x BLOCK

%%
"//".*              { comment_lines++; }             /* single-line comment */
"/*"                { comment_lines++; BEGIN(BLOCK); }
<BLOCK>\n           { comment_lines++; }             /* next line of comment */
<BLOCK>"*/"         { BEGIN(INITIAL); }
<BLOCK>.            { /* skip comment text */ }
.|\n                { /* skip code */ }
%%

int yywrap(void) { return 1; }

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <c-file>\n", argv[0]);
        return 1;
    }
    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("Cannot open file");
        return 1;
    }
    yylex();
    fclose(yyin);
    printf("Number of comment lines = %d\n", comment_lines);
    return 0;
}
