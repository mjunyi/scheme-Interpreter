#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "linkedlist.h"
#include "talloc.h"
#include "value.h"
#include "tokenizer.h"
#include "parser.h"
#include "interpreter.h"
//Primitive Procedures
//Loads filename into stdin, then evals the file silently in f.
Value *load(char *filename,Frame *f){
  if(freopen(filename,"r",stdin)){
    Value *list = tokenize();
    Value *tree = parse(list);
    Frame *curFrame = f;
    while(tree->type==CONS_TYPE){
      Value *out = eval(car(tree), curFrame);
      tree=cdr(tree);
    }
  }
  else{
    printf("Error: unable to load file %s\n",filename);
  }
  Value *v = talloc(sizeof(Value));
  v->type = VOID_TYPE;
  return v;
}

//For scheme load function. Evals silently. Requires str type.
Value *loadEval(Value *args, Frame *f){
  if(args->type!=STR_TYPE){
    printf("Error: load expected a string\n");
    texit(1);
  }
  args->s[strlen(args->s)-1]=0;
  return load(args->s+1,f);
}

/* bind takes in a name, and a primitive function and binds it in the
 * input frame (usually the top frame) */
void bind(char *name, Value *(*function)(Value *), Frame *frame) {
   Value *value = makeNull();
   value->type = PRIMITIVE_TYPE;
   value->pf = function;
   Value *primitiveName = makeNull();
   primitiveName->type = SYMBOL_TYPE;
   primitiveName->sym = name;
   frame->bindings = cons(cons(primitiveName, cons(value,makeNull())),
                          frame->bindings);
}

/* This takes in a number (result), and a boolean telling if that number should
 * be stored as a float or int, and returns a Value that is either an
 * int Value or a float Value depending on the boolean.*/
Value *doubleIntChecker(double result, bool floatOrNot){
  Value *answer = makeNull();
  if (floatOrNot){
    answer->type = DOUBLE_TYPE;
    answer->d = result;
  }
  else{
    answer->type = INT_TYPE;
    answer->i = result;
  }
  return answer;
}
Value *symbolEval(Value *expr, Frame *frame);
Frame *makeBinding(Value *binding, Value *expr, Frame *frame);
// primitiveMult can take in multiple arguments, and prints a double
// if any of the arguments are doubles, and int if all of the arguments are
// integers
Value *primitiveMult(Value *args){
  assert(args!=NULL);
  double result = 1;
  Value *answer = makeNull();
  bool hasDouble = false;
  if(args->type==CONS_TYPE){
    Value *temp = makeNull();
    temp = args;
    while(temp->type==CONS_TYPE){
      if(temp->c.car->type==INT_TYPE){
        result *= temp->c.car->i;
      }
      else if(temp->c.car->type==DOUBLE_TYPE){
        hasDouble = true;
        result *= temp->c.car->d;
      }
      else{
        printf("Error in *: Expected number, check arguments\n");
        texit(1);
      }
      temp=cdr(temp);
    }
    answer = doubleIntChecker(result, hasDouble);
  }
  else if(args->type==NULL_TYPE){
    answer->type = INT_TYPE;
    answer->i = 1;
  }
  else{
    printf("Error: Check args of *\n");
    texit(1);
  }
  return answer;
}

/* primitiveMinus takes in its arguments and subtracts the other arguments
 * from the first argument, and returns that result. If only 1 argument,
 * it returns the negative of that argument */
Value *primitiveMinus(Value *args){
  assert(args!=NULL);
  double sum = 0;
  Value *answer = makeNull();
  bool hasDouble = false;
  Value *temp = makeNull();
  temp = args;
  if (length(args) == 1){
    if(temp->c.car->type==DOUBLE_TYPE){
      sum -= temp->c.car->d;
      hasDouble = true;
    }
    else if (temp->c.car->type==INT_TYPE){
      sum -= temp->c.car->i;
    }
    else{
      printf("Error in -: Expected Number, Check arguments\n");
      texit(1);
    }
    answer = doubleIntChecker(sum, hasDouble);
  }
  else if (length(args) > 1){
    if(temp->c.car->type==DOUBLE_TYPE){
      hasDouble = true;
      sum += temp->c.car->d;
    }
    else if (temp->c.car->type==INT_TYPE){
      sum += temp->c.car->i;
    }
    temp=cdr(temp);
    while(temp->type==CONS_TYPE){
      if(temp->c.car->type==DOUBLE_TYPE){
        hasDouble = true;
        sum -= temp->c.car->d;
      }
      else if (temp->c.car->type==INT_TYPE){
        sum -= temp->c.car->i;
      }
      else{
        printf("Error in -: Expected Number, Check arguments\n");
        texit(1);
      }
      temp=cdr(temp);
    }
    answer = doubleIntChecker(sum, hasDouble);
  }
  else{
    printf("Error in -: arity mismatch; expected at least 1 argument\n");
    texit(1);
  }
  return answer;
}

Value *begin(Value *v, Frame *f){
  if(cdr(v)->type != CONS_TYPE){
    Value *v = makeNull();
    v->type = VOID_TYPE;
    return v;
  }
  v = car(cdr(v));
  Value *res = makeNull();
  while(v->type != NULL_TYPE){
    res = eval(car(v), f);
    v = cdr(v);
  }
  return res;
}

//Supports multiple arguments, and returns a float.
/* primitiveDiv takes in its arguments and if it has multiple arguments, it
 * divides the first argument by all the other other arguments. If it has
 * 1 argument, it divides 1 by that argument*/
