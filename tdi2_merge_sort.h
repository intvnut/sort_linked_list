// Top-down Iterative Merge Sort with O(1) auxillary storage.  Modified to
// measure list length up front and merge the second sublist during extraction
// from the main list.
//
// Primary author:  Drew Eckhardt
// Secondary author:  Joe Zbiciak
// SPDX-License-Identifier:  CC-BY-SA-4.0
#ifndef TDI2_MERGE_SORT_H_
#define TDI2_MERGE_SORT_H_

#include "list_node.h"
#include "list_sort.h"

// Implements a top-down iterative list merge sort with O(1) auxillary storage,
// from Drew Eckhardt's post here:
// https://www.quora.com/What-is-the-best-way-to-sort-an-unsorted-linked-list/answers/3873494
//
// Modified to measure the list length once up front, and to merge sub-lists
// while extracting them from the main list.
ListNode *tdi2_merge_sort(ListNode *const src, ListNodeCompareFxn *cmp);

#endif // TDI2_MERGE_SORT_H_
