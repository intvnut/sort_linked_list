// Naive linked-list QuickSort.
//
// Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
// SPDX-License-Identifier:  CC-BY-SA-4.0
#ifndef TDQ1_QUICK_SORT_H_
#define TDQ1_QUICK_SORT_H_

#include "list_node.h"

// Sorts a singly linked list with a naive pivot Quicksort.
ListNode *tdq1_quick_sort(ListNode *const head, ListNodeCompareFxn *const cmp);

#endif // TDQ1_QUICK_SORT_H_
