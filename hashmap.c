#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hashmap.h"

static void free_elements(hashmap_element *elements, size_t n);
static void rehash(hashmap *map);
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

hashmap *hashmap_new(size_t initial_size)
{
        hashmap *map;
        size_t min_element_count;

        map = NULL;
        map = malloc(sizeof(struct hashmap));
        if (map == NULL)
                return NULL;
        min_element_count = initial_size * 1.25f;
        map->max_elements = 1;
        while (map->max_elements <= min_element_count) {
                map->max_elements <<= 1;
        }
        map->num_elements = 0;
        map->elements = NULL;
        map->elements = calloc(map->max_elements, sizeof(hashmap_element));
        if (map->elements == NULL) {
                free(map);
                return NULL;
        }
        return map;
}

static void rehash(hashmap *map)
{
        size_t new_max_elements;
        hashmap_element *new_elements, *old_elements;
        unsigned int i;

        new_max_elements = map->max_elements << 1;
        new_elements = calloc(new_max_elements, sizeof(hashmap_element));
        if (new_elements == NULL) {
                return;
        }
        old_elements = map->elements;
        map->elements = new_elements;

        for (i = 0; i < map->max_elements; ++i) {
                struct hashmap_element *dest, *src;

                src = &(old_elements[i]);
                while (src != NULL && src->key != NULL) {
                        unsigned int index;
                        
                        index = hash(src->key) % new_max_elements;
                        dest = &(map->elements[index]);
                        if (dest->key != NULL) {
                                while (dest->next != NULL) {
                                        dest = dest->next;
                                }
                                dest->next = calloc(1, sizeof(hashmap_element));
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

void hashmap_insert(hashmap *map, char *key, int value)
{
        unsigned int index;
        hashmap_element *current;

        index = hash(key) % map->max_elements;
        current = &(map->elements[index]);
        if (current->key != NULL && strcmp(current->key, key)) {
                while (current->next != NULL)
                        current = current->next;
                current->next = calloc(1, sizeof(struct hashmap_element));
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

int *hashmap_get(hashmap *map, const char *key)
{
        unsigned int index;
        hashmap_element *current;
        
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

static void free_elements(struct hashmap_element *elements, size_t n)
{
        unsigned int i;
        hashmap_element *root, *iter;

        for (i = 0; i < n; ++i) {
                hashmap_element *prev;
                root = &(elements[i]);
                if (root == NULL)
                        continue;
                prev = root;
                do {
                        iter = root;
                        while (iter->next != NULL) {
                                prev = iter;
                                iter = iter->next;
                        }
                        if (iter != root) {
                                free(iter);
                                prev->next = NULL;
                        }
                } while (root->next != NULL);
        }
        free(elements);
}

void hashmap_free(hashmap *map)
{
        /*
        unsigned int i;
        hashmap_element *current;
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
