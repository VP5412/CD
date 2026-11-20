/*
 * ====================================================================
 * Practical No : 1(b)
 * Title        : Write a C program to recognize identifiers and numbers.
 * ====================================================================
 * THEORY & LOGIC
 *  1. Recognising tokens is the job of the lexical analyser; each token
 *     class is described by a regular expression.
 *  2. Identifier : (letter | _) (letter | digit | _)*
 *  3. Integer    : digit+
 *  4. Real number: digit+ . digit+   (optionally followed by an
 *     exponent such as e+10 or E-3).
 *  5. Each regular expression is converted into a DFA; here the DFA is
 *     coded by hand as a set of states and a character loop.
 *  6. For identifiers the first character is tested separately (it may
 *     not be a digit); after that letters, digits and '_' are accepted.
 *  7. For numbers the DFA first reads digits, then optionally a '.'
 *     followed by at least one digit, then an optional exponent.
 *  8. The string is accepted only if the DFA stops exactly at the end
 *     of the input; otherwise it is reported as INVALID.
 *  9. An accepted identifier is additionally compared with the C
 *     keyword table, because keywords have the same shape as
 *     identifiers but a different meaning.
 *
 * Compile : gcc p01b_identifiers_numbers.c -o p01b
 * Run     : ./p01b      (type words, press Ctrl+D / Ctrl+Z to stop)
 * ====================================================================
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static const char *keywords[] = {
    "auto","break","case","char","const","continue","default","do",
    "double","else","enum","extern","float","for","goto","if","int",
    "long","register","return","short","signed","sizeof","static",
    "struct","switch","typedef","union","unsigned","void","volatile",
    "while"
};

/* returns 1 if s is a C keyword */
int isKeyword(const char *s)
{
    int i, n = sizeof(keywords) / sizeof(keywords[0]);
    for (i = 0; i < n; i++)
        if (strcmp(s, keywords[i]) == 0)
            return 1;
    return 0;
}

/* returns 1 if s matches (letter|_)(letter|digit|_)* */
int isIdentifier(const char *s)
{
    int i;
    if (!(isalpha((unsigned char)s[0]) || s[0] == '_'))
        return 0;
    for (i = 1; s[i] != '\0'; i++)
        if (!(isalnum((unsigned char)s[i]) || s[i] == '_'))
            return 0;
    return 1;
}

/* returns 0 = not a number, 1 = integer, 2 = real number */
int isNumber(const char *s)
{
    int i = 0, digits = 0, type = 1;

    while (isdigit((unsigned char)s[i])) { i++; digits++; }
    if (digits == 0) return 0;               /* must start with a digit */

    if (s[i] == '.') {                       /* fractional part         */
        int frac = 0;
        i++;
        while (isdigit((unsigned char)s[i])) { i++; frac++; }
        if (frac == 0) return 0;             /* "12." is not accepted   */
        type = 2;
    }
    if (s[i] == 'e' || s[i] == 'E') {        /* exponent part           */
        int ex = 0;
        i++;
        if (s[i] == '+' || s[i] == '-') i++;
        while (isdigit((unsigned char)s[i])) { i++; ex++; }
        if (ex == 0) return 0;
        type = 2;
    }
    return (s[i] == '\0') ? type : 0;        /* whole string consumed?  */
}

int main(void)
{
    char str[100];

    printf("Enter strings to check (Ctrl+D to stop):\n");
    while (scanf("%99s", str) == 1) {
        int num = isNumber(str);
        if (isIdentifier(str)) {
            if (isKeyword(str)) printf("%-20s -> KEYWORD\n", str);
            else                printf("%-20s -> IDENTIFIER\n", str);
        } else if (num == 1)
            printf("%-20s -> INTEGER NUMBER\n", str);
        else if (num == 2)
            printf("%-20s -> REAL NUMBER\n", str);
        else
            printf("%-20s -> INVALID\n", str);
    }
    return 0;
}
