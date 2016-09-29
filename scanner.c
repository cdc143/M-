/* CPSC 411
Christian Daniel
10073726
*/

#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "util.h"
#include "scanner.h"
#include "cgen.h"

extern int yylex();  //External vars used by flex
extern int yylineno;
extern char* yytext;
extern FILE *yyin;

struct tokenNode{ //Each token has a type, text, and a line number associated with it
  int lineno;
  int type;
  char * tokenText;
};

int errorFlag=0; //Simple global flag to indicate errors

struct tokenNode list [1024]; //List to store tokens. A call to malloc or calloc would suffice here as well, but it's easier to statically define for the purposes of this assignment

int ind=0; //Index to keep track of position in the list

//Start of recursive descent functions
TreeNode *dopart(){
  TreeNode *t;
  if(list[ind].type==DO){ //Match DO token
    ind++;
    t=stmt();
    return t;
  }
  else { //Found something else
    printf("Expected DO after WHILE block on line %d\n",list[ind].lineno);
    errorFlag=-1;
    ind++;
    return NULL;
  }
}
TreeNode  *elsepart(){
  TreeNode *t;
  if(list[ind].type==ELSE){ //Match else token
    ind++;
    t=stmt();
    return t;
  }
  else{ //Found something else
    printf("Unexpected token %s before ELSE block on line %d\n",list[ind].tokenText,list[ind].lineno);
    errorFlag=-1;
    ind++;
    return NULL;
  }
}

TreeNode *term(){ //Call factor then term1 on input
  TreeNode *p=term1();
  return p;
}

TreeNode *term1(){
  TreeNode *t=factor();
  if(list[ind].type==MUL){ //Try to match MUL token
    TreeNode *p=ExprNode(Op,list[ind].lineno);
    ind++;
    if(p!=NULL){
    p->attr.op=MUL;
    p->attr.name="*";
    p->child[0]=t;
    t=p;
    p->child[1]=factor();
    }
    return p;
  }
  else if(list[ind].type==DIV){ //Try to match DIV token
    TreeNode *p=ExprNode(Op,list[ind].lineno);
    ind++;
    if(p!=NULL){
    p->attr.op=DIV;
    p->attr.name="/";
    p->child[0]=t;
    t=p;
    p->child[1]=factor();
    }
    return p;
  }
  else{
    return t;
  } //Simply return without eating anything (lambda production)
}

void semi(){
  if(list[ind].type==SEMICOLON){ //Check for semicolon at end of statements in a stmtlist
    ind++;
  }
  else{ //Else, generate error
    printf("Expected SEMICOLON at end of statment on line %d\n",list[ind].lineno);
    errorFlag=-1;
    ind++;
  }
}

TreeNode *expr(){ //Call term then expr1 on input
  TreeNode *t=expr1();
  return t;
}

TreeNode *expr1(){
  TreeNode *t=term();
  if(list[ind].type==ADD){ //Try to match add token
    TreeNode *p=ExprNode(Op,list[ind].lineno);
    ind++;
    if(p!=NULL){
    p->attr.op=ADD;
    p->attr.name="+";
    p->child[0]=t;
    t=p;
    p->child[1]=term();
    }
    return p;
  }
  else if (list[ind].type==SUB){ //Try to match sub token
    TreeNode *p=ExprNode(Op,list[ind].lineno);
    ind++;
    if(p!=NULL){
    p->attr.op=SUB;
    p->attr.name="-";
    p->child[0]=t;
    t=p;
    p->child[1]=term();
    }
    return p;
  }
  else{} //Return without eating anything (lambda production)
  return t;
}

TreeNode *factor(){
  TreeNode *t;
  if(list[ind].type==LPAR){ //Match LPAR
    ind++;
    t=expr();
    rpar();
    return t;
  }
  else if(list[ind].type==ID){ //Match ID
    t=ExprNode(Id,list[ind].lineno);
    t->attr.name=list[ind].tokenText;
    ind++;
    return t;
  }
  else if(list[ind].type==NUM){ //Match NUM
    t=ExprNode(Const,list[ind].lineno);
    t->attr.value=atoi(list[ind].tokenText);
    ind++;
    return t;
  }
  else if(list[ind].type==SUB){ //Match SUB
    ind++;
    if(list[ind].type==NUM){ //In FACTOR, the next token after a SUB must be a NUM
      t=ExprNode(Const,list[ind].lineno);
      t->attr.value=atoi(list[ind].tokenText)*-1;
      ind++;
      return t;
    }
    else{ //Something else after SUB
      printf("Expected NUM after SUB in TERM block on line %d\n",list[ind].lineno);
      errorFlag=-1;
      ind++;
    }
  }
  else{ //No token could be eaten by FACTOR, indicating an error
    printf("Encountered unxpected %s token in FACTOR block on line %d\n",list[ind].tokenText,list[ind].lineno);
    ind++;
    errorFlag=-1;
    return NULL;
  }
  return NULL;
}

void rpar(){ //Match RPAR
  if(list[ind].type==RPAR){
    ind++;
  }
  else { //Expected RPAR after LPAR. Error.
    printf ("Expected RPAR following LPAR EXPR block on line %d\n",list[ind].lineno);
    errorFlag=-1;
    ind++;
  }
}

