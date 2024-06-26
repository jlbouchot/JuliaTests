/* $Id: */
#ifndef STB_LOADED
#define STB_LOADED
#include "cvtp.h"

/******************************* Public Forward Declarations: */

void prTt5(int oper) ;
void prTt4(int oper) ;
void prTt3(int oper) ;
void prTt2(int oper) ;
void prTt1(int oper) ;
void prTt0(int oper) ;
void prTtr3(int oper) ;
void prTtr2(int oper) ;
void prTpost(int oper) ;
void prTpre(int oper) ;
void prTrpost(int oper) ;
void prTrpre(int oper) ;
void prTdf1(int oper) ;
void prTdf2(int oper) ;

void stbInit() ;
void stbLanguage(Operator *opers,
		 short nullCode,
		 short labelCode, short labelsCode, short labstatCode,
		 short commentCode, short commentsCode, short intCstCode,
                 short pppLineCode, short stringCode, short identCode) ;
Operator *stbRkOper(int oper) ;
Tree *stbMkNull() ;
void stbLexLine() ;
void stbPush(Tree *t) ;
Tree *stbPop() ;
Tree *stbTop() ;
void stbPlaceLabelAndComment() ;
int stbEmptyStack() ;
void stbLexicalWarning(char* text) ;
void stbLexicalError(char* text) ;
void stbSyntaxError() ;
void stbWaitingComment(int len, char *text) ;
void stbAcceptComments(int depth, int above) ;
void stbWaitingLabel(int intLabel) ;
void stbAcceptLabels() ;

extern short intCstOpCode ;

/******************************* End stackTreeBuilder.h */

#endif 
