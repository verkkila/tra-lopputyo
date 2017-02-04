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

void arraylist_insert(struct arraylist *list, size_t index, char *key)
{
        struct node *current;

        assert(index < list->size);
        current = &(list->elements[index]);
        if (current->key != NULL) {
                while (current->next != NULL)
                        current = current->next;
                current->next = calloc(1, sizeof(struct node));
                current = current->next;
        }
        current->key = key;
        current->next = NULL;
}

void arraylist_free(struct arraylist *list)
{
        unsigned int i;
        struct node *root, *iter, *prev;

        for (i = 0; i < list->size; ++i) {
                root = &(list->elements[i]);
                assert(root != NULL);
                prev = root;
                do {
                        iter = root;
                        /*Move to the last member of the list, while keeping track of the previous one*/
                        while (iter->next != NULL) {
                                prev = iter;
                                iter = iter->next;
                        }
                        /*Free the node if it isn't part of the array*/
                        if (iter != root) {
                                free(iter);
                                prev->next = NULL;
                        }
                } while (root->next != NULL);
        }
        free(list->elements);
        free(list);
}
