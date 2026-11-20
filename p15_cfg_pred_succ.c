/*
 * ====================================================================
 * Practical No : 15
 * Title        : Extract predecessor and successor from a given
 *                control flow graph (CFG).
 * ====================================================================
 * THEORY & LOGIC
 *  1. A control flow graph represents a program: every node is a basic
 *     block (a straight-line sequence of statements with one entry
 *     and one exit) and every directed edge B1 -> B2 means control may
 *     pass from B1 to B2 (a jump, branch or fall-through).
 *  2. Successor of a block B: any block S such that edge B -> S exists
 *     (the blocks that can execute immediately AFTER B).
 *  3. Predecessor of a block B: any block P such that edge P -> B
 *     exists (the blocks that can execute immediately BEFORE B).
 *  4. The CFG is stored as an adjacency matrix: adj[i][j] = 1 when
 *     there is an edge from block i to block j.
 *  5. Successors of i are found by scanning ROW i of the matrix, and
 *     predecessors of i by scanning COLUMN i.
 *  6. A block without predecessors is the ENTRY block; a block without
 *     successors is an EXIT block.
 *  7. A block with two successors ends in a conditional branch; a
 *     block with two predecessors is a join point (or a loop header).
 *  8. An edge to an earlier-numbered block (e.g. 3 -> 2) usually shows
 *     a loop (back edge); the program points these out.
 *  9. Predecessor/successor sets are the base for data-flow analysis
 *     such as reaching definitions and live-variable analysis.
 *
 * Compile : gcc p15_cfg_pred_succ.c -o p15
 * Run     : ./p15 < sample_inputs/cfg_input.txt      (or type the data)
 *   Input : number of blocks n, number of edges e, then e lines "from to"
 *           (blocks are numbered 1 .. n)
 * ====================================================================
 */
#include <stdio.h>

#define MAXN 20

int adj[MAXN + 1][MAXN + 1];        /* adjacency matrix, 1-based */

int main(void)
{
    int n, e, i, j, from, to;
    int backEdges = 0;

    printf("Enter number of blocks (nodes): ");
    if (scanf("%d", &n) != 1 || n < 1 || n > MAXN) {
        printf("Invalid number of blocks (1 to %d allowed).\n", MAXN);
        return 1;
    }
    printf("Enter number of edges: ");
    if (scanf("%d", &e) != 1 || e < 0) {
        printf("Invalid number of edges.\n");
        return 1;
    }

    printf("Enter each edge as: from to   (blocks 1 to %d)\n", n);
    for (i = 0; i < e; i++) {
        if (scanf("%d %d", &from, &to) != 2 ||
            from < 1 || from > n || to < 1 || to > n) {
            printf("Invalid edge number %d.\n", i + 1);
            return 1;
        }
        adj[from][to] = 1;
    }

    printf("\n%-8s %-22s %-22s %s\n", "Block", "Predecessors", "Successors", "Remark");
    printf("-----------------------------------------------------------------\n");

    for (i = 1; i <= n; i++) {
        int np = 0, ns = 0;
        char pred[100] = "", succ[100] = "";
        int pl = 0, sl = 0;

        for (j = 1; j <= n; j++) {
            if (adj[j][i]) {                         /* column i = preds */
                pl += snprintf(pred + pl, sizeof(pred) - pl, "%sB%d",
                               np ? ", " : "", j);
                np++;
            }
            if (adj[i][j]) {                         /* row i = succs    */
                sl += snprintf(succ + sl, sizeof(succ) - sl, "%sB%d",
                               ns ? ", " : "", j);
                ns++;
                if (j <= i) backEdges++;             /* edge to earlier/same block */
            }
        }
        printf("B%-7d %-22s %-22s %s\n", i,
               np ? pred : "{ none }",
               ns ? succ : "{ none }",
               np == 0 ? "<- ENTRY block" :
               ns == 0 ? "<- EXIT block"  :
               ns >= 2 ? "<- conditional branch" : "");
    }

    if (backEdges > 0)
        printf("\n%d back edge(s) found (edge to an earlier block): the graph has a loop.\n",
               backEdges);
    else
        printf("\nNo back edges: the graph has no loops.\n");
    return 0;
}
