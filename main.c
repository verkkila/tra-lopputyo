#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <ctype.h>
#include "intmap.h"

char *read_file_into_memory(const char *filename, unsigned int *OUT_size);
void insert_into_map(intmap *map, char *data_begin, char *data_end);
struct node *find_free_spot(struct node *array, int value);
struct node *allocate_list_array(void);
void get_most_common_words(struct node *array, intmap *map);
void print_most_common_words(struct node *array);
void print_all_related_phrases(const char *key, char *data_begin, char *data_end);

struct node {
        char *key;
        struct node *next;
};

struct kv_pair max_pair = {NULL, 0};

int main(int argc, char **argv)
{
        unsigned int data_size, i;
        char *phrases, *str;
        clock_t start, after_file_read, after_map_insertion, after_finding_most_common;
        intmap *testmap;
        struct node *most_common_words;

        testmap = intmap_new(1000);
        start = clock();
        phrases = read_file_into_memory(argv[1], &data_size);
        str = phrases;
        for (i = 0; i < data_size; ++i) {
                str[i] = tolower(str[i]);
        }
        after_file_read = clock() - start;
        printf("File read to memory in %f seconds.\n", (float)after_file_read / CLOCKS_PER_SEC);
        insert_into_map(testmap, phrases, phrases + data_size);
        after_map_insertion = clock() - after_file_read;
        printf("Hash table built in %f seconds.\n", (float)after_map_insertion / CLOCKS_PER_SEC);
        most_common_words = allocate_list_array();
        if (most_common_words == NULL) {
                return 1;
        }
        get_most_common_words(most_common_words, testmap);
        print_most_common_words(most_common_words);
        after_finding_most_common = clock() - after_map_insertion;
        printf("Found 100 most common words in %f seconds.\n", (float)after_finding_most_common / CLOCKS_PER_SEC);
        printf("Total runtime: %f seconds.\n", ((float)after_file_read + (float)after_map_insertion + (float)after_finding_most_common) / CLOCKS_PER_SEC);
        printf("Phrases related to \"%s\":\n", max_pair.key);
        print_all_related_phrases(max_pair.key, phrases, phrases + data_size);
        free(phrases);
        free(most_common_words);
        (void)argc;
        return 0;
}

char *read_file_into_memory(const char *file_name, unsigned int *OUT_size)
{
        FILE *f;
        size_t file_size;
        char *file_data;
        size_t read_bytes;

        assert(file_name != NULL);
        f = NULL;
        f = fopen(file_name, "rb");
        if (f == NULL) {
                printf("Failed to open file \"%s\"\n", file_name);
                return NULL;
        }
        fseek(f, 0, SEEK_END);
        file_size = ftell(f);
        if (OUT_size != NULL)
                *OUT_size = file_size;   
        fseek(f, 0, SEEK_SET);

        file_data = NULL;
        file_data = malloc(file_size);
        if (file_data == NULL) {
                printf("Could not allocate enough memory for the file. (size = %u\n", file_size);
        } else {
                read_bytes = fread(file_data, 1, file_size, f);
                printf("File data located at: 0x%08x-0x%08x (size = %u bytes, read %u bytes)\n", file_data, (size_t)file_data + file_size, file_size, read_bytes);
        }
        fclose(f);
        return file_data;
}

void insert_into_map(intmap *map, char *data_begin, char *data_end)
{
        char *current_str;
        unsigned int str_len;
        int *map_value;

        map_value = NULL;
        str_len = 0;
        current_str = data_begin;
        while ((unsigned int)current_str < (unsigned int)data_end) {
                char *key;

                str_len = strcspn(current_str, ":");
                key = malloc(str_len+1);
                if (key == NULL) {
                        printf("Could not allocate memory for string.\n");
                        exit(1);
                }
                strncpy(key, current_str, str_len);
                key[str_len] = '\0';
                map_value = intmap_get(map, key);
                if (map_value == NULL) {
                        intmap_insert(map, key, 1);
                } else {
                        ++*map_value;
                }
                if (map_value != NULL && *map_value > max_pair.value) {
                        max_pair.key = key;
                        max_pair.value = *map_value;
                }
                current_str = strchr(current_str, '\n');
                if (current_str == 0)
                        break;
                current_str++;
        }
}

struct node *allocate_list_array(void)
{
        struct node *array;

        array = NULL;
        array = calloc(max_pair.value + 1, sizeof(struct node));
        if (array == NULL)
                return NULL;
        return array;
}

struct node *find_free_spot(struct node *array, int value)
{
        struct node *spot;

        spot = &(array[value]);
        if (spot->key == NULL)
                return spot;
        while (spot->next != NULL)
                spot = spot->next;
        spot->next = calloc(1, sizeof(struct node));
        return spot->next;
}

void get_most_common_words(struct node *array, intmap *map)
{
        intmap_element *current;
        struct node *current_node;
        unsigned int i;

        for (i = 0; i < map->max_elements; ++i) {
                current = &(map->elements[i]);
                while (current != NULL) {
                        int val;

                        val = current->value;
                        current_node = find_free_spot(array, val);
                        current_node->key = current->key;
                        current = current->next;
                }
        }

}

void print_most_common_words(struct node *array)
{
        int i, count;
        FILE *out;

        out = fopen("output.txt", "w");
        if (out == NULL) {
                return;
        }
        count = 0;
        for (i = max_pair.value; i >= 0 && count < 100; --i) {
                struct node *current;

                current = &(array[i]);
                while (current != NULL && current->key != NULL && count < 100) {
                        fprintf(out, "%s %i\n", current->key, i);
                        ++count;
                        current = current->next;
                }
        }
        fclose(out);
}

void print_all_related_phrases(const char *key, char *data_begin, char *data_end)
{
        char *current_str;

        current_str = data_begin;
        while ((unsigned int)current_str < (unsigned int)data_end) {
                size_t str_len;

                str_len = strcspn(current_str, ":");
                if (!strncmp(current_str, key, str_len)) {
                        char buf[32];
                        size_t phrase_len;
                        char *phrase_end;

                        phrase_end = strchr(current_str, '\n');
                        phrase_len = phrase_end - (current_str + str_len); 
                        strncpy(buf, current_str + str_len + 1, phrase_len);
                        buf[phrase_len - 1] = '\0';
                        printf("%s\n", buf);
                }
                current_str = strchr(current_str, '\n');
                if (current_str == 0)
                        return;
                ++current_str;
        }
}
