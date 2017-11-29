#include <stdlib.h>
#include <assert.h>
#include "value.h"
#include <stdio.h>

static Value *ActiveList=NULL;

/*
* Create an empty list (a new Value object of type NULL_TYPE).
*/
Value *makeNullTalloc(){
  Value *v = malloc(sizeof(Value));
  v->type = NULL_TYPE;
  return v;
}

/*
* Create a nonempty list (a new Value object of type CONS_TYPE).
*/
Value *consTalloc(Value *car, Value *cdr){
  Value *v = malloc(sizeof(Value));
  v->type = CONS_TYPE;
  v->c.car = car;
  v->c.cdr = cdr;
  return v;
}

/*
 * Free all pointers allocated by talloc, as well as whatever memory you
 * malloc'ed to create/update the active list.
 */
void tfree(){
  if(ActiveList != NULL){
    while(ActiveList->type == CONS_TYPE){
      assert(ActiveList->c.car->p!=NULL);
      free(ActiveList->c.car->p);
      free(ActiveList->c.car);
      Value *temp = ActiveList;
      ActiveList = ActiveList->c.cdr;
      free(temp);
    }
    free(ActiveList);
  }
  ActiveList = NULL;
}

/*
 * A simple two-line function to stand in the C function "exit", which calls
 * tfree() and then exit().  (You'll use this later to allow a clean exit from
 * your interpreter when you encounter an error: so memory can be automatically
 * cleaned up when exiting.)
 */
void texit(int status){
  tfree();
  exit(status);
}

/*
 * A malloc-like function that allocates memory, tracking all allocated
 * pointers in the "active list."  (You can choose your implementation of the
 * active list, but whatever it is, your talloc code should NOT call functions
 * in linkedlist.h; instead, implement any list-like behavior directly here.
 * Otherwise you'll end up with circular dependencies, since you're going to
 * modify the linked list to use talloc instead of malloc.)
 */
void *talloc(size_t size){
  Value *temp=NULL;
  if(ActiveList==NULL){
    temp=makeNullTalloc();
    if(temp!=NULL){
      ActiveList=temp;
      temp=NULL;
    }
    else{
      printf("Error: Out of memory\n");
      texit(1);
    }
  }
  temp=malloc(sizeof(Value));
  if(temp==NULL){
    printf("Error: Out of memory\n");
    texit(1);
  }
  temp->type=PTR_TYPE;
  temp->p=malloc(size);
  Value *temp2 = consTalloc(temp,ActiveList);
  if(temp2==NULL){
    printf("Error: Out of memory\n");
    texit(1);
  }
  ActiveList = temp2;
  return temp->p;
}


