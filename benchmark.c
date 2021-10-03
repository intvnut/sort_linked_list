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
#include <string.h>
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
} BenchResult;

typedef struct {
  const ListNodeBenchOps *lnb_ops;
  void *list_buf;
  BenchResult *rslt_buf;
  double *time_buf;
  int seed_lo, seed_hi;     // Inclusive range.
  size_t size_lo, size_hi;  // Inclusive range, in bytes.
} BenchSweepDetails;


// Invokes the sort function under test on an already-prepared list, returning
// its total execution time and the checksum associated with its (hopefully)
// sorted list.
static BenchResult run_single_benchmark(
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

  const BenchResult test_result = {
      .time = t2 - t1,
      .csum = check_list_correctness(lnb_ops, out, elems)
  };

  return test_result;
}

// Invokes each of the sort functions in the sort registry with the same size
// input, iterating over a range of seed values for randomization.
static void run_benchmark_suite_at_single_size(
    const BenchSweepDetails *const sweep,
    const size_t elems
) {
  double *const time_buf = sweep->time_buf;
  BenchResult *const rslt_buf = sweep->rslt_buf;

  printf("%zu", elems); fflush(stdout);

  for (size_t i = 0; i < sort_registry.length; ++i) {
    time_buf[i] = 0.;
  }

  for (int seed = sweep->seed_lo; seed <= sweep->seed_hi; ++seed) {
    for (size_t i = 0; i < sort_registry.length; ++i) {
      rslt_buf[i] = run_single_benchmark(sort_registry.entry[i].fxn,
                                         sweep->lnb_ops, sweep->list_buf,
                                         elems, seed);
      time_buf[i] += rslt_buf[i].time;
    }

    // Now check that they all return the same checksum.
    bool ok = true;
    for (size_t i = 1; i < sort_registry.length; ++i) {
      if (rslt_buf[0].csum != rslt_buf[i].csum) {
        ok = false;
      }
    }

    if (!ok) {
      printf("\nFAIL");
      for (size_t i = 0; i < sort_registry.length; ++i) {
        printf(",%" PRIX64, rslt_buf[i].csum);
      }
      putchar('\n');
      exit(1);
    }
  }

  const double seed_scale = 1.0 / (sweep->seed_hi - sweep->seed_lo + 1);
  for (size_t i = 0; i < sort_registry.length; ++i) {
    printf(",%g", time_buf[i] * seed_scale);
  }
  putchar('\n');
  fflush(stdout);
}

// Sweeps over a range of input sizes, running the benchmark suite over a
// range of input sizes.
static void run_benchmark_suite_size_sweep(
    const BenchSweepDetails *const sweep
) {
  const size_t elem_size = sweep->lnb_ops->size;

  // Step the coarse-grain size by powers of 2.
  for (int pow2 = 4; pow2 < 64; ++pow2) {
    size_t prev_elems = 0;

    // At each power-of-2, take 8 fine-grain steps.
    for (int sub_pow2 = 0; sub_pow2 < 8; ++sub_pow2) {
      const size_t bytes = (1ull << pow2) + sub_pow2 * (1ull << (pow2 - 3));
      const size_t elems = bytes / elem_size;

      // If the new size repeats an earlier element count, skip it.  Also, skip
      // it if it's below our size range.
      if (!elems || elems == prev_elems || bytes < sweep->size_lo) {
        continue;
      }

      // Stop when we exceed our maximum size.
      if (bytes > sweep->size_hi) {
        break;
      }
      prev_elems = elems;

      run_benchmark_suite_at_single_size(sweep, elems);
    }
  }
}

// Currently, 256MiB.
#define MAX_POW2  (28)
#define MAX_BYTES (1ull << MAX_POW2)
#define NUM_SEEDS (8)

int main(int argc, char *argv[]) {
  // For now, very simple argument parsing to select one of two benchmark types.
  if (argc != 2) {
    fprintf(stderr, 
        "Usage:  benchmark <int64|cacheline>\n"
        "  'int64' runs the benchmark with Int64ListNode\n"
        "  'cacheline' runs the benchmark with CachelineListNode\n");
    exit(1);
  }

  const ListNodeBenchOps *lnb_ops = NULL;

  if (!strcmp(argv[1], "int64")) {
    lnb_ops = &list_node_bench_ops_int64;
  }

  if (!strcmp(argv[1], "cacheline")) {
    lnb_ops = &list_node_bench_ops_cacheline;
  }

  if (!lnb_ops) {
    fprintf(stderr, "Unknown benchmark type '%s'\n", argv[1]);
    exit(1);
  }

  // Set up the benchmark sweep details.  Eventually, consider adding flags to
  // modify these details.
  const BenchSweepDetails main_sweep = {
    .lnb_ops = lnb_ops,
    .list_buf = malloc(MAX_BYTES),
    .rslt_buf = calloc(sizeof(BenchResult), sort_registry.length),
    .time_buf = calloc(sizeof(double), sort_registry.length),
    .seed_lo = 1,  .seed_hi = NUM_SEEDS,
    .size_lo = 16, .size_hi = MAX_BYTES
  };

  // Set up the warmpup sweep details.  Eventually, consider adding flags to
  // modify these details.
  const BenchSweepDetails warmup_sweep = {
    .lnb_ops = main_sweep.lnb_ops,
    .list_buf = main_sweep.list_buf,
    .rslt_buf = main_sweep.rslt_buf,
    .time_buf = main_sweep.time_buf,
    .seed_lo = 0,.seed_hi = 0, .size_lo = MAX_BYTES, .size_hi = MAX_BYTES
  };

  if (!main_sweep.list_buf || !main_sweep.rslt_buf || !main_sweep.time_buf) {
    fprintf(stderr, "Memory allocation failed.\n");
    exit(1);
  }

  // Warmup.  Run the sorts on a max-size buffer with a single seed.
  print_csv_header("Warmup");
  run_benchmark_suite_size_sweep(&warmup_sweep);

  // Main benchmark.  
  // Sweep over a range of memory sizes, and use multiple seeds.
  print_csv_header("Elems");
  run_benchmark_suite_size_sweep(&main_sweep);
  
  printf("PASS\n");
}
