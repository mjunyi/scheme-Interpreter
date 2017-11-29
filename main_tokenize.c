#include "value.h"
#include "tokenizer.h"
#include "talloc.h"

int main(void) {
   Value *list = tokenize();
   displayTokens(list);
   tfree();
   return 0;
}
