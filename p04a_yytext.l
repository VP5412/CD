%{
/*
 * ====================================================================
 * Practical No : 4(a)
 * Title        : WAP to implement the yytext method in a LEX program.
 * ====================================================================
 * THEORY & LOGIC
 *  1. yytext is a global character array (char *) that LEX fills with
 *     the exact text matched by the rule that is currently executing.
 *  2. yyleng is its companion: an integer holding the length of yytext.
 *  3. yytext is overwritten on every new match, so a program must copy
 *     it (strcpy / strdup) if the text is needed after the action ends.
 *  4. The program has three classes of lexemes: NUMBER, IDENTIFIER and
 *     any other single SYMBOL.
 *  5. Rule  [0-9]+  matches a run of digits; the matched digits are
 *     printed through yytext, and atoi(yytext) converts them to int to
 *     show that yytext is an ordinary C string.
 *  6. Rule  [a-zA-Z_][a-zA-Z0-9_]*  matches identifiers; yytext[0]
 *     (the first character) is shown to prove it can be indexed.
 *  7. White space is ignored by an empty action.
 *  8. The dot rule  .  catches every other character one at a time.
 *  9. Longest-match and first-rule priority decide which rule runs
 *     (e.g. "abc123" is one identifier, not identifier + number).
 *
 * Compile : flex p04a_yytext.l
 *           gcc lex.yy.c -o p04a
 * Run     : ./p04a        (type input, Ctrl+D to finish)
 * ====================================================================
 */
#include <stdio.h>
#include <stdlib.h>
%}

%%
[0-9]+                   { printf("NUMBER     : %-10s length=%d  value+1=%d\n",
                                  yytext, yyleng, atoi(yytext) + 1); }
[a-zA-Z_][a-zA-Z0-9_]*   { printf("IDENTIFIER : %-10s length=%d  first char='%c'\n",
                                  yytext, yyleng, yytext[0]); }
[ \t\n]+                 { /* ignore white space */ }
.                        { printf("SYMBOL     : %s\n", yytext); }
%%

int yywrap(void) { return 1; }

int main(void)
{
    printf("Enter text (Ctrl+D to end):\n");
    yylex();
    return 0;
}
