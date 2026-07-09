#ifndef PRINTF_H
#define PRINTF_H

#include <stdarg.h>
#include <stdio.h>

int n_printf(const char* format, ...);
int n_fprintf(FILE* stream, const char* format, ...);
int n_vprintf(const char* format, va_list ap);
int n_vfprintf(FILE* stream, const char* format, va_list ap);

int n_sprintf(char* str, const char* format, ...);
int n_snprintf(char* str, size_t size, const char* format, ...);
int n_vsprintf(char* str, const char* format, va_list ap);
int n_vsnprintf(char* str, size_t size, const char* format, va_list ap);

#endif
