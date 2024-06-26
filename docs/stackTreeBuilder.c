/* $Id: stackTreeBuilder.c 2475 2008-07-01 14:03:07Z llh $ */
#include "stackTreeBuilder.h"
#include <stdio.h>
#include <stdlib.h>
#include <tablesil.c>

#define new(TYPE) (TYPE*)malloc(sizeof(TYPE))

/******************************* Private Globals: */

static int stackLength = 1000 ;
static Tree **treeStack ;
static ListTree **commentsStack ;
static ListTree **labelsStack ;
static ListTree *waitingLabels ;
static ListTree *waitingComments ;
static int stackIndex = 0 ;
static int lineBegins ;
static long int lineNumber ;

static Operator *languageOperators ;
static short nullOpCode ;
static short labelOpCode ;
static short labelsOpCode ;
static short labstatOpCode ;
static short commentOpCode ;
static short commentsOpCode ;
short intCstOpCode ;  //not extern because used elsewhere. Fix that?
static short pppLineOpCode ;
static short stringOpCode ;
static short identOpCode ;

extern char* INPUTFILENAME;

/******************************* Private Forward Declarations: */

void relocateListComments(Tree *ltree) ;
Tree *appendComments(Tree *OldComments, ListTree *newComments) ;
void insertPrefixComments(Tree *tree, ListTree *comments) ;
void insertPostfixComments(Tree *tree, ListTree *comments) ;
void insertLabel(Tree *origTree, ListTree *labelTrees) ;

/******************************* Predefined Tree-building Functions: */

void prTt5(int oper) {
  Tree *t1, *t2, *t3, *t4, *t5 ;
  t5 = stbPop() ;
  t4 = stbPop() ;
  t3 = stbPop() ;
  t2 = stbPop() ;
  t1 = stbPop() ;
  stbPush(mkTree(stbRkOper(oper), mkSons(t1,mkSons(t2,mkSons(t3,mkSons(t4,mkSons(t5,(ListTree*)NULL))))))) ;
}

void prTt4(int oper) {
  Tree *t1, *t2, *t3, *t4 ;
  t4 = stbPop() ;
  t3 = stbPop() ;
  t2 = stbPop() ;
  t1 = stbPop() ;
  stbPush(mkTree(stbRkOper(oper), mkSons(t1,mkSons(t2,mkSons(t3,mkSons(t4,(ListTree*)NULL)))))) ;
}

void prTt3(int oper) {
  Tree *t1, *t2, *t3 ;
  t3 = stbPop() ;
  t2 = stbPop() ;
  t1 = stbPop() ;
  stbPush(mkTree(stbRkOper(oper), mkSons(t1,mkSons(t2,mkSons(t3,(ListTree*)NULL))))) ;
}

void prTt2(int oper) {
  Tree *t1, *t2 ;
  t2 = stbPop() ;
  t1 = stbPop() ;
  stbPush(mkTree(stbRkOper(oper), mkSons(t1,mkSons(t2,(ListTree*)NULL)))) ;
}

void prTt1(int oper) {
  stbPush(mkTree(stbRkOper(oper), mkSons(stbPop(),(ListTree*)NULL))) ;
}

void prTt0(int oper) {
  stbPush(mkTree(stbRkOper(oper), (ListTree*)NULL)) ;
}

void prTtr3(int oper) {
  Tree *t1, *t2, *t3 ;
  t3 = stbPop() ;
  t2 = stbPop() ;
  t1 = stbPop() ;
  stbPush(mkTree(stbRkOper(oper), mkSons(t3,mkSons(t2,mkSons(t1,(ListTree*)NULL))))) ;
}

void prTtr2(int oper) {
  Tree *t1, *t2 ;
  t2 = stbPop() ;
  t1 = stbPop() ;
  stbPush(mkTree(stbRkOper(oper), mkSons(t2,mkSons(t1,(ListTree*)NULL)))) ;
}

void prTpost(int oper) {
  Tree *nt = stbPop() ;
  Tree *lt = stbTop() ;
  mkPost(lt, nt) ;
}

void prTpre(int oper) {
    Tree *lt = stbPop() ;
    Tree *nt = stbPop() ;
    stbPush(mkPre(nt, lt)) ;
    relocateListComments(lt) ;
}

