/* static const char rcsid[]="$Id: treeBuilder.c 5417 2014-10-22 09:00:54Z llh $"; */

#include "treeBuilder.h"
#include <stdlib.h>
#include <string.h>

#define cnew(TYPE) (TYPE*)malloc(sizeof(TYPE))

void showFrontierRec(TreeGrowthFrontier *frontier) ;

/* Convention on the arity (remainingTrees) in the TreeGrowthFrontier's:
   -- arity >= 0 : normal, fixed, arity. The TreeGrowthFrontier contains a **ListTree
   -- arity = 0  : special case of above: There is no new tree needed in the **ListTree
   -- arity = -1 : list arity. The TreeGrowthFrontier contains a **ListTree, that
                  always accepts new trees, until a terminateListTree() is done.
   -- arity = -2 : single Tree mode: The TreeGrowthFrontier contains a single **Tree
*/


/* Creation and initialization: */
TreeGrowthFrontier *newTreeGrowthFrontier(int remainingTrees, ListTree** place, TreeGrowthFrontier *next) {
  TreeGrowthFrontier *result = cnew(TreeGrowthFrontier) ;
/*   result->memoOfTree = NULL ; */
  result->remainingTrees = remainingTrees ;
  result->place.toListTree = place ;
  result->next = next ;
  return(result) ;
} 

/** Pop the top element of frontier */
TreeGrowthFrontier *popFrontier(TreeGrowthFrontier *frontier) {
    TreeGrowthFrontier *result = frontier->next ;
    free(frontier) ;
    return(result) ;
}

/** Pop frontiers until finding a place waiting for a tree: */
TreeGrowthFrontier *popFinishedFrontiers(TreeBuilder *treeBuilder) {
  if (!treeBuilder->lastAtomTree) {
    while (treeBuilder->frontier
           &&
           (treeBuilder->frontier->remainingTrees==0
            ||
            (treeBuilder->frontier->remainingTrees==-2 && *(treeBuilder->frontier->place.toTree)!=NULL))) {
      if (treeBuilder->frontier->remainingTrees==-2) treeBuilder->acceptAnnotations = 1 ;
      treeBuilder->frontier = popFrontier(treeBuilder->frontier) ;
    }
  }
  return treeBuilder->frontier ;
}

TreeGrowthFrontier *newSingleTreeGrowthFrontier(Tree** place, TreeGrowthFrontier *next) {
  TreeGrowthFrontier *result = cnew(TreeGrowthFrontier) ;
/*   result->memoOfTree = NULL ; */
  result->remainingTrees = -2 ;
  result->place.toTree = place ;
  result->next = next ;
  return(result) ;
}

/* Creates a builder for trees in the language defined by "langOp": */
TreeBuilder *newTreeBuilder(Operator langOp[]) {
  TreeBuilder *result = cnew(TreeBuilder) ;
  result->langOp = langOp ;
  result->topListTree = NULL ;
  result->frontier = newTreeGrowthFrontier(-1,&(result->topListTree),NULL) ;
  result->lastAtomTree = NULL ;
  result->pendingAnnotations = NULL ;
  result->acceptAnnotations = 1 ;
  return result ;
}

/* Getting the tree built after all protocol has been processed: */
Tree *getTreeBuilt(TreeBuilder *treeBuilder) {
  treeBuilder->lastAtomTree = NULL ;
  popFinishedFrontiers(treeBuilder) ;
  /* Hoping we are at top, pop the initial, topmost "-1" list frontier: */
  treeBuilder->frontier = popFrontier(treeBuilder->frontier) ;
  if (treeBuilder->frontier) printf(" **** treeBuilder error: getTreeBuilt() returned an unfinished tree!\n") ;
  return treeBuilder->topListTree->tree ;
}

