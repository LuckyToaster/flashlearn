#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#define MAX_WORDS 50
#define WORD_LEN 50

typedef struct
{
    char name[WORD_LEN];
    char german[MAX_WORDS][WORD_LEN];
    char english[MAX_WORDS][WORD_LEN];
    int word_count;
} bundle_t;

// main functions
bundle_t* new_bundle();
bundle_t* load_bundles(FILE* f);
bundle_t* new_from_file(FILE* f, char* name);
void save_bundle(FILE *f);
void print_bundles(bundle_t *bundles, size_t size);
void play();

// helper functions
void clear_stdin();
void trim(char *str);
void get_input_int(int *dst, char *msg);
void get_input_str(char *dst, size_t dst_len, char *msg);
FILE* open_file(const char *path, char *mode);
size_t file_size(FILE *f);

const char CONFIG_PATH[] = "flashlearn.bin";
const char NAME[] = "NAME\n\tFLASHLEARN - make bundles of flashcards and play a word memorisation game\n";
const char SYNOPSIS[] = "SYNOPSIS\n\tflashlang [OPTION]\n";
const char DESCRIPTION[] = "DESCRIPTION:\n\t-p, --print\tPrint all the bundles\n\t -h, --help\tPrint this menu\n\t-n, --new\tMake new word bundle\n";

int main(int argc, char **argv) {
    if (argc > 2) {
        printf("%s", DESCRIPTION);
        return 1;
    }

    FILE* f = open_file(CONFIG_PATH, "ab+");
    if (file_size(f) < sizeof(bundle_t) && argc == 1) {
        printf("Config File '%s' appears empty\n", CONFIG_PATH);
        exit(1);
    }

    bundle_t* bundles = load_bundles(f);
    size_t bundle_n = file_size(f) / sizeof(bundle_t);

    
    FILE* f2 = open_file("verbs2", "rb");
    bundle_t *v2 = new_from_file(f2, "verbs 2");
    fclose(f2);

    FILE *f3 = open_file("verbs3", "rb");
    bundle_t *v3 = new_from_file(f3, "verbs 3");
    fclose(f3);

    FILE *f4 = open_file("verbs4", "rb");
    bundle_t *v4 = new_from_file(f4, "verbs 4");
    fclose(f4);

    print_bundles(v2, 1);
    print_bundles(v3, 1);
    print_bundles(v4, 1);

    fwrite(v2, sizeof(bundle_t), 1, f);
    fwrite(v3, sizeof(bundle_t), 1, f);
    fwrite(v4, sizeof(bundle_t), 1, f);

    free(v2);
    free(v3);
    free(v4);

    if (argc == 1) play(bundles, bundle_n);

    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
        printf("%s%s%s", NAME, SYNOPSIS, DESCRIPTION);

    else if (strcmp(argv[1], "-p") == 0 || strcmp(argv[1], "--print") == 0)
        print_bundles(bundles, bundle_n);

    else if (strcmp(argv[1], "-n") ==0 || strcmp(argv[1], "--new") == 0) {
        bundle_t* b = new_bundle();
        fwrite(b, sizeof(bundle_t), 1, f);
        free(b);
    }

    fclose(f);
    free(bundles);
    return 0;
}


bundle_t *new_from_file(FILE *f, char* name) {
    bundle_t* b = (bundle_t*) malloc(sizeof(bundle_t));
    strcpy(b->name, name);

    int i = 0, e = 0;
    char line[100];
    while (fgets(line, sizeof(line), f) != NULL) {
        char* token = strtok(line, "-");
        while (token != NULL) {
            trim(token); 
            if (e % 2 == 0) strcpy(b->english[i], token);
            else strcpy(b->german[i], token);
            token = strtok(NULL, "-");
            e++;
        } 
        i++;
    }
    b->word_count = i;
    return b;
}


bundle_t *new_bundle() {
    bundle_t *b = (bundle_t *)malloc(sizeof(bundle_t));
    get_input_str(b->name, WORD_LEN, "Bundle name? >> ");
    get_input_int(&b->word_count, "Number of words? >> ");

    char english_word[WORD_LEN], german_word[WORD_LEN], msg1[100], msg2[100];
    int i;

    for (i = 0; i < b->word_count; i++) {
        sprintf(msg1, "Word %d in english? >> ", i + 1);
        sprintf(msg2, "Word %d in german? >> ", i + 1);

        get_input_str(english_word, WORD_LEN, msg1);
        get_input_str(german_word, WORD_LEN, msg2);

        strcpy(b->english[i], english_word);
        strcpy(b->german[i], german_word);
    }

    if (i < b->word_count - 1) {
        fprintf(stderr, "could not make bundle - bundle impcomplete");
        exit(1);
    }

    return b;
}

bundle_t *load_bundles(FILE *f) {
    size_t bundle_n = file_size(f) / sizeof(bundle_t);
    bundle_t *bundles = (bundle_t *)malloc(sizeof(bundle_t) * bundle_n);
    fread(bundles, sizeof(bundle_t), bundle_n, f);
    return bundles;
}

void get_input_str(char *dst, size_t dst_len, char *msg) {
    printf("%s", msg);
    fgets(dst, dst_len, stdin);
    trim(dst);
}

void get_input_int(int *dst, char *msg) {
    printf("%s", msg);
    scanf("%d", dst);
    clear_stdin();
}

void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n');
}

void trim(char *str) {
    char *start = str;
    char *end = str + strlen(str) - 1;

    while (*start == ' ' || *start == '\n') start++;
    while (end > start && (*end == ' ' || *end == '\n')) end--;
    *(end + 1) = '\0';

    if (start != str) memmove(str, start, end - start + 2);
}

FILE *open_file(const char *path, char *mode) {
    FILE *f = fopen(path, mode);
    if (f == NULL)
    {
        perror("Couldn't open config file");
        exit(EXIT_FAILURE);
    }
    return f;
}

size_t file_size(FILE *f) {
    fseek(f, 0, SEEK_END);
    size_t res = ftell(f);
    rewind(f);
    return res;
}

void print_bundles(bundle_t *bundles, size_t size) {
    for (int i = 0; i < size; i++) {
        printf("%s:\n", bundles[i].name);
        for (int k = 0; k < bundles[i].word_count; k++)
            printf("\t%s => %s\n", bundles[i].german[k], bundles[i].english[k]);
        printf("\n");
    }
}

void play(bundle_t *bundles, size_t size) {
    srand(time(NULL));

    for (int i = 0; i < 3; i++) {
        bundle_t bundle = bundles[rand() % size];

        char guesses = 3;
        char guess[WORD_LEN];
        char msg[100];
        int idx;

        while (guesses > 0) {
            idx = rand() % bundle.word_count;
            sprintf(msg, "What is '%s'? >> ", bundle.german[idx]);
            get_input_str(guess, WORD_LEN, msg);

            if (0 != strcmp(guess, bundle.english[idx])) {
                printf("Nope, its '%s'\n", bundle.english[idx]);
                guesses--;
            }
        } printf("Next ... Boonda!\n");
    } printf("YOU DIED. WOMP WOMP! :()\n");
}
