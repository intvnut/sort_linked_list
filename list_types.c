// Implements comparison functions for Int64ListNode and CachelineListNode.
//
// Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
// SPDX-License-Identifier:  CC-BY-SA-4.0
#include <stddef.h>

#include "list_node.h"
#include "list_sort.h"
#include "list_types.h"
#include "mt64.h"

// Compares two Int64ListNodes, returning true if the first is less than the
// second.
bool compare_int64_list_node(const ListNode *const a, const ListNode *const b) {
  const Int64ListNode *const aa = (const Int64ListNode *)a;
  const Int64ListNode *const bb = (const Int64ListNode *)b;

  return aa->value < bb->value;
}

// Compares two CachelineListNodes, returning true if the first is less than
// the second.
bool compare_cacheline_list_node(
    const ListNode *const a, const ListNode *const b) {
  const CachelineListNode *const aa = (const CachelineListNode *)a;
  const CachelineListNode *const bb = (const CachelineListNode *)b;

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

// Benchmarking interface functions.

// Returns an Int64ListNode at the specified index.
static ListNode *get_int64_list_node(void *const buf, const size_t index) {
  return (ListNode *)((Int64ListNode *)buf + index);
}

// Randomizes an Int64ListNode, given a ListNode* to the node.
static void randomize_int64_list_node(ListNode *const node) {
  Int64ListNode *const int64_node = (Int64ListNode *)node;
  int64_node->value = genrand64_int64();
}

// Returns an index-sensitive checksum for an Int64ListNode.
static uint64_t checksum_int64_list_node(
    const ListNode *const node,
    const size_t index
) {
  return ((uint64_t)((Int64ListNode *)node)->value) * (index + 1);
}

// Validates an Int64ListNode.
static bool validate_int64_list_node(const ListNode *const node) {
  // Int64ListNodes don't have anything to validate.
  (void)node;
  return true;
}

// List node operations for an Int64List.
const ListNodeOps list_node_ops_int64 = {
  .get = get_int64_list_node,
  .randomize = randomize_int64_list_node,
  .compare = compare_int64_list_node,
  .checksum = checksum_int64_list_node,
  .validate = validate_int64_list_node
};

