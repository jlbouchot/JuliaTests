/* $Id: treeBuilder.h,v 1.1 10/.0/.0 .1:.0:.2 vmp Exp $ */
#ifndef TREEBUILDER_LOADED
#define TREEBUILDER_LOADED

#include "cvtp.h"

/* TYPES: */

/** Internal data type : do not use outside! */
typedef struct _TreeGrowthFrontier{
  int                         remainingTrees ;
  union{
      ListTree**                  toListTree ;
      Tree**                      toTree ;
  }                           place ;
  struct _TreeGrowthFrontier *next ;
/*   Tree *memoOfTree ; */
} TreeGrowthFrontier ;

/** Object that receives tree protocol and builds a Tree from it */
typedef struct _TreeBuilder{
  Operator           *langOp ;
  ListTree           *topListTree ;
  TreeGrowthFrontier *frontier ;
  Tree               *lastAtomTree ;
  KeyListTree        *pendingAnnotations ;
  int                 acceptAnnotations ;
} TreeBuilder ;

/******************************* Exported functions: */

/** Creates a builder for trees in the language defined by "langOp" */
TreeBuilder *newTreeBuilder(Operator *langOp) ;

/** Next element coming through the protocol is a tree operator */
extern Tree *startTree(TreeBuilder *treeBuilder, int operRk) ;

/** Force the most recently created Tree (tree) to assume list arity */
void turnListFrontier(TreeBuilder *treeBuilder, Tree *tree) ;

/** Next element coming through the protocol is a Tree,
 * to be put as annotation of the next Tree */
extern void startAnnotation(TreeBuilder *treeBuilder, const char *key) ;

/** Next element coming through the protocol is the value of previous atomTree */
extern void putValue(TreeBuilder *treeBuilder, AtomValue value) ;

/** Insert newTree into the next available slot */
extern void putTree(TreeBuilder *treeBuilder, Tree *newTree) ;

/** Tell the tree builder that one available slot in the frontier has been filled
 * in some way, so the tree builder frontier must be updated accordingly */
void oneLessWaiting(TreeBuilder *treeBuilder) ;

/** Insert all trees in newTrees into the next available slots of treeBuilder */
extern void putListTree(TreeBuilder *treeBuilder, ListTree *newTrees) ;

/** Start a tree with the given arity, but the operator doesn't matter */
extern Tree *startDummyTree(TreeBuilder *treeBuilder, int arity) ;

/** Next element coming through the protocol is an endOfList marker */
extern void terminateListTree(TreeBuilder *treeBuilder) ;

/** Return the tree built by all the protocol received */
extern Tree *getTreeBuilt(TreeBuilder *treeBuilder) ;

/** Return the ListTree built after all protocol has been processed */
extern ListTree *getListTreeBuilt(TreeBuilder *treeBuilder) ;

/** Delete the TreeBuilder and recovers memory */
extern void deleteTreeBuilder(TreeBuilder *treeBuilder) ;

/** Reset the TreeBuilder to its initial, empty state */
extern void resetTreeBuilder(TreeBuilder *treeBuilder) ;

/** Remove given tree from the tree currently built inside the treeBuilder */
void removeTree(TreeBuilder *treeBuilder, Tree *tree) ;

/** For debug, shows the current state of the treeBuilder */
void showTreeBuilderState(TreeBuilder *treeBuilder) ;
/******************************* End treeBuilder.h */

#endif 

