// Top-down Iterative Merge Sort with O(1) auxillary storage.
//
// Author:  Drew Eckhardt
// SPDX-License-Identifier:  CC-BY-SA-4.0
#include "tdi1_merge_sort.h"

#include <stddef.h>

// Implements a top-down iterative list merge sort with O(1) auxillary storage,
// from Drew Eckhardt's post here:
// https://www.quora.com/What-is-the-best-way-to-sort-an-unsorted-linked-list/answers/3873494
//
// Modified only very slightly by Joe Zbiciak (joe.zbiciak@leftturnonly.info).
// Joe Zbiciak makes no intellectual property claims to any modifications he's
// made to the code.
static ListNode *split_after(ListNode *in, size_t count) {
  size_t i;
  ListNode *rest, **prev;

  for (i = 0, rest = in, prev = NULL; rest && i < count;
      ++i, prev = &rest->next, rest = rest->next) { }

  if (prev) {
    *prev = NULL;
  }

  return rest;
}

ListNode *tdi1_merge_sort(ListNode *const src, ListNodeCompareFxn *cmp) {
  ListNode *rest, *in_head[2], *out_head, **out_tail;
  size_t increment, merge_src, size;

  rest = src;
  increment = 1;
  do {
    out_head = NULL;
    out_tail = &out_head;
    size = 0;

    while (rest) {
      in_head[0] = rest;
      in_head[1] = split_after(in_head[0], increment);
      rest = split_after(in_head[1], increment);

      while (in_head[0] || in_head[1]) {
        merge_src = !in_head[1] ||
          (in_head[0] && !cmp(in_head[1], in_head[0])) ? 0 : 1;

        *out_tail = in_head[merge_src];
        in_head[merge_src] = in_head[merge_src]->next;

        (*out_tail)->next = NULL;
        out_tail = &(*out_tail)->next;
        ++size;
      }
    }

    increment *= 2;
    rest = out_head;
  } while (increment < size);

  return rest;
}
