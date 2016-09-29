/* CPSC 411
Christian Daniel
10073726
*/

%{
#include "globals.h"
#include <stdlib.h>
int commentNesting=0; //int to keep track of comment nesting levels
%}

%x IN_COMMENT

%%

<INITIAL>"/*"    BEGIN(IN_COMMENT); ++commentNesting; //Begin multi-line comment
"%".*
<IN_COMMENT>{
"%".* //If single line in multiline, don't adjust comment nesting
"\n"  yylineno++;
"/*"    ++commentNesting; //Increase comment nesting
"*/"    if(--commentNesting == 0) BEGIN(INITIAL); //If comment resting is 0, end comment
<<EOF>> {
printf("Unterminated Comment\n");
exit(0);
}
[^*/] //Match anything else without doing anything
}

"if"    return IF;  //Match standard tokens
"then"  return THEN;
"while" return WHILE;
"do"    return DO;
"input" return INPUT;
"else"  return ELSE;
"begin" return START;
"end"   return END;
"write" return WRITE;
[a-zA-Z][a-zA-Z0-9]*  return ID;
[0-9]+ return NUM;
"*/"   return ERROR;
"+"    return ADD;
":="   return ASSIGN;
"-"    return SUB;
"*"    return MUL;
"/"    return DIV;
"("    return LPAR;
")"    return RPAR;
";"    return SEMICOLON;
"\n"   yylineno++;
[ \t\r]  //Ignore whitespace
.       return ERROR;
%%

int yywrap(void) {
  return 1;
}
