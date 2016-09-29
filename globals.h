#define MAXCHILDREN 3

typedef enum
  {
    //keywords
    TEST,IF,THEN,WHILE,DO,INPUT,ELSE,START,END,WRITE,
    //special characters
    ADD,ASSIGN,SUB,MUL,DIV,LPAR,RPAR,SEMICOLON,
    //vars and constants
    ID,NUM,ERROR
  } TokenType;

typedef enum {Stmt,Expr} NodeKind;
typedef enum {If,Begin,Assign,Input,Write,While} StmtKind;
typedef enum {Op,Const,Id} ExprKind;

typedef struct treeNode
{
  struct treeNode *child[MAXCHILDREN];
  struct treeNode *sibling;
  int lineno;
  NodeKind nodeKind;
  union { StmtKind stmt; ExprKind expr;} kind;
  union {TokenType op; int value; char * name;} attr;
} TreeNode;
