/*
 * ====================================================================
 * Practical No : 2
 * Title        : Write a C program to generate tokens for a C program.
 * ====================================================================
 * THEORY & LOGIC
 *  1. A token is the smallest meaningful unit of a program: keyword,
 *     identifier, constant, string literal, operator or punctuation.
 *  2. The lexical analyser (scanner) reads the source as a stream of
 *     characters and groups them into tokens (this grouping is called
 *     "maximal munch": always take the longest possible match).
 *  3. White space, comments and preprocessor lines (#include ...) carry
 *     no meaning for the parser, so the scanner skips them.
 *  4. A word starting with a letter or '_' is read completely and then
 *     looked up in a keyword table: found -> KEYWORD, else IDENTIFIER.
 *  5. A word starting with a digit is read as a NUMBER (integer or
 *     real, with an optional fraction part).
 *  6. A double quote starts a STRING literal, a single quote starts a
 *     CHARACTER literal; both are read up to the closing quote while
 *     honouring the backslash escape.
 *  7. Two-character operators (==, !=, <=, >=, ++, --, &&, ||, +=, ...)
 *     are tried first, then single-character operators.
 *  8. Brackets, braces, ';' and ',' are reported as SEPARATORS.
 *  9. Output is one line per token  <TOKEN-TYPE , lexeme>  followed by
 *     a count of tokens in every category.
 *
 * Compile : gcc p02_token_generator.c -o p02
 * Run     : ./p02 sample_inputs/sample.c
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

static const char *twoCharOps[] = {
    "==","!=","<=",">=","++","--","&&","||","+=","-=","*=","/=",
    "%=","<<",">>","->"
};

int isKeyword(const char *s)
{
    int i, n = sizeof(keywords) / sizeof(keywords[0]);
    for (i = 0; i < n; i++)
        if (strcmp(s, keywords[i]) == 0) return 1;
    return 0;
}

int isTwoCharOp(char a, char b)
{
    int i, n = sizeof(twoCharOps) / sizeof(twoCharOps[0]);
    for (i = 0; i < n; i++)
        if (twoCharOps[i][0] == a && twoCharOps[i][1] == b) return 1;
    return 0;
}

int main(int argc, char *argv[])
{
    FILE *fp;
    int c, next;
    char buf[256];
    int i;
    int nKey = 0, nId = 0, nNum = 0, nStr = 0, nOp = 0, nSep = 0;

    if (argc < 2) {
        printf("Usage: %s <source.c>\n", argv[0]);
        return 1;
    }
    fp = fopen(argv[1], "r");
    if (fp == NULL) { perror("Cannot open file"); return 1; }

    printf("%-14s %s\n", "TOKEN TYPE", "LEXEME");
    printf("-----------------------------\n");

    while ((c = fgetc(fp)) != EOF) {

        if (isspace(c)) continue;                         /* white space  */

        if (c == '#') {                                   /* preprocessor */
            while ((c = fgetc(fp)) != EOF && c != '\n') ;
            continue;
        }

        if (c == '/') {                                   /* comments?    */
            next = fgetc(fp);
            if (next == '/') {                            /* // comment   */
                while ((c = fgetc(fp)) != EOF && c != '\n') ;
                continue;
            } else if (next == '*') {                     /* block comment*/
                int prev = 0;
                while ((c = fgetc(fp)) != EOF) {
                    if (prev == '*' && c == '/') break;
                    prev = c;
                }
                continue;
            }
            ungetc(next, fp);                             /* it is an op  */
        }

        if (isalpha(c) || c == '_') {                     /* word         */
            i = 0;
            buf[i++] = c;
            while ((c = fgetc(fp)) != EOF && (isalnum(c) || c == '_') && i < 255)
                buf[i++] = c;
            buf[i] = '\0';
            ungetc(c, fp);
            if (isKeyword(buf)) { printf("%-14s %s\n", "KEYWORD", buf);    nKey++; }
            else                { printf("%-14s %s\n", "IDENTIFIER", buf); nId++;  }
        }
        else if (isdigit(c)) {                            /* number       */
            i = 0;
            buf[i++] = c;
            while ((c = fgetc(fp)) != EOF && (isdigit(c) || c == '.') && i < 255)
                buf[i++] = c;
            buf[i] = '\0';
            ungetc(c, fp);
            printf("%-14s %s\n", "NUMBER", buf);
            nNum++;
        }
        else if (c == '"' || c == '\'') {                 /* string/char  */
            int quote = c;
            i = 0;
            buf[i++] = c;
            while ((c = fgetc(fp)) != EOF && i < 254) {
                buf[i++] = c;
                if (c == '\\') {                          /* escape       */
                    int e = fgetc(fp);
                    if (e != EOF) buf[i++] = e;
                } else if (c == quote)
                    break;
            }
            buf[i] = '\0';
            printf("%-14s %s\n", quote == '"' ? "STRING" : "CHARACTER", buf);
            nStr++;
        }
        else if (strchr("(){}[];,", c)) {                 /* separators   */
            printf("%-14s %c\n", "SEPARATOR", c);
            nSep++;
        }
        else {                                            /* operators    */
            next = fgetc(fp);
            if (next != EOF && isTwoCharOp((char)c, (char)next))
                printf("%-14s %c%c\n", "OPERATOR", c, next);
            else {
                if (next != EOF) ungetc(next, fp);
                printf("%-14s %c\n", "OPERATOR", c);
            }
            nOp++;
        }
    }

    printf("-----------------------------\n");
    printf("Keywords   : %d\nIdentifiers: %d\nNumbers    : %d\n"
           "Strings/chr: %d\nOperators  : %d\nSeparators : %d\n",
           nKey, nId, nNum, nStr, nOp, nSep);
    printf("Total tokens: %d\n", nKey + nId + nNum + nStr + nOp + nSep);

    fclose(fp);
    return 0;
}
