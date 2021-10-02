// Benchmarks linked lists of int64_t.
//
// Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
// SPDX-License-Identifier:  CC-BY-SA-4.0
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "bui1_merge_sort.h"
#include "bui2_merge_sort.h"
#include "list_node.h"
#include "list_sort.h"
#include "list_types.h"
#include "mt64.h"
#include "tdi1_merge_sort.h"
#include "tdi2_merge_sort.h"
#include "tdr1_merge_sort.h"
#include "tdr2_merge_sort.h"
#include "tdq1_quick_sort.h"

// Returns the current time in seconds.
static double now(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec + 1e-9 * ts.tv_nsec;
}

// Creates a randomized linked list of int64_t in the designated buffer, with
// the specified seed.
static Int64ListNode *generate_int64_list(
    Int64ListNode *const nbuf, 
    const size_t elems,
    const uint64_t seed
) {
  static size_t *pbuf = NULL;
  static size_t pbuf_size = 0;
  if (elems > pbuf_size) {
    pbuf = (size_t *)realloc(pbuf, sizeof(size_t) * elems);
    pbuf_size = elems;
  }

  // The constant is intended to "temper" simple seeds like 1, 2, 3.
  init_genrand64(seed ^ 0x0A1A2A3A4A5A6A7Aull);

  // Randomize the values.
  for (size_t i = 0; i < elems; ++i) {
    nbuf[i].value = genrand64_int64();
  }

  // Prepare to make a random permutation of nodes.
  for (size_t i = 0; i < elems; ++i) {
    pbuf[i] = i;
  }

  // Fisher-Yates shuffle the node order.
  for (size_t i = 0; i < elems; ++i) {
    size_t j = i + (elems - i) * genrand64_real2();
    size_t t = pbuf[i];
    pbuf[i] = pbuf[j];
    pbuf[j] = t;
  }

  // String together the linked list.
  for (size_t i = 0; i < elems - 1; ++i) {
    nbuf[pbuf[i]].node.next = &nbuf[pbuf[i+1]].node;
  }
  nbuf[pbuf[elems-1]].node.next = NULL;

  return &nbuf[pbuf[0]];
}

// Returns 0 if incorrect; otherwise, returns a checksum of the list contents
// computed with a simple weighted checksum.
static uint64_t check_int64_list_correctness(
    Int64ListNode *const head,
    const size_t elems
) {
  uint64_t csum = 0;
  Int64ListNode *node = head;
  int64_t prev_value = node->value;

  for (size_t i = 0; i < elems; ++i) {
    if (!node) {
      return 0;
    }

    if (node->value < prev_value) {
      return 0;
    }

    csum = (i + 1) * node->value;
    prev_value = node->value;
    node = (Int64ListNode *)node->node.next;
  }

  return csum ? csum : 1;
}

typedef struct {
  double time;
  uint64_t csum;
} TestResult;

// Invokes the sort function under test, returning its total execution time
// and the checksum associated with its (hopefully) sorted list.
static TestResult run_int64_list_test(void *const buf, const size_t elems,
                                      ListSortFxn *const sort, const int seed) {
  Int64ListNode *in = generate_int64_list((Int64ListNode *)buf, elems, seed);

  const double t1 = now();
  ListNode *out = sort((ListNode *)in, compare_int64_list_node);
  const double t2 = now();

  const TestResult test_result = {
      .time = t2 - t1,
      .csum = check_int64_list_correctness((Int64ListNode *)out, elems)
  };

  return test_result;
}

// Currently, 256MiB.
#define MAX_POW2  (28)
#define MAX_BYTES (1ull << MAX_POW2)
#define NUM_SEEDS (8)

