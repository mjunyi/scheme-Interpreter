#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "linkedlist.h"
#include "talloc.h"
#include "value.h"

#ifndef TOKENIZER_H
#define TOKENIZER_H

Value *tokenize();

void displayTokens(Value *list);

#endif