Value *primitiveDiv(Value *args){
  assert(args!=NULL);
  double testNumber;
  double sum = 1;
  Value *answer = makeNull();
  answer->type = DOUBLE_TYPE;
  int lengthOfArgs = length(args);
  Value *temp = makeNull();
  temp = args;
  if (lengthOfArgs == 1){ //Divide 1 by arg
    if(temp->c.car->type==DOUBLE_TYPE){
      testNumber = temp->c.car->d;
    }
    else if (temp->c.car->type==INT_TYPE){
      testNumber = temp->c.car->i;
    }
    else{
      printf("Error in /: Expected Number, Check arguments\n");
      texit(1);
    }
    if (testNumber == 0){
      printf("Error in /: Division by Zero\n");
      texit(1);
    }
    sum /= testNumber;
  }
  else if (lengthOfArgs > 1){ //Divide 1st arg by other args
    if(temp->c.car->type==DOUBLE_TYPE){
      sum = temp->c.car->d;
    }
    else if (temp->c.car->type==INT_TYPE){
      sum = temp->c.car->i;
    }
    temp=cdr(temp);
    while(temp->type==CONS_TYPE){
      if(temp->c.car->type==DOUBLE_TYPE){
        testNumber = temp->c.car->d;
      }
      else if (temp->c.car->type==INT_TYPE){
        testNumber = temp->c.car->i;
      }
      else{
        printf("Error in /: Expected Number, Check arguments\n");
        texit(1);
      }
      if (testNumber == 0){
        printf("Error in /: Division by Zero\n");
        texit(1);
      }
      sum /= testNumber;
      temp=cdr(temp);
    }
  }
  else{ //No arguments
    printf("Error in /: arity mismatch; expected at least 1 argument\n");
    texit(1);
  }
  answer->d = sum;
  return answer;
}

/* primitiveLTEq (less than equals) is a predicate takes in its arguments and
 * returns a boolean type that indicates if the first argument is less than
 * equals all the other arguments, if any argument is less than the first
 * argument, it returns false*/
Value *primitiveLTEq(Value *args){
  //Has at least two arguments
  //This does not short-circuit.
  if(length(args)<2){
    printf("Error: <= takes at least 2 arguments\n");
    texit(1);
  }
  Value *out=talloc(sizeof(Value));
  out->type=BOOL_TYPE;
  out->b=true;
  Value *iter=args;
  while(iter->type==CONS_TYPE){
    if(cdr(iter)->type!=CONS_TYPE){
      if(cdr(iter)->type==NULL_TYPE){
        return out;
      }
      printf("Error: invalid argument to <=\n");
      texit(1);
    }
    else{
      int i1;
      int i2;
      double d1;
      double d2;
      if(car(iter)->type==INT_TYPE){
        if(car(cdr(iter))->type==INT_TYPE){
          if(car(cdr(iter))->i < car(iter)->i){
            out->b=false;
            return out;
          }
          else{
            iter=cdr(iter);
          }
        }
        else if(car(cdr(iter))->type==DOUBLE_TYPE){
          if(car(cdr(iter))->d < car(iter)->i){
            out->b=false;
            return out;
          }
          else{
            iter=cdr(iter);
          }
        }
      }
      else if(car(iter)->type==DOUBLE_TYPE){
        if(car(cdr(iter))->type==INT_TYPE){
          if(car(cdr(iter))->i < car(iter)->d){
            out->b=false;
            return out;
          }
          else{
            iter=cdr(iter);
          }
        }
        else if(car(cdr(iter))->type==DOUBLE_TYPE){
          if(car(cdr(iter))->d < car(iter)->d){
            out->b=false;
            return out;
          }
          else{
            iter=cdr(iter);
          }
        }
      }
      else{
        printf("Error: invalid argument to <=\n");
        texit(1);
      }
    }
  }
  return out;
}

/* primitiveEqPred (eq?) is a predicate takes in 2 arguments and
 * compares them. It is the computational analogue of a mathematical equivalence
 * relation. This returns a boolean type that indicates if the first
 * argument is equals to the second argument*/
Value *primitiveEqPred(Value *args){
  if(length(args) != 2){
    printf("Error in eq?: Unexpected number of arguments\n");
    texit(1);
  }
  Value *val = makeNull();
  val->type = BOOL_TYPE;
  if(car(args)->type == STR_TYPE && car(cdr(args))->type == STR_TYPE)
  {
    val->b = !strcmp(car(args)->s, car(cdr(args))->s);
    return val;
  }
  val->b = (car(args)->i == car(cdr(args))->i);
  return val;
}

/* primitivePairPred (pair?) is a predicate takes in 1 argument and returns #t
 * if obj is a pair, and otherwise returns #f. */
