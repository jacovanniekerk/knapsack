#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_WEIGHT 10

typedef struct item
{
    char description[25];
    int value;
    int cost;
} item_t;

/* A very flaky routine to read in the data.  Assumes that the data
   is in the correct format. */
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
        strcpy((*data)[i].description, token = strtok(line, ","));        
        (*data)[i].value = (int)strtol(token = strtok(NULL, ","), (char **)NULL, 10);                
        (*data)[i].cost = (int)strtol(token = strtok(NULL, ","), (char **)NULL, 10);
    }     
    fclose(file);
    return cnt;
}

void quickprint(item_t *items, int n, unsigned long combo)
{
    int i, curv=0, cost=0;
    for (i = 0; i < n; i++)
    {
        if ((combo >> i) & 1) {
            printf("%s  value=%d, cost=%d\n", 
                items[i].description, items[i].value, items[i].cost);
            curv += items[i].value;
            cost += items[i].cost;
        }
    }
    printf("\nTotal value=%d, Total cost=%d\n", curv, cost);
}

/* Assumes 2^n <= max(unsigned long) */
unsigned long bruteforce(item_t *items, int n, int max)
{
    unsigned long i, combo;
    int j, curv, cost, best;

    if ((sizeof(unsigned long)<<3) < max)
    {
        printf("Won't fit!");
        return 0;
    }


    for (i = 0; i <= ((unsigned long)2) << n; i++)
    {
        curv = cost = 0;
        for (j = 0; j < n; j++)
        {
            if ((i >> j) & 1) {
                curv += items[j].value;
                cost += items[j].cost;
                if (cost > max) {
                    break;
                }
            }
        }
        if (cost <= max && curv > best)
        {
            best = curv;
            combo = i;
        }
    }
    return combo;
}

unsigned long which(item_t *items, int **bag, int n, int max)
{
    unsigned long combo = 0;
    int i = n - 1, j = max;
    while (i > 0)
    {
        if (bag[i][j] > bag[i - 1][j]) {
            combo = (combo | 1) << 1;
            j = j - items[i].cost;
        } else {
            combo = (combo | 0) << 1;
        }
        i--;        
    }

    /* Finally, the first item. */
    if (bag[i][j] > 0) combo = (combo | 1);
    return combo;
}

unsigned long dp(item_t *items, int n, int max)
{
    /* Allocate memory for the array. */
    int i, j, take, notake; 
    int **s = (int **)malloc((n * sizeof(int*)));
    unsigned long combo;

    for (i = 0; i < n; i++)
        s[i] = (int *)malloc((max + 1) * sizeof(int));

    /* Fill in item 1 (we start taking it when the bag is big enough) */
    for (j = 0; j < items[0].cost; j++) s[0][j] = 0;
    for (j = items[0].cost; j <= max; j++) s[0][j] = items[0].value;

    /* Fill in the reset of the items */
    for (i = 1; i < n; i++)
    {
        /* Quick fill scenario where bag is too small for items i 
           (just copy top down) */
        for (j = 0; j < items[i].cost; j++) s[i][j] = s[i - 1][j];

        /* From here on, we need to make a choice, either take it or leave it. */
        for (j = items[i].cost; j <= max; j++)
        {
            /* Take the item and include all the items of an optimal bag which 
               is "lighter" from the items 0 - (i-1), i.e. the previous row */               
            take = items[i].value + s[i - 1][j - items[i].cost];

            /* If we don't take item i, we just have the same items then
               in previous row, so copy their value. */
            notake = s[i - 1][j];

            /* Now we select the best between take and notake to decide what 
               to do (giving preference to taking in case of a tie, but it 
               does not matter really). */
            s[i][j] = (take >= notake) ? take : notake;
        }
    }

    /* Extract combination of items */
    combo = which(items, s, n, max);

    printf("best using dp=%i\n", s[n-1][max]);

    /* Free used memory */ 
    for (i = 0; i < n; i++)
        free(s[i]);
    free(s);

    return combo;
}


int main(int argc, char *argv[])
{
    unsigned long combo;
    int cnt, i, tot;
    item_t *d = NULL;
        
    cnt = get_items("data-small", &d);
    
    /*printf("found %d\n size=%lu\n", cnt, sizeof(unsigned long));*/
    tot = 0;
    for (i = 0; i < cnt; i++)
    {
        printf("%s %d %d\n", d[i].description, d[i].value, d[i].cost);
        tot += d[i].value;
    }

    printf("Total value of all items: %d\n====================\n", tot);

    combo = bruteforce(d, cnt, MAX_WEIGHT);
    quickprint(d, cnt, combo);

    printf("====================\n");

    combo = dp(d, cnt, MAX_WEIGHT);
    quickprint(d, cnt, combo);

    free(d);
    return 0;
}

