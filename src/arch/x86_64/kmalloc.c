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

void mergeNodes(header *firstNode, header *secondNode) {
   firstNode->size = firstNode->size + secondNode->size + HEADER_SIZE;
   firstNode->next = secondNode->next;
   
}

void moveNodeDataFrom(header *sourceNode, header *destNode) {
   memcpy((void *) destNode + HEADER_SIZE,
    (void *)sourceNode + HEADER_SIZE, sourceNode->size);
}

int containsPtr(void *ptr, header *node) {
   if (ptr == NULL) {
      return FALSE;
   }
   
   if (ptr >= (void *) node + HEADER_SIZE && ptr < (void *)
    node + HEADER_SIZE + node->size) {
      return TRUE;
   }
   return FALSE;
}

/* implemented functions */
void *calloc(uint64_t nmemb, uint64_t size) {
   void * ptr = malloc(nmemb * size);
   
   if (ptr) {
      uint64_t i = 0;
      
      char *ptr2 = (char *) ptr;
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
         
         if (containsPtr(ptr, currentNode)) {
            
            currentNode->free = 1;
            nextNode = currentNode->next;
            
            if (prevNode && prevNode->free) {
               mergeNodes(prevNode, currentNode);

               if (nextNode && nextNode->free) {
                  mergeNodes(prevNode, nextNode);
               }
               
            }
            else if (nextNode && nextNode->free) {
               
               mergeNodes(nextNode, currentNode);
               
            }
            
         }
         prevNode = currentNode;
         currentNode = currentNode->next;
         
      }
   }
}


void *realloc(void *ptr, uint64_t size) {
   header *currentNode = NULL;
   header *swapNode = NULL;
   header *foundNode = NULL;
   header *newNode = NULL;
   header *endNode = NULL;
   header *prevNode = NULL;
   
   void *tempBreak = NULL;
   
   
   int endHeapCase = FALSE;
   int noMemoryLeft = FALSE;
   
   
   if (!ptr) {
      return malloc(size);
   }
   
   if (!size) {
      free(ptr);
      return NULL;
   }
   
   /* finding last node and setting endNode to it */
   currentNode = listHead;
   while (currentNode->next != NULL) {
      currentNode = currentNode->next;
   }
   endNode = currentNode;

   /* finding the node the ptr parameter is referencing */
   currentNode = listHead;
   while (currentNode != NULL) {
      if (containsPtr(ptr, currentNode)) {
         foundNode = currentNode;
         break;
      }
      prevNode = currentNode;
      currentNode = currentNode->next;
   }
   
   
   /* case where the ptr parameter is invalid*/
   if (foundNode == NULL) {

      return NULL;
   }
   
   
   /* CASE 1 (BEST): current node is big enough, don't do anything*/
   if (foundNode->size >= nextDivBy16(size)) {

      return (void *) foundNode + HEADER_SIZE;
   }
   
   /* CASE 2: (2ND BEST) next node is free,
      and next node's size plus ptr node's size is big enough.
      Merge them. */
   if (foundNode->next && foundNode->next->free) {
      if (foundNode->next->size + foundNode->size >= nextDivBy16(size)) {
         mergeNodes(foundNode, foundNode->next);

         return (void *) foundNode + HEADER_SIZE;
      }
   }
   
   
   /* CASE 3: It's the last node and ksbrk doesn't need to be called
      Resize node. */
   if (foundNode->next == NULL) {
      if ((void *) foundNode + HEADER_SIZE + nextDivBy16(size) < curBreak) {
         
         foundNode->size = nextDivBy16(size);
         
         return (void *) foundNode + HEADER_SIZE;
      }
   }

   /* check for CASE 4: "endHeapCase" (the ptr node is the 2nd to last node,
    and the last node is free */
   if (foundNode->next) {
      if (foundNode->next->next == NULL) {
         if (foundNode->next->free) {
            endHeapCase = TRUE;
         }
      }
   }
   
   /* CASE4: ("endHeapCase") foundNode is 2nd to last node,
      the last node is free, but the two combined are not large enough*/
   
   if (endHeapCase) {
      tempBreak = curBreak;
      
      /* checking to see if a call of ksbrk is need */
      if ((void *) foundNode + HEADER_SIZE + nextDivBy16(size) > curBreak) {
         
         tempBreak = ksbrk(CHUNK_SIZE + nextDivBy16(size));
         
         if (tempBreak != ksbrk_FAILED) {
            curBreak = tempBreak;
         }
         else {
            noMemoryLeft = TRUE;
         }
         
      }
      
      if (!noMemoryLeft) {
         
         /* ksbrk didn't fail, or ksbrk didn't need to be called because
            there is still space in the allocated memory.
            Node is just resized */
         
         foundNode->size = nextDivBy16(size);
         foundNode->next = NULL;
         
         return (void *) foundNode + HEADER_SIZE;
      }
   }
   
   
   /* Reaches here if endHeapCase doesn't work out
      or if it's not the endHeapCase */

   
   /* find a free node to swap data to*/
   int swapFound = FALSE;
   currentNode = listHead;
   while (currentNode != NULL) {
      
      if (currentNode->free && currentNode->size >= nextDivBy16(size)) {
         swapNode = currentNode;
         swapFound = TRUE;
         break;
      }
      currentNode = currentNode->next;
   }
   
   
   if (!swapFound) {
      /* no free nodes big enough,
         new node needs to be made at the end.
         ksbrk might need to be called */
      
      if (noMemoryLeft) {
         return NULL;
      }
      
      tempBreak = NULL;
      
      if ((void *) endNode + HEADER_SIZE + endNode->size + HEADER_SIZE +
       nextDivBy16(size) > curBreak) {

         tempBreak = ksbrk(CHUNK_SIZE + nextDivBy16(size));
         if (tempBreak != ksbrk_FAILED) {
            curBreak = tempBreak;
         }
         else {
            return NULL;
         }
      }
      
      /* create a new node and copy data over */
      newNode = (header *) ((void *) endNode +
       HEADER_SIZE + endNode->size);
      
      endNode->next = newNode;
      
      newNode->free = 0;
      newNode->size = nextDivBy16(size);
      newNode->next = NULL;
      
      moveNodeDataFrom(foundNode, newNode);
      free(foundNode);

      return (void *) newNode + HEADER_SIZE;
   }
   else {
      /* suitable free node found, move data over*/
      moveNodeDataFrom(foundNode, swapNode);
      free(foundNode);
      
      return (void *) swapNode + HEADER_SIZE;
      
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
      
      tempBreak2 = ksbrk(initialChunk);
      
      if (tempBreak2 != ksbrk_FAILED) {
         curBreak = tempBreak2;
      }
      else {
         return NULL;
      }
      
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
         tempBreak = ksbrk(CHUNK_SIZE + sizeForAddress);
         
         if (tempBreak != ksbrk_FAILED) {
            curBreak = tempBreak;
         }
         else {
            return NULL;
         }

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

