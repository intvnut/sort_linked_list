// Top-down Recursive Merge Sort, without up-front list length measurement.
//
// Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
// SPDX-License-Identifier:  CC-BY-SA-4.0
#ifndef TDR1_MERGE_SORT_H_
#define TDR1_MERGE_SORT_H_

#include "list_node.h"

// Implements a naive top-down recursive merge sort on a linked list.
// This version does not try to measure the list length up front to take
// advantage of it.
ListNode *tdr1_merge_sort(ListNode *head, ListNodeCompareFxn *cmp);

#endif  // TDR1_MERGE_SORT_H_
