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

static void run_int64_list_tests(
    void *const buf, TestResult *const tr, double *const tot_time,
    const size_t elems, const int seed_lo, const int seed_hi) {
  printf("%zu", elems); fflush(stdout);

  for (size_t i = 0; i < sort_registry.length; ++i) {
    tot_time[i] = 0.;
  }

  for (int seed = seed_lo; seed <= seed_hi; ++seed) {
    for (size_t i = 0; i < sort_registry.length; ++i) {
      tr[i] = run_int64_list_test(buf, elems,
                                  sort_registry.entry[i].fxn, seed);
      tot_time[i] += tr[i].time;
    }

    // Now check that they all return the same checksum.
    bool ok = true;
    for (size_t i = 1; i < sort_registry.length; ++i) {
      if (tr[0].csum != tr[i].csum) {
        ok = false;
      }
    }

    if (!ok) {
      printf("\nFAIL");
      for (size_t i = 0; i < sort_registry.length; ++i) {
        printf(",%" PRIX64, tr[i].csum);
      }
      putchar('\n');
      exit(1);
    }
  }

  double seed_scale = 1.0 / (seed_hi - seed_lo + 1);
  for (size_t i = 0; i < sort_registry.length; ++i) {
    printf(",%g", tot_time[i] * seed_scale);
  }
  putchar('\n');
  fflush(stdout);
}

// Currently, 256MiB.
#define MAX_POW2  (28)
#define MAX_BYTES (1ull << MAX_POW2)
#define NUM_SEEDS (8)

int main() {
  void *buf = malloc(MAX_BYTES);
  TestResult *tr = calloc(sizeof(TestResult), sort_registry.length);
  double *tot_time = calloc(sizeof(double), sort_registry.length);

  if (!buf || !tr || !tot_time) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(1);
  }

  // Warmup.
  const size_t elems = MAX_BYTES / sizeof(Int64ListNode);
  print_csv_header("Warmup");
  run_int64_list_tests(buf, tr, tot_time, elems, 0, 0);

  print_csv_header("Elems");

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

      run_int64_list_tests(buf, tr, tot_time, elems, 1, NUM_SEEDS);
    }
  }
  
  printf("PASS\n");
}