/* Getting the ListTree built after all protocol has been processed: */
ListTree *getListTreeBuilt(TreeBuilder *treeBuilder) {
  treeBuilder->lastAtomTree = NULL ;
  popFinishedFrontiers(treeBuilder) ;
  /* Hoping we are at top, pop the initial, topmost "-1" list frontier: */
  treeBuilder->frontier = popFrontier(treeBuilder->frontier) ;
  if (treeBuilder->frontier) printf(" **** treeBuilder error: getListTreeBuilt() returned an unfinished last tree!\n") ;
  return treeBuilder->topListTree ;
}

/* resets the TreeBuilder to its initial, empty state: */
void resetTreeBuilder(TreeBuilder *treeBuilder) {
  treeBuilder->topListTree = NULL ;
  treeBuilder->frontier =
    newTreeGrowthFrontier(-1,&(treeBuilder->topListTree),NULL) ;
  treeBuilder->lastAtomTree = NULL ;
  treeBuilder->pendingAnnotations = NULL ;
  treeBuilder->acceptAnnotations = 1 ;
}

/* deletes the TreeBuilder and recovers memory: */
void deleteTreeBuilder(TreeBuilder *treeBuilder) {
  ListTree *tmpListTree ;
  while (treeBuilder->topListTree) {
    tmpListTree = treeBuilder->topListTree->next ;
    free(treeBuilder->topListTree) ;
    treeBuilder->topListTree = tmpListTree ;
  }
  treeBuilder->lastAtomTree = NULL ;
  popFinishedFrontiers(treeBuilder) ;
  free(treeBuilder) ;
}

/** Forces the most recently created Tree (tree) to assume list arity.
 * This trick is needed because of the visitor mechanism of Clang
 * and because of the fact that arities in Clang are not well fixed (e.g. 4, sometimes 5!). */
void turnListFrontier(TreeBuilder *treeBuilder, Tree *tree) {
  if (treeBuilder->frontier->place.toListTree!=&(tree->contents.sons))
    printf(" **** treeBuilder error: too late to turn frontier into a list!\n") ;
  treeBuilder->frontier->remainingTrees = -1 ;
}

/** Common utility for putTree() and startOperTree() */
void putFinishedOrUnfinishedTree(TreeBuilder *treeBuilder, Tree *newTree) {
  /* Attach pending (i.e. waiting) anotations to the new Tree: */
  if (treeBuilder->acceptAnnotations) {
      newTree->annotations = treeBuilder->pendingAnnotations ;
      treeBuilder->pendingAnnotations = NULL ;
  }
  treeBuilder->lastAtomTree = NULL ;
  TreeGrowthFrontier *frontier = popFinishedFrontiers(treeBuilder) ;
  if (!frontier) printf(" **** treeBuilder error: new tree coming while tree is terminated!\n") ;
  /* Insert new Tree into next awaiting frontier place, and update the frontier: */
  if (frontier->remainingTrees == -2) {
    if (*(frontier->place.toTree)==NULL) {
      *(frontier->place.toTree) = newTree ;
    } else {
      frontier = popFrontier(frontier) ;
    }
  } else {
      *(frontier->place.toListTree) = mkSons(newTree,NULL) ;
      frontier->place.toListTree = &((*(frontier->place.toListTree))->next) ;
      if (frontier->remainingTrees > 0) frontier->remainingTrees-- ;
  }
  treeBuilder->frontier = frontier ;
}

Tree *startOperTree(TreeBuilder *treeBuilder, Operator *oper) {
/* printf("BEFORE startOperTree %s\n", oper->name); */
/* showTreeBuilderState(treeBuilder) ; */
    Tree *newTree = mkTree(oper, NULL) ;
    popFinishedFrontiers(treeBuilder) ;
    putFinishedOrUnfinishedTree(treeBuilder, newTree) ;
    /* Push a new entry on top of the frontier, except if arity==0: */
    if (oper->arity != 0) {
	treeBuilder->frontier =
	    newTreeGrowthFrontier(oper->arity, &(newTree->contents.sons),
				  treeBuilder->frontier) ;
    } else {
      treeBuilder->lastAtomTree = newTree ;
      popFinishedFrontiers(treeBuilder) ;
    }
/* printf("AFTER startOperTree\n"); */
/* showTreeBuilderState(treeBuilder) ; */
    return newTree ;
}