Value *primitivePairPred(Value *arg){
  if(length(arg) != 1){
    printf("Error in pair?: Unexpected number of arguments\n");
    texit(1);
  }
  Value *val = makeNull();
  val->type = BOOL_TYPE;
  bool foundLeft = false;
  bool foundRight = false;
  if(car(arg)->type != CONS_TYPE){
    val->b = false;
    return val;
  }
  Value *tempLeft = (car(car(arg)));
  Value *tempRight = (cdr(car(arg)));
  while(!foundLeft && !foundRight){
    if(tempLeft->type != CONS_TYPE){
      foundLeft = true;
    }
    else if(car(tempLeft)->type == NULL_TYPE){
      tempLeft = cdr(tempLeft);
    }
    else if(cdr(tempLeft)->type == NULL_TYPE){
      tempLeft = car(tempLeft);
    }
    else{
      val->b = false;
      return val;
    }

    if(tempRight->type != CONS_TYPE){
      foundRight = true;
    }
    else if(car(tempRight)->type == NULL_TYPE){
      tempRight = cdr(tempRight);
    }
    else if(cdr(tempRight)->type == NULL_TYPE){
      tempRight = car(tempRight);
    }
    else{
      val->b = false;
      return val;
    }
  }
  val->b = true;
  return val;
}
// primitiveAdd can take in multiple arguments, and prints a double
// if any of the arguments are doubles, and int if all of the arguments are
// integers
/* primitiveAdd takes in its arguments and adds it, and returns the sum.*/
Value *primitiveAdd(Value *args) {
  assert(args!=NULL);
  double sum = 0;
  Value *answer = makeNull();
  bool hasDouble = false; //checks if arguments have Double-type
  if(args->type==CONS_TYPE){
    Value *temp = makeNull();
    temp = args;
    while(temp->type==CONS_TYPE){
      if(temp->c.car->type==DOUBLE_TYPE){
        hasDouble = true;
        sum += temp->c.car->d;
      }
      else if (temp->c.car->type==INT_TYPE){
        sum += temp->c.car->i;
      }
      else{
        printf("Error in +: Expected Number, Check arguments\n");
        texit(1);
      }
      temp=cdr(temp);
    }
    if (hasDouble){
      answer->type = DOUBLE_TYPE;
      answer->d = sum;
    }
    else{
      answer->type = INT_TYPE;
      answer->i = sum; //Does it automatically convert?
    }
  }
  else if (args->type==NULL_TYPE){
    answer->type = INT_TYPE;
    answer->i = 0;
  }
  else{
    printf("Error: Check args of +\n");
    texit(1);
  }
  return answer;
}

/* primitiveIsNull takes in 1 argument and asks if it is a null.*/
Value *primitiveIsNull(Value *arg){
  //Error checking for args
  //Something here...
  if(length(arg)!=1){
    printf("Error: null? takes 1 argument, %d  given.\n",length(arg));
    texit(1);
  }
  Value *v = makeNull();
  v->type = BOOL_TYPE;

  if(car(arg)->type == NULL_TYPE){ //This should be right, but please error check!
    v->b = true;
    return v;
  }
  v->b = false;
  return v;
}

/* primitiveCar takes in a list and returns the first element of the list*/
Value *primitiveCar(Value *arg){
  // check that arg is only 1 argument, not multiple
  // make sure argument is appropriately typed
  if (length(arg)!=1){
    printf("Error: car takes 1 argument, %d given.\n",length(arg));
    texit(1);
  }
  if(car(arg)->type!=CONS_TYPE){
    printf("Error: cannot car something of that type\n");
    texit(1);
  }
  return car(car(arg));
}

/* primitiveCdr takes in a list and returns a list without the first element*/
Value *primitiveCdr(Value *arg){
  // check that arg is only 1 argument, not multiple
  // make sure argument is appropriately typed
  if (length(arg)!=1){
    printf("Error: cdr takes 1 argument, %d given.\n",length(arg));
    texit(1);
  }
  if(car(arg)->type!=CONS_TYPE){
    printf("Error: cannot cdr something of that type\n");
    texit(1);
  }
  return cdr(car(arg));
}

/* primitiveCons takes a list of 2 arguments and returns cons of the args*/
Value *primitiveCons(Value *args){
  if (length(args)!=2){
    printf("Error: we need two arguments in CONS\n");
    texit(1);
  }
  return cons(car(args), car(cdr(args)));
}



//Library Procedures

/* helper function that calculates modulo in C*/
int moduloC(int a, int b){
  return (a % b + b)%b;
}
/* moduloProc takes in 2 integers and returns their modulo.*/
Value *moduloProc(Value *args){
  Value *temp = args;
  Value *answer;
  answer->type = INT_TYPE;
  if(length(args)!=2){
    printf("Error in modulo: expected 2 arguments\n");
    texit(1);
  }
  else{
    int first;
    int second;
    if(car(temp)->type==INT_TYPE){
      first = car(temp)->i;
      temp = cdr(temp);
    }
    else{
      printf("Error in modulo: arguments should be integers.\n");
      texit(1);
    }
    if(car(temp)->type==INT_TYPE){
      second = car(temp)->i;
      if(second==0){
        printf("modulo: undefined for 0.\n");
        texit(1);
      }
    }
    else{
      printf("Error in modulo: arguments should be integers.\n");
      texit(1);
    }
    answer->i = moduloC(first, second);
    return answer;
  }
  return makeNull();
}

/* Equals compares two or more numbers and returns a boolean that tells
 * if the the arguments are mathematically equal to the first argument.*/
Value *Equals(Value *args){
  if(length(args) < 2){
    printf("Error in =: Expected 2 or more arguments\n");
    texit(1);
  }
  Value *val = makeNull();
  val->type = BOOL_TYPE;
  double i;
  if(car(args)->type == DOUBLE_TYPE){
    i = car(args)->d;
  }
  else{
    i = car(args)->i;
  }
  while(args->type != NULL_TYPE){
    if(car(args)->i != i && car(args)->d != i){
      Value *val = makeNull();
      val->type = BOOL_TYPE;
      val->b = false;
      return val;
    }
    args = cdr(args);
  }
  val->b = true;
  return val;
}

/* listProc returns a list of its arguments*/
Value *listProc(Value *args){
  assert(args!=NULL);
  Value *head = makeNull();
  head->type = CONS_TYPE;
  Value *curr = makeNull();
  head->type = CONS_TYPE;
  Value *tempArgs = args;
  if (length(args)==0){
    head->type = NULL_TYPE;
    return head;
  }
  else{
    head->c.car = car(tempArgs);
    curr = head;
    tempArgs = cdr(tempArgs);
    while(tempArgs->type==CONS_TYPE){
      Value *temp = makeNull();
      temp->type = CONS_TYPE;
      temp->c.car = car(tempArgs);
      curr->c.cdr = temp;
      curr = cdr(curr);
      tempArgs = cdr(tempArgs);
    }
    curr->c.cdr = makeNull();
    return head;
  }
}

