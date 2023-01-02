#ifndef INFINITY_COMPILER_LIST_H
#define INFINITY_COMPILER_LIST_H

#include <stdio.h>

typedef struct {
    void **items;
    size_t size;
    size_t item_size;
} List;

List *init_list(size_t item_size);

void list_dispose(List *list);

void *list_get_item(List *list, size_t index);

void *list_get_address_by_index(List *list, size_t index);

void list_push(List *list, void *item);

void *list_pop(List *list);

// inserts `item` at index `idx`
void list_insert(List *list, size_t idx, void *item);

void list_print_integers(const List *list);

void list_print(const List *list, char *(*print_item)(const void *));

#endif //INFINITY_COMPILER_LIST_H
