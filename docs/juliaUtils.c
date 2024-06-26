#include <string.h>
#include "cvtp.h"
#include "treeBuilder.h"
#include "tablesjulia.c"

int getOpRank(Operator* ops, char* opName) {
	int outval = -1;
	while(outval == -1){
		if( strcmp(opName, ops->name) == 0){
			outval = ops->rank;
			break;
		}
		ops++;
	}
	return outval;
}

TreeBuilder* newJuliaTreeBuilder(int dummy){
	return newTreeBuilder(juliaOperators);
}

Tree* startJuliaTree(TreeBuilder* tb, char* opName){
	return startTree(tb, getOpRank(juliaOperators, opName));
}
