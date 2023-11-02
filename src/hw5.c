#include "../include/hw5.h"
/* This method is used to handle the errors based on their priority. */
int handleError(int i) {
    int errors[8] = {MISSING_ARGUMENT, DUPLICATE_ARGUMENT, INPUT_FILE_MISSING, OUTPUT_FILE_UNWRITABLE, S_ARGUMENT_MISSING, R_ARGUMENT_MISSING, L_ARGUMENT_INVALID, WILDCARD_INVALID};
    return errors[i];
}
/* This method is used to find the beginning of a word when the wildcard endsWith is found. */
int findBeginning(char* tmp, char* pos) {
    int count = 0; // Initialize a count to track the number of characters moved
    while (pos > tmp && !isspace(*(pos - 1)) && !ispunct(*(pos - 1))) {
        pos--;
        count++;
    }
    return count;
}
/* This method is used to check if the word that was found matches the wildcard endsWith */
bool checkIsEnding(char* pos, char* word) {
    pos += strlen(word) - 1;
    if (*(pos+1) != '\0') {
        return isspace(*(pos+1)) || ispunct(*(pos+1));
    } else {
        return true;
    }
}
/* This method is used to check if the word that was found matches the wildcard startsWith */
bool checkIsBeginning(char* tmp, char* pos) {
        if (pos - 1 >= tmp) {
            return isspace(*(pos-1)) || ispunct(*(pos-1));
        } else {
            return true;
        }
}
/* This method counts the number of asterisks in a string. */
int countAsterisks(char *str) {
    char* ptr = str;
    int count = 0;
    while (*ptr) {
        if (*ptr == '*') {
            count++;
        }
        ptr++;
    }
    return count;
}
/* Main method of the code */
int main(int argc, char *argv[]) {
    bool isWildcard = false;
    bool hasLine = false;
    bool priorityError[9] = {false};
    int numS = 0, numR = 0, numL = 0, numW = 0;
    /* Checks for missing arguments error. */
    if (argc < 7) {
        priorityError[0] = true;
    }

    char* search_text = NULL, *replace_text = NULL;
    int start_line = 0, end_line = 0;
    /* Loops through each argv string given by the user. */
    for (int i = 0; i < argc; i++) {
        /* Checks for duplicate argument error. */
        if ((numS > 1) || (numR > 1) || (numL > 1) || (numW > 1)) {
            priorityError[1] = true;
        /* agv= -s: store search_text and check if the s argument is missing */
        } else if (strcmp(argv[i], "-s") == 0) {
            numS++;
            if ((i + 1 < argc) && (strncmp(argv[i + 1], "-", 1) != 0)) {
                search_text = argv[i+1];
            } else {
                priorityError[4] = true;
            }
        /* agv= -r: store replace_text and check if the r argument is missing */
        } else if (strcmp(argv[i], "-r") == 0){
            numR++;
            if ((i + 1 < argc) && (strncmp(argv[i + 1], "-", 1) != 0)) {
                replace_text = argv[i+1];
            } else {
                priorityError[5] = true;
            }
        /* agv= -l: store start_line/end_line and check if the l argument is invalid */
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
                char *ptr;
                start_line = strtol(startStr, &ptr, 10);
                end_line = strtol(endStr, &ptr, 10);
                hasLine = true;
                if (end_line < start_line) {
                    priorityError[6] = true;
                } 
            } else {
                priorityError[6] = true;
            }
        /* agv= -w: the -s argument is a wildcard */
        } else if (strcmp(argv[i], "-w") == 0) {
            numW++;
            isWildcard = true;
        }
    }
    /* Checks for missing s arguement error. */
    if (numS == 0) {
        priorityError[4] = true;
    /* Checks for missing r arguement error. */
    } else if (numR == 0) {
        priorityError[5] = true;
    }
    /* This block is used to check if the wildcard is valid as well making changes to search_text respectively. */
    bool beginWith = false, endWith = false;
    if ((isWildcard) && (!priorityError[4])) {
        if ((strncmp(search_text, "*", 1) == 0) ^ (strncmp(search_text + strlen(search_text) - 1, "*", 1) == 0)) {
            int len = strlen(search_text);
            /* Store search_text without the begining '*' */
            if (strncmp(search_text, "*", 1) == 0) {
                endWith = true;
                if (len > 0) {
                    for (int i = 0; i < len; i++) {
                        search_text[i] = search_text[i+1];
                    }
                    search_text[len - 1] = '\0';
                }
            /* Store search_text without the ending '*' */
            } else {
                beginWith = true;
                search_text[len - 1] = '\0';
            }
        } else {
            priorityError[7] = true;
        }
    }
    /* Attempts to open the file to read and checks for input file missing. */
    FILE* infile = fopen(argv[argc-2], "r");
    if (infile == NULL) {
        priorityError[2] = true;
    }
     /* Attempts to open the file to write in and checks if it is unwritable. */
    FILE* outfile = fopen(argv[argc-1], "w"); 
    if (outfile == NULL) {
        priorityError[3] = true;
    }
    /* Calls the handleError method on the most prioritized index of the priorityError array. */
    for (int i = 0; i < 9; i++) {
        if (priorityError[i] == true) {
            return handleError(i);
        }
    }
    /* Search and replace looping done for both wildcards and simple searches respectively. */
    int curr_line = 1;
    char line[MAX_LINE];
    size_t search_len = strlen(search_text);
    /* While a line exists store the line as line. */
    while (fgets(line, sizeof(line), infile) != NULL) {
        char *pos, *tmp = line;
        /* Only replace in the appropriate range if one is given. */
        if ((curr_line >= start_line && curr_line <= end_line) || !hasLine) {
            /* pos = the first occurence of search_text in tmp */
            while ((pos = strstr(tmp, search_text)) != NULL) {
                size_t n = pos - tmp;
                int length = 0;
                bool isValid = true;
                /* Shift n based on the wildcard if the search_text found matches the correct wildcard. */
                if (beginWith || endWith) {
                    if ((endWith && !checkIsEnding(pos, search_text)) || (beginWith && !checkIsBeginning(tmp, pos))) {
                        n += search_len;
                        isValid = false;
                    } else if (endWith && checkIsEnding(pos, search_text)) {
                        n -= findBeginning(tmp, pos);
                    }
                    /* Write into file until point of the beginning of search_text. */
                    fwrite(tmp, 1, n, outfile);
                    if (isValid) {
                        char *curr_char = pos;
                        while (*curr_char != '\0' && !isspace(*curr_char) && !ispunct(*curr_char)) {
                            curr_char++;
                            length++;
                        }
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
            /* Write in the rest of the line into the file. */
            fwrite(tmp, 1, strlen(tmp), outfile);
        } else {
            fputs(line, outfile);
        }
        curr_line++;
    }
    /* Close both files. */
    fclose(infile);
    fclose(outfile);
    return 0;
}