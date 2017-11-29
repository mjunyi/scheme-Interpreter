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

/*Takes a linkedlist of tokens from a Scheme program
 *(the output of the tokenizer) and returns a pointer to a parse tree
 *representing that program.*/
Value *parse(Value *tokens);

/*Displays the parse tree to the screen, using parentheses to
 *denote the tree structure*/
void printTree(Value *tree);

#endif
