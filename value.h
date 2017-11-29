#include <stdbool.h>
#ifndef VALUE_H
#define VALUE_H

/* Frame records the current frame of the bindings. Contains a linkedlist
 * of bindings and a pointer to the parent frame */
struct Value;
struct Frame {
   struct Value *bindings;
   struct Frame *parent;
};
typedef struct Frame Frame;

typedef enum {
   PTR_TYPE,
   OPEN_TYPE,
   CLOSE_TYPE,
   BOOL_TYPE,
   SYMBOL_TYPE,
   INT_TYPE,
   DOUBLE_TYPE,
   STR_TYPE,
   CONS_TYPE,
   CLOSURE_TYPE,
   NULL_TYPE,
   PRIMITIVE_TYPE,
   VOID_TYPE

} valueType;

struct Value {
   valueType type;
   union {
      void *p;
      char po;
      char pc;
      bool b;
      char *sym;
      int i;
      double d;
      char *s;
      struct ConsCell {
         struct Value *car;
         struct Value *cdr;
      } c;
      struct Closure {
         struct Value *formals;
         struct Value *body;
         Frame *frame;
      } proc;
      struct Value *(*pf)(struct Value *);
   };
};

typedef struct Value Value;

#endif
