typedef const char *(*ErrorFn)(void);

extern ErrorFn errorFn;

/**
 * If (COND) is false-ish, then set errorFn to (FN) and goto (GOTO).
 */
#define COND_ERROR_SET(COND, GOTO, FN) do { if (!(COND)) { \
  errorFn = (FN); goto GOTO; } } while (0)

/**
 * IF (COND) is false-ish, then goto (GOTO).
 */
#define COND_ERROR(COND, GOTO) do { if (!(COND)) { goto GOTO; } } while (0)

