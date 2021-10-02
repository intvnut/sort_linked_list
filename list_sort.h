// Type declarations associated with sorting linked lists.
// 
// Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
// SPDX-License-Identifier:  CC-BY-SA-4.0
#ifndef LIST_SORT_H_
#define LIST_SORT_H_

#include <stdbool.h>
#include "list_node.h"

// Function type for node comparison functions.  Returns true if the first
// argument is less than the second argument.
typedef bool ListNodeCompareFxn(ListNode*, ListNode*);

// Function type for list sort functions.  Returns the new head of a list.
typedef ListNode *ListSortFxn(ListNode*, ListNodeCompareFxn*);

#endif  // LIST_SORT_H_
