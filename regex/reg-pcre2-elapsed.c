#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define PCRE2_CODE_UNIT_WIDTH 8  // Define this before including pcre2.h
#include <pcre2.h>

#define NUM_PATTERNS 100        // Number of patterns
#define NUM_SUBJECTS 100000     // Number of subjects
#define PATTERN_TEMPLATE "[a-z]?(\\d+)x(\\d*)" // Base pattern template
#define REPLACEMENT "---$1+++$2==="      // Replacement to reorder matches

int main() {
    // Array of pattern strings (up to 100)
    char patterns[NUM_PATTERNS][50];
    // Create different patterns (modify each with a number, e.g., adding suffix to base template)
    for (int i = 0; i < NUM_PATTERNS; i++) {
        snprintf(patterns[i], sizeof(patterns[i]), PATTERN_TEMPLATE);
    }

    // Array of precompiled PCRE2 pattern objects
    pcre2_code *re_patterns[NUM_PATTERNS];
    int error;
    long unsigned int erroffset;

    /* compile */{
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        // Compile each pattern once and store the compiled objects
        for (int i = 0; i < NUM_PATTERNS; i++) {
            re_patterns[i] = pcre2_compile((PCRE2_SPTR)patterns[i], PCRE2_ZERO_TERMINATED, 0, &error, &erroffset, NULL);
            if (re_patterns[i] == NULL) {
                PCRE2_UCHAR error_message[256];
                pcre2_get_error_message(erroffset, (PCRE2_UCHAR *)error_message, sizeof(error_message));
                printf("Pattern compilation failed for pattern %d at offset %ld: %s\n", i, erroffset, error_message);
                return 1;
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        long elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000L + (end.tv_nsec - start.tv_nsec);
        printf("Compile elapsed time: %ld ns\n", elapsed_ns);
    }

    // Array of dynamically generated subjects
    char subjects[NUM_SUBJECTS][50];
    // Generate subjects like "a123x456", "b123x456", ..., "z123x456"
    for (int i = 0; i < NUM_SUBJECTS; i++) {
        snprintf(subjects[i], sizeof(subjects[i]), "%c123%c456", 'a' + (i % 26), 
            // 'a' + (i % 26)
            'x'
            );
    }

    // Replacement string for PCRE2 substitution
    PCRE2_SPTR replacement = (PCRE2_SPTR)REPLACEMENT;

    long total_operations = 0;
    long total_elapsed_ns = 0;

    // Loop through each pattern and each subject to perform substitution
    for (int s = 0; s < NUM_SUBJECTS; s++) {
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int p = 0; p < NUM_PATTERNS; p++) {
            PCRE2_SIZE subject_length = (PCRE2_SIZE)strlen(subjects[s]);
            PCRE2_UCHAR output_buffer[256];
            PCRE2_SIZE output_length = sizeof(output_buffer) - 1;

            // Create match data for storing the results of the match
            pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re_patterns[p], NULL);

            // Perform the substitution
            int rc = pcre2_substitute(
                re_patterns[p],
                (PCRE2_SPTR)subjects[s],
                subject_length,
                0,
                PCRE2_SUBSTITUTE_GLOBAL,
                match_data,
                NULL,
                replacement,
                PCRE2_ZERO_TERMINATED,
                output_buffer,
                &output_length
            );

            if (rc >= 0) {
                output_buffer[output_length] = '\0'; // Null-terminate the result
                // printf("Pattern %d, Subject %d: %s -> %s\n", p, s, subjects[s], output_buffer);
            } else {
                printf("Substitution failed for pattern %d, subject %d.\n", p, s);
            }

            // Clean up match data after each substitution
            pcre2_match_data_free(match_data);
        }
        total_operations++;
        clock_gettime(CLOCK_MONOTONIC, &end);
        long elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000L + (end.tv_nsec - start.tv_nsec);
        total_elapsed_ns += elapsed_ns;
        // printf("Single regex elapsed time: %ld ns\n", elapsed_ns);
    }

    double total_elapsed_seconds = total_elapsed_ns / 1e9;
    double operations_per_second = total_operations / total_elapsed_seconds;

    printf("Total operations: %ld\n", total_operations);
    printf("Total time: %.6f seconds\n", total_elapsed_seconds);
    printf("Operations per second: %.2f\n", operations_per_second);

    // Free compiled patterns
    for (int i = 0; i < NUM_PATTERNS; i++) {
        pcre2_code_free(re_patterns[i]);
    }

    printf("All substitutions completed.\n");
    return 0;
}
