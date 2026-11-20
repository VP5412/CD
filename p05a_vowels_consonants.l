%{
/*
 * ====================================================================
 * Practical No : 5(a)
 * Title        : Write a Lex program to count the number of vowels and
 *                consonants in a given input string.
 * ====================================================================
 * THEORY & LOGIC
 *  1. The English alphabet splits into two disjoint classes: vowels
 *     (a e i o u, upper or lower case) and consonants (all other
 *     letters). Each class is one regular expression.
 *  2. Rule  [aeiouAEIOU]  increments the vowel counter.
 *  3. Rule  [a-zA-Z]  comes AFTER the vowel rule; LEX uses the first
 *     rule when two rules match the same length, so vowels are taken
 *     by rule 1 and only the remaining letters reach rule 2.
 *  4. Digits, spaces and punctuation are ignored by the last rule
 *     (.|\n) with an empty action.
 *  5. The string is read with fgets() and given to the scanner using
 *     yy_scan_string(), which makes LEX read from memory instead of a
 *     file (a FLEX feature).
 *  6. yylex() then runs over the string once and returns at its end.
 *  7. Time complexity is O(n) because every character is matched once.
 *  8. main() finally prints the two counters.
 *  9. Example: "Compiler Design" -> vowels = 5, consonants = 9.
 *
 * Compile : flex p05a_vowels_consonants.l
 *           gcc lex.yy.c -o p05a
 * Run     : ./p05a
 * ====================================================================
 */
#include <stdio.h>
int vowels = 0, consonants = 0;
%}

%%
[aeiouAEIOU]    { vowels++; }
[a-zA-Z]        { consonants++; }
.|\n            { /* ignore digits, spaces, symbols */ }
%%

int yywrap(void) { return 1; }

int main(void)
{
    char str[256];

    printf("Enter a string: ");
    if (fgets(str, sizeof(str), stdin) == NULL)
        return 1;

    yy_scan_string(str);        /* make the scanner read from this string */
    yylex();

    printf("Vowels     = %d\n", vowels);
    printf("Consonants = %d\n", consonants);
    return 0;
}
