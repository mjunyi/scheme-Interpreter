#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "linkedlist.h"
#include "talloc.h"
#include "value.h"
#include "tokenizer.h"
#include "parser.h"

#ifndef INTERPRETER_H
#define INTERPRETER_H

/* interpret function handles a list of S-expressions
 * calls eval on each S-expression in the top-level (global) environment,
 * and prints each result in turn. */
void interpret(Value *tree);

/* eval function takes a parse tree of a single S-expression
 * and an environment frame in which to evaluate the expression, and
 * returns a pointer to a Value representing the value.*/
Value *eval(Value *expr, Frame *frame);

#endif
