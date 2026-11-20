%{
/*
 * ====================================================================
 * Practical No : 4(c)
 * Title        : WAP to implement the BEGIN directive in a LEX program.
 * ====================================================================
 * THEORY & LOGIC
 *  1. A "start condition" (state) lets the scanner behave differently
 *     in different contexts - for example inside a comment or a string.
 *  2. States are declared in the definition section:
 *        %s  name  - inclusive state (rules without a state also work)
 *        %x  name  - exclusive state (ONLY rules tagged <name> work)
 *  3. The directive  BEGIN(name);  switches the scanner to that state;
 *     BEGIN(INITIAL); returns to the normal default state.
 *  4. A rule is tied to a state by writing the state name in angle
 *     brackets before the pattern, e.g.  <COMMENT>"*" "/".
 *  5. This program has two exclusive states: COMMENT and STRING.
 *  6. "/ *" switches to COMMENT; every character inside is skipped
 *     until "* /" switches back to INITIAL.
 *  7. A double quote switches to STRING; the text up to the closing
 *     quote is printed as one string literal.
 *  8. Words found in the INITIAL state are reported as WORD.
 *  9. BEGIN therefore gives a lightweight finite-state machine on top
 *     of the regular expressions, with no extra C code.
 *
 * Compile : flex p04c_begin_directive.l
 *           gcc lex.yy.c -o p04c
 * Run     : ./p04c        (type a line with a block comment and a
 *                          "quoted string" in it, Ctrl+D to end)
 * ====================================================================
 */
#include <stdio.h>
int comments = 0, strings = 0;
%}

%x COMMENT
%x STRING

%%
"/*"                 { BEGIN(COMMENT); }                 /* enter comment  */
<COMMENT>"*/"        { BEGIN(INITIAL); comments++;       /* leave comment  */
                       printf("[comment skipped]\n"); }
<COMMENT>(.|\n)      { /* ignore text inside comment */ }

\"                   { BEGIN(STRING); printf("STRING : "); }
<STRING>[^"\n]+      { ECHO; }
<STRING>\"           { BEGIN(INITIAL); strings++; printf("\n"); }
<STRING>\n           { BEGIN(INITIAL); printf("  (unterminated)\n"); }

[a-zA-Z0-9_]+        { printf("WORD   : %s\n", yytext); }
(.|\n)               { /* ignore other characters */ }
%%

int yywrap(void) { return 1; }

int main(void)
{
    printf("Enter text (Ctrl+D to end):\n");
    yylex();
    printf("Comments: %d, Strings: %d\n", comments, strings);
    return 0;
}
