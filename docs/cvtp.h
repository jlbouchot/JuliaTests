/* $Id: cvtp.h,v 1.1 10/.0/.0 .1:.0:.2 vmp Exp $ */
#ifndef CVTP_LOADED
#define CVTP_LOADED

#include <stdio.h>

/******************************* Structures: */

typedef char *AtomValue ;

typedef struct _Operator{
  const char         *name ;
  /** Arity of this operator,
   * >0 for a general fixed arity operator, i.e. of a Tree with a fixed number of children Tree's
   * 0 for an leaf operator, i.e. either a leaf Tree with an AtomValue, or a leaf Tree with fixed 0 children,
   * -1 for a variable arity list operator, i.e. of a Tree with an arbitrary number of children Tree's. */
  short              arity ;
  short              rank ;
} Operator ;

typedef struct _Tree{
  Operator          *oper ;
  union{
    struct _ListTree  *sons ;
    AtomValue          value ;
  }                  contents ;
  struct _KeyListTree  *annotations ;
} Tree ;

typedef struct _ListTree{
  struct _Tree      *tree ;
  struct _ListTree  *next ;
} ListTree ;

typedef struct _KeyListTree{
  const char                 *key ;
  struct _Tree         *tree ;
  struct _KeyListTree  *next ;
} KeyListTree ;

/******************************* Public Forward Declarations: */

/** Build a Tree with operator "oper" and list of children Trees "sons" */
Tree *mkTree(Operator *oper, ListTree *sons) ;

/** Build an atomic Tree with operator "oper" holding the given atomic "value" */
Tree *mkAtom(Operator *oper, AtomValue value) ;

/** Build a list of Trees with head "tree" and tail "sons" */
ListTree *mkSons(Tree *tree, ListTree *sons) ;

/** Add tree "nt" as an extra rightmost child of tree "lt" */
Tree *mkPost(Tree *lt, Tree *nt) ;

/** Add tree "nt" as an extra leftmost child of tree "lt" */
Tree  *mkPre(Tree *nt, Tree *lt) ;

/* Get a pointer to the annotation named "key" in "tree".
 * If no annotation "key" is found, create one (empty) and return it.
 * The returned pointer may be used for later change of the annotation value */
Tree **getSetToAnnotationTree(Tree *tree, const char *key) ;

/** Remove annotation named "key" from "tree", if present */
void deleteAnnotation(Tree *tree, const char *key) ;

/** Return the index code of the operator of "tree" */
short treeOpCode(Tree *tree) ;

/** Return the actual number of children of "tree" */
short treeNbSons(Tree *tree) ;

/** Return the atomic value held by atomic Tree "tree" */
AtomValue treeAtomValue(Tree *tree) ;

/** Return the list of children Trees of the given non-atomic "tree" */
ListTree *treeSons(Tree *tree) ;

/** Return the "rank"-th child tree of the given non-atomic "tree". Rank 1 is first child */
Tree *treeDown(Tree *tree, short rank) ;

/** Cuts off the "rank"-th child tree of the given non-atomic "tree". Rank 1 is first child */
void cutSon(Tree *tree, short rank) ;

Tree *tailSon(ListTree *listTree, short tailRank) ;
int listTreeLength(ListTree *listTree) ;
Tree *listTreeNth(ListTree *listTree, int rank) ;
Tree *lastTree(ListTree *listTree) ;
KeyListTree *getAnnotationPlace(Tree *tree, const char *key) ;
Tree *getAnnotationTree(Tree *tree, const char *key) ;

/** Returns 0 iff tree1 and tree2 are not identical */
int equalTrees(Tree *tree1, Tree *tree2) ;
/** Returns 0 iff listTree1 and listTree2 are not identical */
int equalListTrees(ListTree* listTree1, ListTree* listTree2) ;

void freeTreeNode (Tree *tree) ;
void freeListTreeNode(ListTree * listTree) ;
void freeTree (Tree *tree) ;
void showTree(Tree *tree, int indent) ;
void showListTree(ListTree *listTree, int indent) ;

/******************************* End cvtp.h */

#endif 
