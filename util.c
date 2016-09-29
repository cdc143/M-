/* CPSC 411
Christian Daniel
10073726
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "util.h"

int tabno=-1;

TreeNode * StmtNode(StmtKind kind,int lineno){ //Create new StmtNode
  TreeNode *t;
  t= (TreeNode *) malloc(sizeof(TreeNode));
  if (t==NULL){
    printf("No memory to allocate additional STMT nodes. Now exiting\n");
    exit(0);
  }
  int i=0;
  while(i<MAXCHILDREN){ //Constructor for StmtNode
    t->child[i]=NULL;
    t->sibling=NULL;
    t->nodeKind=Stmt;
    t->kind.stmt=kind;
    t->lineno=lineno;
    i++;
  }
  return t;
}

TreeNode * ExprNode (ExprKind kind,int lineno){ //Create new ExprNode
  TreeNode *t;
  t= (TreeNode *) malloc(sizeof(TreeNode));
  if(t==NULL){
    printf("No memory to allocate additional STMT nodes. Now exiting\n");
    exit(0);
  }
  int i=0;
  while (i<MAXCHILDREN){ //Constructor for ExprNode
    t->child[i]=NULL;
    t->sibling=NULL;
    t->nodeKind=Expr;
    t->kind.expr=kind;
    t->lineno=lineno;
    i++;
  }
  return t;
}

char * stringCopy( char * string){ //Utility function for performing string copies
  char * t;
  if(string!=NULL){
    t=malloc(strlen(string)+1);
    if(t==NULL){
      printf("No memory to allocate for string copy. Now exiting\n");
      exit(0);
    }
    strcpy(t,string);
    return t;
  }
  return NULL;
}

void prettyPrintTree (TreeNode *t){
  int i;
  tabno++; //Keep track of how many tabs are needed
  while(t!=NULL){
    printTab();
    if(t->nodeKind==Stmt){
      switch(t->kind.stmt){
      case If: //Print to screen what type of node you encountered
	printf("If\n");
	break;
      
      case While:
	printf("While\n");
	break;
      
      case Assign:
	printf("ID assignment:%s\n",t->attr.name);
	break;
      
      case Write:
	printf("Write\n");
	break;
      
      case Input:
	printf("Input: %s\n",t->attr.name);
	break;
      
      case Begin:
	printf("Begin\n");
	break;
      
      default:
	printf("Unknown StmtNode kind\n");
	break;
      }
    }
    else if(t->nodeKind==Expr){ //Print the type of expr, as well as any relevant information stored in the TreeNode at t
      switch (t->kind.expr){
      case Op:
	printf("Op:%s\n",t->attr.name);
	break;
	
      case Const:
	printf("Constant: %d\n",t->attr.value);
	break;
	
      case Id:
       printf("Id: %s\n",t->attr.name);
       break;
       
      default:
	printf("Unknown ExprNode kind\n");
	break;
      }
    }
    else { printf("Unprintable node kind\n"); }
  for(i=0;i<MAXCHILDREN;i++){
    prettyPrintTree(t->child[i]); //Print each nodes children
    }
  t=t->sibling; //Print each nodes siblings (if they exist)
  }
  tabno--;
}

void printTab (){ //Print tabs for pretty printing
  int i=0;
  while(i<tabno){
    printf("\t");
    i++;
  }
}


