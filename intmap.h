#ifndef INTMAP_INCLUDED
#define INTMAP_INCLUDED

typedef struct intmap intmap;
typedef struct intmap_element intmap_element;

intmap *intmap_new(size_t initial_size);
void intmap_insert(intmap *map, char *key, int value);
int *intmap_get(intmap *map, const char *key);
void intmap_delete(intmap *map);

struct kv_pair {
        char *key;
        int value;
};

struct intmap_element {
        char *key;
        int value;
        intmap_element *next;
};

struct intmap {
        size_t max_elements;
        size_t num_elements;
        intmap_element *elements;
};

#endif
