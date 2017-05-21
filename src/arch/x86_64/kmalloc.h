/* Gloria Liu
   CPE 453-03
   Assignment #1
 
   malloc.h 
 
   Header file for malloc.c
 */
 
#include <stdint.h>
void free(void *ptr);
void *malloc(uint64_t size);
void *calloc(uint64_t nmemb, uint64_t size);
void *realloc(void *ptr, uint64_t size);


