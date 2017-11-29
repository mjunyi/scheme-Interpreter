#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "linkedlist.h"
#include "talloc.h"
#include "value.h"

#ifndef TOKENIZER_H
#define TOKENIZER_H

Value *readNum(char charRead,int sign,int *symbolCharTable){
  int number[2]={0};
  if(charRead == '.'){
    char lookAhead=fgetc(stdin);
    if(!(symbolCharTable[lookAhead] & 8)){
      printf("Error: failed to tokenize '.'\n");
      texit(1);
    }
    ungetc(lookAhead,stdin);
  }
  int factor=0;
  while(symbolCharTable[charRead] & 8){
    if(charRead == '.'){
      factor=1;
      symbolCharTable[charRead]=
      (symbolCharTable[charRead] ^ 8) & (symbolCharTable[charRead]);
    }
    else{
      factor*=10;
      number[(symbolCharTable['.'] & 8)/8]*=10;
      number[(symbolCharTable['.'] & 8)/8]+=charRead-'0';
    }
    charRead = fgetc(stdin);
  }
  if(charRead!=EOF){
    if(!(symbolCharTable[charRead] & 4)){ //If charRead isn't delimiter
      printf("Error: Failed to tokenize number\n");
      texit(1);
    }
  }
  ungetc(charRead,stdin);
  Value *newItem = talloc(sizeof(Value));
  if(symbolCharTable['.'] & 8){
    newItem->type = INT_TYPE;
    newItem->i = number[1];
    newItem->i = sign*number[1];
  }
  else {
    symbolCharTable['.'] |= 8;
    double d = (double) number[0] / (double) factor;
    newItem->type = DOUBLE_TYPE;
    newItem->d = (double) number[1];
    newItem->d +=d;
    newItem->d *= (double) sign;
  }
  return newItem;
}

Value *readString(char charRead){
  //Future problem: if size of String >256
  int strSize = 512;
  int nextStrLoc = 0;
  char finalStr[512];
  finalStr[nextStrLoc] = charRead; //adding quotation
  nextStrLoc++;
  charRead = fgetc(stdin);
  while(charRead != '\"'){
    if (charRead == EOF){
      printf("String syntax error, no ending quotes.\n");
      texit(0);
    }
    //Handling escape characters
    else if (charRead == '\\'){
      charRead = fgetc(stdin);
      if (charRead == EOF){
        printf("String syntax error, no ending quotes.\n");
        texit(0);
      }
      switch (charRead){
        case 'n':
          finalStr[nextStrLoc] = '\n';
          nextStrLoc++;
          break;
        case 't':
          finalStr[nextStrLoc] = '\t';
          nextStrLoc++;
          break;
        case '\\':
          finalStr[nextStrLoc] = '\\';
          nextStrLoc++;
          break;
        case '\'':
          finalStr[nextStrLoc] = '\'';
          nextStrLoc++;
          break;
        case '\"':
          finalStr[nextStrLoc] = '\"';
          nextStrLoc++;
          break;
      }
    }
    else{
      finalStr[nextStrLoc] = charRead;
      nextStrLoc++;
    }
    charRead = fgetc(stdin);
  }
  finalStr[nextStrLoc] = charRead; //adding ending quotation
  nextStrLoc++;
  finalStr[nextStrLoc] = '\0';
  nextStrLoc++;
  char *talString = talloc(sizeof(char)*nextStrLoc);
  sprintf(talString,"%s",finalStr);
  Value *newItem = talloc(sizeof(Value));
  newItem->type = STR_TYPE;
  newItem->s = talString;
  return newItem;
}

Value *readSymbol(char charRead, int *symbolCharTable){
  char *out= "";
  int len=1;
  char *new;
  while(symbolCharTable[charRead] & 2){ //Check for subsequent bit
    len++;
    new=talloc(len*sizeof(char)); //Need tresize. Seriously.
    sprintf(new,"%s%c",out,tolower(charRead));
    out=new;
    charRead = fgetc(stdin);
    if(charRead == EOF){
      break;
    }
  }
  ungetc(charRead,stdin);
  Value *add = talloc(sizeof(Value));
  add->type=SYMBOL_TYPE;
  add->sym=out;
  return add;
}

Value *readBool(char charRead){
      bool temp;
      charRead = fgetc(stdin);
      if (charRead == EOF){
        printf("read: bad syntax '#'\n");
        texit(1);
      }
      else if (charRead == 't'){
        temp = true;
      }
      else if (charRead == 'f'){
        temp = false;
      }
      else{
        printf("read: bad syntax '#%c'\n",charRead);
        texit(1);
      }

      Value *new = talloc(sizeof(Value));
      new->type=BOOL_TYPE;
      new->b = temp;
  return new;
}

