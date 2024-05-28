/**
 * Basic functions to test C calling in Julia
 */

#include <stdio.h>

static unsigned int cnt = 0;

void calling_c() {
	cnt++;
}

int big_update(unsigned int v) {
	cnt += v;
	return 0;
}

unsigned int returncnt() {
	return cnt;
}

int returnbyref(int *v){
	*v = cnt;
	return 0;
}

int print_in_c() {
	printf("\t\t***** cnt is %i\n", cnt);
	return 0;
}

