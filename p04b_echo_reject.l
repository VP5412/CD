%{
/*
 * ====================================================================
 * Practical No : 4(b)
 * Title        : WAP to implement the ECHO and REJECT functions
 *                provided in LEX.
 * ====================================================================
 * THEORY & LOGIC
 *  1. ECHO is a macro that copies the current yytext to the output
 *     file yyout; it is also what the default (unmatched) rule does.
 *  2. REJECT tells the scanner: "this match is not what I want - go
 *     back and use the next best rule for the SAME input text".
 *  3. The next best choice is either a shorter match by another rule
 *     or an equal-length match by a rule written later in the file.
 *  4. The four rules  a | ab | abc | abcd  share one action (the
 *     '|' means "same action as the next rule").
 *  5. For the input "abcd": abcd is matched first (longest), ECHO
 *     prints it, REJECT falls back to abc, then ab, then a - each is
 *     printed and rejected in turn.
 *  6. After 'a' is rejected, the catch-all rule  .|\n  swallows the
 *     character, and scanning continues with "bcd".
 *  7. Therefore the output for the input abcd is  abcdabcaba  - this
 *     shows that REJECT lets one piece of text be matched many times.
 *  8. The number rule shows ECHO used on its own, wrapped in < >.
 *  9. REJECT makes the scanner slower and cannot be used with yymore().
 *
 * Compile : flex p04b_echo_reject.l
 *           gcc lex.yy.c -o p04b
 * Run     : ./p04b       (type  abcd  and press Enter, then Ctrl+D)
 * ====================================================================
 */
#include <stdio.h>
%}

%%
a       |
ab      |
abc     |
abcd    { ECHO; REJECT; }           /* print, then try the next best rule */
[0-9]+  { printf("<"); ECHO; printf(">"); }   /* ECHO used by itself      */
.|\n    { /* swallow everything else */ }
%%

int yywrap(void) { return 1; }

int main(void)
{
    printf("Type 'abcd' (or numbers) and press Enter, Ctrl+D to end:\n");
    yylex();
    printf("\n");
    return 0;
}