void prTrpost(int oper) {
  Tree *lt = stbPop() ;
  Tree *nt = stbPop() ;
  stbPush(mkPost(lt, nt)) ;
}

void prTrpre(int oper) {
  Tree *nt = stbPop() ;
  Tree *lt = stbTop() ;
  mkPre(nt,lt) ;
}

void prTdf1(int oper) {
    stbPush(mkTree(stbRkOper(oper), mkSons(stbMkNull(), (ListTree*)NULL))) ;
}

void prTdf2(int oper) {
    stbPush(mkTree(stbRkOper(oper), mkSons(stbMkNull(), mkSons(stbMkNull(), (ListTree*)NULL)))) ;
}

/******************************* Tree Stack Functions: */

void showComments(ListTree *comments) {
    if (comments) {
	while (comments) {
	    printf("""%s""", comments->tree->contents.value) ;
	    comments = comments->next ;
	}
    }
}

void showLabels(ListTree *labels) {
    if (labels) {
	printf(" ") ;
	while (labels) {
	    printf("*%s", labels->tree->contents.value) ;
	    labels = labels->next ;
	}
    }
}

void showstacks() {
    int i ;
    printf("STACK: %s (",
	   (treeStack[stackIndex]?treeStack[stackIndex]->oper->name:".")) ;
    showComments(commentsStack[stackIndex]) ;
    showLabels(labelsStack[stackIndex]) ;
    printf(") |") ;
    for (i=stackIndex-1; i>=0 ; i--) {
	printf(" %s (",
	       (treeStack[i]?treeStack[i]->oper->name:".")) ;
	showComments(commentsStack[i]) ;
	showLabels(labelsStack[i]) ;
	printf(")") ;
    }
    printf(")\n") ;
}

void stbInit() {
    int i ;
    waitingLabels = (ListTree *)NULL ;
    waitingComments = (ListTree *)NULL ;
    if (treeStack == NULL)
	treeStack = (Tree **)malloc(sizeof(Tree*) * stackLength) ;
    if (commentsStack == NULL)
	commentsStack = (ListTree **)malloc(sizeof(ListTree*) * stackLength) ;
    if (labelsStack == NULL)
	labelsStack = (ListTree **)malloc(sizeof(ListTree*) * stackLength) ;
    for (i=0 ; i<stackLength ; i++) {
	treeStack[i] = NULL ;
	commentsStack[i] = NULL ;
	labelsStack[i] = NULL ;
    }
    stackIndex = 0 ;
    lineBegins = 1 ;
    lineNumber = 0 ;
}

void stbLanguage(Operator *opers,
		 short nullCode,
		 short labelCode, short labelsCode, short labstatCode,
		 short commentCode, short commentsCode, short intCstCode,
                 short pppLineCode, short stringCode, short identCode) {
    languageOperators = opers ;
    nullOpCode = nullCode;
    labelOpCode = labelCode;
    labelsOpCode = labelsCode;
    labstatOpCode = labstatCode;
    commentOpCode = commentCode;
    commentsOpCode = commentsCode;
    intCstOpCode = intCstCode;
    pppLineOpCode = pppLineCode;
    stringOpCode = stringCode;
    identOpCode = identCode;
}

Operator *stbRkOper(int oper) {
    return &(languageOperators[oper]) ;
}

Tree *stbMkNull() {
    return mkTree(stbRkOper(nullOpCode), (ListTree*)NULL) ;
}

void stbLexLine() {
   lineNumber++ ;
   lineBegins = 1 ;
}

void stbLexicalWarning(char* text) {
  fprintf(stderr,"Fortran Parser: Lexical warning at line %d in %s, %s\n",
          lineNumber+1, INPUTFILENAME, text) ;
}

void stbLexicalError(char* text) {
    fprintf(stdout,"%d\n", ilParsingErrorCode) ; 
    fprintf(stdout,"Fortran Parser: Interrupted analysis\n") ;
    fprintf(stderr,"Fortran Parser: Lexical error at line %d in %s, %s\n",
	    lineNumber+1, INPUTFILENAME, text) ;
    /*    stbInit() ;*/
}

