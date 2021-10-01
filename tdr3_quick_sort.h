// Naive linked-list QuickSort.
//
// Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
// SPDX-License-Identifier:  CC-BY-SA-4.0
#ifndef TDR3_QUICK_SORT_H_
#define TDR3_QUICK_SORT_H_

#include "list_node.h"

// Sorts a singly linked list with a naive pivot Quicksort.
ListNode *tdr3_quick_sort(ListNode *const head, ListNodeCompareFxn *const cmp);

#endif // TDR3_QUICK_SORT_H_
