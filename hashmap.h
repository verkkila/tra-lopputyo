#ifndef INTMAP_INCLUDED
#define INTMAP_INCLUDED

#include <stdbool.h>

typedef struct hashmap hashmap;
typedef struct hashmap_element hashmap_element;

hashmap *hashmap_new(size_t initial_size);
void hashmap_insert(hashmap *map, char *key, int value);
int *hashmap_get(hashmap *map, const char *key);
void hashmap_free(hashmap *map);

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

#endif