void stbSyntaxError() {
    fprintf(stdout,"%d\n", ilParsingErrorCode) ; 
    fprintf(stdout,"Fortran Parser: Interrupted analysis\n") ;
    fprintf(stderr,"Fortran Parser: Syntax error at line %d in %s\n",
	    lineNumber+1, INPUTFILENAME) ;
    /*    stbInit() ; */
}

void stbWaitingLabel(int intLabel) {
  char *newText ;
  Tree *newTree ;
  ListTree *newList ;

  newText = (char*)malloc(7*sizeof(char)) ;
  sprintf(newText, "%d\0", intLabel) ;
  newTree = mkAtom(stbRkOper(labelOpCode), (AtomValue)newText) ;
  newList = new(ListTree) ;
  newList->tree = newTree ;
  newList->next = (ListTree*)NULL ;
  if (waitingLabels) {
      ListTree *tailWaitingLabels = waitingLabels ;
      while (tailWaitingLabels->next)
	  tailWaitingLabels = tailWaitingLabels->next ;
      tailWaitingLabels->next = newList ;
  } else
      waitingLabels = newList ;
  /*  printf("WAITING LABELS: ") ; showLabels(waitingLabels) ; printf("\n") ; */
}

int nextIndexOf(char *str, int len, char closing) {
  int i;
  for (i=0 ; i<len ; ++i) {
    if (str[i]==closing) return i ;
  }
  return -1 ;
}

int isNumber(char *str, int len) {
  if (str[0]<'0' || str[0]>'9') return 0;
  int i = 1;
  while (i<len && str[i]>='0' && str[i]<='9') ++i ;
  if (i==len || str[i]==' ' || str[i]=='\t')
    return 1 ;
  else
    return 0 ;
}

/** Accumulates the coming text either as a comment or as a pppLine (post preprocessor),
  * into the list of comments or pppLines waiting to be attached to a future Tree.
  * This code is SPECIFIC to the Fortran parser (TODO: move it elsewhere?) */
void stbWaitingComment(int len, char *text) {
    Tree *newTree ;
    char firstChar = text[0] ;
    if (firstChar=='#') { // Case of original file and lineNo comments added by cpp/fpp
      newTree = mkTree(stbRkOper(pppLineOpCode), (ListTree*)NULL) ;
      Tree *childTree ;
      int i = 1;
      while (i<len && (text[i]==' ' || text[i]=='\t')) ++i ;
      while (i<len) {
        childTree = (Tree*)NULL ;
        if (text[i]=='\"' || text[i]=='<') {
          char closing = (text[i]=='<' ? '>' : '\"') ;
          int offset = nextIndexOf(text+i+1, len-i-1, closing) ;
          if (offset==-1) {
            childTree = NULL ;
            i=len ;
          } else {
            char *newText = (char*)malloc((offset+1)*sizeof(char)) ;
            memcpy(newText, text+i+1, offset) ;
            newText[offset] = '\0' ;
            childTree = mkAtom(stbRkOper(stringOpCode), (AtomValue)newText) ;
            i += offset+2 ;
          }
        } else {
          int isInt = isNumber(text+i, len-i) ;
          int offset = len-i ;
          int whiteOffset = nextIndexOf(text+i, len-i, ' ') ;
          if (whiteOffset!=-1) offset = whiteOffset ;
          whiteOffset = nextIndexOf(text+i, len-i, '\t') ;
          if (whiteOffset!=-1 && whiteOffset<offset) offset = whiteOffset ;
          char *newText = (char*)malloc((offset+1)*sizeof(char)) ;
          memcpy(newText, text+i, offset) ;
          newText[offset] = '\0' ;
          int opCode = (isInt ? intCstOpCode : identOpCode) ;
          childTree = mkAtom(stbRkOper(opCode), (AtomValue)newText) ;
          i += offset ;
        }
        if (childTree) mkPost(newTree, childTree) ;
        while (i<len && (text[i]==' ' || text[i]=='\t')) ++i ;
      }
    } else { // Case of normal Fortran-style comments (starting with firstChar, removed here)
      char *newText = (char*)malloc(len*sizeof(char)) ;
      memcpy(newText, text+1, len-1) ;
      newText[len-1] = '\0' ;
      newTree = mkAtom(stbRkOper(commentOpCode), (AtomValue)newText) ;
    }
    ListTree *newList = new(ListTree) ;
    newList->tree = newTree ;
    newList->next = (ListTree*)NULL ;
    if (waitingComments) {
      ListTree *tailWaitingComments = waitingComments ;
      while (tailWaitingComments->next)
        tailWaitingComments = tailWaitingComments->next ;
      tailWaitingComments->next = newList ;
    } else {
      waitingComments = newList ;
    }
/* printf("JUST ADDED A WAITING COMMENT:\n") ; */
/* showListTree(waitingComments,2) ; */
}

