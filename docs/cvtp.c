/* $Id: cvtp.c 1015 2006-04-06 08:29:48Z cmassol $ */
#include "cvtp.h"
#include <stdlib.h>
#include <string.h>

#define cnew(TYPE) (TYPE*)malloc(sizeof(TYPE))

/******************************* Private Forward Declarations: */

void freeListTreeNode (ListTree *listTree) ;
void freeListTree (ListTree *listTree) ;
void freeKeyListTree (KeyListTree *listTree) ;
void freeKeyListTreeNode (KeyListTree *listTree) ;
void printIndent(int indent) ;
void showListTree(ListTree *listTree, int indent) ;

/******************************* Tree construction functions: */

/** Build a Tree with operator "oper" and list of children trees "sons" */
Tree *mkTree(Operator *oper, ListTree *sons) {
  Tree *result = cnew(Tree) ;
  result->oper = oper ;
  result->contents.sons = sons ;
  result->annotations = (KeyListTree*)NULL ;
  return result ;
}

/** Build an atomic Tree with operator "oper" holding the given atomic "value" */
Tree *mkAtom(Operator *oper, AtomValue value) {
  Tree *result = cnew(Tree) ;
  result->oper = oper ;
  result->contents.value = value ;
  result->annotations = (KeyListTree*)NULL ;
  return result ;
}

/** Build a list of trees with head "tree" and tail "sons" */
ListTree *mkSons(Tree *tree, ListTree *sons) {
    ListTree *result = cnew(ListTree) ;
    result->tree = tree ;
    result->next = sons ;
    return result ;
}

/** Add tree "nt" as an extra rightmost child of tree "lt" */
Tree *mkPost(Tree *lt, Tree *nt) {
  ListTree *tmpChain = cnew(ListTree) ;
  tmpChain->tree = nt ;
  tmpChain->next = (ListTree*)NULL ;
  if (lt->contents.sons == NULL)
  {
      lt->contents.sons = tmpChain ;
  }
  else
  {
      ListTree *oldChain = lt->contents.sons ;
      while ((oldChain->next) != NULL)
        oldChain = oldChain->next ;
      oldChain->next = tmpChain ;
  }
  return lt ;
}

/** Add tree "nt" as an extra leftmost child of tree "lt" */
Tree *mkPre(Tree *nt, Tree *lt) {
  ListTree *tmpChain = cnew(ListTree) ;
  tmpChain->tree = nt ;
  tmpChain->next = lt->contents.sons ;
  lt->contents.sons = tmpChain ;
  return lt ;
}

/* Get a pointer to the annotation named "key" in "tree".
 * If no annotation "key" is found, create one (empty) and return it.
 * The returned pointer may be used for later change of the annotation value */
Tree **getSetToAnnotationTree(Tree *tree, const char *key) {
  KeyListTree *annots = tree->annotations ;
  while (annots && (0 != strcmp(annots->key, key)))
    annots = annots->next ;
  if (annots == NULL) {
    annots = cnew(KeyListTree) ;
    annots->key = key ;
    annots->tree = NULL ;
    annots->next = tree->annotations ;
    tree->annotations = annots ;
  }
  return &(annots->tree) ;
}

/** Remove annotation named "key" from "tree", if present */
void deleteAnnotation(Tree *tree, const char *key) {
  KeyListTree hat ;
  KeyListTree *toHat, *topHat ;
  topHat = toHat = &hat ;
  toHat->next = tree->annotations ;
  while ((toHat->next) && (0 != strcmp(toHat->next->key, key)))
      toHat = toHat->next ;
  if (toHat->next)
      toHat->next = toHat->next->next ;
  tree->annotations = topHat->next ;
}

/******************************* Tree Navigation Functions: */

/** Return the index code of the operator of "tree" */
short treeOpCode(Tree *tree) {
    return tree->oper->rank ;
}

/** Return the actual number of children of "tree" */
short treeNbSons(Tree *tree) {
    if (tree->oper->arity == -1)
      return listTreeLength(tree->contents.sons) ;
    else
      return tree->oper->arity ;
}

/** Return the atomic value held by atomic Tree "tree" */
AtomValue treeAtomValue(Tree *tree) {
  return tree->contents.value ;
}

/** Return the list of children Trees of the given non-atomic "tree" */
ListTree *treeSons(Tree *tree) {
  return tree->contents.sons ;
}

/** Return the "rank"-th child tree of the given non-atomic "tree". Rank 1 is first child */
Tree *treeDown(Tree *tree, short rank) {
  return listTreeNth(tree->contents.sons, rank) ;
}

/** Cuts off the "rank"-th child tree of the given non-atomic "tree". Rank 1 is first child */
void cutSon(Tree *tree, short rank) {
  ListTree *children = tree->contents.sons ;
  if (rank==1) {
    tree->contents.sons = children->next ;
  } else {
    while (rank>2) {
      --rank ;
      children = children->next ;
    }
    children->next = children->next->next ;
    //TODO? recover memory of cut child ?
  }
}

