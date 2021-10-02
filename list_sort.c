// Generic list sort related helpers.
//
// Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
// SPDX-License-Identifier:  CC-BY-SA-4.0
#include "list_sort.h"

#include "bui1_merge_sort.h"
#include "bui2_merge_sort.h"
#include "tdi1_merge_sort.h"
#include "tdi2_merge_sort.h"
#include "tdr1_merge_sort.h"
#include "tdr2_merge_sort.h"
#include "tdr3_merge_sort.h"
#include "tdq1_quick_sort.h"

// Registry of sort functions.
const SortRegistry sort_registry = {
  .length = 8,
  .entry = {
    { "Bottom-Up Iter. MergeSort 1", bui1_merge_sort },
    { "Bottom-Up Iter. MergeSort 2", bui2_merge_sort },
    { "Top-Down Rec. MergeSort 1",  tdr1_merge_sort },
    { "Top-Down Rec. MergeSort 2",  tdr2_merge_sort },
    { "Top-Down Rec. MergeSort 3",  tdr3_merge_sort },
    { "Top-Down Rec. QuickSort 1",  tdq1_quick_sort },
    { "Top-Down Iter. MergeSort 1", tdi1_merge_sort },
    { "Top-Down Iter. MergeSort 2", tdi2_merge_sort },
  }
};