void stbPush(Tree *tree) {
    treeStack[stackIndex++] = tree ;
    /* If we push a tree, we are no longer just after a "lexline": */
    lineBegins = 0 ;
}

Tree *stbPop() {
    stbPlaceLabelAndComment() ;
    return treeStack[--stackIndex] ;
}

Tree *stbTop() {
    stbPlaceLabelAndComment() ;
    return treeStack[stackIndex-1] ;
}

void stbPlaceLabelAndComment() {
    /* Look at the previously popped tree treeStack[stackIndex]. Since we are going to
     * pop one more level, or at least we are going to modify the level above,
     * we are sure that this previously popped tree is the largest tree at this level.
     * Therefore we attach its waiting labels and fixed comments to it:
     * ATTENTION: since the tree is already popped, and thus referenced at other places in the code,
     * we must MODIFY it in place, so that it becomes a labstat containing its old value. !! */
  if (labelsStack[stackIndex]) {
    if (treeStack[stackIndex]) insertLabel(treeStack[stackIndex], labelsStack[stackIndex]) ;
    labelsStack[stackIndex] = NULL ;
  }
  if (commentsStack[stackIndex]) {
    if (treeStack[stackIndex])
      insertPrefixComments(treeStack[stackIndex], commentsStack[stackIndex]) ;
    else
      /* if there was no treeStack[stackIndex], attach the comment as
	 postfix of treeStack[stackIndex-1]: */
      insertPostfixComments(treeStack[stackIndex-1],commentsStack[stackIndex]);
    commentsStack[stackIndex] = NULL ;
  }
  treeStack[stackIndex] = NULL ;
}

int stbEmptyStack() {
  return (stackIndex == 0) ;
}

/* Appends comments "newComments" at the tail of "oldComments".
 * Works correctly even if oldComments is NULL, or newComments
 * is null or is a "comment" or "comments" Tree. */
Tree *appendComments(Tree *oldComments, ListTree *newCommentsList) {
    if (newCommentsList) {
	ListTree *oldCommentsList ;
	if (oldComments == NULL)
	    oldComments = mkTree(stbRkOper(commentsOpCode), NULL) ;
	oldCommentsList = oldComments->contents.sons ;
	if (oldCommentsList) {
	    while (oldCommentsList->next)
		oldCommentsList = oldCommentsList->next ;
	    oldCommentsList->next = newCommentsList ;
	} else
	    oldComments->contents.sons = newCommentsList ;
	return oldComments ;
    }
}

/* Attach "comments" as a "preComments" of "tree". */
void insertPrefixCommentsExactly(Tree *tree, ListTree *comments) {
    Tree** prefixPlace = getSetToAnnotationTree(tree, "preComments") ;
    *prefixPlace = appendComments(*prefixPlace, comments) ;
}

/* Attach "comments" as a "preComments" of "tree".
 * If "tree" is a list, attach on its first child. */
void insertPrefixComments(Tree *tree, ListTree *comments) {
    if ((tree->oper->arity == -1) && (treeSons(tree) != NULL))
	tree = treeDown(tree, 1) ;
    Tree** prefixPlace = getSetToAnnotationTree(tree, "preComments") ;
    *prefixPlace = appendComments(*prefixPlace, comments) ;
}

void insertPostfixComments(Tree *tree, ListTree *comments) {
    if ((tree->oper->arity == -1) && (treeSons(tree) != NULL))
	tree = lastTree(treeSons(tree)) ;
    Tree** postfixPlace = getSetToAnnotationTree(tree, "postComments") ;
    *postfixPlace = appendComments(*postfixPlace, comments) ;
}

