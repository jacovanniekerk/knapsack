/******************************************************************************
The Knapsack Problem
Jaco van Niekerk
See youtube video: https://youtu.be/13FZv4n75UM

Have fun!

Note:

(1) The program was implemented in ANSI-C so that it would compile on Borland 
    Turbo C++ (version 3.0) and on a 80286 12Mhz. I don't know which version of 
    C it actially uses, but ANSI C seems to be compatible.
(2) The terms "Knapsack" and "Bag" are used interchangeably.
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h> /* for more accurate timing */

/* The maximum weight that the knapsack can hold. */
#define MAX_WEIGHT 50

typedef struct item
{
    char description[25];
    int value;
    int cost;
} item_t;

/* A very flaky routine to read in the data.  Assumes that the data is in the 
   correct format:  The first line contains the number of items to read (it 
   must be equal or less than the number of items provided).  Each item then 
   follows on its own line: <item description spaces allowed> <value> <cost>.

   This functions does no error checking.            
*/
int get_items(char *filename, item_t **data)
{    
    FILE* file = fopen(filename, "r");
    char line[256], *token = NULL;
    int i, cnt;

    /* Get numbner of items, allocate memory? */
    if (!fgets(line, sizeof(line), file)) return 0;
    cnt = (int)strtol(token = strtok(line, ","), (char **)NULL, 10);
    *data = (item_t *)malloc(cnt * sizeof(item_t));

    /* Read in the items. */
    for (i = 0; i < cnt; i++)
    {        
        if (!fgets(line, sizeof(line), file)) break;

        /* Copy the data, the string must be copied and the values converted to
           ints.  The cool strtok tokenises the string for us. */        
        strcpy((*data)[i].description, token = strtok(line, ","));        
        (*data)[i].value = (int)strtol(token = strtok(NULL, ","), (char **)NULL, 10);                
        (*data)[i].cost = (int)strtol(token = strtok(NULL, ","), (char **)NULL, 10);
    }     
    fclose(file);
    return cnt;
}

/* Given the combination of items, this prints the items slected by the 
   algorithm. */
void quickprint(item_t *items, int n, unsigned long combo)
{
    int i, curv=0, cost=0;
    for (i = 0; i < n; i++)
    {
        /* Shift right and then check if the rightmost bit is set.  If it is 
           then display the item detail. */
        if ((combo >> i) & 1) {
            printf("%s  value=%d, cost=%d\n", 
                items[i].description, items[i].value, items[i].cost);
            curv += items[i].value;
            cost += items[i].cost;
        }
    }
    printf("\nTotal value=%d, Total cost=%d\n", curv, cost);
}

/* This is the terribly slow brute-force technique.  It requires that: 
    2^n <= max(unsigned long).  For more values (and much more time), the code
    can be adopted to accept anyu number of items.    
*/
unsigned long bruteforce(item_t *items, int n, int max)
{
    unsigned long i, combo;
    int j, curv, cost, best;

    /* Check that the number of items will fit in an unsinged long. The 
       easiest way to extend this is to rather use a char-array and then count
       manually as if the array is a big number. */
    if ((sizeof(unsigned long)<<3) < max)
    {
        printf("Won't fit!");
        return 0;
    }

    /* Iterate through every possible combination to find the optimal 
       configuration. */
    for (i = 0; i <= ((unsigned long)2) << n; i++)
    {
        curv = cost = 0;
        for (j = 0; j < n; j++)
        {
            if ((i >> j) & 1) {
                curv += items[j].value;
                cost += items[j].cost;

                /* If the bag already exceeds the weight, terminiate 
                   immediately.  This is the only real optimisation (I can think 
                   of) that can be made to the brute-force technique.  I left it
                   out of the video for dramatic effect. */
                if (cost > max) {
                    break;
                }
            }
        }

        /* If the cost is less than max, we found a better configuration, 
           so store it.*/
        if (cost <= max && curv > best)
        {
            best = curv;
            combo = i;
        }
    }
    return combo;
}

/* Given the items and the 2D array of solved sub-problems, this determines
   the numeric representation of all the items finally selected. */
unsigned long which(item_t *items, int **bag, int n, int max)
{
    unsigned long combo = 0;

    /* Start at the rightmost, bottom corner (i.e. the final solution) */
    int i = n - 1, j = max;

    /* While we haven't reach the first item (which is handled slightly
       differently) */
    while (i > 0)
    {
        /* If the value at sub-problem i,j is the graeter than the value of
           sub-problem (i-1), j then it means we did choose to take item i 
           (assuming non-zero values).  We therefore set the rightmost bit 
           and shift everything right with 1 bit and update j to move left
           with the weight of the selected item.  

           If the values are the same then we did not select the item, so the
           weight (j) remains the same and we just shift combo left with 1 
           bit. */
        if (bag[i][j] > bag[i - 1][j]) {
            combo = (combo | 1) << 1;
            j = j - items[i].cost;
        } else {
            combo = combo << 1;
        }
        i--;    
    }

    /* Finally, the first item. Its value is compare to 0 to determine if we 
       took it or not. */
    if (bag[i][j] > 0) combo = (combo | 1);
    return combo;
}