/** Insert newTree (which must be a finished tree) into the next available slot */
void putTree(TreeBuilder *treeBuilder, Tree *newTree) {
/* printf("BEFORE putTree\n"); */
/* showTreeBuilderState(treeBuilder) ; */
  putFinishedOrUnfinishedTree(treeBuilder, newTree) ;
  popFinishedFrontiers(treeBuilder) ;
/* printf("AFTER putTree\n"); */
/* showTreeBuilderState(treeBuilder) ; */
}

/** Tell the tree builder that the next available slot in the frontier has been filled
 * by someone else, so the tree builder frontier must be updated accordingly */
void oneLessWaiting(TreeBuilder *treeBuilder) {
  TreeGrowthFrontier *frontier = popFinishedFrontiers(treeBuilder) ;
  if (frontier->remainingTrees == -2) {
    frontier = popFrontier(frontier) ;
  } else {
    frontier->place.toListTree = &((*(frontier->place.toListTree))->next) ;
    if (frontier->remainingTrees > 0) frontier->remainingTrees-- ;
  }
  treeBuilder->frontier = frontier ;
}

/* next element coming through the protocol is a tree operator: */
Tree *startTree(TreeBuilder *treeBuilder, int operRk) {
  Operator *oper = &(treeBuilder->langOp[operRk]) ;
  return startOperTree(treeBuilder, oper) ;
}

/* next element coming through the protocol is a Tree,
 * to be put as annotation of the next Tree: */
void startAnnotation(TreeBuilder *treeBuilder, const char *key) {
    KeyListTree *newAnnotation = cnew(KeyListTree) ;
    newAnnotation->key = key ;
    newAnnotation->tree = NULL ;
    newAnnotation->next = NULL ;
    treeBuilder->acceptAnnotations = 0 ;
    if (treeBuilder->pendingAnnotations) {
	KeyListTree *tailAnnotations = treeBuilder->pendingAnnotations ;
	while (tailAnnotations->next) tailAnnotations=tailAnnotations->next ;
	tailAnnotations->next = newAnnotation ;
    } else
	treeBuilder->pendingAnnotations = newAnnotation ;
    /* insert &(newAnnotation->tree) in front of the frontier: */
    treeBuilder->frontier =
	newSingleTreeGrowthFrontier(&(newAnnotation->tree), treeBuilder->frontier) ;
}

/* next element coming through the protocol is an endOfList marker: */
void terminateListTree(TreeBuilder *treeBuilder) {
/* printf("BEFORE terminateListTree\n"); */
/* showTreeBuilderState(treeBuilder) ; */
  treeBuilder->lastAtomTree = NULL ;
  popFinishedFrontiers(treeBuilder) ;
  if ((!treeBuilder->frontier) || (treeBuilder->frontier->remainingTrees != -1)) {
      printf(" **** treeBuilder error: no list tree here to terminate!\n") ;
  } else
      treeBuilder->frontier = popFrontier(treeBuilder->frontier) ;
  popFinishedFrontiers(treeBuilder) ;
/* printf("AFTER terminateListTree\n"); */
/* showTreeBuilderState(treeBuilder) ; */
}

/* next element coming through the protocol is the value of previous atomTree: */
void putValue(TreeBuilder *treeBuilder, AtomValue value) {
/* printf("BEFORE putValue\n"); */
/* showTreeBuilderState(treeBuilder) ; */
  if (treeBuilder->lastAtomTree) {
    treeBuilder->lastAtomTree->contents.value =
      (AtomValue)strdup((const char *)value) ;
    treeBuilder->lastAtomTree = NULL ;
    popFinishedFrontiers(treeBuilder) ;
  } else
    printf(" **** treeBuilder error: no atom tree here to receive a value!\n") ;
/* printf("AFTER putValue\n"); */
/* showTreeBuilderState(treeBuilder) ; */
}

