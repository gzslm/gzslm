#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_WORDS 1000
#define MAX_WORD_LEN 50

// ��ȡ�ļ�����
char* read_file(const char* file_path) {
    FILE* file = fopen(file_path, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(length + 1);
    if (!buffer) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0';

    fclose(file);
    return buffer;
}

// �ִʺ���
void tokenize(char* text, char words[][MAX_WORD_LEN], int* word_count) {
    char* token = strtok(text, " ,.!\n");
    *word_count = 0;

    while (token != NULL && *word_count < MAX_WORDS) {
        strncpy(words[(*word_count)++], token, MAX_WORD_LEN);
        token = strtok(NULL, " ,.!\n");
    }
}

// �����Ƶ����
void compute_word_freq(char words[][MAX_WORD_LEN], int word_count, int* freq, char unique_words[][MAX_WORD_LEN], int* unique_count) {
    *unique_count = 0;

    for (int i = 0; i < word_count; i++) {
        int found = 0;
        for (int j = 0; j < *unique_count; j++) {
            if (strcmp(words[i], unique_words[j]) == 0) {
                freq[j]++;
                found = 1;
                break;
            }
        }
        if (!found) {
            strncpy(unique_words[*unique_count], words[i], MAX_WORD_LEN);
            freq[*unique_count] = 1;
            (*unique_count)++;
        }
    }
}

// �����������ƶ�
double cosine_similarity(int* freq1, int* freq2, int unique_count) {
    double dot_product = 0.0;
    double magnitude1 = 0.0;
    double magnitude2 = 0.0;

    for (int i = 0; i < unique_count; i++) {
        dot_product += freq1[i] * freq2[i];
        magnitude1 += freq1[i] * freq1[i];
        magnitude2 += freq2[i] * freq2[i];
    }

    magnitude1 = sqrt(magnitude1);
    magnitude2 = sqrt(magnitude2);

    if (magnitude1 == 0 || magnitude2 == 0) {
        return 0.0;
    }

    return dot_product / (magnitude1 * magnitude2);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <original_file> <plagiarized_file> <output_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // ��ȡ�ļ�����
    char* original_text = read_file(argv[1]);
    char* plagiarized_text = read_file(argv[2]);

    // �ִ�
    char original_words[MAX_WORDS][MAX_WORD_LEN];
    char plagiarized_words[MAX_WORDS][MAX_WORD_LEN];
    int original_word_count, plagiarized_word_count;

    tokenize(original_text, original_words, &original_word_count);
    tokenize(plagiarized_text, plagiarized_words, &plagiarized_word_count);

    // �����Ƶ����
    int original_freq[MAX_WORDS] = {0};
    int plagiarized_freq[MAX_WORDS] = {0};
    char unique_words[MAX_WORDS][MAX_WORD_LEN];
    int unique_count;

    compute_word_freq(original_words, original_word_count, original_freq, unique_words, &unique_count);
    compute_word_freq(plagiarized_words, plagiarized_word_count, plagiarized_freq, unique_words, &unique_count);

    // �����������ƶ�
    double similarity = cosine_similarity(original_freq, plagiarized_freq, unique_count);

    // ���������ļ�
    FILE* output_file = fopen(argv[3], "w");
    if (!output_file) {
        perror("Failed to open output file");
        return EXIT_FAILURE;
    }

    fprintf(output_file, "%.2f\n", similarity);
    fclose(output_file);

    // �ͷ��ڴ�
    free(original_text);
    free(plagiarized_text);

    return EXIT_SUCCESS;
}