/* nullTerminated takes in a list and returns if the list is null
 * terminated (proper list) or not (improper list)*/
bool nullTerminated(Value *arg){
  Value *temp = arg;
  while(temp->type==CONS_TYPE){
    temp = cdr(temp);
  }
  if(temp->type==NULL_TYPE){
    return true;
  }
  else{
    return false;
  }
}

/* appendProc, when given all list arguments, it returns a list that contains
 * all of the elements of the given lists in order. The last argument is used
 * directly in the tail of the result. The last argument need not be a list,
 * in which case the result is an “improper list.”*/
Value *appendProc(Value *args){
  assert(args!=NULL);
  Value *head = makeNull();
  Value *curr = makeNull();
  Value *tempArgs = args;
  if (length(args)==0){
    head->type = NULL_TYPE;
    return head;
  }
  else if (length(args)==1){
    return car(args);
  }
  else{
    if(car(tempArgs)->type == CONS_TYPE){
      head = car(tempArgs); //head points to first list
      Value *last = car(tempArgs);
      while(last->type==CONS_TYPE){
        curr = last;
        last = cdr(last);
      } //After while loop, *curr points to the last CONCELL of the 1st list
      while(cdr(cdr(tempArgs))->type==CONS_TYPE){
        if (car(cdr(tempArgs))->type==CONS_TYPE){
          Value *temp = car(cdr(tempArgs));
          while(temp->type==CONS_TYPE){
            curr->c.cdr = cons(car(temp), makeNull());
            curr = cdr(curr);
            temp = cdr(temp);
          } //Iterates through the next argument and add them to *curr
          tempArgs = cdr(tempArgs);
        }
        else{
          printf("append: only last argument can be a non-list\n");
          texit(1);
        }
      }
      if(car(cdr(tempArgs))->type==CONS_TYPE){ //Looking at the last argument
        Value *temp = car(cdr(tempArgs));
        if (nullTerminated(temp)){ //for proper lists
          while(temp->type==CONS_TYPE){
            curr->c.cdr = cons(car(temp), makeNull());
            curr = cdr(curr);
            temp = cdr(temp);
          } //Iterates through the last argument and add them to *curr
          curr->c.cdr = makeNull();
        }
        else{ //for improper lists
          while(temp->type==CONS_TYPE){
            curr->c.cdr = cons(car(temp), makeNull());
            curr = cdr(curr);
            temp = cdr(temp);
          } //Iterates through the last argument and add them to *curr
          curr->c.cdr = temp;
        }
      }
      else{
        curr->c.cdr = car(cdr(tempArgs));
      }
    }
    else if(car(tempArgs)->type == NULL_TYPE){
      return car(cdr(tempArgs));
    }
    else{
      printf("append: 1st argument has to be a list\n");
      texit(1);
    }
    return head;
  }
}

/* Checks the items of 2 Values*/
bool listChecker(Value *arg1, Value *arg2){
  if (arg1->type == arg2->type){
    switch(arg1->type){
      case CONS_TYPE:
        if(length(arg1)==length(arg2)){
          if(listChecker(car(arg1), car(arg2))){
            return listChecker(cdr(arg1), cdr(arg2));
          }
        }
        return false;
      case NULL_TYPE:
        return true;
      case INT_TYPE:
        if(arg1->i == arg2->i){
          return true;
        }
          return false;
      case DOUBLE_TYPE:
        if(arg1->d == arg2->d){
          return true;
        }
        return false;
      case STR_TYPE:
        if(strcmp(arg1->s,arg2->s)==0){
          return true;
        }
        return false;
      case BOOL_TYPE:
        if(arg1->b == arg2->b){
          return true;
        }
        return false;
      case SYMBOL_TYPE:
        if(strcmp(arg1->sym,arg2->sym)==0){
          return true;
        }
        return false;
      case CLOSURE_TYPE:
        //Unspecified in R5RS, so here we will just do what DrRacket does
        return false;
      case VOID_TYPE:
        return true;
      default:
        printf("Error in comparing in equal?\n");
        texit(1);
    }
  }
  else{
    return false;
  }
  return false;
}

/* equalPred (equal?) takes in 2 arguments and compares the contents of pairs,
 * vectors, and strings, applying eqv? on other objects such as numbers
 * and symbols.*/
Value *equalPred(Value *args){
  Value *predicate = makeNull();
  predicate->type = BOOL_TYPE;
  predicate->b = false;
  if (length(args)!=2){
    printf("Error in equal?: expects two arguments\n");
    texit(1);
  }
  else{
    Value *first = car(args);
    Value *second = car(cdr(args));
    if (listChecker(first, second)){
      predicate->b = true;
      return predicate;
    }
    else{
      return predicate;
    }
  }
  return predicate;
}

/* zeroPred takes in 1 number and returns a boolean stating whether that numbers
 * is the number 0.*/
Value *zeroPred(Value *arg){
  assert(arg!=NULL);
  Value *answer = talloc(sizeof(Value));
  answer->type = BOOL_TYPE;
  answer->b = false;
  int lengthOfArgs = length(arg);
  if (lengthOfArgs==0){
    printf("zero?: arity mismatch, expected 1 argument, given 0.\n");
    texit(1);
  }
  else if (lengthOfArgs==1){
    if(car(arg)->type==INT_TYPE){
      if (car(arg)->i == 0){
        answer->b = true;
        return answer;
      }
      else{
        return answer;
      }
    }
    else if(car(arg)->type==DOUBLE_TYPE){
      if (car(arg)->d == 0){
        answer->b = true;
        return answer;
      }
      else{
        return answer;
      }
    }
    else{
      printf("zero?: expected a number.\n");
      texit(1);
    }
  }
  else if (lengthOfArgs>1){
    printf("zero?:expected 1 argument,given %i.\n", lengthOfArgs);
    texit(1);
  }
  else{
    printf("zero?: Error\n");
    texit(1);
  }
  return answer;
}