/* inserts all trees in newTrees into the next available slots of treeBuilder. */
void putListTree(TreeBuilder *treeBuilder, ListTree *newTrees) {
    while (newTrees) {
	putTree(treeBuilder, newTrees->tree) ;
	newTrees = newTrees->next ;
    }
}

/** start a tree with the given arity, but the operator doesn't matter. */
Tree *startDummyTree(TreeBuilder *treeBuilder, int arity) {
    Operator* oper = cnew(Operator) ;
    oper->name = "dummy" ;
    oper->arity = arity ;
    oper->rank = 999 ;
    return startOperTree(treeBuilder, oper) ;
}

/** Returns the address of the pointer variable that points to the ListTree whose head is the given tree */
ListTree** findTreeIn(Tree *tree, ListTree** toListTree) {
  ListTree** found = NULL ;
  while (!found && *toListTree!=NULL) {
    if (tree==(*toListTree)->tree) {
      found = toListTree ;
    } else if (treeNbSons((*toListTree)->tree)!=0) {
      found = findTreeIn(tree, &((*toListTree)->tree->contents.sons)) ;
    }
    toListTree = &((*toListTree)->next) ;
  }
  return found ;
}


/** remove given tree from the tree currently built inside the treeBuilder.
 * Assumptions (may be lifted if needed, but intricate coding ahead!):
 * -- the given tree is finished, i.e. it contains no holes,
 * -- treeBuilder doesn't point anywhere inside the given tree,
 * -- the parent of the given tree is of list arity
 * -- the given tree is not contained in single Tree mode (i.e. Tree held in an annotation)
 */
void removeTree(TreeBuilder *treeBuilder, Tree *tree) {
/* printf("BEFORE removeTree\n"); */
/* showTreeBuilderState(treeBuilder) ; */
  ListTree** toLocation = findTreeIn(tree, &(treeBuilder->topListTree)) ;
  if (toLocation!=NULL) {
    TreeGrowthFrontier* inFrontier = treeBuilder->frontier ;
    while (inFrontier!=NULL) {
      if (&((*toLocation)->next) == inFrontier->place.toListTree) {
        inFrontier->place.toListTree = toLocation ;
      }
      inFrontier = inFrontier->next ;
    }
    *toLocation = (*toLocation)->next ;
  }
/* printf("AFTER removeTree\n"); */
/* showTreeBuilderState(treeBuilder) ; */
}

// Utility procedures for debugging:

int *remainingTreesS = NULL ;
ListTree*** toListTreeS = NULL ;
Tree*** toTreeS = NULL ;
Tree *curLastAtomTree = NULL ;
KeyListTree *curPendingAnnotations = NULL ;

void showListTreeBeingBuilt(ListTree *listTree, int depth, int frontierDepth, int curArity) ;
void showAnnotationsBeingBuilt(KeyListTree *tlAnnotations, int depth, int frontierDepth) ;
void showTreeBeingBuilt(Tree *tree, int depth, int frontierDepth) ;
void showAnnotations(KeyListTree *tlAnnotations, int indent) ;

