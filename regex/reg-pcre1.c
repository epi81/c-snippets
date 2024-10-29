#include <stdio.h>
#include <string.h>
#include <pcre.h>

#define SUBJECT "a123x456ABC"
#define PATTERN "[a-z]?(\\d+)x(\\d*)([A-Z]*)"
#define REPLACEMENT ">>>\\1---\\2+++\\3"

int main() {
    const char *error;
    int erroffset;
    int ovector[30];
    pcre *re;
    int rc;

    re = pcre_compile(PATTERN, 0, &error, &erroffset, NULL);
    if (re == NULL) {
        printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
        return 1;
    }

    rc = pcre_exec(re, NULL, SUBJECT, strlen(SUBJECT), 0, 0, ovector, 30);
    if (rc < 0) {
        printf("No match found.\n");
        pcre_free(re);
        return 1;
    }

    char result[256];
    char temp[256];
    result[0] = '\0';

    const char *replace_ptr = REPLACEMENT;
    while (*replace_ptr != '\0') {
        if (*replace_ptr == '\\' && *(replace_ptr + 1) >= '1' && *(replace_ptr + 1) <= '9') {
            int group_num = *(replace_ptr + 1) - '0';  // Convert '1' or '2' to 1 or 2
            pcre_copy_substring(SUBJECT, ovector, rc, group_num, temp, sizeof(temp));
            strcat(result, temp);
            replace_ptr += 2;
        } else {
            strncat(result, replace_ptr, 1);
            replace_ptr++;
        }
    }

    printf("Result: %s\n", result);

    pcre_free(re);
    return 0;
}
