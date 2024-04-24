#ifndef UTILS_H
#define UTILS_H

#define TODO() fprintf(stderr, "%d:%s: TODO: `%s` is not implemented yet", __LINE__, __FILE__, __func__); exit(1);

#define return_defer(code) result = code; goto defer;

#define FE_0(WHAT)
#define FE_1(WHAT, X) WHAT(X) 
#define FE_2(WHAT, X, ...) WHAT(X)FE_1(WHAT, __VA_ARGS__)
#define FE_3(WHAT, X, ...) WHAT(X)FE_2(WHAT, __VA_ARGS__)
#define FE_4(WHAT, X, ...) WHAT(X)FE_3(WHAT, __VA_ARGS__)
#define FE_5(WHAT, X, ...) WHAT(X)FE_4(WHAT, __VA_ARGS__)

#define GET_MACRO(_0,_1,_2,_3,_4,_5,NAME,...) NAME 
#define FOR_EACH(action,...) GET_MACRO(_0,__VA_ARGS__,FE_5,FE_4,FE_3,FE_2,FE_1,FE_0)(action,__VA_ARGS__)

#define UNUSED(X) (void)X;
  
#endif

#ifdef UTILS_IMPLEMENTATION

#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  LOG_ERR,
  LOG_WARN,
  LOG_INFO,
} LogLevel;

void log_def(LogLevel level, const char *fmt, ...) {
  FILE *stream;
  switch (level) {
    case LOG_ERR: stream = stderr; fprintf(stream, "[ERR] "); break;
    case LOG_WARN: stream = stderr; fprintf(stream, "[WARN] "); break;
    case LOG_INFO: stream = stdout; fprintf(stream, "[INFO] "); break;
  }

  va_list args;
  va_start(args, fmt);
  vfprintf(stream, fmt, args);
  va_end(args);

  fprintf(stream, "\n");
}

#define log_err(...) log_def(LOG_ERR, __VA_ARGS__)
#define log_warn(...) log_def(LOG_WARN, __VA_ARGS__)
#define log_info(...) log_def(LOG_INFO, __VA_ARGS__)

#define print_errno(fmt, ...)                                                  \
  log_err(fmt, __VA_ARGS__);                                                   \
  fprintf(stderr, ": %s\n", strerror(errno));

char *slurp_file(const char *file_path) {
#define panic()                                                                \
  {                                                                            \
    print_errno("could not read file `%s`", file_path);                        \
    if (f) fclose(f);                                                          \
    exit(1);                                                                   \
  }

  FILE *f = fopen(file_path, "r");
  if (!f) panic();

  if (fseek(f, 0, SEEK_END) < 0) panic();

  long size = ftell(f);
  if (size < 0) panic();

  char *buffer = malloc(size + 1);
  if (!buffer) panic();

  if (fseek(f, 0, SEEK_SET) < 0) panic();

  fread(buffer, 1, size, f);
  if (ferror(f) < 0) panic();
  fclose(f);

  buffer[size] = '\0';
  return buffer;

#undef panic
}

#endif
