%{
/*
 * ====================================================================
 * Practical No : 6(b)
 * Title        : WAP to implement yyterminate and yy_flush_buffer in a
 *                LEX program.
 * ====================================================================
 * THEORY & LOGIC
 *  1. yyterminate() is a macro that makes yylex() return 0 at once,
 *     even though input is still left; it is the "stop scanning now"
 *     command (the scanner also does this itself at end of file).
 *  2. After yyterminate() control returns to the caller of yylex(),
 *     so main() can print its final report.
 *  3. yy_flush_buffer(b) (FLEX only) empties the internal input buffer
 *     b, throwing away characters that were read but not yet scanned.
 *  4. The macro YY_FLUSH_BUFFER is shorthand for flushing the buffer
 *     currently in use, YY_CURRENT_BUFFER.
 *  5. When a file is scanned, FLEX reads a big block (several KB) in
 *     advance, so flushing discards the rest of that block; on a
 *     terminal it discards the rest of the typed line.
 *  6. Rule "STOP"  : prints a message and calls yyterminate().
 *  7. Rule "FLUSH" : prints a message and flushes the buffer, so the
 *     words that follow it in the same buffer are never counted.
 *  8. Rule [a-zA-Z]+ counts and prints ordinary words.
 *  9. Try:  one two FLUSH three   -> only 'one' and 'two' are counted;
 *           one two STOP three    -> scanning ends at STOP.
 *
 * Compile : flex p06b_yyterminate_flush.l
 *           gcc lex.yy.c -o p06b
 * Run     : ./p06b        (type words, Ctrl+D to end)
 * ====================================================================
 */
#include <stdio.h>
int words = 0;
%}

%%
"STOP"       { printf(">> STOP found: calling yyterminate()\n");
               yyterminate(); }
"FLUSH"      { printf(">> FLUSH found: calling yy_flush_buffer()\n");
               YY_FLUSH_BUFFER; }
[a-zA-Z]+    { words++; printf("word %d : %s\n", words, yytext); }
.|\n         { /* ignore everything else */ }
%%

int yywrap(void) { return 1; }

int main(void)
{
    printf("Type words. Use STOP to terminate, FLUSH to drop the buffer.\n");
    yylex();
    printf("Scanner finished. Total words counted = %d\n", words);
    return 0;
}
