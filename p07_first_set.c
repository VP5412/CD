/*
 * ====================================================================
 * Practical No : 7
 * Title        : WAP to find the "FIRST" set.
 *                Input : a string of grammar symbols
 *                Output: the FIRST set of that string
 * ====================================================================
 * THEORY & LOGIC
 *  1. FIRST(alpha) is the set of terminals that can appear as the very
 *     first symbol of some string derived from alpha; if alpha can
 *     derive the empty string, epsilon (#) is also in FIRST(alpha).
 *  2. Rule 1: if X is a terminal, FIRST(X) = { X }.
 *  3. Rule 2: if X -> epsilon is a production, add epsilon to FIRST(X).
 *  4. Rule 3: if X -> Y1 Y2 ... Yk, add FIRST(Y1) (without epsilon) to
 *     FIRST(X); if Y1 can vanish also add FIRST(Y2), and so on.
 *  5. If ALL of Y1..Yk can vanish, epsilon is added to FIRST(X).
 *  6. Because one set may depend on another, the rules are applied in
 *     a loop until no set changes any more (fixed-point iteration).
 *  7. The FIRST set of a whole string X1 X2 ... Xn is found with the
 *     same idea: take FIRST(X1), continue with X2 only while epsilon
 *     is in FIRST(Xi).
 *  8. Assumed grammar (typical expression grammar, R=E', Y=T', i=id):
 *        E->TR   R->+TR|#   T->FY   Y->*FY|#   F->(E)|i
 *  9. Capital letters are non-terminals; '#' stands for epsilon.
 *
 * Compile : gcc p07_first_set.c -o p07
 * Run     : ./p07      (try:  E   TR   +TR   Y   (E)   RY)
 * ====================================================================
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXP 20
#define MAXS 32

char prod[MAXP][MAXS];          /* productions stored as "A=alpha"        */
int  np = 0;                    /* number of productions                  */
char firstSet[128][MAXS];       /* firstSet['E'] = FIRST(E) as a string   */

int isNT(char c) { return isupper((unsigned char)c); }

/* add symbol c to a set; returns 1 if the set really changed */
int addSym(char *set, char c)
{
    int n;
    if (strchr(set, c)) return 0;
    n = (int)strlen(set);
    set[n] = c;
    set[n + 1] = '\0';
    return 1;
}

/* FIRST of a string of symbols s -> result in out */
void firstOfString(const char *s, char *out)
{
    int i, allNullable = 1;
    out[0] = '\0';
    for (i = 0; s[i] != '\0' && allNullable; i++) {
        char X = s[i];
        if (X == '#') continue;                 /* epsilon: skip it        */
        if (!isNT(X)) {                         /* terminal                */
            addSym(out, X);
            allNullable = 0;
        } else {                                /* non-terminal            */
            int j, hasEps = 0;
            for (j = 0; firstSet[(int)X][j] != '\0'; j++) {
                char c = firstSet[(int)X][j];
                if (c == '#') hasEps = 1;
                else          addSym(out, c);
            }
            if (!hasEps) allNullable = 0;       /* cannot look further     */
        }
    }
    if (allNullable) addSym(out, '#');          /* whole string can vanish */
}

/* repeat the FIRST rules until nothing changes */
void computeFirst(void)
{
    int p, k, changed;
    char tmp[MAXS];
    do {
        changed = 0;
        for (p = 0; p < np; p++) {
            char A = prod[p][0];
            firstOfString(prod[p] + 2, tmp);
            for (k = 0; tmp[k] != '\0'; k++)
                changed |= addSym(firstSet[(int)A], tmp[k]);
        }
    } while (changed);
}

void initGrammar(void)
{
    strcpy(prod[np++], "E=TR");
    strcpy(prod[np++], "R=+TR");
    strcpy(prod[np++], "R=#");
    strcpy(prod[np++], "T=FY");
    strcpy(prod[np++], "Y=*FY");
    strcpy(prod[np++], "Y=#");
    strcpy(prod[np++], "F=(E)");
    strcpy(prod[np++], "F=i");
}

void printSet(const char *set)
{
    int i;
    printf("{ ");
    for (i = 0; set[i] != '\0'; i++)
        printf("%s%s", set[i] == '#' ? "eps" : (char[]){set[i], '\0'},
               set[i + 1] ? ", " : "");
    printf(" }");
}

int main(void)
{
    const char *nts = "ERTYF";
    char str[64], res[MAXS];
    int i;

    initGrammar();
    computeFirst();

    printf("Grammar (R = E', Y = T', i = id, # = epsilon):\n");
    for (i = 0; i < np; i++)
        printf("  %c -> %s\n", prod[i][0], prod[i] + 2);

    printf("\nFIRST sets of all non-terminals:\n");
    for (i = 0; nts[i] != '\0'; i++) {
        printf("  FIRST(%c) = ", nts[i]);
        printSet(firstSet[(int)nts[i]]);
        printf("\n");
    }

    printf("\nEnter a string of grammar symbols (e.g. TR, +TR, (E), RY): ");
    if (scanf("%63s", str) != 1) return 1;

    firstOfString(str, res);
    printf("FIRST(%s) = ", str);
    printSet(res);
    printf("\n");
    return 0;
}
