#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef DELIM
// Define rows delimiter
// LF
#define DELIM 10
// ,
//#define DELIM 44
// Max count
#define MAX_C 48
#endif

typedef struct {
    int amount;
    int starts[MAX_C];
    int ends[MAX_C];
} match_res;

int strl(char* c) {
    return (int) strlen(c);
}

void read_db(char* str) {
    char c;
    int i = 0;
    do {
        c = getchar();
        str[i] = c;
        i++;
    } while (c != EOF);
    str[i] = '\0';
}

/**
 * Convert provided char array to upper case.
 * ** input: The input
*/
void strupper(char* input) {
    for (int i = 0; i < strl(input); i++) {
        int c = input[i];
        if (c >= 'a' && c <= 'z') {
            c = 'A' + (c - 'a');
        }
        input[i] = c;
    }
}

int strcontains(char* input, int c) {
    for (int i = 0; i < strl(input); i++) {
        if (input[i] == c) {
            return 1;
        }
    }
    return 0;
}

void put_word_starts(char* db, match_res* res) {
    res->starts[0] = 0;
    res->ends[0] = 0;
    int loci = 1;
    for (int i = 0; i < strl(db); i++) {
        char c = db[i];
        if (c == DELIM && i != strl(db) - 1) {
            res->starts[loci] = i + 1;
            res->ends[loci] = i + 1;
            loci++;
        }
    }
    res->amount = loci;
}

void put_word_matches(char* db, char* in, match_res* res) {
    // Current location (match) index
    int loci = 0;
    // Relative index in the current match
    int reli = 0;
    int match = 0;
    int stored = 0;
    
    for (int i = 0; i < strl(db); i++) {
        int c = db[i];
        if (i == 0 && c == in[0]) {
            // Start pos equality
            res->starts[0] = 0;
            match = 1;
            reli++;
        } else if (c == DELIM || i == strl(db) - 1) {
            // On delimiter break
            if (!stored && match) {
                // This substr is currently match and has not
                // been closed yet (probably exactly matches the input),
                // setting index of its last char as closing.
                res->ends[loci++] = i == strl(db) - 1 ? i : i - 1;
                if (strl(in) - 1 == res->ends[loci - 1] - res->starts[loci - 1]) {
                    // Currently, we found exact match. Breaking the loop.
                    // We set this as the only match.
                    res->starts[0] = res->starts[loci - 1];
                    res->ends[0] = res->ends[loci - 1];
                    loci = 1;
                    break;
                }
            }
            match = 0;
            if (c == DELIM && db[i + 1] == in[0]) {
                // The next char matches inputs first char,
                // marking it as starting point of current substring.
                // (matches at least partially)
                res->starts[db[0] == in[0] ? loci + 1 : loci] = i + 1;
                match = 1;
            }
            stored = 0;
            reli = 0;
        } else {
            // Any other symbol
            if (
                ((strl(in) > reli && c != in[reli]) || (strl(in) <= reli))
                && !stored && match) {
                if (reli == strl(in)) {
                    // Symbols dont match, setting
                    // end index
                    res->ends[loci++] = i - 1;
                    stored = 1;
                } else {
                    // Symbols dont match, but in also is not
                    // prefix, skipping.
                    match = 0;
                }
            }
            reli++;
        }
    }
    res->amount = loci;
}

/**
 * Scans the provided db string with provided input and
 * returns all matches according to project specification.
 * 
 * ** db: The database string separated with newline symbols
 * ** in The requested input string
 * 
 * returns: The match result
*/
match_res match(char* db, char* in) {
    match_res res;
    if (strl(in) == 0) {
        put_word_starts(db, &res);
    } else {
        put_word_matches(db, in, &res);
    }
    while (1) {
        if (res.amount != 1) {
            break;
        }
        // Only one match was found, extending
        // the bounds to whole word.
        if (res.ends[0] == strl(db) - 1) {
            break;
        } else if (db[res.ends[0] + 1] == DELIM) {
            break;
        }
        res.ends[0]++;
    }
    return res;
}

void sort_alphabetically(int chars[], int size) {
    int i = 0;
    while (i < size - 1) {
        if (chars[i] > chars[i+1]) {
            int temp = chars[i];
            chars[i] = chars[i+1];
            chars[i+1] = temp;
            i = 0;
            continue;
        }
        i++;
    }
}


/**
 * Entrypoint.
 * ** argc
 * ** argv
*/
int main(int argc, char* argv[]) {
    //char* input_db = "";
    char* input_addr = "";
    if (argc == 2) {
        input_addr = argv[1];
    }
    char input_db[(100 * 46) * 8];
    read_db(&input_db[0]);
    if (input_db[0] == DELIM || input_db[strl(input_db) - 1] == DELIM) {
        printf("Input can't start or end with %c!", DELIM);
        return 0;
    }
    // Convert the inputs to upper case (case insensitive)
    strupper(input_db);
    strupper(input_addr);
    match_res m_res = match(input_db, input_addr);
    // Match count
    int m_count = m_res.amount;
    // Match start indexes
    int* starts = m_res.starts;
    // Match end indexes
    int* ends = m_res.ends;

    if (m_count == 0) {
        printf("Not found");
    } else if (m_count == 1) {
        printf("Found: ");
        int i = starts[0];
        while (i <= ends[0]) {
            printf("%c", input_db[i++]);
        }
    } else {
        printf("Enable: ");
        int chars[m_count];
        int c_i = 0;
        for (int i = 0; i < m_count; i++) {
            int c = input_db[
                strl(input_addr) == 0 ? ends[i] : ends[i] + 1
            ];
            int duplicate = 0;
            for (int i2 = 0; i2 < i; i2++) {
                // Check for duplicates (Show only first occurencies)
                if (chars[i2] == c) {
                    duplicate = 1;
                }
            }
            if (!duplicate) {
                chars[c_i] = c;
                c_i++;
            }
        }
        sort_alphabetically(chars, c_i);
        for (int i = 0; i < c_i; i++) {
            printf("%c", chars[i]);
        }
    }
    printf("\n");
}