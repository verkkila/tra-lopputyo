#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hashmap.h"

static void free_elements(hashmap_element *elements, size_t n);
static void rehash(hashmap *map);
/*
 * djb2 - http://www.cse.yorku.ca/~oz/hash.html
 */

static unsigned int hash(unsigned const char *str)
{
        unsigned int hash = 5381;
        int c;

        while (c = *str++)
                hash = ((hash << 5) + hash) + c;
        return hash;
}

hashmap *hashmap_new(size_t initial_size)
{
        hashmap *map;
        size_t min_element_count;

        map = NULL;
        map = malloc(sizeof(struct hashmap));
        if (map == NULL) {
                printf("Failed to allocate memory for hashmap.\n");
                return NULL;
        }
        /*min_element_count is the smallest possible size with 0.75 load factor*/
        min_element_count = initial_size * 1.33f;
        map->max_elements = 1;
        /*real size will be the next power of two*/
        while (map->max_elements <= min_element_count) {
                map->max_elements *= 2;
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

        /*create a new array with twice the size*/
        new_max_elements = map->max_elements * 2;
        new_elements = calloc(new_max_elements, sizeof(hashmap_element));
        if (new_elements == NULL) {
                printf("Hashmap rehashing failed, could not allocate memory for the new map.\n");
                return;
        }
        old_elements = map->elements;
        map->elements = new_elements;

        for (i = 0; i < map->max_elements; ++i) {
                struct hashmap_element *dest, *src;

                src = &(old_elements[i]);
                assert(src != NULL);
                /*relies on loop short-circuiting*/
                while (src != NULL && src->key != NULL) {
                        unsigned int index;
                        
                        index = hash(src->key) % new_max_elements;
                        dest = &(map->elements[index]);
                        assert(dest != NULL);
                        /*if the spot is taken, create a new one at the end of the list*/
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
        assert(current != NULL);
        /*if the spot is taken, but key is different, create a new node at the end of the list*/
        if (current->key != NULL && strcmp(current->key, key)) {
                while (current->next != NULL)
                        current = current->next;
                current->next = calloc(1, sizeof(struct hashmap_element));
                current = current->next;
        }
        ++map->num_elements;
        current->key = key;
        current->value = value;
        if (map->num_elements > map->max_elements * 0.75f) {
                rehash(map);
        }
}

int *hashmap_get(hashmap *map, const char *key)
{
        unsigned int index;
        hashmap_element *current;
        
        index = hash(key) % map->max_elements;
        current = &(map->elements[index]);
        assert(current != NULL);
        if (current->key == NULL)
                return NULL;
        do {
                if (!strcmp(current->key, key))
                        return &(current->value);
        } while ((current = current->next) != NULL);
        return NULL;
}

char *hashmap_get_key(hashmap *map, const char *key)
{
        unsigned int index;
        hashmap_element *current;
        
        index = hash(key) % map->max_elements;
        current = &(map->elements[index]);
        assert(current != NULL);
        if (current->key == NULL)
                return NULL;
        do {
                if (!strcmp(current->key, key))
                        return current->key;
        } while ((current = current->next) != NULL);
        return NULL;
}

unsigned int hashmap_count_collisions(hashmap *map)
{
        unsigned int i, collisions;
        hashmap_element *current;

        collisions = 0;
        for (i = 0; i < map->max_elements; ++i) {
                current = &(map->elements[i]);
                assert(current != NULL);
                while (current->next != NULL) {
                        ++collisions;
                        current = current->next;
                }
        }
        return collisions;
}

static void free_elements(struct hashmap_element *elements, size_t n)
{
        unsigned int i;
        hashmap_element *root, *iter;

        for (i = 0; i < n; ++i) {
                hashmap_element *prev;
                root = &(elements[i]);
                assert(root != NULL);
                prev = root;
                do {
                        iter = root;
                        /*Move to the end of the list while keeping track of the previous node*/
                        while (iter->next != NULL) {
                                prev = iter;
                                iter = iter->next;
                        }
                        /*If the node isn't part of the array, free it*/
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
        free_elements(map->elements, map->max_elements);
        free(map);
}