/* The dynamic programming apprach of the 0/1 Knapsack problem.  This is the 
   bottom-up approach we requires a 2D array of size (item count) by (weight).
   Each cell of the array now needs to be solved and then we solve larger 
   versions by building on the the smaller versions.

   The rows (going down) represent the items that we are allowed to pick from,
   that is for row i, we can pick all items from row 0 to i.  So on the first 
   row we can wither pick/leave item 1.  The columns represent the allowed 
   weight limit of the knapsack, so s[0][0] asks: "If you can carry zero kg, 
   do you take item 1 or not?"  Only if item 1 is weightless can it be added at
   this stage.  So all the elemens s[0][0] to s[0][w-1] (where w is item 1's 
   weight) is set to 0.

   For the next row (and every subsequent row), we now have a slightly more 
   complicated process, but the question remains the same.  "Do we take item j
   or not?".  If we don't take item j, then the value of s[i][j] is simply the 
   same as the value s[i-1][j].  If we do take item i, then the value we need
   to add to s[i][j] has to be s[i-1][j-w] (where w is item i's weight).  This
   is the trickiest part.  Let's rewrite the two values:

   notake: s[i][j] = s[i-1][j]
   take  : s[i][j] = s[i-1][j-w] + v (where w and v item i's weight and value.)

   Then we simply compare take with notake and set s[i][j] to the larger value.

   The two functions are:

   solve_sub_problems: Solves the subproblems (the heart of the algorithm).
   knapsack_dp: Mostly house-keeping and calling solve_sub_problems (begin
                looking here).
*/
void solve_sub_problems(int **s, item_t *items, int n, int max)
{
    int i, j, take, notake;

    /* Fill in item 1 (we only start taking it when the bag is big enough). */
    for (j = 0; j < items[0].cost; j++) s[0][j] = 0; /* no-take*/
    for (j = items[0].cost; j <= max; j++) s[0][j] = items[0].value; /* take */

    /* Fill in the rest of the items, starting from the 2nd item. */
    for (i = 1; i < n; i++)
    {
        /* Quick fill scenario where bag is too small for item i 
           (just copy the value down as we don't take it). */
        for (j = 0; j < items[i].cost; j++) s[i][j] = s[i - 1][j];

        /* From here on, we need to make a choice, either take it or don't. */
        for (j = items[i].cost; j <= max; j++)
        {
            /* Take the item and include all the items of an optimal bag which 
               is "lighter" from the items 0 - (i-1), i.e. the previous row */               
            take = items[i].value + s[i - 1][j - items[i].cost];

            /* If we don't take item i, we just have the same items that was  
               in the previous row of the same weight, so copy the value. */
            notake = s[i - 1][j];

            /* Now we select the best between take and notake to decide what 
               to do (giving preference to taking in case of a tie, but it 
               does not matter really). */
            s[i][j] = (take >= notake) ? take : notake;
        }
    }
}

unsigned long knapsack_dp(item_t *items, int n, int max)
{
    /* Allocate memory for the table. A dynamic array of dynamic arrays. (This 
       here is why we hava C++, Java and Python.) */
    unsigned long combo;
    int i, **s = (int **)malloc((n * sizeof(int*)));
    for (i = 0; i < n; i++) {
        s[i] = (int *)malloc((max + 1) * sizeof(int));
    }

    /* Solve sub-problems and get the numeric representation. */
    solve_sub_problems(s, items, n, max);
    combo = which(items, s, n, max);
  
    /* Free used memory */ 
    for (i = 0; i < n; i++) {
        free(s[i]);
    }
    free(s);

    return combo;
}

int main(int argc, char *argv[])
{
    unsigned long combo;
    int cnt, i, tot, weight;
    item_t *d = NULL;

    /* Variable used for timing. */
    clock_t start, end;
    double cpu_time_used;

    /* Get the items from the file. */        
    cnt = get_items("data", &d);

    /* Display all the items we got. */        
    tot = 0, weight = 0;
    for (i = 0; i < cnt; i++)
    {
        printf("%s %d %d\n", d[i].description, d[i].value, d[i].cost);
        tot += d[i].value;
        weight += d[i].cost;
    }
    printf("\nTotal items          : %d\n", cnt);    
    printf("Total value          : %d\n", tot);    
    printf("Total weight         : %d\n", weight);    
    printf("Total allowed weight : %d\n", MAX_WEIGHT);
    printf("===============================\n");

    /* This is the brute force routine. */
    start = clock();

    combo = bruteforce(d, cnt, MAX_WEIGHT);
    quickprint(d, cnt, combo);

    end = clock();    
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;    
    printf("Time used: %f\ns", cpu_time_used);

    printf("===============================\n");

    /* This is the bottum-up dynamic programming routine. */
    start = clock();

    start = clock();
    combo = knapsack_dp(d, cnt, MAX_WEIGHT);
    quickprint(d, cnt, combo);

    end = clock();    
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;    
    printf("Time used: %f\ns", cpu_time_used);
    printf("===============================\n");

    free(d);
    return 0;
}

