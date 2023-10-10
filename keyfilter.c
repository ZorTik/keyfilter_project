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
#define MAX_C 42
#endif

typedef unsigned __int64 str_i;

typedef struct {
    int amount;
    str_i starts[MAX_C];
    str_i ends[MAX_C];
} match_res;

void read_db(char* str);
void strupper(char* input);
int strcontains(char* input, int c);
match_res matchlocs(char* db, char* in);


/**
 * Entrypoint.
 * ** argc
 * ** argv
*/
int main(int argc, char* argv[]) {
    char* input_db = "";
    char* input_addr = "";
    if (argc == 2) {
        input_addr = argv[1];
    }
    char input_db[(100 * 43) * 8];
    read_db(&input_db);
    if (input_db[0] == DELIM || input_db[strlen(input_db) - 1] == DELIM) {
        printf("Input can't start or end with %c!", DELIM);
        return 0;
    }
    if (strcontains(input_addr, DELIM)) {
        printf("Input can't contain %c", DELIM);
        return 0;
    }
    // Convert the inputs to upper case (case insensitive)
    strupper(input_db);
    strupper(input_addr);
    match_res m_res = matchlocs(input_db, input_addr);
    // Match count
    int m_count = m_res.amount;
    // Match start indexes
    str_i* starts = m_res.starts;
    // Match end indexes
    str_i* ends = m_res.ends;

    if (m_count == 0) {
        printf("Not found");
    } else if (m_count == 1) {
        printf("Found: ");
        str_i i = starts[0];
        while (i <= ends[0]) {
            printf("%c", input_db[i++]);
        }
    } else {
        printf("Enable: ");
        int chars[m_count];
        for (int i = 0; i < m_count; i++) {
            chars[i] = input_db[ends[i] + 1];
            int duplicate = 0;
            for (int i2 = 0; i2 < i; i2++) {
                // Check for duplicates (Show only first occurencies)
                if (chars[i2] == chars[i]) {
                    duplicate = 1;
                }
            }
            if (!duplicate) {
                printf("%c", chars[i]);
            }
        }
    }
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
    for (str_i i = 0; i < strlen(input); i++) {
        int c = input[i];
        if (c >= 'a' && c <= 'z') {
            c = 'A' + (c - 'a');
        }
        input[i] = c;
    }
}

int strcontains(char* input, int c) {
    for (str_i i = 0; i < strlen(input); i++) {
        if (input[i] == c) {
            return 1;
        }
    }
    return 0;
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
match_res matchlocs(char* db, char* in) {
    // Current location (match) index
    int loci = 0;
    // Relative index in the current match
    str_i reli = 0;
    int match = 0;
    int stored = 0;

    match_res res;
    
    for (str_i i = 0; i < strlen(db); i++) {
        int c = db[i];
        if (i == 0 && c == in[0]) {
            // Start pos equality
            res.starts[0] = 0;
            match = 1;
            reli++;
        } else if (c == DELIM || i == strlen(db) - 1) {
            // On delimiter break
            if (!stored && match) {
                // This substr is currently match and has not
                // been closed yet (probably exactly matches the input),
                // setting index of its last char as closing.
                res.ends[loci++] = i == strlen(db) - 1 ? i : i - 1;
                if (strlen(in) - 1 == res.ends[loci - 1] - res.starts[loci - 1]) {
                    // Currently, we found exact match. Breaking the loop.
                    // We set this as the only match.
                    res.starts[0] = res.starts[loci - 1];
                    res.ends[0] = res.ends[loci - 1];
                    loci = 1;
                    break;
                }
            }
            match = 0;
            if (c == DELIM && db[i + 1] == in[0]) {
                // The next char matches inputs first char,
                // marking it as starting point of current substring.
                // (matches at least partially)
                res.starts[db[0] == in[0] ? loci + 1 : loci] = i + 1;
                match = 1;
            }
            stored = 0;
            reli = 0;
        } else {
            // Any other symbol
            if (
                ((strlen(in) > reli && c != in[reli]) || (strlen(in) <= reli))
                && !stored && match) {
                if (reli == strlen(in)) {
                    // Symbols dont match, setting
                    // end index
                    res.ends[loci++] = i - 1;
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
    res.amount = loci;
    while (1) {
        if (res.amount != 1) {
            break;
        }
        // Only one match was found, extending
        // the bounds to whole word.
        if (res.ends[0] == strlen(db) - 1) {
            break;
        } else if (db[res.ends[0] + 1] == DELIM) {
            break;
        }
        res.ends[0]++;
    }
    return res;
}