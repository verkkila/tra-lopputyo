#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hashmap.h"

/*
 * sdbm - http://www.cse.yorku.ca/~oz/hash.html
 */
static unsigned int hash(hashmap *map, const char *key)
{
        unsigned int hash;
        int c;

        hash = 0;
        while ((c = *key++) != 0) {
                hash = c + (hash << 6) + (hash << 16) - hash;
        }
        return hash % map->size;
}

hashmap *hashmap_new(size_t initial_size)
{
        hashmap *map;

        map = NULL;
        map = malloc(sizeof(struct hashmap));
        if (map == NULL)
                return NULL;
        map->size = initial_size;
        map->elements = NULL;
        map->elements = calloc(initial_size, sizeof(struct hashmap_element));
        if (map->elements == NULL) {
                free(map);
                return NULL;
        }
        return map;
}

void hashmap_insert(hashmap *map, char *key, int value)
{
        unsigned int index;
        struct hashmap_element *current;

        index = hash(map, key);
        current = &(map->elements[index]);
        if (current->key != NULL) {
                while (current->next != NULL)
                        current = current->next;
                current->next = calloc(1, sizeof(struct hashmap_element));
                current = current->next;
        }
        /*potentially unsafe*/
        current->key = key;
        current->value = value;
}

void hashmap_print(hashmap *map)
{
        unsigned int i;
        struct hashmap_element *current;

        for (i = 0; i < map->size; ++i) {
                current = &(map->elements[i]);
                if (current->key == NULL)
                        continue;
                printf("%s:%i\n", map->elements[i].key, map->elements[i].value); 
                while ((current = current->next) != NULL) {
                        printf("%s:%i (chained)\n", current->key, current->value);
                }
        }
}

void hashmap_delete(hashmap *map)
{
        unsigned int i;
        struct hashmap_element *current;

        for (i = 0; i < map->size; ++i) {
                current = &(map->elements[i]);
                while (current->next != NULL) {
                        free(current->next);
                        current = current->next;
                }
        }
        free(map->elements);
        free(map);
}
