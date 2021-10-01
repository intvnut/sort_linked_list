// Top-down Recursive Merge Sort, measuring list length up front.
//
// Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
// SPDX-License-Identifier:  CC-BY-SA-4.0
#include "tdr2_merge_sort.h"

#include <stddef.h>

// Implements the recursive portion of the top-down recursive sort, taking
// advantage of the length information computed up-front.
static ListNode *tdr2_merge_sort_internal(
    ListNode *const head,
    ListNodeCompareFxn *const cmp,
    const size_t length
) {
  // Degenerate list: return as-is.
  if (length < 2) {
    return head;
  }

  // Two-node list: sort and return.
  if (length == 2) {
    ListNode *const a = head;
    ListNode *const b = head->next;

    // Do we need to swap them?
    if (cmp(a, b)) {
      return head;  // No.
    }

    // Yes.
    b->next = a;
    a->next = NULL;
    return b;
  }

  // Find midpoint and cut into two lists.
  const size_t len_a = length / 2, len_b = length - len_a;
  ListNode *pmid = head;

  for (size_t i = 1; i < len_a; ++i) {
    pmid = pmid->next;
  }

  ListNode *const mid = pmid->next;
  pmid->next = NULL;

  // Recursively sort the halves.
  ListNode *a = tdr2_merge_sort_internal(head, cmp, len_a);
  ListNode *b = tdr2_merge_sort_internal(mid, cmp, len_b);
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

// Implements a top-down recursive merge sort on a linked list.  This version
// measures the list's length up front, and uses that to optimize the recursive
// function.
ListNode *tdr2_merge_sort(ListNode *const head, ListNodeCompareFxn *const cmp) {
  size_t length = 0;
  ListNode *node = head;

  // Measure length of the list once up-front.
  while (node) {
    length++;
    node = node->next;
  }

  return tdr2_merge_sort_internal(head, cmp, length);
}
