#include "mpc.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef WIN_32

#include <string.h>

static char buffer[2048];

char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);

  char* copy = malloc(strlen(buffer)+1);
  strcpy(copy, buffer);
  copy[strlen(copy)-1] = '\0';

  return copy;
}

void add_history(char* unused) {}

#else

#include <editline/readline.h>

#endif

typedef struct {
  int type;
  long num;
  int err;
} lval;

/* lval types */
enum { LVAL_NUM, LVAL_ERR };

/* Error types */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

lval lval_num(long x) {
  lval v;
  v.type = LVAL_NUM;
  v.num = x;

  return v;
}

lval lval_err(int x) {
  lval v;
  v.type = LVAL_ERR;
  v.err = x;

  return v;
}

void lval_print(lval v) {
  switch (v.type) {
    case LVAL_NUM: printf("%li", v.num); break;

    case LVAL_ERR:
      if (v.err == LERR_DIV_ZERO) { printf("Error: Division by zero!"); }
      if (v.err == LERR_BAD_OP)   { printf("Error: Invalid operator!"); }
      if (v.err == LERR_BAD_NUM)  { printf("Error: Invalid number!"); }
    break;
  }
}

void lval_println(lval v) { lval_print(v); putchar('\n'); }

lval eval_op(lval x, char* op, lval y) {
  if (x.type == LVAL_ERR) { return x; }
  if (y.type == LVAL_ERR) { return y; }

  if (strcmp(op, "+") == 0) { return lval_num(x.num + y.num); }
  if (strcmp(op, "-") == 0) { return lval_num(x.num - y.num); }
  if (strcmp(op, "*") == 0) { return lval_num(x.num * y.num); }
  if (strcmp(op, "/") == 0) {
    return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num);
  }

  return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t) {
  if (strstr(t->tag, "number")) {
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
  }

  char* op = t->children[1]->contents;
  lval x = eval(t->children[2]);

  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }

  return x;
}

int main(int argc, char** argv) {
  /* Parsers */
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* Mike     = mpc_new("mike");

  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                    \
      number   : /-?[0-9]+/ ;                            \
      operator : '+' | '-' | '*' | '/' ;                 \
      expr     : <number> | '(' <operator> <expr>+ ')' ; \
      mike     : /^/ <operator> <expr>+ /$/ ;            \
    ",
    Number, Operator, Expr, Mike);

  puts("Mike - Version 0.0.1");
  puts("Press Ctrl+c to exit\n");

  while(1) {
    char* input = readline("mike> ");
    add_history(input);
  
    /* Parse the input */
    mpc_result_t result;
    if (mpc_parse("<stdin>", input, Mike, &result)) {
      lval res = eval(result.output);
      lval_println(res);
      mpc_ast_delete(result.output);
    } else {
      /* Print the error */
      mpc_err_print(result.error);
      mpc_err_delete(result.error);
    }

    free(input);
  }

  /*  Undefine and Delete our Parsers */
  mpc_cleanup(4, Number, Operator, Expr, Mike);

  return 0;
}
