// Implements a bottom-up iterative merge sort on a linked list.
//
// Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
// SPDX-License-Identifier:  CC-BY-SA-4.0
#include "bui1_merge_sort.h"

#include <stddef.h>

#define MAX_STACK (64)

typedef struct {
  size_t length;
  ListNode *node;
} StackNode;

typedef struct {
  int top;
  StackNode stk[MAX_STACK];
} Stack;

// Pushes the first node from the rest of the list onto the top of stack,
// and returns the rest of the list.
static inline ListNode *push_first(
    Stack *const restrict stk,
    ListNode *const first
) {
  const StackNode sn = { .length = 1, .node = first };
  stk->stk[stk->top++] = sn;
  ListNode *rest = first->next;
  first->next = NULL;
  return rest;
}

// Pushes a sub-list onto the stack, along with its length.
static inline void push_list(Stack *const restrict stk, const int length,
                             ListNode *const node) {
  const StackNode sn = { .length = length, .node = node };
  stk->stk[stk->top++] = sn;
}

// Pops the top of stack, returning the ListNode* at the top.
static inline ListNode *pop_list(Stack *const restrict stk) {
  return stk->stk[--stk->top].node;
}

// Returns the length of the nth previous stack push.
static inline int peek_length(Stack *const restrict stk, const int dist) {
  return stk->stk[stk->top - dist].length;
}

// Implements a merge sort on a singly linked list, using a bottom-up iterative
// power-of-2 collapsing merge sort, based on a strawman I posted here:
// https://www.quora.com/Which-is-the-best-the-most-efficient-sorting-algorithm-implemented-by-linked-list-Merge-sort-Insertion-sort-heap-sort-or-Quick-sort/answer/David-Vandevoorde?comment_id=216999829&comment_type=2
ListNode *bui1_merge_sort(ListNode *const first,
                          ListNodeCompareFxn *const cmp) {
  // Handle degenerate cases of an empty list or a single-node list.
  if (!first || !first->next) {
    return first;
  }

  // Our stack of partially merged lists.  Only need to initialize stk.top.
  Stack stk;
  stk.top = 0;

  // Push the first nodes onto the stack.
  ListNode *rest = push_first(&stk, push_first(&stk, first));

  // While there's sub-lists to merge, keep merging. 
  do {
    // Merge sub-lists at top of stack, if possible.
    while (stk.top > 1 &&
           (!rest || peek_length(&stk, 1) == peek_length(&stk, 2))) {
      // Extract the top two nodes from the stack to merge.
      const size_t length = peek_length(&stk, 1) + peek_length(&stk, 2);
      ListNode *a = pop_list(&stk);
      ListNode *b = pop_list(&stk);

      // Merge the two lists, with merged as its head. pnext points to the
      // next pointer at the tail of the list, or merged at the start of the
      // merge process.
      ListNode *merged = NULL;
      ListNode **pnext = &merged;

      // Take the smallest from a or b, as long as both lists are non-empty.
      while (a && b) {
        ListNode **l = cmp(a, b) ? &a : &b;
        *pnext = *l;
        pnext = &(*pnext)->next;
        *l = (*l)->next;
      }

      // Once we exhaust one list, append the other as-is to the merged list.
      *pnext = a ? a : b;

      push_list(&stk, length, merged);
    }

    // If there are more unsorted nodes, add a new sub-list containing the next
    // item from it.
    if (rest) {
      rest = push_first(&stk, rest);
    }
  } while (stk.top > 1);

  // Return the final merged result.
  return pop_list(&stk);
}
