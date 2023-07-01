#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

void print_usage(char *msg);
int *get_counts(char *filename);
void print_counts(int *show, int *count, char *name);

void print_usage(char *msg) 
{
    if(msg == NULL)
    {
        return;
    }
    if(strcmp(msg, "invalid argument")!= 0) 
    {
        perror(msg);
    }
    else
    {
        printf("%s", msg);
        printf("\n");
    }
    printf("\nUsage: wc [-l] [-w] [-c] [FILES...]\n"
    "where:\n"
    "       -l    prints the number of lines\n"
    "       -w    prints the number of words\n"
    "       -c    prints the number of characters\n"
    "       FILES if no files are given, then read\n"
    "             from standard input\n");

    exit(1);
}

int *get_counts(char *filename)
{
    bool in_whitespace = true;
    int f = (filename == NULL) ? STDIN_FILENO : open(filename, O_RDONLY);
    if (f == -1 && filename != NULL) {
        printf("%s: No such file or directory\n", filename);
        int *error = (int *) malloc(sizeof(int));
        error[0] = -1;
        return error;
    }
    char c;
    int wordcount = 0;
    int linecount = 0;
    int charcount = 0;
    while (read(f, &c, 1) > 0) 
    {
        if (c == '\n') {
            linecount++;
        }
        if (isspace(c)) {
            if (!in_whitespace) {
                wordcount++;
            }
            in_whitespace = true;
        } else {
            if (in_whitespace) {
                in_whitespace = false;
            }
        }
        charcount++;
    }
  
    int *counts = (int *) malloc(3 * sizeof(int));
    counts[0] = linecount;
    counts[1] = wordcount;
    counts[2] = charcount;

    if (f != STDIN_FILENO) {
        close(f);
    }

    return counts;
}

void print_counts(int *show, int *count, char *name)
{
    if(show == NULL || count == NULL || name == NULL)
    {
        return;
    }
    if (count[0] == -1)
    {
        return;
    }
   for (int i = 0; i < 3; i++)
    {
        if (show[i])
        {
            printf("%8d ", count[i]);
        }
    }

    if (!show[0] && !show[1] && !show[2])
    {
        printf("%8d , %8d , %8d \n", count[0], count[1], count[2]);
    }

    if (strcmp(name, "") != 0) {
        printf("%s\n", name);
    }
}

int main(int argc, char *argv[]) {
    int show[3] = {0, 0, 0};
    int arg_index = 1;
    bool no_flags = true;
    int total_counts[3] = {0, 0, 0};
    int valid_file_count = 0;
    bool invalid_argument = false;

    while (arg_index < argc && argv[arg_index][0] == '-') {
        char *option = argv[arg_index];

        if (strlen(option) == 1) {
            print_usage("invalid argument");
            return 1;
        }

        for (int i = 1; option[i] != '\0'; i++) {
            switch (option[i]) {
                case 'l':
                    show[0] = 1;
                    no_flags = false;
                    break;
                case 'w':
                    show[1] = 1;
                    no_flags = false;
                    break;
                case 'c':
                    show[2] = 1;
                    no_flags = false;
                    break;
                default:
                    print_usage("invalid argument");
                    invalid_argument = true;
                    break;
            }
            if (invalid_argument) {
                break;
            }
        }

        if (invalid_argument) {
            return 1;
        }

        arg_index++;
    }

    if (no_flags) {
        show[0] = 1;
        show[1] = 1;
        show[2] = 1;
    }

    if (arg_index == argc) {
        int *counts = get_counts(NULL);
        if (counts) {
            print_counts(show, counts, "");
            free(counts);
        }
    } else {
        while (arg_index < argc) {
            char *filename = argv[arg_index];

            int *counts = get_counts(filename);
            if (counts) {
                if (counts[0] != -1) {
                    valid_file_count++;
                    total_counts[0] += counts[0];
                    total_counts[1] += counts[1];
                    total_counts[2] += counts[2];
                }
                print_counts(show, counts, filename);
                free(counts);
            }

            arg_index++;
        }

        if (valid_file_count > 1) {
            print_counts(show, total_counts, "total");
        }
    }

    return 0;
}
