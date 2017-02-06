#ifndef INTMAP_INCLUDED
#define INTMAP_INCLUDED

#include <assert.h>
#include <stdbool.h>

typedef struct hashmap hashmap;
typedef struct hashmap_element hashmap_element;

struct hashmap_element {
        char *key;
        int value;
        hashmap_element *next;
};

struct hashmap {
        size_t max_elements;
        size_t num_elements;
        hashmap_element *elements;
};

hashmap *hashmap_new(size_t initial_size);
void hashmap_insert(hashmap *map, char *key, int value);
int *hashmap_get(hashmap *map, const char *key);
char *hashmap_get_key(hashmap *map, const char *key);
unsigned int hashmap_count_collisions(hashmap *map);
void hashmap_free(hashmap *map);

#endif