//Special Forms

/* <Expression> is evaluated, and the resulting value is stored in the location
 * to which <variable> is bound. <Variable> must be bound either in some region
 * enclosing the set! expression or at top level.*/
Value *SetBang(Value *args, Frame *f){
  Value *v = makeNull();
  v->type = VOID_TYPE;

  if(length(args) != 2){
    printf("Error in set!: Expected 2 arguments\n");
    texit(1);
  }

  if(car(args)->type != SYMBOL_TYPE){
    printf("Error in set!: Expected symbol, check arguments\n");
    texit(1);
  }
  bool found = false;
  Frame *frameIter = f;
  while(frameIter!=NULL){
     Value *iter=frameIter->bindings;
     assert(iter!=NULL);
     while(iter->type==CONS_TYPE){
        assert(car(car(iter))->type==SYMBOL_TYPE);
        if(strcmp(car(car(iter))->sym,car(args)->sym)==0){
           found = true;
           cdr(car(iter))->c.car=eval(car(cdr(args)),f);
           return v;
        }
        iter=cdr(iter);
     }
     frameIter=frameIter->parent;
  }
  printf("Error in set!: symbol is unbound\n");
  texit(1);
  return v;
}

/* The cond form chains a series of tests to select a result expression.
 * Each test-expr is evaluated in order. If it produces #f, the corresponding
 * bodys are ignored, and evaluation proceeds to the next test-expr.
 * As soon as a test-expr produces a true value, its bodys are evaluated to
 * produce the result for the cond form, and no further test-exprs are
 * evaluated.*/
Value *cond(Value *args, Frame *f){
  if(length(args) < 1){
    Value *v = makeNull();
    v->type = NULL_TYPE;
    return v;
  }
  args = cdr(args);
  while(car(args)->type != NULL_TYPE){
    if(car(args)->type != CONS_TYPE){
      printf("Error in cond: Improperly formated\n");
      texit(1);
    }
    if(strcmp(car(car(args))->sym,"else")==0){
      return eval(car(cdr(car(args))), f);
    }
    if(eval(car(car(args)), f)->type != BOOL_TYPE){
      printf("Error in cond: Improperly formated\n");
      texit(1);
    }
    if(eval(car(car(args)),f)->b){
      return eval(car(cdr(car(args))), f);
    }
    args = cdr(args);
  }
  Value *v = makeNull();
  v->type = NULL_TYPE;
  return v;
}

/* derivedAnd is the 'and' conditional. If no exprs are provided,
 * then result is #t. If a single expr is provided, the results of the 'and'
 * expression are the results of the expr. Otherwise, the first expr is
 * evaluated. If it produces #f, the result of the and expression is #f.
 * Otherwise, the result is the same as an and expression with the remaining
 * exprs in tail position with respect to the original and form.*/
Value *derivedAnd(Value *args, Frame *frame){
  if(args==NULL){
    printf("Error: Incorrect arguments to and\n");
    texit(1);
  }
  while(args->type==CONS_TYPE){
    if(cdr(args)->type==NULL_TYPE){
      return eval(car(args),frame);
    }
    else{
      Value *test=eval(car(args),frame);
      if(test->type==BOOL_TYPE){
        if(test->b==false){
          Value *out = talloc(sizeof(Value));
          out->type=BOOL_TYPE;
          out->b = false;
          return out;
        }
      }
    }
    args=cdr(args);
  }
  if(args->type==NULL_TYPE){
    Value *out = talloc(sizeof(Value));
    out->type=BOOL_TYPE;
    out->b = true;
    return out;
  }
  printf("Error: Incorrect arguments to and\n");
  texit(1);
  return NULL;
}
/* derivedOr is the 'or' conditional. If no exprs are provided,
 * then result is #f. If a single expr is provided, the results of the 'and'
 * expression are the results of the expr. Otherwise, the first expr is
 * evaluated. If it produces #t, the result of the and expression is #t.
 * Otherwise, the result is the same as an and expression with the remaining
 * exprs in tail position with respect to the original and form.*/
Value *derivedOr(Value *args, Frame *frame){
  if(args==NULL){
    printf("Error: Incorrect arguments to or\n");
    texit(1);
  }
  while(args->type==CONS_TYPE){
    if(cdr(args)->type==NULL_TYPE){
      return eval(car(args),frame);
    }
    else{
      Value *test=eval(car(args),frame);
      if(test->type==BOOL_TYPE){
        if(test->b==true){
          Value *out = talloc(sizeof(Value));
          out->type=BOOL_TYPE;
          out->b = true;
          return out;
        }
      }
      else{
        return test;
      }
    }
    args=cdr(args);
  }
  if(args->type==NULL_TYPE){
    Value *out = talloc(sizeof(Value));
    out->type=BOOL_TYPE;
    out->b = false;
    return out;
  }
  printf("Error: Incorrect arguments to or\n");
  texit(1);
  return NULL;
}

/* interpret function handles a list of S-expressions
 * calls eval on each S-expression in the top-level (global) environment,
 * and prints each result in turn. */
