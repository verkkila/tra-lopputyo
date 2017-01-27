#ifndef HASHMAP_INCLUDED
#define HASHMAP_INCLUDED

typedef struct hashmap hashmap;

hashmap *hashmap_new(size_t initial_size);
void hashmap_insert(hashmap *map, char *key, int value);
void hashmap_print(hashmap *map);
void hashmap_delete(hashmap *map);

struct hashmap_element {
        char *key;
        int value;
        struct hashmap_element *next;
};

struct hashmap {
        size_t size;
        struct hashmap_element *elements;
};

#endif
