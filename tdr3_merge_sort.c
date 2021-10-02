// Top-down Recursive Merge Sort, without up-front list length measurement.
// This version sub-divides lists with an "even/odd" approach.
//
// Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
// SPDX-License-Identifier:  CC-BY-SA-4.0

#include "tdr3_merge_sort.h"

#include <stddef.h>

// Implements a naive top-down recursive merge sort on a linked list.
// This version does not try to measure the list length up front to take
// advantage of it.  Instead, it partitions nodes into "even/odd" lists,
// as per Jerry Coffin's description here:
//
// https://www.quora.com/Which-is-the-best-the-most-efficient-sorting-algorithm-implemented-by-linked-list-Merge-sort-Insertion-sort-heap-sort-or-Quick-sort/answer/David-Vandevoorde?comment_id=217455001&comment_type=2
ListNode *tdr3_merge_sort(ListNode *const head, ListNodeCompareFxn *const cmp) {
  // Degenerate list: return as-is.
  if (!head || !head->next) {
    return head;
  }

  // Two-node list: sort and return.
  if (!head->next->next) {
    ListNode *const a = head;
    ListNode *const b = head->next;
    // Do we need to swap them?
    if (cmp(b, a)) {
      b->next = a;
      a->next = NULL;
      return b;
    }
    return head;
  }

  // Partition incoming list into two, putting even nodes on 'a' and odd nodes
  // on 'b'.  The sublists get reversed in the process, which may help us a
  // little with locality on at least one side of the recursion.
  ListNode *a = NULL, *b = NULL;

  for (ListNode *node = head, *temp; node; ) {
    temp = node->next;
    node->next = a;
    a = node;
    node = temp;

    if (!node) {
      break;
    }

    temp = node->next;
    node->next = b;
    b = node;
    node = temp;
  }

  // Recursively sort the halves.
  a = tdr3_merge_sort(a, cmp);
  b = tdr3_merge_sort(b, cmp);
  ListNode *merged = NULL, **pnext = &merged;

  // Take the smallest from a or b, as long as both lists are non-empty.
  while (a && b) {
    ListNode **const l = cmp(a, b) ? &a : &b;
    *pnext = *l;
    pnext = &(*pnext)->next;
    *l = (*l)->next;
  }

  // Once we exhaust one list, append the other as-is to the merged list.
  *pnext = a ? a : b;

  // Return the final merged result.
  return merged;
}
