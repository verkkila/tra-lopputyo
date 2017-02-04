#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <ctype.h>
#include "hashmap.h"
#include "arraylist.h"

#define NUM_PRINTED_PHRASES 100

void print_usage_and_exit(void);
char *read_file_into_memory(const char *filename, size_t *OUT_size);
void change_to_lowercase(char *data_begin, char *data_end);
void insert_into_map(hashmap *map, char *data_begin, char *data_end);
void insert_in_order(struct arraylist *list, hashmap *map);
void print_most_common_words(struct arraylist *list);
void print_all_related_phrases(const char *key, char *data_begin, char *data_end);
void free_keys_and_hashmap(hashmap *map);

char *max_key = NULL;
int max_value = 0;

int main(int argc, char **argv)
{
        size_t data_size;
        char *file_data;
        clock_t start, after_file_read, after_map_insertion, after_finding_most_common;
        hashmap *words;
        struct arraylist *sorted_phrases;

        if (argc < 2 || argc > 3) {
                print_usage_and_exit();
        }

        words = hashmap_new(1000);
        if (words == NULL)
                return 1;
        start = clock();

        file_data = read_file_into_memory(argv[1], &data_size);
        change_to_lowercase(file_data, file_data + data_size);
        after_file_read = clock() - start;
        printf("File read to memory in %f seconds.\n", (float)after_file_read / CLOCKS_PER_SEC);

        insert_into_map(words, file_data, file_data + data_size);
        after_map_insertion = clock() - after_file_read;
        printf("Hash table built in %f seconds.\n", (float)after_map_insertion / CLOCKS_PER_SEC);

        sorted_phrases = arraylist_new(max_value + 1);
        if (sorted_phrases == NULL)
                return 1;

        insert_in_order(sorted_phrases, words);
        print_most_common_words(sorted_phrases);
        after_finding_most_common = clock() - after_map_insertion;
        printf("Found 100 most common words in %f seconds.\n", (float)after_finding_most_common / CLOCKS_PER_SEC);

        printf("Total runtime: %f seconds.\n", ((float)after_file_read + (float)after_map_insertion + (float)after_finding_most_common) / CLOCKS_PER_SEC);

        if (argc == 3 && !strncmp(argv[2], "-p", 2)) {
                printf("Phrases related to \"%s\":\n", max_key);
                print_all_related_phrases(max_key, file_data, file_data + data_size);
        }
        arraylist_free(sorted_phrases);
        free_keys_and_hashmap(words);
        free(file_data);
        return 0;
}

void print_usage_and_exit(void)
{
        printf("Usage: ./tra-lopputyo phrases.txt [-p]\n-p\tPrint all phrases related to the most common one.\n");
        exit(1);
}

void change_to_lowercase(char *data_begin, char *data_end)
{
        char *str;

        str = data_begin;
        while (str < data_end) {
                *str = tolower(*str);
                ++str;
        }
}

char *read_file_into_memory(const char *file_name, size_t *OUT_size)
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
                print_usage_and_exit();
                return NULL;
        }
        /*measure the size of the file*/
        fseek(f, 0, SEEK_END);
        file_size = ftell(f);
        fseek(f, 0, SEEK_SET);

        file_data = NULL;
        file_data = malloc(file_size);
        if (file_data == NULL) {
                printf("Could not allocate enough memory for the file. (size = %lu\n", file_size);
                exit(1);
        } else {
                read_bytes = fread(file_data, 1, file_size, f);
                printf("File data located at: 0x%08lx-0x%08lx (read %lu/%lu bytes)\n", (unsigned long)file_data, (unsigned long)(file_data + file_size - 1), read_bytes, file_size);
                if (OUT_size != NULL)
                        *OUT_size = read_bytes;
        }
        fclose(f);
        return file_data;
}

void insert_into_map(hashmap *map, char *data_begin, char *data_end)
{
        char *current_str;
        unsigned int str_len;
        int *map_value;

        map_value = NULL;
        str_len = 0;
        current_str = data_begin;
        while (current_str < data_end) {
                char *key, *map_key;

                /*calculate the length of the phrase and allocate memory for it*/
                str_len = strcspn(current_str, ":");
                key = malloc(str_len+1);
                if (key == NULL) {
                        printf("Could not allocate memory for key.\n");
                        exit(1);
                }
                strncpy(key, current_str, str_len);
                key[str_len] = '\0';
                map_key = hashmap_get_key(map, key);
                map_value = hashmap_get(map, key);
                if (map_value == NULL) {
                        hashmap_insert(map, key, 1);
                } else {
                        /*key is already in table so we can free this one*/
                        free(key);
                        ++*map_value;
                }
                /*keep track of the highest key and value*/
                if (map_value != NULL && *map_value > max_value) {
                        max_key = map_key;
                        max_value = *map_value;
                }
                current_str = strchr(current_str, '\n');
                /*end of data*/
                if (current_str == 0)
                        break;
                current_str++;
        }
}

void insert_in_order(struct arraylist *list, hashmap *map)
{
        hashmap_element *current;
        unsigned int i;

        for (i = 0; i < map->max_elements; ++i) {
                current = &(map->elements[i]);
                if (current->key == NULL)
                        continue;
                while (current != NULL) {
                        arraylist_insert(list, current->value, current->key);
                        current = current->next;
                }
        }

}

void print_most_common_words(struct arraylist *list)
{
        int i, count;
        FILE *out;

        out = fopen("output.txt", "w");
        if (out == NULL) {
                printf("Could not open output file \"output.txt\".\n");
                exit(1);
        }
        count = 0;
        for (i = max_value; i >= 0 && count < NUM_PRINTED_PHRASES; --i) {
                struct node *current;

                current = &(list->elements[i]);
                assert(current != NULL);
                while (current != NULL && current->key != NULL && count < NUM_PRINTED_PHRASES) {
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
        while (current_str < data_end) {
                size_t str_len;

                str_len = strcspn(current_str, ":");
                if (!strncmp(current_str, key, str_len)) {
                        char buf[32];
                        size_t phrase_len;
                        char *phrase_end;

                        /*phrase_end is at the last character of related_phrase*/
                        phrase_end = strchr(current_str, '\n') - 1;
                        /*current_str is at the beginning of phrase:related_phrase,
                        str_len is the length of phrase including the ':'*/
                        phrase_len = phrase_end - (current_str + str_len);
                        /*phrase_len should be 32 at max*/
                        phrase_len = phrase_len > 32 ? 32 : phrase_len;
                        /*current_str + str_len + 1 is the first character of related_phrase*/
                        strncpy(buf, current_str + str_len + 1, phrase_len);
                        buf[phrase_len] = '\0';
                        printf("%s\n", buf);
                }
                current_str = strchr(current_str, '\n');
                if (current_str == 0)
                        return;
                ++current_str;
        }
}

/*
Because keys for the hashmap were allocated outside of the hashmap, we need to first free them
before freeing the map. We can't free keys inside the hashmap's free function because the user
decides where the keys are (they may not be malloc'd at all).
*/
void free_keys_and_hashmap(hashmap *map)
{
        unsigned int i;
        struct hashmap_element *current;

        for (i = 0; i < map->max_elements; ++i) {
                current = &(map->elements[i]);
                if (current->key == NULL)
                        continue;
                while (current != NULL) {
                        free(current->key);
                        current = current->next;
                }
        }
        hashmap_free(map);
}
