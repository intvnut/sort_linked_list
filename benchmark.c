// Benchmarks linked lists of various types.
//
// Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
// SPDX-License-Identifier:  CC-BY-SA-4.0
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "list_node.h"
#include "list_sort.h"
#include "list_types.h"
#include "mt64.h"

// Returns the current time in seconds.
static double now(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec + 1e-9 * ts.tv_nsec;
}

// Prints the set of sort names as column headings for a CSV.  The context
// argument sets the label for the first column, to allow us to distinguish the
// warmup pass from the main benchmark.
static void print_csv_header(const char *context) {
  fputs(context, stdout);
  for (size_t i = 0; i < sort_registry.length; ++i) {
    printf(",%s", sort_registry.entry[i].name);
  }
  putchar('\n');
  fflush(stdout);
}

// Creates a randomized linked list of int64_t in the designated buffer, with
// the specified seed.
static ListNode *generate_list(
    const ListNodeBenchOps *const lnb_ops,
    void *const list_buf,
    const size_t elems,
    const uint64_t seed
) {
  static size_t *perm_buf = NULL;
  static size_t perm_buf_size = 0;
  if (elems > perm_buf_size) {
    perm_buf = (size_t *)realloc(perm_buf, sizeof(size_t) * elems);
    perm_buf_size = elems;
  }

  // The constant is intended to "temper" simple seeds like 1, 2, 3.
  init_genrand64(seed ^ 0x0A1A2A3A4A5A6A7Aull);

  // Randomize the values.
  for (size_t i = 0; i < elems; ++i) {
    lnb_ops->randomize(lnb_ops->get(list_buf, i));
  }

  // Prepare to make a random permutation of nodes.
  for (size_t i = 0; i < elems; ++i) {
    perm_buf[i] = i;
  }

  // Fisher-Yates shuffle the node order.
  for (size_t i = 0; i < elems; ++i) {
    size_t j = i + (elems - i) * genrand64_real2();
    size_t t = perm_buf[i];
    perm_buf[i] = perm_buf[j];
    perm_buf[j] = t;
  }

  // String together the linked list.
  ListNode *const first = lnb_ops->get(list_buf, perm_buf[0]);
  ListNode *prev = first;
  for (size_t i = 1; i < elems; ++i) {
    ListNode *const curr = lnb_ops->get(list_buf, perm_buf[i]);
    prev->next = curr;
    prev = curr;
  }
  prev->next = NULL;

  return first;
}

// Returns 0 if incorrect; otherwise, returns a checksum of the list contents
// computed with a simple weighted checksum.
static uint64_t check_list_correctness(
    const ListNodeBenchOps *const lnb_ops,
    ListNode *const head,
    const size_t elems
) {
  ListNode *curr = head, *prev = NULL;
  uint64_t csum = 0;

  for (size_t i = 0; i < elems; ++i) {
    // Fail if we hit end-of-list too soon.
    if (!curr) {
      return 0;
    }

    // Fail if current node is less than the previous node.
    if (prev && lnb_ops->compare(curr, prev)) {
      return 0;
    }

    // Fail if node fails to validate.
    if (!lnb_ops->validate(curr)) {
      return 0;
    }

    // Update checksum.
    csum = ((csum << 1) ^ (csum >> 1)) + lnb_ops->checksum(curr, i);

    // Advance down the list.
    prev = curr;
    curr = curr->next;
  }

  return csum ? csum : 1;
}

typedef struct {
  double time;
  uint64_t csum;
} TestResult;

// Invokes the sort function under test, returning its total execution time
// and the checksum associated with its (hopefully) sorted list.
static TestResult run_test(
    ListSortFxn *const sort,
    const ListNodeBenchOps *const lnb_ops,
    void *const list_buf,
    const size_t elems,
    const int seed
) {
  ListNode *const in = generate_list(lnb_ops, list_buf, elems, seed);

  const double t1 = now();
  ListNode *const out = sort(in, lnb_ops->compare);
  const double t2 = now();

  const TestResult test_result = {
      .time = t2 - t1,
      .csum = check_list_correctness(lnb_ops, out, elems)
  };

  return test_result;
}

static void run_tests(
    TestResult *const tr_buf,
    double *const time_buf,
    const ListNodeBenchOps *const lnb_ops,
    void *const list_buf,
    const size_t elems,
    const int seed_lo,
    const int seed_hi
) {
  printf("%zu", elems); fflush(stdout);

  for (size_t i = 0; i < sort_registry.length; ++i) {
    time_buf[i] = 0.;
  }

  for (int seed = seed_lo; seed <= seed_hi; ++seed) {
    for (size_t i = 0; i < sort_registry.length; ++i) {
      tr_buf[i] = run_test(sort_registry.entry[i].fxn,
                           lnb_ops, list_buf, elems, seed);
      time_buf[i] += tr_buf[i].time;
    }

    // Now check that they all return the same checksum.
    bool ok = true;
    for (size_t i = 1; i < sort_registry.length; ++i) {
      if (tr_buf[0].csum != tr_buf[i].csum) {
        ok = false;
      }
    }

    if (!ok) {
      printf("\nFAIL");
      for (size_t i = 0; i < sort_registry.length; ++i) {
        printf(",%" PRIX64, tr_buf[i].csum);
      }
      putchar('\n');
      exit(1);
    }
  }

  const double seed_scale = 1.0 / (seed_hi - seed_lo + 1);
  for (size_t i = 0; i < sort_registry.length; ++i) {
    printf(",%g", time_buf[i] * seed_scale);
  }
  putchar('\n');
  fflush(stdout);
}

// Currently, 256MiB.
#define MAX_POW2  (28)
#define MAX_BYTES (1ull << MAX_POW2)
#define NUM_SEEDS (8)

int main() {
  const ListNodeBenchOps *const lnb_ops = &list_node_bench_ops_int64;
  void *const list_buf = malloc(MAX_BYTES);
  TestResult *const tr_buf = calloc(sizeof(TestResult), sort_registry.length);
  double *const time_buf = calloc(sizeof(double), sort_registry.length);

  if (!list_buf || !tr_buf || !time_buf) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(1);
  }

  // Warmup.
  const size_t elems = MAX_BYTES / sizeof(Int64ListNode);
  print_csv_header("Warmup");
  run_tests(tr_buf, time_buf, lnb_ops, list_buf, elems, 0, 0);

  print_csv_header("Elems");
  // Now run all three on different sizes up to MAX_BYTES, using eight sizes
  // between each power of 2, and NUM_SEEDS different seeds for each size.
  for (int pow2 = 4; pow2 <= MAX_POW2; pow2++) {
    size_t prev_elems = 0;
    for (int sub_pow2 = 0; sub_pow2 < 8; sub_pow2++) {
      const size_t bytes = (1ull << pow2) + sub_pow2 * (1ull << (pow2 - 3));
      const size_t elems = bytes / lnb_ops->size;
      if (!elems || elems == prev_elems) {
        continue;
      }
      if (bytes > MAX_BYTES) {
        break;
      }
      prev_elems = elems;

      run_tests(tr_buf, time_buf, lnb_ops, list_buf, elems, 1, NUM_SEEDS);
    }
  }
  
  printf("PASS\n");
}
