#include "linkedlist.h"
#include "talloc.h"
#include "value.h"
#include "tokenizer.h"
#include "parser.h"

#include <stdio.h>

int main(void) {
   Value *list = tokenize();
   Value *tree = parse(list);
   printTree(tree);
   printf("\n");
   tfree();
   return 0;
}
