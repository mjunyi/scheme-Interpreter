#include <stdbool.h>
#include "value.h"
#include "talloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

/*
* Create an empty list (a new Value object of type NULL_TYPE).
*/
Value *makeNull(){
  Value *v = talloc(sizeof(Value));
  v->type = NULL_TYPE;
  return v;
}

/*
* Create a nonempty list (a new Value object of type CONS_TYPE).
*/
Value *cons(Value *car, Value *cdr){
  Value *v = talloc(sizeof(Value));
  v->type = CONS_TYPE;
  v->c.car = car;
  v->c.cdr = cdr;
  return v;
}

/*
* Print a representation of the contents of a linked list.
*/
void display(Value *list){
  assert(list!=NULL);
  if(list->type==NULL_TYPE){
    printf("()");
  }
  if(list->type==CONS_TYPE){
    printf("(");
  }
  while(list->type==CONS_TYPE){
    display(list->c.car);
    if(list->c.cdr->type==NULL_TYPE){
      printf(")");
      break;
    }
    else if(list->c.cdr->type!=CONS_TYPE){
      printf(" . ");
      display(list->c.cdr);
      printf(")");
      break;
    }
    else{
      printf(" ");
      list=list->c.cdr;
    }
  }
  switch(list->type){
    case CONS_TYPE:
      break;
    case NULL_TYPE:
      break;
    case INT_TYPE:
      printf("%d",list->i);
      break;
    case DOUBLE_TYPE:
      printf("%f",(*list).d);
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

/*
* Get the car value of a given list.
* (Uses assertions to ensure that this is a legitimate operation.)
*/
Value *car(Value *list){
  assert(list!=NULL);
  assert(list->type == CONS_TYPE);
  return list->c.car;
}

/*
* Get the cdr value of a given list.
* (Uses assertions to ensure that this is a legitimate operation.)
*/
Value *cdr(Value *list){
  assert(list!=NULL);
  assert(list->type == CONS_TYPE);
  return list->c.cdr;
}

/*
* Test if the given value is a NULL_TYPE value.
* (Uses assertions to ensure that this is a legitimate operation.)
*/
bool isNull(Value *value){
  assert(value != NULL);
  if (value->type == NULL_TYPE)
    return 1;
  else
    return 0;
}

/*
* Compute the length of the given list.
* (Uses assertions to ensure that this is a legitimate operation.)
*/
int length(Value *value){
  assert(value != NULL);
  int count = 0;
  switch(value->type){
    case CONS_TYPE:
      count = 1+length(value->c.cdr);
      break;
    case NULL_TYPE:
      break;
    default:
      break;
  }
  return count;
}

/*
* Fully copies all values of a list.
* Not needed for the specification, but makes valgrind happy
* after running reverse, even with nested lists.
* Also just good to have
*/
Value *deep_copy(Value *list){
  assert(list!=NULL);
  Value *out;
  switch(list->type){
    case CONS_TYPE:
      out=cons(deep_copy(car(list)),deep_copy(cdr(list)));
      break;
    case INT_TYPE:
      out=talloc(sizeof(Value));
      out->type=list->type;
      out->i=list->i;
      break;
    case DOUBLE_TYPE:
      out=talloc(sizeof(Value));
      out->type=list->type;
      out->d=list->d;
      break;
    case STR_TYPE:
      out=talloc(sizeof(Value));
      out->type=list->type;
      out->s=list->s;
      break;
    case NULL_TYPE:
      out=makeNull();
      break;
    case PTR_TYPE:
      out=talloc(sizeof(Value));
      out->type=list->type;
      out->p=list->p;
      break;
    case OPEN_TYPE:
      out=talloc(sizeof(Value));
      out->type=list->type;
      out->po=list->po;
      break;
    case CLOSE_TYPE:
      out=talloc(sizeof(Value));
      out->type=list->type;
      out->pc=list->pc;
      break;
    case BOOL_TYPE:
      out=talloc(sizeof(Value));
      out->type=list->type;
      out->b=list->b;
      break;
    case SYMBOL_TYPE:
      out=talloc(sizeof(Value));
      out->type=list->type;
      out->sym=list->sym;
      break;
    case CLOSURE_TYPE:
      out=talloc(sizeof(Value));
      out->type=list->type;
      out->proc=list->proc;
      break;
    case PRIMITIVE_TYPE:
      out=talloc(sizeof(Value));
      out->type=list->type;
      out->pf=list->pf;
      break;
    case VOID_TYPE:
      break;
  }
  return out;
}

/*
* Create a new linked list whose entries correspond to the given list's
* entries, but in reverse order.  The resulting list is a deep copy of the
* original: that is, there should be no shared memory between the original
* list and the new one.
*
* (Uses assertions to ensure that this is a legitimate operation.)
*
* FAQ: What if there are nested lists inside that list?
* ANS: There won't be for this assignment. There will be later, but that will
*      be after we've got an easier way of managing memory.
*/
Value *reverse(Value *list){
  assert(list!=NULL);
  Value *tempIter=list;
  if(tempIter->type==NULL_TYPE){
    return list;
  }
  Value *build=makeNull();
  assert(tempIter->type == CONS_TYPE);
  while(tempIter->type == CONS_TYPE){
    build=cons(car(tempIter),build);
    tempIter=cdr(tempIter);
    assert(tempIter!=NULL);
  }
  return build;
}


/*
* Frees up all memory directly or indirectly referred to by list.
*
* (Uses assertions to ensure that this is a legitimate operation.)
*
* FAQ: What if there are nested lists inside that list?
* ANS: There won't be for this assignment. There will be later, but that will
*      be after we've got an easier way of managing memory.
*
void cleanup(Value *list){
  switch(list->type){
    case CONS_TYPE:
      cleanup(list->c.car);
      cleanup(list->c.cdr);
      free(list);
      break;

    case NULL_TYPE:
      free(list);
      break;

    case INT_TYPE:
      free(list);
      break;

    case DOUBLE_TYPE:
      free(list);
      break;

    case STR_TYPE:
      free(list);
      break;
  }
}
*/
#endif
