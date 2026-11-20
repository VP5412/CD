%{
/*
 * ====================================================================
 * Practical No : 6(d)
 * Title        : WAP to implement yymore and yyless in a LEX program.
 * ====================================================================
 * THEORY & LOGIC
 *  1. yymore() tells the scanner: "do not throw away the text just
 *     matched; the NEXT match must be APPENDED to it". The next rule's
 *     yytext therefore holds old text + new text.
 *  2. yyless(n) tells the scanner: "keep only the first n characters of
 *     this match; give the remaining characters back to the input so
 *     they will be scanned again".
 *  3. yymore() is useful for tokens built from several parts, e.g.
 *     a hyphenated word such as  well-known.
 *  4. yyless() is useful to look ahead and then un-read part of what
 *     was matched, e.g. to split a long word into pieces.
 *  5. Rule  [a-z]+"-"  matches letters followed by a hyphen, then
 *     calls yymore(); the following [a-z]+ rule prints the joined word.
 *  6. Rule  [A-Z][A-Za-z]+  matches a capitalised word; if it has more
 *     than 3 letters, yyless(3) keeps just the first three.
 *  7. The returned characters are re-scanned, so  Program  gives the
 *     piece "Pro" and then the rest "gram" is read as a new match.
 *  8. After yyless(), yytext and yyleng describe the shortened match.
 *  9. Note: yymore() cannot be combined with REJECT in the same file.
 *
 * Compile : flex p06d_yymore_yyless.l
 *           gcc lex.yy.c -o p06d
 * Run     : ./p06d     (try:  well-known Program Compiler)
 * ====================================================================
 */
#include <stdio.h>
%}

%%
[a-z]+"-"          { yymore(); }          /* remember "well-" and continue */

[a-z]+             { printf("lexeme  : \"%s\" (length %d)\n",
                            yytext, yyleng); }

[A-Z][A-Za-z]+     { printf("yyless : full word = \"%s\" (%d)", yytext, yyleng);
                     if (yyleng > 3)
                         yyless(3);       /* keep 3 chars, push rest back  */
                     printf("  -> kept \"%s\" (%d)\n", yytext, yyleng); }

[ \t\n]+           { /* ignore white space */ }
.                  { /* ignore other characters */ }
%%

int yywrap(void) { return 1; }

int main(void)
{
    printf("Enter words (Ctrl+D to end):\n");
    yylex();
    return 0;
}
