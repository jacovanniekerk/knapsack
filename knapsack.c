#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

int main(int argc, char *argv[])
{
    unsigned long combo;
    int cnt;
    item_t *d = NULL;
        
    cnt = get_items("data-small", &d);
    
    /*printf("found %d\n size=%lu\n", cnt, sizeof(unsigned long));
    for (i = 0; i < cnt; i++)
    {
        printf("%s %d %d\n", d[i].description, d[i].value, d[i].cost);
    }*/

    combo = bruteforce(d, cnt, 5);
    quickprint(d, cnt, combo);

    free(d);
    return 0;
}
