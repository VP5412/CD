%{
/*
 * ====================================================================
 * Practical No : 3(b)
 * Title        : Create a Lex program to take input from a text file and
 *                count the number of characters, words and lines.
 * ====================================================================
 * THEORY & LOGIC
 *  1. LEX turns the regular expressions below into a DFA (function
 *     yylex) which scans the input file one match at a time.
 *  2. The input file is attached to the scanner by assigning an opened
 *     file to the global pointer yyin before yylex() is called.
 *  3. Rule 1  \n        : a newline ends one line, so lines++ and,
 *     because '\n' is also a character, chars++.
 *  4. Rule 2  [^ \t\n]+ : a maximal run of non-blank characters is one
 *     WORD; words++ and chars += yyleng (length of the matched text).
 *  5. Rule 3  [ \t]+    : blanks and tabs are not words, but they are
 *     characters, so only chars is increased.
 *  6. Because LEX always takes the LONGEST match, a whole word is
 *     consumed by one rule instead of character by character.
 *  7. yywrap() returns 1 so the scanner stops at end of file.
 *  8. After yylex() returns, main() prints the three counters.
 *  9. Result is the same as the UNIX command  wc  (a last line with no
 *     trailing newline is not counted as a line, exactly like wc -l).
 *
 * Compile : flex p03b_count_chars_words_lines.l
 *           gcc lex.yy.c -o p03b
 * Run     : ./p03b sample_inputs/sample_text.txt
 * ====================================================================
 */
#include <stdio.h>
int chars = 0, words = 0, lines = 0;
%}

%%
\n          { lines++; chars++; }
[^ \t\n]+   { words++; chars += yyleng; }
[ \t]+      { chars += yyleng; }
%%

int yywrap(void)
{
    return 1;               /* no more input files */
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <text-file>\n", argv[0]);
        return 1;
    }
    yyin = fopen(argv[1], "r");     /* attach file to the scanner */
    if (yyin == NULL) {
        perror("Cannot open file");
        return 1;
    }
    yylex();
    fclose(yyin);

    printf("Characters : %d\n", chars);
    printf("Words      : %d\n", words);
    printf("Lines      : %d\n", lines);
    return 0;
}
