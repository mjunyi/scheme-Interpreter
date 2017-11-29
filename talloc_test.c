#include <stdio.h>
#include "value.h"
#include "talloc.h"
#include "linkedlist.h"

int main() {
  Value *val1 = talloc(sizeof(Value));
  val1->type = INT_TYPE;
  val1->i = 12;
  Value *val2 = talloc(sizeof(Value));
  val2->type = DOUBLE_TYPE;
  val2->d = 4.3;

  Value *head = makeNull();
  head = cons(val1, head);
  head = cons(val2, head);

  Value *val3 = talloc(sizeof(Value));
  val3->type = STR_TYPE;
  val3->s = "helllooooo";

  Value *val4 = makeNull();
  val4 = cons(val1, val4);
  val4 = cons(val2, val4);

  head = makeNull();
  head = cons(val1, head);
  head = cons(val2, head);
  head = cons(val3, head);
  head = cons(val4, head);
  display(head);
  printf("\n");
  display(car(head));
  printf("\n");
  display(cdr(head));
  printf("\n");
  printf("Length = %i\n", length(head));
  printf("Empty? %i\n", isNull(head));
  Value *reversed = reverse(head);
  display(reverse(makeNull()));
  printf("\n");
  display(reversed);
  printf("\n");
  display(head);
  printf("\n");
  tfree();
  printf("I can see this.\n");
  Value *val5 = talloc(sizeof(Value));

  texit(1);
  printf("I should never see this.\n");
}
