// Top-down Recursive Merge Sort, measuring list length up front.
//
// Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
// SPDX-License-Identifier:  CC-BY-SA-4.0
#ifndef TDR2_MERGE_SORT_H_
#define TDR2_MERGE_SORT_H_

#include "list_node.h"
#include "list_sort.h"

// Implements a top-down recursive merge sort on a linked list.  This version
// measures the list's length up front, and uses that to optimize the recursive
// function.
ListNode *tdr2_merge_sort(ListNode *head, ListNodeCompareFxn *cmp);

#endif  // TDR2_MERGE_SORT_H_
