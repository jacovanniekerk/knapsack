#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct item
{
    char description[25];
    int value;
    int weight;
} item_t;

/* A very flaky routine to read in the data.  Assumes that the data
   is in the correct format. */
int get_items(char *filename, item_t **data)
{    
    FILE* file = fopen(filename, "r");
    char line[256], *token = NULL;    
    int i, cnt; 

    /* Get numbner of items, allocate memory? */
    fgets(line, sizeof(line), file);    
    cnt = (int)strtol(token = strtok(line, ","), (char **)NULL, 10);        
    *data = (item_t *)malloc(sizeof(item_t) * cnt);

    /* Read in the items. */
    for (i = 0; i < cnt; i++)
    {

    } 

    /*while (fgets(line, sizeof(line), file)) {       
        token = strtok(line, ",");
        printf("Item : [%s]\n", token);

        token = strtok(NULL, ",");
        if (token == NULL) continue;


        tmp =  (int)strtol(token, (char **)NULL, 10);
        printf("Value: [%d]\n", tmp);
        
        token = strtok(NULL, ",");
        tmp =  (int)strtol(token, (char **)NULL, 10);
        printf("Cost : [%d]\n", tmp);

    }*/

    /* may check feof here to make a difference between eof and io failure -- network
       timeout for instance */

    fclose(file);
    return cnt;
}


int main(int argc, char *argv[])
{
    int cnt;
    item_t *d = NULL;
    
    cnt = get_items("data-small", &d);
    free(d);

    printf("found %d\n", cnt);

    return 0;
}
