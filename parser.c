#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "linkedlist.h"
#include "talloc.h"
#include "value.h"
#include "tokenizer.h"

#ifndef PARSER_H
#define PARSER_H

int indentation = 0;
/*Takes a linkedlist of tokens from a Scheme program
 *(the output of the tokenizer) and returns a pointer to a parse tree
 *representing that program.*/
Value *parse(Value *tokens){
  Value *stack = makeNull();
  Value *cur = tokens;
  assert(cur != NULL && "Error (parse): null pointer");
  for (; cur->type==CONS_TYPE; cur=cdr(cur)){
    stack = cons(car(cur), stack); //shift
    if (car(cur)->type == CLOSE_TYPE){
      Value *pop = makeNull();
      while(car(stack)->type!=OPEN_TYPE){
        if (car(stack)->type!=CLOSE_TYPE){
          pop = cons(car(stack), pop); //pop
        }
        stack = cdr(stack); //pop
        if (stack->type==NULL_TYPE){
          printf("Parse error: missing (\n");
          texit(1);
        }
      }
      stack = cdr(stack); //popping open paren.
      stack = cons(pop, stack); //pushing back onto stack
    }
  }
  if (cur->type != NULL_TYPE){
    printf("Parse error: passed a non-list.\n");
    texit(1);
  }
  //Checking if there was an extra open paren in the stack
  for (cur=stack; cur->type==CONS_TYPE; cur=cdr(cur)){
    if (car(cur)->type == OPEN_TYPE){
      printf("Parse error: missing )\n");
      texit(1);
    }
  }
  return reverse(stack);
}

/* Helper function for printHelper.
 * Prints out each item appropriately.
 */
void itemPrinter(Value *list){
  switch(list->type){
    case CONS_TYPE:
      printf("Linkedlist Error. Wut?\n");
      break;
    case NULL_TYPE:
      break;
    case INT_TYPE:
      printf("%d",list->i);
      break;
    case DOUBLE_TYPE:
      printf("%g",(*list).d);
      break;
    case STR_TYPE:
      printf("%s",list->s);
      break;
    case PTR_TYPE:
      printf("%p",list->p);
      break;
    case OPEN_TYPE:
      printf("\\(");
      break;
    case CLOSE_TYPE:
      printf("\\)");
      break;
    case BOOL_TYPE:
      if (list->b){
        printf("#t");
      }
      else{
        printf("#f");
      }
      break;
    case SYMBOL_TYPE:
      printf("%s",list->sym);
      break;
    case CLOSURE_TYPE:
      printf("<procedure>");
      break;
    case VOID_TYPE:
      break;
    case PRIMITIVE_TYPE:
      printf("<primitive>");
      break;
  }
}

/*Helper function. Displays the parse tree to the screen, using parentheses to
 *denote the tree structure, but doesn't have a newline at the end*/
void printHelper(Value *tree){
  Value *list = tree;
   assert(list!=NULL);
   char Close =' ';
   if(list->type==CONS_TYPE){
     for(int i = 0; i < indentation; ++i){
       printf(" ");
     }
     printf("(");
     indentation++;
     Close=')';
   }
   while(list->type==CONS_TYPE){
     printf("\n");
     printHelper(list->c.car);
     printf(" ");
     list=list->c.cdr;
   }
   for(int i = 0; i < indentation; ++i){
     printf(" ");
   }
   itemPrinter(list);
   if(Close == ')'){
     printf("\n");
     indentation--;
     for(int i = 0; i < indentation; ++i){
       printf(" ");
     }
   }
   printf("%c",Close);
}

/*Displays the parse tree to the screen, using parentheses to
 *denote the tree structure, adds a newline to the end*/
void printTree(Value *tree){
  printHelper(tree);
  printf("\n");
}

#endif