Value *tokenize() {
  int symbolCharTable[128]={}; //Used to make test for symbol run faster.
  char *symbolInitials =
  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!$%&*/:<=>?~_^";
  char *symbolSubsequents = "1234567890.+-@";
  char *delimiters = " \n\t)";
  char *numberInitials="0123456789.";
  int i=0;
  for(i=0;symbolInitials[i]!=atoi("\0");i++){
    symbolCharTable[symbolInitials[i]]|=3; //OR check bits if initial
  }
  for(i=0;symbolSubsequents[i]!=atoi("\0");i++){
    symbolCharTable[symbolSubsequents[i]] |= 2; //OR check bits if subsequent
  }
  for(i=0;delimiters[i]!=atoi("\0");i++){
    symbolCharTable[delimiters[i]]|=4; //OR bits if delimiter.
  }
  for(i=0;numberInitials[i]!=atoi("\0");i++){
    symbolCharTable[numberInitials[i]]|=8; //OR bits if delimiter.
  }
  char charRead;
  Value *list = makeNull();
  charRead = fgetc(stdin);
  while (charRead != EOF) {
    //Handling parens. Solves conflict with )
    if(charRead == '('){
      Value *newItem = talloc(sizeof(Value));
      newItem->type = OPEN_TYPE;
      newItem->po='(';
      list=cons(newItem,list);
      charRead = fgetc(stdin);
    }
    else if(charRead == ')'){
      Value *newItem = talloc(sizeof(Value));
      newItem->type = CLOSE_TYPE;
      newItem->pc=')';
      list=cons(newItem,list);
      charRead = fgetc(stdin);
    }
    //Handling whitespace via lookup
    else if(symbolCharTable[charRead] & 4)
    {
      charRead = fgetc(stdin);
    }
    //Handling + and - symbols
    else if(charRead == '+' || charRead =='-'){
      char lookAhead;
      lookAhead=fgetc(stdin);
      if(symbolCharTable[lookAhead] & 4){ //Lookahead for delimiter
        Value *add = talloc(sizeof(Value));
        add->type=SYMBOL_TYPE;
        add->sym=talloc(2*sizeof(char));
        sprintf(add->sym,"%c",tolower(charRead));
        list=cons(add,list);
        charRead=lookAhead;
      }
      else{ //Handling numbers - Should be a number
        int sign=1;
        if(charRead== '-'){ //Maybe a negative
          sign = -1;
        }
        charRead=lookAhead; //Basically fgetc
        list = cons(readNum(charRead,sign,symbolCharTable),list);
        charRead=fgetc(stdin);
      }
    }
    //Handling Strings
    else if (charRead == '\"'){
      list = cons(readString(charRead), list);
      charRead = fgetc(stdin);
    } //End String Handling


    //Handling ';' Comments
    else if (charRead == ';'){
      charRead = fgetc(stdin);
      while (charRead != '\n'){
        if (charRead == EOF){
          break;
        }
        charRead = fgetc(stdin);
      }
      charRead = fgetc(stdin);
    }

    //Handling Numbers - See handling + - for details
    else if(symbolCharTable[charRead] & 8){
      list = cons(readNum(charRead,1,symbolCharTable),list);
      charRead=fgetc(stdin);
    }

    //Handling Booleans
    else if (charRead == '#'){
      list = cons(readBool(charRead),list);
      charRead = fgetc(stdin);
    }

    //Handling Symbols
    else if(symbolCharTable[charRead] & 1 ){ //AND to check for initial bit
      list=cons(readSymbol(charRead,symbolCharTable),list);
      charRead=fgetc(stdin);
    }
    //Tokenizing failure.
    else{
      printf("Error: Failed to tokenize\n");
      texit(1);
    }
  }
  return reverse(list); // see below
}


//displayTokens not done yet.
void displayTokens(Value *list){
  assert(list!=NULL);
  switch(list->type){
    case CONS_TYPE:
      displayTokens(list->c.car);
      displayTokens(list->c.cdr);
      break;
    case INT_TYPE:
      printf("%d:integer\n",list->i);
      break;
    case DOUBLE_TYPE:
      printf("%f:double\n",list->d);
      break;
    case STR_TYPE:
      printf("%s:string\n",list->s);
      break;
    case OPEN_TYPE:
      printf("%c:open\n",list->po);
      break;
    case CLOSE_TYPE:
      printf("%c:close\n", list->pc);
      break;
    case BOOL_TYPE:
      if (list->b){
        printf("#t:boolean\n");
      }
      else{
        printf("#f:boolean\n");
      }
      break;
    case SYMBOL_TYPE:
      printf("%s:symbol\n",list->sym);
      break;
    default:
      break;
  }
}

#endif
