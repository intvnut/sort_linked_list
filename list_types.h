// Defines derived ListNode types Int64ListNode and CachelineListNode.
// Declares functions that compare functions for Int64ListNode and
// CachelineListNodes.
//
// Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
// SPDX-License-Identifier:  CC-BY-SA-4.0
#ifndef LIST_TYPES_H_
#define LIST_TYPES_H_

#include <stdint.h>

#include "list_node.h"
#include "list_sort.h"

// Nodes containing an int64_t.
typedef struct int64_list_node {
  ListNode node;
  int64_t value;
} Int64ListNode;

// Nodes containing 64 bytes worth of data (typical cacheline).
enum {
  kCachelineListNodeDataLen = ((64 - sizeof(ListNode)) / sizeof(int32_t))
};
typedef struct cacheline_list_node {
  ListNode node;
  int32_t data[kCachelineListNodeDataLen];
} CachelineListNode;

// Comparison functions for Int64Node and CachelineNode.
extern bool compare_int64_list_node(ListNode*, ListNode*);
extern bool compare_cacheline_list_node(ListNode*, ListNode*);

#endif  // LIST_TYPES_H_
