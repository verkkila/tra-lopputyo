#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "hashmap.h"

char *read_file_into_memory(const char *filename);

int main(int argc, char **argv)
{
        char *phrases;
        clock_t start;
        hashmap *testmap;

        start = clock();
        phrases = read_file_into_memory(argv[1]);
        printf("Time taken: %f\n", (float)(clock() - start) / CLOCKS_PER_SEC);
        free(phrases);
        (void)argc;
        return 0;
}

char *read_file_into_memory(const char *file_name)
{
        FILE *f;
        size_t file_size;
        char *file_data;

        assert(file_name != NULL);
        f = NULL;
        f = fopen(file_name, "r");
        if (f == NULL) {
                printf("Failed to open file \"%s\"\n", file_name);
                return NULL;
        }
        fseek(f, 0, SEEK_END);
        file_size = ftell(f);
        fseek(f, 0, SEEK_SET);

        file_data = NULL;
        file_data = malloc(file_size);
        if (file_data == NULL) {
                printf("Could not allocate enough memory for the file. (size = %u\n", file_size);
        } else {
                fread(file_data, file_size, 1, f);
                printf("File data located at: 0x%08x-0x%08x (size = %u bytes)\n", file_data, (size_t)file_data + file_size, file_size);
        }
        fclose(f);
        return file_data;
}
