/**
 * Basic functions to test C calling in Julia
 */

#include <stdio.h>
#include <stdlib.h>

static unsigned int cnt = 0;

typedef struct _bin_tree {
	float val;
	struct _bin_tree* lchild;
	struct _bin_tree* rchild;
} bin_tree;

bin_tree* new_tree(float v) {
	bin_tree* out = (bin_tree*) malloc(sizeof(bin_tree));
	out->val = v;
	out->lchild = 0;
	out->rchild = 0;
	return out;
}

void print_to_the_left(bin_tree* tree) {
	/* This is really just for some debugging nin Julia */
	unsigned int child_nb = 0;
	while (tree) {
		printf("Value number %i is %f\n", child_nb++, tree->val);
		tree = tree->lchild;
	}
}

void print_to_the_right(bin_tree* tree) {
	/* This is really just for some debugging nin Julia */
	unsigned int child_nb = 0;
	while (tree) {
		printf("Value number %i is %f\n", child_nb++, tree->val);
		tree = tree->rchild;
	}
}

int add_left(bin_tree* parent, bin_tree* l) {
	parent->lchild = l;
	return 0;
}

int add_right(bin_tree* parent, bin_tree* r) {
	parent->rchild = r;
	return 0;
}

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

