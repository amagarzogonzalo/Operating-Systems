#include <stdio.h>

typedef struct {
    char data[4096];
    int key;
} item;

item array[] = {
    {"bill", 3},
    {"neil", 4},
    {"john", 2},
    {"rick", 5},
    {"alex", 1},
};

void sort(item *a, int n) {
    int i = 0, j = 1;
    item* p;
    
    while(i < n){
    p = a+i;
    j = 0;
     while (j < n-1){
        	item *aux= a+j;
            if( p->key < aux->key) {
                item t = *p;
 					*p  = *(aux);
 					*(aux) = t;
            }
            j++;
        }
        i++;
    }
}

int main() {
    int i;
    sort(array,5);
    for(i = 0; i < 5; i++)
        printf("array[%d] = {%s, %d}\n",
                i, array[i].data, array[i].key);
    return 0;
}
