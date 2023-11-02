#include "../include/hw5.h"

int handleError(int i) {
    int errors[8] = {MISSING_ARGUMENT, DUPLICATE_ARGUMENT, INPUT_FILE_MISSING, OUTPUT_FILE_UNWRITABLE, S_ARGUMENT_MISSING, R_ARGUMENT_MISSING, L_ARGUMENT_INVALID, WILDCARD_INVALID};
    char* s;
    int error = errors[i];
    if (error == MISSING_ARGUMENT) {
        s = "MISSING_ARGUMENT";
    } else if (error == DUPLICATE_ARGUMENT) {
        s = "DUPLICATE_ARGUMENT";
    } else if (error == INPUT_FILE_MISSING) {
        s = "INPUT_FILE_MISSING";
    } else if (error == OUTPUT_FILE_UNWRITABLE) {
        s = "OUTPUT_FILE_UNWRITABLE";
    } else if (error == S_ARGUMENT_MISSING) {
        s = "S_ARGUMENT_MISSING";
    } else if (error == R_ARGUMENT_MISSING) {
        s = "R_ARGUMENT_MISSING";
    } else if (error == L_ARGUMENT_INVALID) {
        s = "L_ARGUMENT_INVALID";
    } else {
        s = "WILDCARD_INVALID";
    }
    fprintf(stderr, "Error: %s\n",s);
    return error;
}
int findBeginning(char* tmp, char* pos) {
    int count = 0; // Initialize a count to track the number of characters moved
    while (pos > tmp && !isspace(*(pos - 1)) && !ispunct(*(pos - 1))) {
        pos--; // Move the pointer backward
        count++; // Increment the count for each character moved
    }
    return count;
}
bool checkIsEnding(char* pos, char* word) {
    pos += strlen(word) - 1;
    if (*(pos+1) != '\0') {
        printf("\ncheckIsEnding:\n");
        printf("pos=%c\n", *pos);
        printf("pos+1=%c\n\n", *(pos+1));
        return isspace(*(pos+1)) || ispunct(*(pos+1));
    } else {
        return true;
    }
}
// char* set_text(char* text, char* argv[], int i, int argc) {
//     int count = 0;
//     int k = 0;
//     if (argv[i][0] == '"') {
//         while (i + k < argc) {
//             const char* current_arg = argv[i + k];
//             if (current_arg[strlen(current_arg) - 1] == '"') {
//                 // Found the closing double quote
//                 count = k + 1;
//                 break;
//             }
//             k++;
//         }
//         printf("count: %d\n", count);
//         int len = strlen(argv[i]);
//         if (len > 0) {
//             for (int p = 0; p < len; p++) {
//                 argv[i][p] = argv[i][p+1];
//             }
//         }
//         int newLength = 0;
//         for (int j = 0; j < count; j++) {
//             if (j != count - 1) {
//                 newLength += 1;
//             }
//             newLength += strlen((argv[i + j]));
//         }
//         newLength -= 1;
//         printf("newLength: %d\n", newLength);
//         char* new_text = (char*)malloc(newLength);
//         new_text[0] = '\0';
//         for (int j = 0; j < count; j++) {
//             strcat(new_text, (argv[i + j]));
//             if (j != count - 1) {
//                 strcat(new_text, " ");
//             } else {
//                 new_text[strlen(new_text)-1] = '\0';
//                 printf("new_text length: %lu\n", strlen(new_text));
//             }
//         }
//         text = new_text;
//     } else {
//         text = argv[i];
//     }
//     return text;
// }
bool checkIsBeginning(char* tmp, char* pos) {
        if (pos - 1 >= tmp) {
            printf("\ncheckIsBeginning:\n");
            printf("pos=%c\n", *pos);
            printf("pos-1=%c\n\n", *(pos-1));
            return isspace(*(pos-1)) || ispunct(*(pos-1));
        } else {
            return true;
        }
}
// Have an error where each index is an error and return the error at the very end
int main(int argc, char *argv[]) {
    bool isWildcard = false;
    bool hasLine = false;
    bool priorityError[9] = {false};
    int numS = 0, numR = 0, numL = 0, numW = 0;
    if (argc < 7) {
        priorityError[0] = true;
    }
    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }
    char* search_text = NULL, *replace_text = NULL;
    int start_line = 0, end_line = 0;
    for (int i = 0; i < argc; i++) {
        if ((numS > 1) || (numR > 1) || (numL > 1) || (numW > 1)) {
            priorityError[1] = true;
        } else if (strcmp(argv[i], "-s") == 0) {
            numS++;
            if ((i + 1 < argc) && (strncmp(argv[i + 1], "-", 1) != 0)) {
                search_text = argv[i+1];
            } else {
                priorityError[4] = true;
            }
        } else if (strcmp(argv[i], "-r") == 0){
            numR++;
            if ((i + 1 < argc) && (strncmp(argv[i + 1], "-", 1) != 0)) {
                replace_text = argv[i+1];
            } else {
                priorityError[5] = true;
            }
        } else if (strcmp(argv[i], "-l") == 0) {
            numL++;
            hasLine = true;
            if ((i + 1 < argc) && (strncmp(argv[i + 1], "-", 1) != 0)) {
                const char s = ',';
                char* startStr = strtok(argv[i+1], &s);
                if (startStr == NULL) {
                    priorityError[6] = true;
                    continue;
                }
                char* endStr = strtok(NULL, &s);
                if (endStr == NULL) {
                    priorityError[6] = true;
                    continue;
                }
                start_line = atoi(startStr);
                end_line = atoi(endStr);
                printf("start_line= %d\n", start_line);
                printf("end_line= %d\n", end_line);
                if (end_line < start_line) {
                    priorityError[6] = true;
                } 
            } else {
                priorityError[6] = true;
            }
        } else if (strcmp(argv[i], "-w") == 0) {
            numW++;
            isWildcard = true;
        }
    }
    if (numS == 0) {
        priorityError[4] = true;
    } else if (numR == 0) {
        priorityError[5] = true;
    }
    bool beginWith = false, endWith = false;
    if ((isWildcard) && (!priorityError[4])) {
        if ((strncmp(search_text, "*", 1) == 0) ^ (strncmp(search_text + strlen(search_text) - 1, "*", 1) == 0)) {
            int len = strlen(search_text);
            if (strncmp(search_text, "*", 1) == 0) {
                endWith = true;
                if (len > 0) {
                    for (int i = 0; i < len; i++) {
                        search_text[i] = search_text[i+1];
                    }
                    search_text[len - 1] = '\0';
                }
            } else {
                beginWith = true;
                search_text[len - 1] = '\0';
            }
        } else {
            priorityError[7] = true;
        }
    }
    fprintf(stderr, "search_text: %s\n", search_text);
    fprintf(stderr, "replace_text: %s\n", replace_text);
    //open infile for reading if possible
    const char* inputf = argv[argc-2];
    const char* outputf = argv[argc-1];
    fprintf(stderr,"inputFile: %s\n", inputf);
    fprintf(stderr,"outputFile: %s\n", outputf);
    fprintf(stderr,"beginWith: %d\n", beginWith);
    fprintf(stderr,"endWith: %d\n", endWith);
    FILE* infile = fopen(argv[argc-2], "r");
    // Check for error: INPUT_FILE_MISSING 
    if (infile == NULL) {
        priorityError[2] = true;
    }
    // open outfile for writing if possible
    FILE* outfile = fopen(argv[argc-1], "w");
    // Check for error: OUTPUT_FILE_UNWRITABLE  
    if (outfile == NULL) {
        priorityError[3] = true;
    }

    for (int i = 0; i < 9; i++) {
        if (priorityError[i] == true) {
            return handleError(i);
        }
    }
    int curr_line = 1;
    char line[MAX_LINE];
    size_t search_len = strlen(search_text);
    while (fgets(line, sizeof(line), infile) != NULL) {
        char *pos, *tmp = line;
        if (curr_line >= start_line && curr_line <= end_line || !hasLine) {
            while ((pos = strstr(tmp, search_text)) != NULL) {
                fprintf(stderr,"\ntmp from beginning: |%c|\n", *tmp);
                fprintf(stderr,"\npos from beginning: |%c|\n", *pos);
                size_t n = pos - tmp;
                char *current;
                int length = 0;
                bool isValid = true;
                if (beginWith || endWith) {
                    if (endWith && !checkIsEnding(pos, search_text)) {
                        n += search_len;
                        isValid = false;
                        fprintf(stderr, "\ntmp=%c\n", *tmp);
                        fprintf(stderr, "Not valid ending\n");
                    } else if (beginWith && !checkIsBeginning(tmp, pos)) {
                        n += search_len;
                        isValid = false;
                        fprintf(stderr, "\ntmp=%c\n", *tmp);
                        fprintf(stderr, "Not valid beginning\n");
                    }
                    else if (endWith && checkIsEnding(pos, search_text)) {
                        n -= findBeginning(tmp, pos);
                    }
                    fprintf(stderr, "Valid Wildcard\n");
                    fprintf(stderr, "tmp to n: |");
                    fwrite(tmp, 1, n, stderr);
                    fprintf(stderr, "|\n");
                    fwrite(tmp, 1, n, outfile);
                    current = pos;
                    fprintf(stderr, "\ncharacters when looking for length: ");
                    while (*current != '\0' && !isspace(*current) && !ispunct(*current)) {
                        fprintf(stderr,"%c", *current);
                        current++;
                        length++;
                    }
                    fprintf(stderr, "|\n");
                    fprintf(stderr, "length:%d\n", length);
                    if (isValid) {
                        fwrite(replace_text, 1, strlen(replace_text), outfile);
                        tmp = pos + length;
                    } else {
                        tmp = pos + search_len;
                    }
                } else {
                    fwrite(tmp, 1, n, outfile);
                    fwrite(replace_text, 1, strlen(replace_text), outfile);
                    tmp = pos + search_len;
                }
            }
            fwrite(tmp, 1, strlen(tmp), outfile);
        } else {
            fputs(line, outfile);
        }
        curr_line++;
    }
    
    fclose(infile);
    fclose(outfile);
    return 0;
}