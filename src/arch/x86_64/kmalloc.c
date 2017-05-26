/* Gloria Liu 
   Assignment #1 
   CPE453-03
   
   malloc.c
   Implements my own version of std lib functions:
 
   malloc, calloc, free, and realloc.
 
   void *malloc(size_t size):
      -allocates size bytes in memory
      -returns a void pointer to the beginning of allocation
      -returns null if memory cannot be allocated
 
   void *calloc(size_t nmemb, size_t size):
      -allocates nmemb * size bytes in memory
      -behaves the same as malloc, except mem initialized to zero
 
   void free(void *ptr):
      -frees the allocated block of memory ptr is in
   
   void *realloc(void *ptr, size_t size):
      -reallocates the block ptr is in to size
      -if called with a NULL ptr, then it calls malloc(size)
      -if called with a non NULL ptr and size zero, it calls free(ptr)
      -returns pointer to newly allocated memory, or null if failure
 */

#include "kmalloc.h"
#include "page_table.h"
#include "vga.h"


#define FALSE 0
#define TRUE 1
#define CHUNK_SIZE 64000
#define HEADER_SIZE 32
#define TWO_BYTES 16
#define ksbrk_FAILED ((void *)-1)
#define NULL 0

typedef struct header {
   int free;
   uint64_t size;
   struct header * next;
} header;

header *listHead = NULL;
void * curBreak = 0;


/* helper functions */
uint64_t nextDivBy16(uint64_t num) {
   if (num  % TWO_BYTES) {
      return (num / TWO_BYTES + 1) * TWO_BYTES;
   }
   else {
      return num;
   }
}

/* implemented functions */
void *calloc(uint64_t nmemb, uint64_t size) {
   void * ptr = malloc(nmemb * size);
   
   if (ptr) {
      uint64_t i = 0;
      
      char *ptr2 = (char *) ptr;
    //TODO: do a memset here instead
      for (i = (uint64_t) (ptr + HEADER_SIZE); i < nextDivBy16(size); i++) {
         ptr2[i] = '\0';
      }
      return ptr;
   }
   
   return NULL;
}
   

void free(void *ptr) {
   
   header *currentNode = listHead;
   header *prevNode = NULL;
   header *nextNode = NULL;


   if (ptr) {

      while (currentNode != NULL) {
         
         if (ptr - HEADER_SIZE == currentNode) { 
            currentNode->free = 1;

            //printk("something was actually freed\n");
            currentNode = NULL;           
         }

        if (currentNode) {
            prevNode = currentNode;
            currentNode = currentNode->next;
        }  
      }
   }
}

void *malloc(uint64_t size) {
   uint64_t sizeForAddress = nextDivBy16(size);
   header *currentNode;
   header *lastNode;
   header *newNode;
   void *tempBreak;
   void *tempBreak2;
   
   int initialChunk = CHUNK_SIZE;

   if (listHead == NULL) {
      /* initial call to malloc */
      curBreak = ksbrk(0);
      
      tempBreak = curBreak;
      
      if (sizeForAddress > CHUNK_SIZE) {
         initialChunk = sizeForAddress + CHUNK_SIZE;
      }
      
      curBreak = ksbrk(initialChunk);
      
      newNode = (header *) tempBreak;
      newNode->free = 0;
      newNode->size = sizeForAddress;
      newNode->next = NULL;
      
      listHead = newNode;

      return (void *) newNode + HEADER_SIZE;
      
   }
   else {

      /* traverse linked list to find a suitable free node */
      
      currentNode = listHead;
      while (currentNode != NULL) {
         
         if (currentNode->free) {
            if (currentNode->size >= sizeForAddress) {
               currentNode->free = 0;
               
               return (void *) currentNode + HEADER_SIZE;
               
            }
         }
         
         lastNode = currentNode;
         currentNode = currentNode->next;
         
      }
      
      /* suitable node was not found, create a new node.
         call ksbrk if necessary */
      
      if (((void *) lastNode + HEADER_SIZE + lastNode->size + HEADER_SIZE +
       sizeForAddress) > curBreak) {
         curBreak = ksbrk(CHUNK_SIZE + sizeForAddress);

      }
      
      /* only gets here if a free and valid node was not found while
         traversing the list. Create a new node. */
      
      newNode = (header *) ((void *) lastNode +
       HEADER_SIZE + lastNode->size);

      /* lastNode is currently a pointer to the last node in list*/
      lastNode->next = newNode;
      
      newNode->free = 0;
      newNode->size = sizeForAddress;
      newNode->next = NULL;
      
      
      return (void *) newNode + HEADER_SIZE;
      
   }
   
}

