#include "arraylist.h"

struct arraylist *arraylist_new(size_t array_size)
{
        struct arraylist *list;

        list = NULL;
        list = malloc(sizeof(struct arraylist));
        if (list == NULL) {
                printf("Failed to allocate memory for arraylist.\n");
                return NULL;
        }
        list->size = array_size;
        list->elements = NULL;
        list->elements = calloc(array_size, sizeof(struct node));
        if (list->elements == NULL) {
                printf("Failed to allocate memory for arraylist elements (size %lu).\n", array_size);
                return NULL;
        }
        return list;
}

void arraylist_insert(struct arraylist *list, char *key, size_t index)
{
        struct node *current;

        assert(index < list->size);
        current = &(list->elements[index]);
        if (current->key == NULL)
                return current;
        while (current->next != NULL)
                current = current->next;
        current->next = calloc(1, sizeof(struct node));
        current = current->next;
        current->key = key;
        current->next = NULL;
}

void arraylist_delete(struct arraylist *list)
{
        
}

