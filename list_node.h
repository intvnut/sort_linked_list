// Defines a base ListNode.
//
// Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
// SPDX-License-Identifier:  CC-BY-SA-4.0
#ifndef LIST_NODE_H_
#define LIST_NODE_H_

// Simple node "base."
typedef struct list_node {
  struct list_node *next;
} ListNode;

#endif  // LIST_NODE_H_
