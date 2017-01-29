#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <ctype.h>
#include "intmap.h"

char *read_file_into_memory(const char *filename, unsigned int *OUT_size);
void insert_into_map(intmap *map, char *data_begin, char *data_end);

int main(int argc, char **argv)
{
        unsigned int data_size, i;
        char *phrases, *str;
        clock_t start, after_file_read, after_map_insertion;
        intmap *testmap;

        testmap = intmap_new(1000);
        start = clock();
        phrases = read_file_into_memory(argv[1], &data_size);
        str = phrases;
        for (i = 0; i < data_size; ++i) {
                str[i] = tolower(str[i]);
        }
        after_file_read = clock() - start;
        printf("Read file into memory in %f seconds.\n", (float)after_file_read / CLOCKS_PER_SEC);
        insert_into_map(testmap, phrases, phrases+data_size);
        after_map_insertion = clock() - after_file_read;
        printf("Inserted everything into a hash table in %f seconds.\n", (float)after_map_insertion / CLOCKS_PER_SEC);
        free(phrases);
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
                char *temp;

                str_len = strcspn(current_str, ":");
                temp = malloc(str_len+1);
                if (temp == NULL) {
                        printf("Could not allocate memory for string.\n");
                        exit(1);
                }
                strncpy(temp, current_str, str_len);
                temp[str_len] = '\0';
                map_value = intmap_get(map, temp);
                if (map_value == NULL) {
                        intmap_insert(map, temp, 1);
                } else {
                        ++*map_value;
                }
                current_str = strchr(current_str, '\n');
                if (current_str == 0)
                        break;
                current_str++;
        }
}
