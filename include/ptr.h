#ifndef __POINTER_LIBRARY_H__
#define __POINTER_LIBRARY_H__
#include <stdlib.h>

/**
 * check if pointer is valid which is not NULL
 */
static inline bool valid_ptr(void*ptr){
  return ptr != NULL;
}

/**
 * @brief Deallocates pointer memory if valid 
 * @param[out] ptr pointer to memory address will. Will be set to NULL
 */
static inline void free_ptr(void* ptr){
  if (valid_ptr(ptr)){
    free(ptr);
    ptr = NULL;
  }
}


#endif