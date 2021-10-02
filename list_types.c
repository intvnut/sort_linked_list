// Implements comparison functions for Int64ListNode and CachelineListNode.
//
// Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
// SPDX-License-Identifier:  CC-BY-SA-4.0
#include <stddef.h>

#include "list_node.h"
#include "list_sort.h"
#include "list_types.h"

bool compare_int64_list_node(ListNode *a, ListNode *b) {
  Int64ListNode *aa = (Int64ListNode *)a;
  Int64ListNode *bb = (Int64ListNode *)b;

  return aa->value < bb->value;
}

bool compare_cacheline_list_node(ListNode *a, ListNode *b) {
  CachelineListNode *aa = (CachelineListNode *)a;
  CachelineListNode *bb = (CachelineListNode *)b;

  for (size_t i = 0; i < kCachelineListNodeDataLen; ++i) {
    if (aa->data[i] < bb->data[i]) {
      return true;
    }
    if (aa->data[i] > bb->data[i]) {
      return false;
    }
  }
  return false;
}