Tree *tailSon(ListTree *listTree, short tailRank) {
    ListTree *tail = listTree ;
    while (tailRank <= 0) {
      tail = tail->next ;
      tailRank++ ;
    }
    while (tail) {
      listTree = listTree->next ;
      tail = tail->next ;
    }
    return listTree->tree ;
}

//gets a ListTree length
int listTreeLength(ListTree *listTree){
    int length = 0 ;
    while (listTree != NULL) {
      length++ ;
      listTree = listTree->next ;
    }
    return length ;
}

//gets the Nth Tree of a ListTree
Tree *listTreeNth(ListTree *listTree, int rank) {
    while (rank > 1) {
      rank-- ;
      listTree = listTree->next ;
    }
    return listTree->tree ;
}

//gets the last Tree of a ListTree
Tree *lastTree(ListTree *listTree) {
    while (listTree->next != NULL) listTree = listTree->next ;
    return listTree->tree ;
}

KeyListTree *getAnnotationPlace(Tree *tree, const char *key) {
  KeyListTree *annots = tree->annotations ;
  while (annots && (0 != strcmp(annots->key, key)))
    annots = annots->next ;
  return annots ;
}

Tree *getAnnotationTree(Tree *tree, const char *key) {
  KeyListTree *annots = getAnnotationPlace(tree, key) ;
  if (annots)
    return annots->tree ;
  else
    return NULL ;
}

int equalTrees(Tree *tree1, Tree *tree2) {
  if (!tree1 || !tree2 || tree1->oper != tree2->oper) return 0 ;
  if (tree1->oper->arity==0)
    return ((!tree1->contents.value)
            ?(!tree2->contents.value)
            :(tree2->contents.value
              && strcmp(tree1->contents.value, tree2->contents.value)==0)) ;
  else
    return equalListTrees(tree1->contents.sons, tree2->contents.sons) ;
}

int equalListTrees(ListTree* listTree1, ListTree* listTree2) {
  int equal = 1 ;
  while (equal && listTree1 && listTree2) {
    equal = equalTrees(listTree1->tree, listTree2->tree) ;
    listTree1 = listTree1->next ;
    listTree2 = listTree2->next ;
  }
  return equal && !listTree1 && !listTree2 ;
}

/******************************* Tree destruction: */

void freeListTreeNode (ListTree *listTree) {
    if (listTree != NULL) {
      freeListTreeNode(listTree->next) ;
      free(listTree) ;
    }
}

void freeListTree (ListTree *listTree) {
  if (listTree != NULL) {
    freeListTree(listTree->next) ;
    listTree->next = NULL ;
    freeTree(listTree->tree) ;
    listTree->tree = NULL ;
    free(listTree) ;
  }
}

void freeKeyListTreeNode (KeyListTree *keyListTree) {
    if (keyListTree != NULL) {
  freeKeyListTreeNode(keyListTree->next) ;
  free(keyListTree) ;
    }
}

void freeKeyListTree (KeyListTree *keyListTree) {
  if (keyListTree != NULL) {
    freeKeyListTree(keyListTree->next) ;
    keyListTree->next = NULL ;
    freeTree(keyListTree->tree) ;
    keyListTree->tree = NULL ;
    free(keyListTree) ;
  }
}

void freeTreeNode (Tree *tree) {
  if (tree != NULL) {
    if (tree->oper->arity == 0) {
      if (tree->contents.value) free(tree->contents.value) ;
    }
    freeKeyListTree(tree->annotations) ;
    free(tree) ;
  }
}

void freeTree (Tree *tree) {
  if (tree != NULL) {
    if (tree->oper->arity == 0) {
      if (tree->contents.value) free(tree->contents.value) ;
    } else {
      freeListTree(tree->contents.sons) ;
    }
    freeKeyListTree(tree->annotations) ;
    free(tree) ;
  }
}

/******************************* Tree display: */

void printIndent(int indent) {
  int i ;
  for (i=indent; i>0 ; i--) printf("  ") ;
}

void showListTree(ListTree *listTree, int indent) {
  while (listTree != NULL) {
    showTree(listTree->tree, indent) ;
    listTree = listTree->next ;
  }
}

void showAnnotations(KeyListTree *tlAnnotations, int indent) {
  int i;
  while (tlAnnotations != NULL) {
    i = 0 ;
    printf(" "); while (i++<indent) printf("| ");
    printf("++%s++\n", tlAnnotations->key) ;
    showTree(tlAnnotations->tree, indent+1) ;
    tlAnnotations = tlAnnotations->next ;
  }
}

