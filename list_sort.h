// Type declarations associated with sorting linked lists, and a registry of
// known sorting functions.
// 
// Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
// SPDX-License-Identifier:  CC-BY-SA-4.0
#ifndef LIST_SORT_H_
#define LIST_SORT_H_

#include <stdbool.h>
#include <stddef.h>
#include "list_node.h"

// Function type for node comparison functions.  Returns true if the first
// argument is less than the second argument.
typedef bool ListNodeCompareFxn(const ListNode*, const ListNode*);

// Function type for list sort functions.  Returns the new head of a list.
typedef ListNode *ListSortFxn(ListNode*, ListNodeCompareFxn*);

// Defines a registry of sorting algorithms for the benchmarks to refer to, so
// that we don't have to edit the benchmark drivers every time we add an
// algorithm.
typedef struct sort_registry {
  size_t length;
  struct {
    const char *name;
    ListSortFxn *fxn;
  } entry[];
} SortRegistry;

extern const SortRegistry sort_registry;

#endif  // LIST_SORT_H_
