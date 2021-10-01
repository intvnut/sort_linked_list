#ifndef TDI1_MERGE_SORT_H_
#define TDI1_MERGE_SORT_H_

#include "list_node.h"

// Implements a top-down iterative list merge sort with O(1) auxillary storage,
// from Drew Eckhardt's post here:
// https://www.quora.com/What-is-the-best-way-to-sort-an-unsorted-linked-list/answers/3873494
//
// Modified only very slightly by Joe Zbiciak (joe.zbiciak@leftturnonly.info).
// Joe Zbiciak makes no intellectual property claims to any modifications he's
// made to the code.
ListNode *tdi1_merge_sort(ListNode *src, ListNodeCompareFxn *cmp);

#endif // TDI1_MERGE_SORT_H_
