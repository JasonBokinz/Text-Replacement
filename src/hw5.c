#include "../include/hw5.h"

int handleError(int i) {
    int errors[8] = {MISSING_ARGUMENT, DUPLICATE_ARGUMENT, INPUT_FILE_MISSING, OUTPUT_FILE_UNWRITABLE, S_ARGUMENT_MISSING, R_ARGUMENT_MISSING, L_ARGUMENT_INVALID, WILDCARD_INVALID};
    return errors[i];
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
        return isspace(*(pos+1)) || ispunct(*(pos+1));
    } else {
        return true;
    }
}
bool checkIsBeginning(char* tmp, char* pos) {
        if (pos - 1 >= tmp) {
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
                hasLine = true;
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
    //open infile for reading if possible
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
        if ((curr_line >= start_line && curr_line <= end_line) || !hasLine) {
            while ((pos = strstr(tmp, search_text)) != NULL) {
                size_t n = pos - tmp;
                char *current;
                int length = 0;
                bool isValid = true;
                if (beginWith || endWith) {
                    if (endWith && !checkIsEnding(pos, search_text)) {
                        n += search_len;
                        isValid = false;
                    } else if (beginWith && !checkIsBeginning(tmp, pos)) {
                        n += search_len;
                        isValid = false;
                    }
                    else if (endWith && checkIsEnding(pos, search_text)) {
                        n -= findBeginning(tmp, pos);
                    }
                    fwrite(tmp, 1, n, outfile);
                    current = pos;
                    while (*current != '\0' && !isspace(*current) && !ispunct(*current)) {
                        current++;
                        length++;
                    }
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