TreeNode *stmtlist(){
  TreeNode *t=stmt();
  TreeNode *p=t;
  //While not at end of statement list, get a statement and a semicolon from input
  while(list[ind].type!=0 && list[ind].type!=END){
    TreeNode *q;
    semi();
    if(list[ind].type!=END){
    q=stmt();
    if(q!=NULL){
      if(t==NULL){
	t=q;
	p=q;
      }
      else {
	p->sibling=q;
	p=q;
      }
    }
    }
  }
  return t;
}
TreeNode *stmt() { //Similar to stmtlist, except that statement cannot be recursively called
  if(list[ind].type==IF){ //Match IF token
    TreeNode * t=StmtNode(If,list[ind].lineno);
    ind++;
    if(t!=NULL){
      t->child[0]=expr();
      t->child[1]=thenpart();
      t->child[2]=elsepart();
    }
    return t;
  }
  else if (list[ind].type==WHILE){ //Match WHILE token
    TreeNode *t=StmtNode(While,list[ind].lineno);
    ind++;
    if(t!=NULL){
    t->child[0]=expr();
    t->child[1]=dopart();
    return t;
    }
  }
  else if(list[ind].type==INPUT){ //Match INPUT token
    ind++;
    if(list[ind].type==ID){
      TreeNode *t=StmtNode(Input,list[ind].lineno);
      if(t!=NULL){
	t->attr.name=list[ind].tokenText;
      }
      ind++;
      return t;
    }
    else{ //IDs are the only thing that can follow an INPUT
      printf("Expected ID after INPUT on line %d\n",list[ind].lineno);
      errorFlag=-1;
      ind++;
    }
  }
  else if (list[ind].type==ID){
    ind++;
    if(list[ind].type==ASSIGN){ 
      TreeNode *t= StmtNode(Assign,list[ind].lineno);
      if(t!=NULL){
	t->attr.name=list[ind-1].tokenText;
      }
      ind++;
      t->child[0]=expr();
      return t;
    }
    else{//ASSIGN must follow ID
      printf("Expected := following ID on line %d\n",list[ind].lineno);
      errorFlag=-1;
      ind++;
    }  
  }
  else if (list[ind].type==WRITE){ //Match WRITE
    TreeNode *t=StmtNode(Write,list[ind].lineno);
    ind++;
    if(t!=NULL){
      t->child[0]=expr();
    }
    return t;
  }
  else if (list[ind].type==START){ //Match Begin
    TreeNode *t=StmtNode(Begin,list[ind].lineno);
    ind++;
    if(t!=NULL){
      t->child[0]=stmtlist();
      ind++;
      return t;
    }
  }
  else{ //Encountered unknown STMT token
    printf("Unexpected %s token in STMT block on line %d\n",list[ind].tokenText,list[ind].lineno); 
    errorFlag=-1;
    ind++;
  }
  return NULL;
}

TreeNode *thenpart(){ //Match THEN
  TreeNode *t;
  if (list[ind].type==THEN){
    ind++;
    t=stmt();
    return t;
  }
  else{ //THEN must follow an IF
    printf("Unexpected token %s before THEN block on line %d\n",list[ind].tokenText,list[ind].lineno);
    errorFlag=-1;
    ind++;
    return NULL;
  }
}

TreeNode *prog() { //Start production
  TreeNode *t=stmt();
  return t;
}


//Start of main
int main(int argc, char **argv) {

  FILE *outFile;

  if(argc!=3){ //Check number of arguments
    printf("Please run the program with an input file argument as follows\n ./scanner inputFile outputFile\n");
    exit(0);
  }
  if(fopen(argv[1],"r")!=NULL){ //Try to open file
    yyin=fopen(argv[1],"r");
}
  else{
    printf("Error opening file specified by the input argument. Please check the input file, and try again\n");
    exit(0); //Bail out if error opening file
  }
  int i=0;
  int token=yylex();
  TreeNode *t;
  while(token){ //Interate through tokens, putting them in a list
    if(token==ERROR){ //If lexer error, report it
      printf("Unexpected token %s on line %d.\nExiting before parsing stage.\n",yytext,yylineno);
      errorFlag=-1;
      exit(0);
    }
    list[i].type=token; //Setup of token list
    list[i].tokenText=stringCopy(yytext);
    list[i].lineno=yylineno;
    token=yylex();
    i++;
  }
  if(errorFlag!=-1){ //If no lexer error, proceed with parsing
  t=prog();
  if(t==NULL){
    errorFlag=-1;
  }
  if(list[ind+1].type!=0){ //If end of parse, but there are still tokens in the list,error
    errorFlag=-1;
    printf("Parse was successful, but not all tokens were parsed. Perhaps you meant to put your code in an BEGIN/END block\n");
  }
  
  }
  if (errorFlag!=-1){ //If no parser error, parsing was successful
    printf("File Parsed Successfully. Now printing AST.\nNote that tabs are used to indicate a subtree.\n\n");
    prettyPrintTree(t); //Pretty print AST
    printf("Now doing code generation and printing code to output file\n");
    outFile=fopen(argv[2],"w+"); //Open outfile
    if(outFile==NULL){
      printf("Error opening output file. Please check the file and try again\n");
      exit(0);
    }
    codegenHelper(t,outFile); //Do codegen
  }
  else{exit(0);}
  fclose(yyin); //Close files and exit
  fclose(outFile);
  return 0;
}
