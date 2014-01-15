/**
 * @file   smemory.h
 * @author mathslinux <riegamaths@gmail.com>
 * @date   Tue May 22 00:41:48 2012
 *
 * @brief  Small Memory Wrapper
 * 
 * 
 */


#ifndef SMEMORY_H
#define SMEMORY_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void *s_malloc(size_t size);
void *s_malloc0(size_t size);
void *s_calloc(size_t nmemb, size_t lsize);
void *s_realloc(void *ptr, size_t size);
//void s_free(void *p);
char *s_strdup(const char *s1);
long s_atol(const char* s,long init);
#define s_atoi(s,init) s_atol(s,init)
#define s_free(p) (p=p?free(p),NULL:NULL)
#if 0
char *s_strndup(const char *s1, size_t n);
int s_vasprintf(char **buf, const char * format, va_list arg);
int s_asprintf(char **buf, const char *format, ...);
#endif

#endif  /* SMEMORY_H */