int main() {
  void *buf = malloc(MAX_BYTES);

  // Warmup.
  {
    const size_t elems = MAX_BYTES / sizeof(Int64ListNode);
    printf("Warmup,Bottom-Up Iterative 1,Bottom-Up Iterative 2,"
           "Top-Down Recursive 1,Top-Down Recursive 2,Top-Down Recursive 3,"
           "Top-Down Iterative 1,Top-Down Iterative 2\n"
           "%zu", elems);
    fflush(stdout);

    const TestResult bui1_tr = run_int64_list_test(buf, elems, bui1_merge_sort, 0);
    printf(",%f", bui1_tr.time); fflush(stdout);
    const TestResult bui2_tr = run_int64_list_test(buf, elems, bui2_merge_sort, 0);
    printf(",%f", bui2_tr.time); fflush(stdout);
    const TestResult tdr1_tr = run_int64_list_test(buf, elems, tdr1_merge_sort, 0);
    printf(",%f", tdr1_tr.time); fflush(stdout);
    const TestResult tdr2_tr = run_int64_list_test(buf, elems, tdr2_merge_sort, 0);
    printf(",%f", tdr2_tr.time); fflush(stdout);
    const TestResult tdq1_tr = run_int64_list_test(buf, elems, tdq1_quick_sort, 0);
    printf(",%f", tdq1_tr.time); fflush(stdout);
    const TestResult tdi1_tr = run_int64_list_test(buf, elems, tdi1_merge_sort, 0);
    printf(",%f", tdi1_tr.time); fflush(stdout);
    const TestResult tdi2_tr = run_int64_list_test(buf, elems, tdi2_merge_sort, 0);
    printf(",%f", tdi2_tr.time); fflush(stdout);

    if (bui1_tr.csum != bui2_tr.csum || bui1_tr.csum != tdr1_tr.csum ||
        bui1_tr.csum != tdr2_tr.csum || bui1_tr.csum != tdq1_tr.csum ||
        bui1_tr.csum != tdi1_tr.csum || bui1_tr.csum != tdi2_tr.csum) {
      printf(",Warmup Failed: " "%" PRIX64 " %" PRIX64
             " %" PRIX64 " %" PRIX64 " %" PRIX64 " %" PRIX64 " %" PRIX64 "\n",
             bui1_tr.csum, bui2_tr.csum, tdr1_tr.csum, tdr2_tr.csum,
             tdq1_tr.csum, tdi1_tr.csum, tdi2_tr.csum);
      exit(1);
    } else {
      printf(",Warmup passed.\n");
    }
  }

  printf("Elems,Bottom-Up Iterative 1,Bottom-Up Iterative 2,"
         "Top-Down Recursive 1,Top-Down Recursive 2,Top-Down Recursive 3,"
         "Top-Down Iterative 1,Top-Down Iterative 2\n");
  fflush(stdout);

  // Now run all three on different sizes up to MAX_BYTES, using eight sizes
  // between each power of 2, and NUM_SEEDS different seeds for each size.
  for (int pow2 = 4; pow2 <= MAX_POW2; pow2++) {
    size_t prev_elems = 0;
    for (int sub_pow2 = 0; sub_pow2 < 8; sub_pow2++) {
      const size_t size = (1ull << pow2) + sub_pow2 * (1ull << (pow2 - 3));
      const size_t elems = size / sizeof(Int64ListNode);
      if (!elems || elems == prev_elems) {
        continue;
      }
      if (size > MAX_BYTES) {
        break;
      }
      prev_elems = elems;

      printf("%zu", elems); fflush(stdout);

      double tot_bui1_time = 0., tot_bui2_time = 0.,
             tot_tdr1_time = 0., tot_tdr2_time = 0., tot_tdq1_time = 0.,
             tot_tdi1_time = 0., tot_tdi2_time = 0.;

      for (int seed = 1; seed <= NUM_SEEDS; ++seed) {
        const TestResult bui1_tr =
          run_int64_list_test(buf, elems, bui1_merge_sort, seed);
        const TestResult bui2_tr =
          run_int64_list_test(buf, elems, bui2_merge_sort, seed);
        const TestResult tdr1_tr =
          run_int64_list_test(buf, elems, tdr1_merge_sort, seed);
        const TestResult tdr2_tr =
          run_int64_list_test(buf, elems, tdr2_merge_sort, seed);
        const TestResult tdq1_tr =
          run_int64_list_test(buf, elems, tdq1_quick_sort, seed);
        const TestResult tdi1_tr =
          run_int64_list_test(buf, elems, tdi1_merge_sort, seed);
        const TestResult tdi2_tr =
          run_int64_list_test(buf, elems, tdi2_merge_sort, seed);

        tot_bui1_time += bui1_tr.time;
        tot_bui2_time += bui2_tr.time;
        tot_tdr1_time += tdr1_tr.time;
        tot_tdr2_time += tdr2_tr.time;
        tot_tdq1_time += tdq1_tr.time;
        tot_tdi1_time += tdi1_tr.time;
        tot_tdi2_time += tdi2_tr.time;

        if (bui1_tr.csum != bui2_tr.csum || bui1_tr.csum != tdr1_tr.csum ||
            bui1_tr.csum != tdr2_tr.csum || bui1_tr.csum != tdq1_tr.csum ||
            bui1_tr.csum != tdi1_tr.csum || bui1_tr.csum != tdi2_tr.csum) {
          printf(",Sort Failed: %" PRIX64 " %" PRIX64 " %" PRIX64 " %" PRIX64
                 " %" PRIX64 " %" PRIX64  " %" PRIX64 " seed=%d\n",
                 bui1_tr.csum, bui2_tr.csum, tdr1_tr.csum, tdr2_tr.csum,
                 tdq1_tr.csum, tdi1_tr.csum, tdi2_tr.csum, seed);
          exit(1);
        }
      }

      tot_bui1_time /= NUM_SEEDS;
      tot_bui2_time /= NUM_SEEDS;
      tot_tdr1_time /= NUM_SEEDS;
      tot_tdr2_time /= NUM_SEEDS;
      tot_tdq1_time /= NUM_SEEDS;
      tot_tdi1_time /= NUM_SEEDS;
      tot_tdi2_time /= NUM_SEEDS;

      printf(",%g,%g,%g,%g,%g,%g,%g\n",
             tot_bui1_time, tot_bui2_time,
             tot_tdr1_time, tot_tdr2_time, tot_tdq1_time,
             tot_tdi1_time, tot_tdi2_time);
    }
  }
  
  printf("PASS\n");
}