void showTree(Tree *tree, int indent) {
  char *leafColor = "" ;
  char *listColor = "" ;
  char *fixColor = "" ;
  char *endColor = "" ;
/*   leafColor = "\033[02;33m" ; */
/*   listColor = "\033[01;33m" ; */
/*   fixColor = "\033[01;32m" ; */
/*   endColor = "\033[00m" ; */
  if (tree == NULL) {
    printf(" NULL tree") ;
    exit(1);
  } else if (tree->oper == NULL) {
    printf(" INCOMPLETE tree") ;
  } else {
    showAnnotations(tree->annotations, indent) ;
    printf(" ");
    int i = 0;
    switch (tree->oper->arity) {
      case 0: //leaf tree
        while (i++<indent) printf("| ");
        if (tree->contents.value)
          printf("%s%s%s : %s\n", leafColor, tree->oper->name, endColor,  tree->contents.value) ;
        else
          printf("%s%s%s.\n", leafColor, tree->oper->name, endColor) ;
        break ;
      case -1: //list-arity tree
        while (i++<indent) printf("| ");
        if (tree->contents.sons == NULL)
          printf("%s%s[]%s\n", listColor, tree->oper->name, endColor) ;
        else {
          printf("%s[%s%s\n", listColor, tree->oper->name, endColor) ;
          showListTree(tree->contents.sons,indent+1) ;
          printf(" ") ;
          i = 0;
          while (i++<indent) printf("| ");
          printf("%s]%s\n", listColor, endColor) ;
        }
        break ;
      default : //fixed-arity tree
        while (i++<indent) printf("| ");
        if (tree->contents.sons == NULL)
          printf("%s%s()%s\n", fixColor, tree->oper->name, endColor) ;
        else {
          printf("%s(%s%s\n", fixColor, tree->oper->name, endColor) ;
          if (tree->oper->arity!=listTreeLength(tree->contents.sons))
            printf("       ARITY MISMATCH:expect %i, has %i\n", tree->oper->arity, listTreeLength(tree->contents.sons)) ;
          showListTree(tree->contents.sons,indent+1) ;
          printf(" ") ;
          i = 0;
          while (i++<indent) printf("| ");
          printf("%s)%s\n", fixColor, endColor) ;
        }
        break ;
    }
  }
}

static void internalShowListTreeAsDot(ListTree *listTree, FILE* out, Tree* parent);
static void internalShowAnnotationsAsDot(KeyListTree *annot, FILE* out, Tree* parent);

static void internalShowTreeAsDot(Tree *tree, FILE* out, Tree* parent, const char* key) {
  if (tree == NULL) {
    return;
  } else if (tree->oper == NULL) {
    return;
  } else {
    switch (tree->oper->arity) {
    case 0:
      if (tree->contents.value)
        fprintf(out, "  \"%p\" [label=\"%s:%s\"];\n", (void*)tree, tree->oper->name, tree->contents.value) ;
      else
        fprintf(out, "  \"%p\" [label=\"%s\"];\n", (void*)tree, tree->oper->name) ;
      break ;
    case -1:
      if (tree->contents.sons == NULL)
        fprintf(out, "  \"%p\" [label=\"%s\"];\n", (void*)tree, tree->oper->name) ;
      else {
        fprintf(out, "  \"%p\" [label=\"%s\"];\n", (void*)tree, tree->oper->name) ;
        internalShowListTreeAsDot(tree->contents.sons, out, tree) ;
      }
      break ;
    default :
      if (tree->contents.sons == NULL)
        fprintf(out, "  \"%p\" [label=\"%s\"];\n", (void*)tree, tree->oper->name) ;
      else {
        fprintf(out, "  \"%p\" [label=\"%s\"];\n", (void*)tree, tree->oper->name) ;
        internalShowListTreeAsDot(tree->contents.sons, out, tree) ;
      }
      break ;
    }
    if(parent != NULL) {
      fprintf(out, " \"%p\" -> \"%p\"", (void*)parent, (void*)tree);
      if(key != NULL)
        fprintf(out, " [style=dashed, label=\"%s\"]", key);
      fputc('\n', out);
    }
    internalShowAnnotationsAsDot(tree->annotations, out, tree);
  }
}

static void internalShowListTreeAsDot(ListTree *listTree, FILE* out, Tree* parent) {
  while (listTree != NULL) {
    internalShowTreeAsDot(listTree->tree, out, parent, NULL) ;
    listTree = listTree->next ;
  }
}

static void internalShowAnnotationsAsDot(KeyListTree *annot, FILE* out, Tree* parent) {
  while(annot != NULL) {
    internalShowTreeAsDot(annot->tree, out, parent, annot->key);
    annot = annot->next;
  }
}

void showTreeAsDot(Tree* tree, FILE* out)
{
  fprintf(out, "digraph ILTree {\n");
  internalShowTreeAsDot(tree, out, NULL, NULL);
  fprintf(out, "}\n");
}

/******************************* End */
