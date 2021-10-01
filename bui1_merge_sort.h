// Implements a bottom-up iterative merge sort on a linked list.
//
// Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
// SPDX-License-Identifier:  CC-BY-SA-4.0
#ifndef BUI1_MERGE_SORT_H_
#define BUI1_MERGE_SORT_H_

#include "list_node.h"

// Implements a merge sort on a singly linked list, using a bottom-up iterative
// power-of-2 collapsing merge sort, based on a strawman I posted here:
// https://www.quora.com/Which-is-the-best-the-most-efficient-sorting-algorithm-implemented-by-linked-list-Merge-sort-Insertion-sort-heap-sort-or-Quick-sort/answer/David-Vandevoorde?comment_id=216999829&comment_type=2
ListNode *bui1_merge_sort(ListNode *first, ListNodeCompareFxn *cmp);

#endif  // BUI1_MERGE_SORT_H_
