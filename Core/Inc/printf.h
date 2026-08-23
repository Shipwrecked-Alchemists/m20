#ifndef PRINTF_H
#define PRINTF_H

#include <stdarg.h>
#include <stdio.h>

#if ENABLE_PRINTF
int n_printf(const char* format, ...);
int n_fprintf(FILE* stream, const char* format, ...);
int n_vprintf(const char* format, va_list ap);
int n_vfprintf(FILE* stream, const char* format, va_list ap);

int n_sprintf(char* str, const char* format, ...);
int n_snprintf(char* str, size_t size, const char* format, ...);
int n_vsprintf(char* str, const char* format, va_list ap);
int n_vsnprintf(char* str, size_t size, const char* format, va_list ap);
#else
#define n_printf(format, ...);
#define n_fprintf(stream, format, ...);
#define n_vprintf(format, ap);
#define n_vfprintf(stream, format, ap);

#define n_sprintf(str, format, ...);
#define n_snprintf(str, size, format, ...);
#define n_vsprintf(str, format, ap);
#define n_vsnprintf(str, size, format, ap);
#endif

#endif
