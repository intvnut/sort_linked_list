// Defines a base ListNode, as well as derived Int64ListNode and
// CachelineListNode data types.  Declares types for functions that compare
// list nodes and sort list nodes, and declares functions that compare
// Int64ListNodes and compare CachelineListNodes.
//
// Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
// SPDX-License-Identifier:  CC-BY-SA-4.0
#ifndef LIST_NODE_H_
#define LIST_NODE_H_

#include <stdbool.h>
#include <stdint.h>

// Simple node "base."
typedef struct list_node {
  struct list_node *next;
} ListNode;

// Nodes containing an int64_t.
typedef struct int64_list_node {
  ListNode node;
  int64_t value;
} Int64ListNode;

// Nodes containing 64 bytes worth of data (typical cacheline).
#define CACHELINE_LIST_NODE_ARRAY_LEN ((64 - sizeof(ListNode)) / sizeof(int32_t))
typedef struct cacheline_list_node {
  ListNode node;
  int32_t data[CACHELINE_LIST_NODE_ARRAY_LEN];
} CachelineListNode;

// Function type for node comparison functions.  Returns true if the first
// argument is less than the second argument.
typedef bool ListNodeCompareFxn(ListNode*, ListNode*);

// Comparison functions for Int64Node and CachelineNode.
extern bool compare_int64_list_node(ListNode*, ListNode*);
extern bool compare_cacheline_list_node(ListNode*, ListNode*);

// Function type for list sort functions.  Returns the new head of a list.
typedef ListNode *ListSortFxn(ListNode*, ListNodeCompareFxn*);

#endif  // NODE_H_
