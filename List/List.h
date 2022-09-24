#pragma once
#include <stdlib.h>
#include <stdbool.h>

typedef struct _List* List;

List 	List_create(size_t element_size);
void	List_free(List l);
void*   List_get(List l, signed long long int index);
size_t 	List_size(List l);
void*	List_start(List l);
void*	List_end(List l);
size_t 	List_capacity(List l);
bool 	List_reserve(List l, size_t capacity);
void*	List_find(List l, bool (*compare)(void*, void*), void* arg);
void*	List_append(List l, const void *element);
void*   List_pop(List l);
bool	List_copy(List a, List b); // copy b into a
void    List_clear(List l);
void*   Buff_find(char *start, char *end, size_t el_size, bool (*compare)(void*, void*), void *arg);
void    List_foreach(List l, void (*func)(void*));
void    List_remove(List l, size_t index);
void    List_concat(List a, List b); // cat b to a
void    List_grow(List l, size_t size);