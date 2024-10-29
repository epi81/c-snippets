#include <stdio.h>
#include <regex.h>

int main(int argc, char **argv)
{
  struct {
    const char *input;
    int expect;
  } tests[] = {
    /* should match */
    { "salam TEST=25 bye", 1 },
    { "TEST=25 bye", 1 },
    { " TEST=25 bye", 1 },
    { "state: q0",  1 },
    { "state:q0s",  1 },

    /* should not match */
    { "#state :q0",  0 },
    { "state q0",    0 },
    { "# state :q0", 0 },
  };
  int i;
  regex_t start_state;
  // const char *pattern = "^[ \\t]*(state)[ \\t]*:.*$";
  // const char *pattern = "TEST[ \\t]*=[ \\t]*[0-9]+";
  const char *pattern = "TEST[ \\t]*=[ \\t]*[^ \\t\\n]+";

  if (regcomp(&start_state, pattern, REG_EXTENDED)) {
    fprintf(stderr, "%s: bad pattern: '%s'\n", argv[0], pattern);
    return 1;
  }

  for (i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
    regmatch_t r={.rm_so=0, .rm_eo=0};
    int status = regexec(&start_state, tests[i].input, 1, &r, 0);

    printf("%s: %s (%s) rm_so=%d rm_eo=%d\n", tests[i].input,
                            status == 0 ? "match" : "no match",
                            !status == !!tests[i].expect
                              ? "PASS" : "FAIL",
                            r.rm_so, r.rm_eo);
  }

  regfree(&start_state);

  return 0;
}
