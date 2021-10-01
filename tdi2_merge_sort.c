#include "tdi2_merge_sort.h"

#include <stddef.h>

// Implements a top-down iterative list merge sort with O(1) auxillary storage,
// from Drew Eckhardt's post here:
// https://www.quora.com/What-is-the-best-way-to-sort-an-unsorted-linked-list/answers/3873494
//
// Modified by Joe Zbiciak (joe.zbiciak@leftturnonly.info) to measure the list
// length once up front, and to merge sub-lists while extracting them from the
// main list.
ListNode *tdi2_merge_sort(ListNode *const src, ListNodeCompareFxn *const cmp) {
  ListNode *rest, *out_head, **out_tail;
  size_t increment = 1, size = 0;

  // Scan once to find our size.
  for (ListNode *n = src; n; n = n->next) {
    size++;
  }

  rest = src;
  while (increment < size) {
    out_head = NULL;
    out_tail = &out_head;

    while (rest) {
      size_t ar = increment, br = increment;
      ListNode *a = rest;
      ListNode *b = a;

      // Find the start of 'b'.
      for (size_t i = 0; i < increment && b; ++i) {
        b = b->next;
      }

      // If 'a' was shorter than increment, just append it and break out.
      if (!b) {
        rest = NULL;
        *out_tail = a;
        break;
      }

      // Merge 'b' into 'a'.
      while (ar && br && b) {
        ListNode **l = cmp(a, b) ? (--ar, &a) : (--br, &b);
        *out_tail = *l;
        out_tail = &(*out_tail)->next;
        *l = (*l)->next;
      }

      // Push any remaining 'a' nodes.
      while (ar) {
        *out_tail = a;
        out_tail = &a->next;
        a = a->next;
        --ar;
      }

      // Push any remaining 'b' nodes. 'b' can end early.
      while (br && b) {
        *out_tail = b;
        out_tail = &b->next;
        b = b->next;
        --br;
      }

      // Terminate our partial list.
      *out_tail = NULL;

      // The final advance on 'b' will make it point to 'rest'.
      rest = b;
    }

    increment *= 2;
    rest = out_head;
  }

  return rest;
}
