#ifndef ARRAYLIST_INCLUDED
#define ARRAYLIST_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

struct node {
        char *key;
        struct node *next;
};

struct arraylist {
        size_t size;
        struct node *elements;
};

struct arraylist *arraylist_new(size_t array_size);
void arraylist_insert(struct arraylist *list, char *key, size_t index);
void arraylist_delete(struct arraylist *list);

#endif
