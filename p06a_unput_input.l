%{
/*
 * ====================================================================
 * Practical No : 6(a)
 * Title        : WAP to implement unput and input in a LEX program.
 * ====================================================================
 * THEORY & LOGIC
 *  1. input() reads the NEXT character directly from the scanner's
 *     input stream, bypassing the pattern rules; it returns 0 (or EOF
 *     in newer FLEX) at end of file.
 *  2. unput(c) does the opposite: it pushes the character c back so
 *     that it will be read again by the scanner as the next input.
 *  3. Several unput() calls must be made in REVERSE order, because the
 *     last character pushed back is the first one read again.
 *  4. Rule "PI"  uses unput(): the word PI is replaced by 3.14159 by
 *     pushing the digits back from the last to the first; the scanner
 *     then re-reads and prints them (like a tiny macro expander).
 *  5. The comment rule uses input(): once a comment starts, the action reads
 *     characters itself in a loop until the closing "* /" is found, so
 *     the whole comment is skipped without extra rules.
 *  6. unput() destroys the contents of yytext, so yytext must not be
 *     used after the call.
 *  7. Every other character is copied to the output by ECHO.
 *  8. Both functions give the programmer control of the input stream
 *     that plain regular expressions cannot give.
 *  9. Example: the input  area = PI * r  followed by a block comment
 *     becomes  area = 3.14159 * r  with the comment removed.
 *
 * Compile : flex p06a_unput_input.l
 *           gcc lex.yy.c -o p06a
 * Run     : ./p06a        (type text, Ctrl+D to end)
 * ====================================================================
 */
#include <stdio.h>
#include <string.h>
int comments = 0, replaced = 0;
%}

%%
"PI"    {   /* expand PI using unput - push back in REVERSE order */
            const char *value = "3.14159";
            int i;
            replaced++;
            for (i = (int)strlen(value) - 1; i >= 0; i--)
                unput(value[i]);
        }

"/*"    {   /* skip a block comment using input() */
            int c;
            comments++;
            for (;;) {
                while ((c = input()) != '*' && c != EOF && c != 0)
                    ;                                   /* look for '*'   */
                if (c == EOF || c == 0) break;          /* unterminated   */
                while ((c = input()) == '*')
                    ;                                   /* skip extra '*' */
                if (c == '/' || c == EOF || c == 0) break;
            }
        }

.|\n    { ECHO; }
%%

int yywrap(void) { return 1; }

int main(void)
{
    printf("Enter text (use PI and /* comments */), Ctrl+D to end:\n");
    yylex();
    printf("\nPI replaced %d time(s), %d comment(s) skipped.\n",
           replaced, comments);
    return 0;
}