void interpret(Value *tree){
   assert(tree!=NULL);
   Frame *topFrame = talloc(sizeof(Frame));
   topFrame->parent = NULL;
   topFrame->bindings = makeNull();
   bind("+", primitiveAdd, topFrame);
   bind("null?", primitiveIsNull, topFrame);
   bind("car", primitiveCar, topFrame);
   bind("cdr", primitiveCdr, topFrame);
   bind("cons", primitiveCons, topFrame);
   bind("*", primitiveMult, topFrame);
   bind("-", primitiveMinus, topFrame);
   bind("pair?", primitivePairPred, topFrame);
   bind("eq?", primitiveEqPred, topFrame);
   bind("=", Equals, topFrame);
   bind("/", primitiveDiv, topFrame);
   bind("zero?", zeroPred, topFrame);
   bind("list", listProc, topFrame);
   bind("append", appendProc, topFrame);
   bind("modulo", moduloProc, topFrame);
   bind("<=", primitiveLTEq,topFrame);
   bind("equal?", equalPred, topFrame);
   load("lists.scm",topFrame);
   Frame *curFrame = topFrame;
   while(tree->type==CONS_TYPE){
     Value *out = eval(car(tree), curFrame);

     if (out->type==VOID_TYPE){
       tree = cdr(tree);
       assert(tree!=NULL);
     }
     else {
       display(out);
       printf("\n");
       tree = cdr(tree);
       assert(tree!=NULL);
     }
   }
   return;
}

/* ifEval is a helper function that helps to evaluate if statements. It
 * takes in the expression and the frame it is evaluating in and returns
 * the evaluated value.*/
Value *ifEval(Value *expr, Frame *frame){
  if (cdr(expr)->type!=CONS_TYPE){
    printf("Error: if statement has no test or consequent\n");
    texit(1);
  }
  if (cdr(cdr(expr))->type!=CONS_TYPE){
    printf("Error: if statement has no consqeuent\n");
    texit(1);
  }
  bool alternate; //Checks for for alternate is present
  if (cdr(cdr(cdr(expr)))->type == NULL_TYPE){
    alternate = false; //no alternate
  }
  else{
    if (cdr(cdr(cdr(cdr(expr))))->type != NULL_TYPE){
      printf("Error: too many arguments in if statement\n");
      texit(1);
    }
    alternate = true; //there is alternate
  }
  bool ifTest; //Checks if types
  Value *evaledTest=eval(car(cdr(expr)), frame);
  switch (evaledTest->type){
    case BOOL_TYPE:
      ifTest = true;
      break;
    case CONS_TYPE:
      ifTest = true;
      break;
    default:
      ifTest = false;
      break;
  }
  if (ifTest){
    if (evaledTest->b == false){
      if (alternate){
        if (cdr(cdr(cdr(expr)))->type!=CONS_TYPE){
          printf("Error: if statement in eval has parsing error\n");
          texit(1);
        }
        return eval(car(cdr(cdr(cdr(expr)))), frame);
      }
      else{ //there is no alternate, if statement is done, move on
        Value *nothing = talloc(sizeof(Value));
        nothing->type = VOID_TYPE;
        return  nothing; //should be some value of void type
      }
    }
    else{ //test is true
      return eval(car(cdr(cdr(expr))), frame);
    }
  }
    return eval(car(cdr(cdr(expr))), frame);
}

/* Helper function. Returns true if symbol is in list of bindings.
 * False otherwise. */
bool symbolInBindings(Value *symbol, Value *bindings){
  if(symbol->type!=SYMBOL_TYPE){
    return false;
  }
  Value *iter=bindings;
  if(iter==NULL){
    return false;
  }
  while(iter->type==CONS_TYPE){
    assert(car(car(iter))->type==SYMBOL_TYPE);
    if(strcmp(car(car(iter))->sym,symbol->sym)==0){
      return true;
    }
    iter=cdr(iter);
  }
  return false;
}

/* letStar's helper function. It takes a binding, makes only 1 binding,
 * and then returns its frame.*/
Frame *letOnce(Value *bindings, Frame *frame){
  Frame *newFrame=talloc(sizeof(Frame));
  newFrame->parent=frame;
  newFrame->bindings=cons(bindings, makeNull());
  Value *tempBind = newFrame->bindings;
  if (car(tempBind)->type!=CONS_TYPE){
     printf("let*: bad syntax (error in bindings)1\n");
     texit(1);
   }
   if (cdr(car(tempBind))->type!=CONS_TYPE){
     printf("let*: bad syntax (error in bindings)2\n");
     texit(1);
   }
   cdr(car(tempBind))->c.car=eval(car(cdr(car(tempBind))),frame);

  return newFrame;
}

/* letStar is similar to let except it creates a new frame every time it
 * binds something. And eventually, the it evauluates the expressions
 * in the last bound frame.*/
Value *letStar(Value *expr, Frame *frame){
  Value *allBindings = car(cdr(expr)); //the list of bindings
  Frame *frameToBindIn = frame;
  while(allBindings->type == CONS_TYPE){
    frameToBindIn = letOnce(car(allBindings), frameToBindIn);
    allBindings = cdr(allBindings);
  }
  if (cdr(expr)->type!=CONS_TYPE){
      printf("Error: let has no arguments\n");
    texit(1);
  }
  if (cdr(cdr(expr))->type!=CONS_TYPE){
    printf("let body: no expression in body\n");
    texit(1);
  }
  while(cdr(cdr(cdr(expr)))->type!=NULL_TYPE){
    eval(car(cdr(cdr(expr))),frameToBindIn);
    expr=cdr(expr);
  }
  return eval(car(cdr(cdr(expr))), frameToBindIn);
}

