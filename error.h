#ifndef ERROR_H
#define ERROR_H

struct ErrorInfo;

typedef struct ErrorInfo ErrorInfo;

struct BasicString {
  char *data;
  int len;
};

typedef struct BasicString BasicString;

struct ErrorInfo {
  BasicString msg, file_name, func_name, code;
  int line;
  ErrorInfo *next;
};

/**
 * Will attempt to link a new error node in the current structure. If that's
 * not possible, errors will be printed to the console.
 *
 * Returns 0 on succes and negative values on failure, as usual.
 */
int
link_error(const char *msg,
           int line,
           const char *file_name,
           const char *func_name,
           const char *code);

/**
 * Copies and returns the current internal error structure. You're responsible
 * to call clean_error on it later.
 */
ErrorInfo*
get_error(void);

/**
 * Passing null as the argument will clean the internally kept error
 * structure.
 *
 * Remember to call this function before closing your program or else memory
 * leak detecting tools will annoy you.
 */
void
free_error(ErrorInfo *err);

/*
 * Control flow macros for simpler error handling.
 */

/**
 * Macro expands to:
 *
 * do {
 *  int ERROR_H__err = (EXPR);
 *  if (ERROR_H__err < 0) return ERROR_H__err;
 * } while (0)
 *
 * If ERROR_H__err conflicts with an identifier you have in your code, I'm
 * sorry for you.
 */
#define COND_RET_LT0(EXPR) \
  do { \
    int ERROR_H__err = (EXPR); \
    if (ERROR_H__err < 0) return ERROR_H__err; \
  } while (0)

/**
 * If (EXPR) < 0, goto LABEL.
 */
#define COND_GOTO_LT0(EXPR, LABEL) \
  do { \
    if ((EXPR) < 0) goto LABEL; \
  } while (0)

/**
 * If !(COND), then goto LABEL.
 */
#define COND_GOTO_IF0(COND, LABEL) \
  do { \
    if (!(COND)) goto LABEL; \
  } while (0)

/*
 * Control flow macros for error handling that also sets error information
 * through link_error calls.
 *
 * You can always pass 0 (or any null-ish eqv) where MSG is required. This
 * will cause a 'messageless' error to be set up (file name and line number
 * are still recorded).
 *
 * Some macros will have EGOTO or ERET in the middle. Some others will have
 * PGOTO or PRET. The 'P' prefix stands for "pass error along". It's equivalent
 * to the 'E' prefix macros, but with 0 as the MSG parameter.
 */

/**
 * If (EXPR) < 0, then set error information and goto GOTO.
 */
#define COND_EGOTO_LT0(EXPR, GOTO, MSG) do { \
    if ((EXPR) < 0) { \
      link_error((MSG), __LINE__, __FILE__, __func__, # EXPR); \
      goto GOTO; \
    } \
  } while (0)

#define COND_PGOTO_LT0(EXPR, GOTO) COND_EGOTO_LT0(EXPR, GOTO, 0)

/**
 * If !(COND), then set error information and goto LABEL.
 */
#define COND_EGOTO_IF0(COND, LABEL, MSG) do { \
    if (!(COND)) { \
      link_error((MSG), __LINE__, __FILE__, __func__, # COND); \
      goto LABEL; \
    } \
  } while (0)

#define COND_PGOTO_IF0(COND, LABEL) COND_EGOTO_IF0(COND, LABEL, 0)

/**
 * If (COND) is true-ish, then set error information and goto GOTO.
 */
#define COND_EGOTO(COND, LABEL, MSG) do { \
    if ((COND)) { \
      link_error((MSG), __LINE__, __FILE__, __func__, # COND); \
      goto LABEL; \
    } \
  } while (0)

#define COND_PGOTO(COND, LABEL) COND_EGOTO(COND, LABEL, 0)

/**
 * If e = (EXPR) and e < 0, set error information and return e.
 */
#define COND_ERET_LT0(EXPR, MSG) do { \
    int ERROR_H__err = (EXPR); \
    if (ERROR_H__err < 0) { \
      link_error((MSG), __LINE__, __FILE__, __func__, # EXPR); \
      return ERROR_H__err; \
    } \
  } while (0)

#define COND_PRET_LT0(EXPR) COND_ERET_LT0(EXPR, 0)

/**
 * If !(COND), set error information and return 0.
 */
#define COND_ERET_IF0(COND, VAL, MSG) do { \
    if (!(COND)) { \
      link_error((MSG), __LINE__, __FILE__, __func__, # COND); \
      return (VAL); \
    } \
  } while (0)

#define COND_PRET_IF0(EXPR) COND_ERET_IF0(COND, VAL, 0)

/**
 * If (COND), set error information and return VAL.
 */
#define COND_ERET(COND, VAL, MSG) do { \
    if (COND) { \
      link_error((MSG), __LINE__, __FILE__, __func__, # COND); \
      return (VAL); \
    } \
  } while (0)

#define COND_PRET(COND, VAL) COND_ERET(COND, VAL, 0)

/**
 * Special macro to syntactically indicate that you're ignoring an error. This
 * doesn't affect the code at all. ERR_IGNORE(X) is just the same as just
 * writing X.
 */
#define ERR_IGNORE(STMT) STMT

#endif
