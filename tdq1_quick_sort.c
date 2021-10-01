// Naive linked-list QuickSort.
//
// Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
// SPDX-License-Identifier:  CC-BY-SA-4.0
#include "tdq1_quick_sort.h"

// Quicksort with naive pivot selection.
#include <stddef.h>

#include "mt64.h"

typedef struct {
  ListNode *head;
  ListNode **tail_next;
} QuickSortRet;

static QuickSortRet quick_sort_recurse(
    ListNode *const head, ListNodeCompareFxn *const cmp
) {
# define SORT(x,y)                  \
  if (!cmp(x, y)) do {              \
    ListNode *t = x; x = y; y = t;  \
  } while (0)

  const int len = !head->next ? 1
                : !head->next->next ? 2 
                : !head->next->next->next ? 3
                : -1;  // "many"

  // Sorting network for 3 nodes:
  if (len == 3) {
    ListNode *a = head;
    ListNode *b = a->next;
    ListNode *c = b->next;

    SORT(a, b);
    SORT(a, c);
    SORT(b, c);

    a->next = b;
    b->next = c;
    c->next = NULL;

    const QuickSortRet qsr = { .head = a, .tail_next = &c->next };
    return qsr;
  }

  // If we're down to 2 nodes, swap them if needed, and return.
  if (len == 2) {
    ListNode *a = head;
    ListNode *b = a->next;

    SORT(a, b);

    a->next = b;
    b->next = NULL;

    const QuickSortRet qsr = { .head = a, .tail_next = &b->next };
    return qsr;
  }
# undef SORT

  // If we're down to 1 node, just return it.
  if (len == 1) {
    const QuickSortRet qsr = { .head = head, .tail_next = &head->next };
    return qsr;
  }

  ListNode *const pivot = head;
  ListNode *node = head->next;

  // Partition the elements around the pivot.  Pull as large of a sublist as
  // we can, to minimize the number of cachelines we dirty.
  ListNode *less = NULL;
  ListNode *more = NULL;

  while (node) {
    ListNode *const tmp1 = node;
    ListNode *tmp2 = node->next;
    ListNode *ptm2 = tmp1;
    if (cmp(tmp1, pivot)) {
      // Pull as large a sublist as we can into less.
      while (tmp2 && cmp(tmp2, pivot)) {
        ptm2 = tmp2;
        tmp2 = tmp2->next;
      }
      ptm2->next = less;
      less = tmp1;
    } else {
      // Pull as large a sublist as we can into more.
      while (tmp2 && !cmp(tmp2, pivot)) {
        ptm2 = tmp2;
        tmp2 = tmp2->next;
      }
      ptm2->next = more;
      more = tmp1;
    }
    node = tmp2;
  }

  // Sort the sublists.
  const QuickSortRet qsm = { .head = NULL, .tail_next = &pivot->next };
  const QuickSortRet qsl = { .head = pivot, .tail_next = &node /* dummy */ };

  const QuickSortRet more_ret = more ? quick_sort_recurse(more, cmp) : qsm;
  const QuickSortRet less_ret = less ? quick_sort_recurse(less, cmp) : qsl;

  // Reconnect them, with the pivot in the middle.
  *less_ret.tail_next = pivot;
  pivot->next = more_ret.head;

  const QuickSortRet qsr = {
      .head = less_ret.head,
      .tail_next = more_ret.tail_next
  };
  return qsr;
}

// Sorts a singly linked list with a naive pivot Quicksort.
ListNode *tdq1_quick_sort(ListNode *const head, ListNodeCompareFxn *const cmp) {
  return quick_sort_recurse(head, cmp).head;
}