void showTreeBuilderState(TreeBuilder *treeBuilder) {
  TreeGrowthFrontier *topFrontier = treeBuilder->frontier ;
  /* Peel frontiers until finding a place waiting for a tree: */
  while (topFrontier && topFrontier->remainingTrees==0) topFrontier = topFrontier->next ;
  TreeGrowthFrontier *inFrontier = topFrontier ;
  int frontierDepth = 0 ;
  while (inFrontier) {
    ++frontierDepth ;
    inFrontier = inFrontier->next ;
  }
  remainingTreesS = (int *)malloc(frontierDepth*sizeof(int)) ;
  toListTreeS = (ListTree***)malloc(frontierDepth*sizeof(ListTree**)) ;
  toTreeS = (Tree***)malloc(frontierDepth*sizeof(Tree**)) ;
  inFrontier = topFrontier ;
  int i = frontierDepth-1 ;
  while (inFrontier) {
    remainingTreesS[i] = inFrontier->remainingTrees ;
    if (remainingTreesS[i]==-2) {
      toListTreeS[i] = NULL ;
      toTreeS[i] = inFrontier->place.toTree ;
    } else {
      toListTreeS[i] = inFrontier->place.toListTree;
      toTreeS[i] = NULL ;
    }
    --i ;
    inFrontier = inFrontier->next ;
  }
  curLastAtomTree = treeBuilder->lastAtomTree ;
  curPendingAnnotations = treeBuilder->pendingAnnotations ;

  showFrontierRec(treeBuilder->frontier) ;
  if (curLastAtomTree) {
    printf("..and lastAtomTree is \n") ;
    showTree(curLastAtomTree, 2) ;
  }
  showListTreeBeingBuilt(treeBuilder->topListTree, 0, frontierDepth, -1) ;
  free(remainingTreesS) ; remainingTreesS = NULL ;
  free(toListTreeS) ; toListTreeS = NULL ;
  free(toTreeS) ; toTreeS = NULL ;
  curLastAtomTree = NULL ;
  curPendingAnnotations = NULL ;
}

void showListTreeBeingBuilt(ListTree *listTree, int depth, int frontierDepth, int curArity) {
  int remaining = remainingTreesS[depth] ;
  int currentChildrenNumber = 0 ;
  ListTree** curBuildingTail = toListTreeS[depth] ;
  while (listTree!=NULL && (&(listTree->next))!=curBuildingTail) {
    ++currentChildrenNumber ;
    showTree(listTree->tree, depth) ;
    listTree = listTree->next ;
  }
  if (listTree!=NULL && (&(listTree->next))==curBuildingTail) {
    ++currentChildrenNumber ;
    if ((depth+1<frontierDepth && remainingTreesS[depth+1]!=-2)
        || (curLastAtomTree && listTree->tree==curLastAtomTree))
      showTreeBeingBuilt(listTree->tree, depth, frontierDepth) ;
    else
      showTree(listTree->tree, depth) ;
    listTree = listTree->next ;
  }
  if (listTree!=NULL) {
    int i;
    printf(" ") ; for (i=depth ; i>0 ; --i) printf("| ");
    printf(" !! ERROR: the list tail should be empty, and it is not\n") ;
  }
  if (remaining!=0 && curPendingAnnotations) {
    int i ;
    KeyListTree *pendingAnnotationsForShow = curPendingAnnotations ;
    curPendingAnnotations = NULL ;
    printf(" ") ; for (i=depth ; i>0 ; --i) printf("| ");
    if (depth+1<frontierDepth && remainingTreesS[depth+1]==-2) {
      printf("Unfinished pending annotations:\n") ;
      showAnnotationsBeingBuilt(pendingAnnotationsForShow, depth+1, frontierDepth) ;
    } else {
      printf("Pending annotations, waiting to be attached to next Tree outside:\n") ;
      showAnnotations(pendingAnnotationsForShow, depth+1) ;
    }
  }
  if (remaining!=0 || currentChildrenNumber+remaining!=curArity) {
    int i;
    printf(" ") ; for (i=depth ; i>0 ; --i) printf("| ");
    if (remaining==-1)
      printf("<---------- accepting any number of Trees here\n") ;
    else {
      if (remaining==0)
        printf("<---------- accepting no more Trees here\n") ;
      else
        printf("<---------- accepting %i more Tree(s) here\n", remaining) ;
      if (currentChildrenNumber+remaining!=curArity) {
        printf(" ") ; for (i=depth ; i>0 ; --i) printf("| ");
        printf("    !! which is inconsistent: %i+%i != %i\n", currentChildrenNumber,remaining,curArity) ;
      }
    }
  }
  while (listTree!=NULL) { // show extra children (that should not exist, cf ERROR above)
    showTree(listTree->tree, depth) ;
    listTree = listTree->next ;
  }
}

