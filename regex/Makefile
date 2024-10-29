all:
	gcc -DINFO_MODE reg-pcre2-elapsed.c -g -o x-reg-pcre2-elapsed -l pcre2-8
	gcc -DINFO_MODE reg-pcre2.c -g -o x-reg-pcre2 -l pcre2-8
	gcc -DINFO_MODE reg-pcre1.c -g -o x-reg-pcre1 -l pcre
	gcc reg.c x-reg

