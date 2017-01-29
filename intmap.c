#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "intmap.h"

static void free_elements(intmap_element *elements, size_t n);
static void rehash(intmap *map);
/*
 * sdbm - http://www.cse.yorku.ca/~oz/hash.html
 */
static unsigned int hash(const char *key)
{
        unsigned int hash;
        int c;

        hash = 0;
        while ((c = *key++) != 0) {
                hash = c + (hash << 6) + (hash << 16) - hash;
        }
        return hash;
}

intmap *intmap_new(size_t initial_size)
{
        intmap *map;
        size_t min_element_count;

        map = NULL;
        map = malloc(sizeof(struct intmap));
        if (map == NULL)
                return NULL;
        min_element_count = initial_size * 1.25f;
        map->max_elements = 1;
        while (map->max_elements <= min_element_count) {
                map->max_elements <<= 1;
        }
        map->num_elements = 0;
        map->elements = NULL;
        map->elements = calloc(map->max_elements, sizeof(intmap_element));
        if (map->elements == NULL) {
                free(map);
                return NULL;
        }
        return map;
}

static void rehash(intmap *map)
{
        size_t new_max_elements;
        intmap_element *new_elements, *old_elements;
        unsigned int i;

        new_max_elements = map->max_elements << 1;
        new_elements = calloc(new_max_elements, sizeof(intmap_element));
        if (new_elements == NULL) {
                return;
        }
        old_elements = map->elements;
        map->elements = new_elements;

        for (i = 0; i < map->max_elements; ++i) {
                struct intmap_element *dest, *src;

                src = &(old_elements[i]);
                while (src != NULL && src->key != NULL) {
                        unsigned int index;
                        
                        index = hash(src->key) % new_max_elements;
                        dest = &(map->elements[index]);
                        if (dest->key != NULL) {
                                while (dest->next != NULL) {
                                        dest = dest->next;
                                }
                                dest->next = calloc(1, sizeof(intmap_element));
                                dest = dest->next;
                        }
                        dest->key = src->key;
                        dest->value = src->value;
                        src = src->next;
                }
        }
        free_elements(old_elements, map->max_elements);
        map->max_elements = new_max_elements;
}

void intmap_insert(intmap *map, char *key, int value)
{
        unsigned int index;
        intmap_element *current;

        index = hash(key) % map->max_elements;
        current = &(map->elements[index]);
        if (current->key != NULL && strcmp(current->key, key)) {
                while (current->next != NULL)
                        current = current->next;
                current->next = calloc(1, sizeof(struct intmap_element));
                current = current->next;
                ++map->num_elements;
        } else if (current->key == NULL) {
                ++map->num_elements;
        }
        current->key = key;
        current->value = value;
        if (map->num_elements >= map->max_elements * 0.75f) {
                rehash(map);
        }
}

int *intmap_get(intmap *map, const char *key)
{
        unsigned int index;
        intmap_element *current;
        
        index = hash(key) % map->max_elements;
        current = &(map->elements[index]);
        if (current == NULL) {
                return NULL;
        }
        if (current->key == NULL)
                return NULL;
        do {
                if (!strcmp(current->key, key))
                        return &(current->value);
        } while ((current = current->next) != NULL);
        return NULL;
}

static void free_elements(struct intmap_element *elements, size_t n)
{
        unsigned int i;
        intmap_element *root, *iter;

        for (i = 0; i < n; ++i) {
                root = &(elements[i]);
                iter = root;
                do {
                        while (iter->next != NULL)
                                iter = iter->next;
                        if (iter != root)
                                free(iter);
                        iter = root;
                } while (iter != root);
        }
        free(elements);
}

void intmap_delete(intmap *map)
{
        /*
        unsigned int i;
        intmap_element *current;
        */

        free_elements(map->elements, map->max_elements);
        /*
        for (i = 0; i < map->size; ++i) {
                current = &(map->elements[i]);
                while (current->next != NULL) {
                        free(current->next);
                        current = current->next;
                }
        }
        free(map->elements);
        */
        free(map);
}