Value *letRec(Value *expr, Frame *frame){
  Frame *newFrame=talloc(sizeof(Frame)); //Need frame
  newFrame->parent=frame;
  if (cdr(expr)->type!=CONS_TYPE){
      printf("Error: let has no arguments\n");
    texit(1);
  }
  newFrame->bindings=car(cdr(expr));
  Value *tempBind=newFrame->bindings; //Set bindings
  while(tempBind->type==CONS_TYPE){ //Eval things to bind to.
     if (car(tempBind)->type!=CONS_TYPE){
       printf("let: bad syntax (error in bindings)\n");
       texit(1);
     }
     if (cdr(car(tempBind))->type!=CONS_TYPE){
       printf("let: bad syntax (error in bindings)\n");
       texit(1);
     }
     if(symbolInBindings(car(car(tempBind)),cdr(tempBind))){
       printf("let: bad syntax (value already bound)\n");
       texit(1);
     }
     cdr(car(tempBind))->c.car=eval(car(cdr(car(tempBind))),newFrame);
     tempBind=cdr(tempBind);
     assert(tempBind!=NULL);
  }
  if (cdr(cdr(expr))->type!=CONS_TYPE){
    printf("let body: no expression in body\n");
    texit(1);
  }
  while(cdr(cdr(cdr(expr)))->type!=NULL_TYPE){
    eval(car(cdr(cdr(expr))),newFrame);
    expr=cdr(expr);
  }
  return eval(car(cdr(cdr(expr))),newFrame);
}

/* letEval is a helper function that helps to evaluate let statements. It
 * takes in the expression and the frame it is evaluating in and returns
 * the evaluated value.*/
Value *letEval(Value *expr, Frame *frame){
  Frame *newFrame=talloc(sizeof(Frame)); //Need frame
  newFrame->parent=frame;
  if (cdr(expr)->type!=CONS_TYPE){
      printf("Error: let has no arguments\n");
    texit(1);
  }
  newFrame->bindings=car(cdr(expr));
  Value *tempBind=newFrame->bindings; //Set bindings
  while(tempBind->type==CONS_TYPE){ //Eval things to bind to.
     if (car(tempBind)->type!=CONS_TYPE){
       printf("let: bad syntax (error in bindings)\n");
       texit(1);
     }
     if (cdr(car(tempBind))->type!=CONS_TYPE){
       printf("let: bad syntax (error in bindings)\n");
       texit(1);
     }
     if(symbolInBindings(car(car(tempBind)),cdr(tempBind))){
       printf("let: bad syntax (value already bound)\n");
       texit(1);
     }
     cdr(car(tempBind))->c.car=eval(car(cdr(car(tempBind))),frame);
     tempBind=cdr(tempBind);
     assert(tempBind!=NULL);
  }
  if (cdr(cdr(expr))->type!=CONS_TYPE){
    printf("let body: no expression in body\n");
    texit(1);
  }
  while(cdr(cdr(cdr(expr)))->type!=NULL_TYPE){
    eval(car(cdr(cdr(expr))),newFrame);
    expr=cdr(expr);
  }
  return eval(car(cdr(cdr(expr))),newFrame);
}

/* makeBinding binds binding to eval(expr) in frame */
Frame *makeBinding(Value *binding, Value *expr, Frame *frame){
   frame->bindings=cons
                      (cons
                           (binding,
                           cons(expr,makeNull())),
                      frame->bindings);
   return frame;
}

/* quoteEval is a helper function that helps to evaluate quotes. It
 * takes in the expression and the frame it is evaluating in and returns
 * the un-evaluated value.*/
Value *quoteEval(Value *expr, Frame *frame){

  if(cdr(expr)->type==NULL_TYPE){
    printf("Bad syntax, quote doesn't have arguments.\n");
    texit(1);
  }
  if(cdr(expr)->type!=CONS_TYPE){
    printf("Bad arguments to quote\n");
    texit(1);
  }
  //otherwise just return the rest of the arguments unevaluated
  return car(cdr(expr));
  return cons(car(car((cdr(expr)))), cons(makeNull(), makeNull()));
}

/* symbolEval is a helper function that helps find a specific binding. It
 * takes in the symbol and the frame it is in to search for the bindings in
 * the frame. If it doesn't find the binding it searches to up to the parent
 * frame. If it finds the binding, it returns the associated value of the
 * symbol, else it prints an error message that symbol is undefined. */
Value *symbolEval(Value *expr, Frame *frame){
  Frame *frameIter = frame;
  while(frameIter!=NULL){
     Value *iter=frameIter->bindings;
     assert(iter!=NULL);
     while(iter->type==CONS_TYPE){
        assert(car(car(iter))->type==SYMBOL_TYPE);
        if(strcmp(car(car(iter))->sym,expr->sym)==0){
           return car(cdr(car(iter)));
        }
        iter=cdr(iter);
     }
     frameIter=frameIter->parent;
  }
  printf("Evaluation Error: %s Undefined\n",expr->sym);
  texit(1);
  return makeNull();  //to make the compiler not complain
}


/* lambdaEval is a helper function that checks for the syntax of the
 * expression and also returns the procedure. */