void showAnnotationsBeingBuilt(KeyListTree *tlAnnotations, int depth, int frontierDepth) {
  int i ;
  while (tlAnnotations != NULL) {
    printf(" ") ; for (i=depth ; i>0 ; --i) printf("| ");
    printf("++%s++\n", tlAnnotations->key) ;
    if (remainingTreesS[depth]==-2 && (tlAnnotations->tree==NULL || *(toTreeS[depth])==tlAnnotations->tree)) {
      if (tlAnnotations->tree)
        showTreeBeingBuilt(tlAnnotations->tree, depth, frontierDepth) ;
      else {
        printf(" ") ; for (i=depth+1 ; i>0 ; --i) printf("| ");
        printf("<---------- accepting one Tree here\n") ;
      }
    } else
      showTree(tlAnnotations->tree, depth+1) ;
    tlAnnotations = tlAnnotations->next ;
  }
}

void showTreeBeingBuilt(Tree *tree, int depth, int frontierDepth) {
  char *leafColor = "" ;
  char *listColor = "" ;
  char *fixColor = "" ;
  char *endColor = "" ;
/*   leafColor = "\033[02;33m" ; */
/*   listColor = "\033[01;33m" ; */
/*   fixColor = "\033[01;32m" ; */
/*   endColor = "\033[00m" ; */
  if (tree == NULL) {
    printf(" NULL tree\n") ;
    exit(1);
  } else {
    int i ;
    showAnnotations(tree->annotations, depth) ;
    printf(" ");
    switch (tree->oper->arity) {
      case 0: //leaf tree
        for (i=depth ; i>0 ; --i) printf("| ");
        if (tree==curLastAtomTree) {
          printf("%s%s%s : ? <---------- accepting value here\n", leafColor, tree->oper->name, endColor) ;
          curLastAtomTree = NULL ;
        } else if (tree->contents.value)
          printf("%s%s%s : %s\n", leafColor, tree->oper->name, endColor,  tree->contents.value) ;
        else
          printf("%s%s%s.\n", leafColor, tree->oper->name, endColor) ;
        break ;
      case -1: //list-arity tree
        for (i=depth ; i>0 ; --i) printf("| ");
        printf("%s[%s%s <---------- currently being built\n", listColor, tree->oper->name, endColor) ;
        showListTreeBeingBuilt(tree->contents.sons, depth+1, frontierDepth, -1) ;
        printf(" ") ; for (i=depth ; i>0 ; --i) printf("| ");
        printf("%s]%s\n", listColor, endColor) ;
        break ;
      default : //fixed-arity tree
        for (i=depth ; i>0 ; --i) printf("| ");
        if (remainingTreesS[depth+1]==-1)
          printf("%s(%s%s <---------- currently being built AS A LIST\n", fixColor, tree->oper->name, endColor) ;
        else
          printf("%s(%s%s <---------- currently being built\n", fixColor, tree->oper->name, endColor) ;
        showListTreeBeingBuilt(tree->contents.sons, depth+1, frontierDepth, tree->oper->arity) ;
        printf(" ") ; for (i=depth ; i>0 ; --i) printf("| ");
        printf("%s)%s\n", fixColor, endColor) ;
        break ;
    }
  }
}

void showFrontierRec(TreeGrowthFrontier *frontier) {
  if (frontier) {
/*     printf(" %08x op:%s ",frontier->memoOfTree,(frontier->memoOfTree?frontier->memoOfTree->oper->name:"")) ; */
    if (frontier->remainingTrees == -2)
      printf("S|") ;
    else
      printf("%d|", frontier->remainingTrees) ;
    showFrontierRec(frontier->next) ;
  } else
    printf("()") ;
}

/* void *treeBuilderDebugMark(TreeBuilder *treeBuilder) { */
/*   return treeBuilder->frontier->place.toListTree ; */
/* } */

/* int treeBuilderDebugCheck(TreeBuilder *treeBuilder, void *mark) { */
/*   ListTree **reference = (ListTree**)mark ; */
/*   ... */
/* } */