void relocateListComments(Tree *lTree) {
    Tree *sonTree ;
    Tree *commentTree ;
    Tree **newlocation ;
    sonTree = lastTree(treeSons(lTree)) ;
    commentTree = getAnnotationTree(lTree, "postComments") ;
    if (sonTree && commentTree) {
	deleteAnnotation(lTree, "postComments") ;
	newlocation = getSetToAnnotationTree(sonTree, "postComments") ;
	*newlocation = appendComments(*newlocation, commentTree->contents.sons) ;
	freeTreeNode(commentTree) ;
    }
}

/* Says that the next incoming tree on the stack will accept a label when
 * finished. Therefore flushes the list of waiting labels into the comments
 * for this place in the stack. */
void stbAcceptLabels() {
  if (waitingLabels) {
    labelsStack[stackIndex] =  waitingLabels ;
    /*  printf("ACCEPT LABELS: ") ; showLabels(waitingLabels) ; */
    waitingLabels = (ListTree *)NULL ;
  }
}

void stbAcceptComments(int depth, int above) {
  if (waitingComments) {

/* int dd ; */
/* printf("COMMENTS WAITING:\n") ; */
/* showListTree(waitingComments,2) ; */
/* printf(" ACCEPTED depth:%i above:%i AT TOP TREE:\n", depth, above) ; */
/* for (dd=0 ; dd<3 && stackIndex-dd>=0 ; ++dd) { */
/*  printf("  TREE IN STACK AT DEPTH %i:\n", dd) ; */
/*  if (treeStack[stackIndex-dd]) */
/*    showTree(treeStack[stackIndex-dd],3) ; */
/*  else */
/*    printf("     null\n") ; */
/* } */
/* printf(" WHERE TREE STACK IS:\n") ; */
/*  showstacks() ; */
/* printf("\n") ; */

    if (above == 0) {
      insertPrefixComments(treeStack[stackIndex-depth], waitingComments) ;
/*       printf("ATTACH COMMENTS: ") ; showComments(waitingComments) ; */
/*       printf(" DIRECTLY PREFIX TO: %s\n", treeStack[stackIndex-depth]->oper->name) ; */
    } else if (above == 1) {
      commentsStack[stackIndex-depth-1] =  waitingComments ;
/*       printf("ACCEPT COMMENTS: ") ; showComments(waitingComments) ; */
/*       printf(" AT LEVEL: %s\n", treeStack[stackIndex-depth-1]->oper->name) ; */
    } else if (above == 2) {
      insertPrefixCommentsExactly(treeStack[stackIndex-depth], waitingComments) ;
/*       printf("ATTACH COMMENTS: ") ; showComments(waitingComments) ; */
/*       printf(" DIRECTLY PREFIX TO EXACTLY: %s\n", treeStack[stackIndex-depth]->oper->name) ; */
/*       showstacks() ; */
    } else {
      insertPostfixComments(treeStack[stackIndex-depth], waitingComments) ;
/*       printf("ATTACH COMMENTS: ") ; showComments(waitingComments) ; */
/*       printf(" DIRECTLY POSTFIX TO: %s\n", treeStack[stackIndex-depth]->oper->name) ; */
    }
    waitingComments = (ListTree *)NULL ;
  }
}

/* Transform origTree so as to insert a labstat in front.
 * If "origTree" is a non-empty list (i.e. a stats), do it on its first son. */
void insertLabel(Tree *origTree, ListTree *labelTrees) {
    Operator *origOper = origTree->oper ;
    Tree *copyTree = new(Tree) ;
    if ((origOper->arity == -1) && (origTree->contents.sons != NULL)) {
	origTree = treeDown(origTree, 1) ;
	origOper = origTree->oper ;
    }
    copyTree->oper = origOper ;
    if ((origOper) && (origOper->arity == 0))
      copyTree->contents.value = origTree->contents.value ;
    else
      copyTree->contents.sons = origTree->contents.sons ;
    copyTree->annotations = NULL ;
    origTree->oper = stbRkOper(labstatOpCode) ;
    origTree->contents.sons = mkSons(
				     mkTree(stbRkOper(labelsOpCode), labelTrees),
				     mkSons(copyTree, (ListTree*)NULL)) ;
}