Value *lambdaEval(Value *expr, Frame *frame){
  //TODO: Make sure symbol_type formals work.

  if (cdr(expr)->type!=CONS_TYPE){
    printf("lambda: bad syntax\n");
    texit(1);
  }
  if (cdr(cdr(expr))->type!=CONS_TYPE){
    printf("lambda body: no expression in body\n");
    texit(1);
  }
  if (car(cdr(expr))->type==CONS_TYPE){
    if (car(car(cdr(expr)))->type!=SYMBOL_TYPE){
      printf("Syntax error: ");
      printf("lambda takes in symbol as first argument\n");
      texit(1);
    }
  }
  else{
    if ((car(cdr(expr)))->type!=SYMBOL_TYPE){
      if((car(cdr(expr))->type!=NULL_TYPE)){
        printf("Syntax error: ");
        printf("lambda takes in symbol as first argument\n");
        texit(1);
      }
    }
  }
  Value *procedure=talloc(sizeof(Value));
  procedure->type=CLOSURE_TYPE;
  procedure->proc.formals=car(cdr(expr));
  procedure->proc.body=cdr(cdr(expr));
  procedure->proc.frame=frame;
  return procedure;
}

/* apply allows us to apply procedures to its arguments. It takes in the
 * function and the args to returns the evaluated value. */
Value *apply(Value *function, Value *args){
  if (function->type==PRIMITIVE_TYPE){
    return (function->pf)(args);
  }
  else{
    assert(args!=NULL);
    if(function->type!=CLOSURE_TYPE){
      printf("Error: Invalid Procedure\n");
      texit(1);
    }
    //Make new frame
    Frame *applyFrame=talloc(sizeof(Frame));
    applyFrame->parent=function->proc.frame;
    applyFrame->bindings=makeNull();
    //Set bindings for new frame
    Value *formal=function->proc.formals;
    Value *body=function->proc.body;
    body=deep_copy(body);
    if(formal->type==SYMBOL_TYPE){
      applyFrame=makeBinding(formal,args,applyFrame);
    }
    else{
      while(args->type==CONS_TYPE){
        if(formal->type!=CONS_TYPE){
          printf("Syntax error: Too many arguments\n");
          texit(1);
        }
        applyFrame = makeBinding(car(formal), car(args), applyFrame);
        formal = cdr(formal);
        args = cdr(args);
        assert(formal!=NULL);
        assert(args!=NULL);
      }
      if(formal->type==CONS_TYPE){
        printf("Error: Too few arguments\n");
        texit(1);
      }
    }
    //display(applyFrame->bindings);
    //printf("\n");
    assert(formal->type!=CONS_TYPE);
    while(body->type==CONS_TYPE){
      if(cdr(body)->type==NULL_TYPE){
        return eval(car(body), applyFrame);
      }
      eval(car(body),applyFrame);
      body=cdr(body);
    }
    printf("Error: Procedure failure\n");
    texit(1);
    return body;
  }
}

/* eval function takes a parse tree of a single S-expression
 * and an environment frame in which to evaluate the expression, and
 * returns a pointer to a Value representing the value.*/
Value *eval(Value *expr, Frame *frame){
   assert(expr!=NULL);
   if(expr->type==CONS_TYPE){
      if(car(expr)->type==SYMBOL_TYPE){
         if(strcmp(car(expr)->sym,"if")==0){
           return ifEval(expr, frame);
         }
         else if(strcmp(car(expr)->sym,"and")==0){
           return derivedAnd(cdr(expr),frame);
         }
         else if(strcmp(car(expr)->sym,"or")==0){
           return derivedOr(cdr(expr),frame);
         }
         else if(strcmp(car(expr)->sym,"set!")==0){
           return SetBang(cdr(expr),frame);
         }
         else if(strcmp(car(expr)->sym,"let")==0){ //let
           return letEval(expr, frame);
         }
         else if(strcmp(car(expr)->sym,"let*")==0){
           return letStar(expr, frame);
         }
         else if(strcmp(car(expr)->sym,"letrec")==0){
           return letRec(expr, frame);
         }
         else if(strcmp(car(expr)->sym,"quote")==0){
           return quoteEval(expr, frame);
         }
         else if(strcmp(car(expr)->sym,"cond")==0){
           return cond(expr, frame);
         }
         else if(strcmp(car(expr)->sym,"begin")==0){
           return begin(expr, frame);
         }
         else if(strcmp(car(expr)->sym,"load")==0){
           if(cdr(expr)->type!=CONS_TYPE){
             printf("Error: No arguments to load\n");
             texit(1);
           }
           return loadEval(car(cdr(expr)),frame);
         }
         else if(strcmp(car(expr)->sym,"lambda")==0){

           return lambdaEval(expr, frame);
         }
         else if(strcmp(car(expr)->sym,"define")==0){
           if(length(expr)!=3){
             printf("Syntax error: ");
             printf("define takes 2 arguments,%d given\n",length(expr)-1);
             texit(1);
           }
           else if(car(cdr(expr))->type!=SYMBOL_TYPE){
             printf("Syntax error: define takes in symbol as first argument\n");
             texit(1);
           }
           frame=makeBinding(car(cdr(expr)),
           eval(car(cdr(cdr(expr))),frame),frame);
           Value *nothing = talloc(sizeof(Value));
           nothing->type = VOID_TYPE;
           return nothing;
         }
         else{
           Value *arguments = cdr(expr);
           for (;arguments->type==CONS_TYPE;arguments=cdr(arguments)){
             arguments->c.car = eval(arguments->c.car, frame);
           }
           return apply(eval(car(expr), frame),cdr(expr));
         }
      }
      else{
        Value *proc=eval(car(expr),frame);
        if(proc->type==CLOSURE_TYPE ||proc->type==PRIMITIVE_TYPE){
          Value *arguments=cdr(expr);
          for(;arguments->type==CONS_TYPE;arguments=cdr(arguments)){
            arguments->c.car = eval(arguments->c.car,frame);
          }
          return apply(proc,cdr(expr));
        }
        else{
          printf("Error: expression not a procedure\n");
          texit(1);
        }
      }
   }
   else if(expr->type==SYMBOL_TYPE){
     return symbolEval(expr, frame);
   }
   return expr;
}
