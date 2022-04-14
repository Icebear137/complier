#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct word {
    char *word;
    int coordinates[100][2];
    int count;
} word;
//change to lowercase
void normalize(char *str) {
    int i;
    for (i = 0; str[i] != '\0'; i++) {
        str[i] = tolower(str[i]);
        // Remove punctuation
        if (!isalnum(str[i])) {
            str[i] = ' ';
        }
    }
    int j = 0;
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] != ' ') {
            str[j] = str[i];
            j++;
        }
    }
    str[j] = '\0';
}
//get line from file
int getline(FILE *fp, char *str) {
    int i = 0;
    char c;
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            str[i] = '\0';
            return 1;
        }
        str[i] = c;
        i++;
    }
    return 0;
}
//get word from line
void get_words(char *filename, word **words, int *word_count) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file\n");
        exit(1);
    }
    char line[1024];
    char tmp[100];
    int line_num = 1;
    int i = 0;
    while(getline(fp, line)) {
        char* data = line;
        int total_offset = 0;
        int offset = 0;
        while (sscanf(data, "%s%n", tmp, &offset) == 1) {
            normalize(tmp);
            // Check if word is already in the list
            int found = 0;
            int j;
            for (j = 0; j < i; j++) {
                if (strcmp(words[j]->word, tmp) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                words[i] = malloc(sizeof(word));
                words[i]->word = malloc(sizeof(char) * (strlen(tmp) + 1));
                strcpy(words[i]->word, tmp);
                words[i]->coordinates[0][0] = line_num;
                // Get character position in line
                words[i]->coordinates[0][1] = total_offset == 0 ? 1 : total_offset + 2;
                words[i]->count = 1;
                i++;
            } else {
                words[j]->coordinates[words[j]->count][0] = line_num;
                // Get character position in line
                words[j]->coordinates[words[j]->count][1] = total_offset == 0 ? 1 : total_offset + 2;
                words[j]->count = words[j]->count + 1;
            }
            data += offset;
            total_offset += offset;
        }
        line_num++;
    }
    fclose(fp);
    *word_count = i;
}
//sort words by alphabetical order
void sort_words(word **words, int *word_count) {
    int i;
    for (i = 0; i < *word_count - 1; i++) {
        int j;
        for (j = i + 1; j < *word_count; j++) {
            if (strcmp(words[i]->word, words[j]->word) > 0) {
                word *tmp = words[i];
                words[i] = words[j];
                words[j] = tmp;
            }
        }
    }
}
//main function
int main() {
    word** words = malloc(sizeof(word) * 100);
    int word_count = 0;

    get_words("vanban.txt", words, &word_count);
    sort_words(words, &word_count);
    // Print the words
    for (int i = 0; i < word_count; i++) {
        printf("%s %d:", words[i]->word, words[i]->count);
        // Print coordinates
        for (int j = 0; j < words[i]->count; j++) {
            printf(" %d", words[i]->coordinates[j][0]);
        }
        printf("\n");
    }
}