// Top-down Recursive Merge Sort, without up-front list length measurement.
//
// Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
// SPDX-License-Identifier:  CC-BY-SA-4.0
#include "tdr1_merge_sort.h"

#include <stddef.h>

// Top-down recursive merge sort on a linked list.
// This version does not measure the list length up-front.
ListNode *tdr1_merge_sort(ListNode *const head, ListNodeCompareFxn *const cmp) {
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

  // Find midpoint and cut into two lists.
  ListNode *pmid = NULL, *mid = head, *tail = head;

  while (tail) {
    pmid = mid;
    mid = mid->next;
    tail = tail->next;
    tail = tail ? tail->next : NULL;
  }
  pmid->next = NULL;

  // Recursively sort the halves.
  ListNode *a = tdr1_merge_sort(head, cmp);
  ListNode *b = tdr1_merge_sort(mid, cmp);
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
