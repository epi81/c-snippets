#include <stdio.h>
#include <string.h>
#define PCRE2_CODE_UNIT_WIDTH 8  // Define this before including pcre2.h
#include <pcre2.h>

#define SUBJECT "a123x456"
#define PATTERN "[a-z]?(\\d+)x(\\d*)"  // Pattern to match and capture two groups
#define REPLACEMENT "$1$2"           // Replacement to reorder matches

int main() {
    PCRE2_SPTR pattern = (PCRE2_SPTR)PATTERN;
    PCRE2_SPTR subject = (PCRE2_SPTR)SUBJECT;
    PCRE2_SPTR replacement = (PCRE2_SPTR)REPLACEMENT;  // Use $1 and $2 for PCRE2 backreferences
    PCRE2_SIZE subject_length = (PCRE2_SIZE)strlen((char *)subject);

    int error_code;
    PCRE2_SIZE error_offset;
    pcre2_code *re;
    pcre2_match_data *match_data;

    // Compile the regular expression pattern
    re = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, 0, &error_code, &error_offset, NULL);
    if (re == NULL) {
        PCRE2_UCHAR error_message[256];
        pcre2_get_error_message(error_code, error_message, sizeof(error_message));
        printf("PCRE2 compilation failed at offset %d: %s\n", (int)error_offset, error_message);
        return 1;
    }

    // Create match data for storing the results of the match
    match_data = pcre2_match_data_create_from_pattern(re, NULL);

    // Define buffer to hold the result of the substitution
    PCRE2_UCHAR output_buffer[256];
    PCRE2_SIZE output_length = sizeof(output_buffer) - 1;

    // Perform the substitution, reordering as specified
    int rc = pcre2_substitute(
        re,
        subject,
        subject_length,
        0,
        PCRE2_SUBSTITUTE_GLOBAL,   // Global substitution flag
        match_data,
        NULL,                      // No substitute context needed
        replacement,
        PCRE2_ZERO_TERMINATED,
        output_buffer,
        &output_length
    );

    if (rc >= 0) {
        output_buffer[output_length] = '\0';  // Null-terminate the result
        printf("Result: %s\n", output_buffer);
    } else {
        printf("Substitution failed.\n");
    }

    // Clean up
    pcre2_code_free(re);
    pcre2_match_data_free(match_data);

    return 0;
}
