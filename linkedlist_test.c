#include <stdio.h>
#include <stdlib.h>
#include "value.h"
#include "linkedlist.h"

int main() {

  Value *val1 = talloc(sizeof(Value));
  val1->type = INT_TYPE;
  val1->i = 12;

  Value *val2 = talloc(sizeof(Value));
  val2->type = DOUBLE_TYPE;
  val2->d = 4.3;

  Value *val3 = talloc(sizeof(Value));
  val3->type = STR_TYPE;
  val3->s = "helllooooo";

  Value *val4 = makeNull();
  val4 = cons(val1, val4);
  val4 = cons(val2, val4);

  Value *head = makeNull();
  head = cons(val1, head); //Using deep_copy here because
  head = cons(val2, head); //val4 contains val1 & val2
  head = cons(val3, head);
  head = cons(val4, head);
  display(head);
  printf("\n");
  display(head); //Should be unchanged after deep_copy
  printf("\n");
  display(car(head));
  printf("\n");
  display(cdr(head));
  printf("\n");
  printf("Length = %i\n", length(head));
  printf("Empty? %i\n", isNull(head));
  Value *reversed = reverse(head);
  display(reversed);
  printf("\n");
}
