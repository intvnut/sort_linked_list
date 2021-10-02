// Top-down Recursive Merge Sort, without up-front list length measurement.
// This version sub-divides lists with an "even/odd" approach.
//
// Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
// SPDX-License-Identifier:  CC-BY-SA-4.0
#ifndef TDR3_MERGE_SORT_H_
#define TDR3_MERGE_SORT_H_

#include "list_node.h"
#include "list_sort.h"

// Implements a naive top-down recursive merge sort on a linked list.
// This version does not try to measure the list length up front to take
// advantage of it.  Instead, it partitions nodes into "even/odd" lists,
// as per Jerry Coffin's description here:
//
// https://www.quora.com/Which-is-the-best-the-most-efficient-sorting-algorithm-implemented-by-linked-list-Merge-sort-Insertion-sort-heap-sort-or-Quick-sort/answer/David-Vandevoorde?comment_id=217455001&comment_type=2
ListNode *tdr3_merge_sort(ListNode *head, ListNodeCompareFxn *cmp);

#endif  // TDR3_MERGE_SORT_H_
