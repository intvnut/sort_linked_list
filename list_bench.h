// Benchmarking interface for ListNodes.
//
// Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
// SPDX-License-Identifier:  CC-BY-SA-4.0
#ifndef LIST_BENCH_H_
#define LIST_BENCH_H_
#include <stdbool.h>
#include <stddef.h>

#include "list_node.h"
#include "list_sort.h"

// Treats a buffer as an array of a particular list node type, returning a
// ListNode* to a given index.
typedef ListNode *ListNodeGetFxn(void *buf, size_t index);

// Randomizes the value a list node of a particular type, given a ListNode*.
typedef void ListNodeRandomizeFxn(ListNode *node);

// Returns an index-sensitive checksum for a list node of a particular type.
typedef uint64_t ListNodeChecksumFxn(const ListNode *node, size_t index);

// Returns false if a list node of a particular type violates an internal
// constraint, given a ListNode*.
typedef bool ListNodeValidateFxn(const ListNode *node);

// Provides a set of function pointers for working with list nodes of different
// types in a generic manner.
typedef struct {
  size_t size;  // Holds the size of one element of this type.
  ListNodeGetFxn *get;
  ListNodeRandomizeFxn *randomize;
  ListNodeCompareFxn *compare;
  ListNodeChecksumFxn *checksum;
  ListNodeValidateFxn *validate;
} ListNodeBenchOps;

#endif  // LIST_BENCH_H_
