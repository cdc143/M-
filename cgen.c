#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "util.h"
#include "cgen.h"

int loc=0; //Counter to generate new labels
char loop[sizeof(loc)+1]; //This is defined here because C doesn't allow definitions at the start of switch cases
FILE * out;

void genexpr(TreeNode *t){ //Check what kind of expr the node represents, but the appropriate lines to file
  switch(t->kind.expr){
  case Const:
    fprintf(out,"cPUSH %d\n",t->attr.value);
    break;
  case Id:
    fprintf(out,"rPUSH %s\n",t->attr.name);
    break;
  case Op:
    codegen(t->child[0]); //Do code generation on left and right oprands
    codegen(t->child[1]);
    fprintf(out,"OP2 %s\n",t->attr.name);
    break;
  default:
    break;
}
}

void genstmt(TreeNode *t){
  //Match kind of statement, put appropriate lines to file
  switch(t->kind.stmt){
  case Begin:
    codegen(t->child[0]);
    break;
  case If:
    codegen(t->child[0]); //Condition code generation
    char elsepart[sizeof(loc)+1];
    loc++;
    sprintf(elsepart,"L%d",loc); //Create label for else part
    char rest[sizeof(loc)+1]; //Create label for code after the If
    loc++;
    sprintf(rest,"L%d",loc);
    fprintf(out,"cJUMP %s\n",elsepart); //Jump to else part if condition is false
    codegen(t->child[1]); //Then part
    fprintf(out,"JUMP %s\n",rest); //Skip else part
    fprintf(out,"%s:\n",elsepart); //Else part
    codegen(t->child[2]);
    fprintf(out,"%s:\n",rest); //Rest of code
    break;
  case While:
    sprintf(loop,"L%d",loc); //Create label for start of loop
    loc++;
    char rest1[sizeof(loc)+1];
    sprintf(rest1,"L%d",loc); //Create label for rest of code after loop
    loc++;
    fprintf(out,"%s:\n",loop);
    codegen(t->child[0]); //Loop condition generation
    fprintf(out,"cJUMP %s\n",rest1); //If false, jump to rest of code
    codegen(t->child[1]); //Loop body
    fprintf(out,"JUMP %s\n",loop); //At end of loop body, jump back to start
    fprintf(out,"%s:\n",rest1); //Return and generate rest of code
    break;
  case Write:
    codegen(t->child[0]);
    fprintf(out,"PRINT\n");
    break;
  case Assign:
    codegen(t->child[0]);
    fprintf(out,"LOAD %s\n",t->attr.name);
    break;
  case Input:
    fprintf(out,"READ r\n"); //Read input variable into register
    fprintf(out,"rPUSH r\n"); //Push register onto stack
    fprintf(out,"LOAD %s\n",t->attr.name); //Load top of stack into variable
    break;
  default:
    break;
}
}
void codegenHelper(TreeNode *t,FILE * outFile){
  out=outFile;
  codegen(t);
}
void codegen(TreeNode *t){
  if(t!=NULL){ //Check what type of node is currently pointed to by t
    if(t->nodeKind==Stmt){
      genstmt(t);
    }
    else if (t->nodeKind=Expr){
      genexpr(t);
    }
    else { //Not a recognized node kind
      printf("Unknown node kind encountered during code generation. Now Exiting\n");
      exit(0);
    }
  }
  if(t->sibling!=NULL){ //Check if node has any siblings. If so, do codegen on them
    codegen(t->sibling);
  }
}
  
  
