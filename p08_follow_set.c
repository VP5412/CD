/*
 * ====================================================================
 * Practical No : 8
 * Title        : WAP to find the "FOLLOW" set.
 *                Input : a string of grammar symbols
 *                Output: the FOLLOW set of the non-terminals in it
 * ====================================================================
 * THEORY & LOGIC
 *  1. FOLLOW(A), for a non-terminal A, is the set of terminals that can
 *     appear immediately to the right of A in some sentential form.
 *  2. FOLLOW is defined only for non-terminals and never contains
 *     epsilon; the end-marker $ may be in it.
 *  3. Rule 1: put $ in FOLLOW(S) where S is the start symbol.
 *  4. Rule 2: for a production A -> alpha B beta, add FIRST(beta)
 *     (without epsilon) to FOLLOW(B).
 *  5. Rule 3: for A -> alpha B, or A -> alpha B beta where beta can
 *     derive epsilon, add everything in FOLLOW(A) to FOLLOW(B).
 *  6. FOLLOW needs FIRST sets, so the program first computes FIRST
 *     with the same fixed-point method as Practical 7.
 *  7. The FOLLOW rules are then applied repeatedly to every production
 *     until a full pass changes nothing (fixed point).
 *  8. Assumed grammar (R=E', Y=T', i=id, # = epsilon):
 *        E->TR   R->+TR|#   T->FY   Y->*FY|#   F->(E)|i
 *  9. FOLLOW sets are used to fill the LL(1) table (Practical 10) and
 *     to decide reductions in SLR parsers.
 *
 * Compile : gcc p08_follow_set.c -o p08
 * Run     : ./p08      (try:  E   TR   F   RY)
 * ====================================================================
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXP 20
#define MAXS 32

char prod[MAXP][MAXS];          /* productions stored as "A=alpha"  */
int  np = 0;
char firstSet[128][MAXS];
char followSet[128][MAXS];

int isNT(char c) { return isupper((unsigned char)c); }

int addSym(char *set, char c)
{
    int n;
    if (strchr(set, c)) return 0;
    n = (int)strlen(set);
    set[n] = c;
    set[n + 1] = '\0';
    return 1;
}

void firstOfString(const char *s, char *out)
{
    int i, allNullable = 1;
    out[0] = '\0';
    for (i = 0; s[i] != '\0' && allNullable; i++) {
        char X = s[i];
        if (X == '#') continue;
        if (!isNT(X)) {
            addSym(out, X);
            allNullable = 0;
        } else {
            int j, hasEps = 0;
            for (j = 0; firstSet[(int)X][j] != '\0'; j++) {
                char c = firstSet[(int)X][j];
                if (c == '#') hasEps = 1;
                else          addSym(out, c);
            }
            if (!hasEps) allNullable = 0;
        }
    }
    if (allNullable) addSym(out, '#');
}

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

void computeFollow(void)
{
    int p, i, k, changed;
    char tmp[MAXS];

    addSym(followSet[(int)prod[0][0]], '$');        /* Rule 1: start symbol */

    do {
        changed = 0;
        for (p = 0; p < np; p++) {
            char A = prod[p][0];
            const char *rhs = prod[p] + 2;
            for (i = 0; rhs[i] != '\0'; i++) {
                char B = rhs[i];
                if (!isNT(B)) continue;             /* only non-terminals   */

                /* Rule 2: FIRST of what comes after B (without epsilon)   */
                firstOfString(rhs + i + 1, tmp);
                for (k = 0; tmp[k] != '\0'; k++)
                    if (tmp[k] != '#')
                        changed |= addSym(followSet[(int)B], tmp[k]);

                /* Rule 3: what follows B can vanish -> add FOLLOW(A)      */
                if (strchr(tmp, '#'))
                    for (k = 0; followSet[(int)A][k] != '\0'; k++)
                        changed |= addSym(followSet[(int)B],
                                          followSet[(int)A][k]);
            }
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
        printf("%c%s", set[i], set[i + 1] ? ", " : "");
    printf(" }");
}

int main(void)
{
    const char *nts = "ERTYF";
    char str[64];
    int i, shown = 0;

    initGrammar();
    computeFirst();
    computeFollow();

    printf("Grammar (R = E', Y = T', i = id, # = epsilon):\n");
    for (i = 0; i < np; i++)
        printf("  %c -> %s\n", prod[i][0], prod[i] + 2);

    printf("\nFOLLOW sets of all non-terminals:\n");
    for (i = 0; nts[i] != '\0'; i++) {
        printf("  FOLLOW(%c) = ", nts[i]);
        printSet(followSet[(int)nts[i]]);
        printf("\n");
    }

    printf("\nEnter a string of grammar symbols (e.g. E, TR, F): ");
    if (scanf("%63s", str) != 1) return 1;

    for (i = 0; str[i] != '\0'; i++) {
        if (isNT(str[i]) && strchr(nts, str[i])) {
            printf("FOLLOW(%c) = ", str[i]);
            printSet(followSet[(int)str[i]]);
            printf("\n");
            shown++;
        } else
            printf("'%c' is not a non-terminal of the grammar "
                   "(FOLLOW exists only for non-terminals)\n", str[i]);
    }
    if (shown == 0)
        printf("No non-terminal found in the given string.\n");
    return 0;
}
