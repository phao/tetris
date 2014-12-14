#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "error.h"

/*
 * The "no dynamic allocation" constraint is off here, but this could surely
 * be improved by some form of custom allocation scheme. I don't think it's
 * worth the trouble.
 */

static struct ErrorInfo *err_info;

static int
set_string(struct BasicString *string, const char *src) {
  if (src) {
    int src_len = strlen(src);
    free(string->data);
    string->data = malloc(src_len+1);
    if (!string->data) {
      string->len = -1;
      return -1;
    }
    strcpy(string->data, src);
    string->len = src_len;
  }
  else {
    string->data = 0;
    string->len = -1;
  }
  return 0;
}

static struct ErrorInfo*
link_error_to(struct ErrorInfo *old_head,
              const char *msg,
              int line,
              const char *file_name,
              const char *func_name,
              const char *code)
{
  struct ErrorInfo *new_head = malloc(sizeof (struct ErrorInfo));
  if (!new_head) {
    return 0;
  }
  new_head->line = line;
  new_head->next = old_head;
  new_head->msg.data = 0;
  new_head->file_name.data = 0;
  new_head->func_name.data= 0;
  new_head->code.data = 0;
  ERR_IGNORE(set_string(&new_head->msg, msg));
  ERR_IGNORE(set_string(&new_head->file_name, file_name));
  ERR_IGNORE(set_string(&new_head->func_name, func_name));
  ERR_IGNORE(set_string(&new_head->code, code));
  return new_head;
}

int
link_error(const char *msg,
           int line,
           const char *file_name,
           const char *func_name,
           const char *code)
{
  struct ErrorInfo *new_head = link_error_to(err_info, msg, line, file_name,
    func_name, code);
  if (!new_head
      || (!new_head->msg.data && msg)
      || (!new_head->file_name.data && file_name)
      || (!new_head->func_name.data && func_name)
      || (!new_head->code.data && code))
  {
    fprintf(stderr, "%s: %s: %d: %s\n\t%s\n", file_name, func_name, line, msg,
      code);
    return -1;
  }
  err_info = new_head;
  return 0;
}

struct ErrorInfo*
get_error(void) {
  struct ErrorInfo *out = 0;
  for (struct ErrorInfo *n = err_info; n; n = n->next) {
    struct ErrorInfo *aux = link_error_to(out, n->msg.data, n->line,
      n->file_name.data, n->func_name.data, n->code.data);
    if (aux) {
      out = aux;
    }
  }
  return out;
}

void
free_error(struct ErrorInfo *err) {
  if (!err) {
    err = err_info;
    err_info = 0;
  }
  while (err) {
    struct ErrorInfo *aux = err->next;
    free(err->msg.data);
    free(err->file_name.data);
    free(err->func_name.data);
    free(err->code.data);
    free(err);
    err = aux;
  }
}
