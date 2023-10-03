#include <stdio.h>
#include <string.h>

#ifndef DELIM
// Define rows delimiter
// LF
//#define DELIM 10
// ,
#define DELIM 44
// Max count
#define MAX_C 42
#endif

typedef struct {
    int amount;
    int starts[MAX_C];
    int ends[MAX_C];
} match_res;

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
        // Database and input provided.
        input_db = argv[1];
    } else if (argc == 3) {
        input_db = argv[2];
        input_addr = argv[1];
    }
    match_res m_res = matchlocs(input_db, input_addr);
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
    int loci = 0;
    int reli = 0;
    int match = 0;
    int stored = 0;

    match_res res;
    
    for (int i = 0; i < strlen(db); i++) {
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