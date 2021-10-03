# Linked List Sort Benchmark


## License

Everything in this benchmark authored by me (Joe Zbiciak,
joe.zbiciak@leftturnonly.info) is licensed under the Creative Commons
Attribution-ShareAlike 4.0 International license, aka.
[CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/).

Drew Eckhardt's top-down iterative implementation is owned by him.  I place no
intellectual property claim of my own to any modifications I made to his code.
I will update this section if/when Drew provides guidance on a preferred
license.

The Mersenne Twister 64-bit implementation carries its own license, which I
_believe_ is derived from the BSD 3-Clause license.  But, I am not a lawyer, so
don't take my word for it.  Please consult the Mersenne Twister source.  I do
believe I have followed all of the conditions set forth in the Mersenne Twister
source.

## Background

This is a simple linked list sorting benchmark inspired by
[a question on Quora.](https://www.quora.com/Which-is-the-best-the-most-efficient-sorting-algorithm-implemented-by-linked-list-Merge-sort-Insertion-sort-heap-sort-or-Quick-sort)
In [David Vandevoorde's answer,](https://www.quora.com/Which-is-the-best-the-most-efficient-sorting-algorithm-implemented-by-linked-list-Merge-sort-Insertion-sort-heap-sort-or-Quick-sort/answer/David-Vandevoorde)
I posted a merge sort technique I came up with on the spot that merges
sub-lists in a power-of-2 fashion bottom-up.  Meanwhile, Jerry Coffin mentioned
a traditional top-down recursive approach, and Drew Eckhardt posted code for a
top-down iterative version with O(1) storage along with a link to
[his answer to a similar question](https://www.quora.com/What-is-the-best-way-to-sort-an-unsorted-linked-list/answers/3873494).
David's original answer mentions a number of very interesting approaches,
including using QuickSort and InsertionSort, or making hybrids involving
QuickSort or MergeSort, combined with InsertionSort for the leaves.

Drew wondered out loud:

> I wonder how your greedy bottom up approach compares to a traditional
> implementation with constant space. It definitely has better memory locality.

So, I decided to write a benchmark.  My benchmark focused primarily on Drew's
question, and not the larger design space David suggested.

## C or C++?

I wrote this benchmark in C, rather than C++, just to make it abundantly clear
exactly what was happening at each step of each algorithm.  I'm quite confident
C++ will perform just as well (or possibly better!) on these algorithms if
written correctly.

In my original comment describing my bottom-up algorithm, I used a `std::vector`
to store some state to make the code conceptually simpler in the comment I
posted.  In reality, I'd use a fixed-size array, as `log2(n)` is bounded on any
practical system.  A single call to the allocator, though, could be relatively
expensive, and there's _at least_ one lurking inside `std::vector`.

So, rather than worry about all that, I decided to take everything to the
lowest common denominator and write it in C.

## The Sort Algorithms

So far, I have only benchmarked 7 approaches, and I haven't even attempted
InsertionSort, or applying InsertionSort to QuickSort or MergeSort to make a
hybrid.  I've really only implemented 4 basic approaches, and optimized
variants of 3 of them.  They are:

| Short Name | Long Name | Description |
| :--: | :-- | :-- |
| `bui1_merge_sort` | Bottom-Up MergeSort, version 1. | This is the version I posted originally, with a bug fix. |
| `bui2_merge_sort` | Bottom-Up MergeSort, version 2. | The same algorithm as `bui1_merge_sort`, but with a minor tweak that puts sorted pairs onto the work stack rather than single nodes. |
| `tdr1_merge_sort` | Top-Down Recursive MergeSort, version 1. | This is the simplest top-down recursive merge sort.  It does not take advantage of list length information. |
| `tdr2_merge_sort` | Top-Down Recursive MergeSort, version 2. | Similar to `tdr1_merge_sort`, except that it measures the list length up front, and uses that to optimize finding the midpoint. |
| `tdr3_merge_sort` | Top-Down Recursive MergeSort, version 3. | Similar to `tdr1_merge_sort`, except that it subdivides lists in an even-odd fashion. |
| `tdq1_quick_sort` | Top-Down Recursive QuickSort, version 1. | This is the only QuickSort implementation in the mix.  This is a naive QuickSort that just pulls its pivot from the first element.  My benchmark uses randomized data, so this actually is the best case for QuickSort in many ways. |
| `tdi1_merge_sort` | Top-Down Iterative MergeSort, version 1. | This is Drew Eckhardt's original code, with very minor tweaks to make it work in this framework. |
| `tdi2_merge_sort` | Top-Down Iterative MergeSort, version 2. | I modified Drew's code to merge the first sub-list with the second sub-list while extracting the second sub-list from the main list.  This provides a nice locality-related boost when the sub-lists are long. |

## The List Types

I defined all of the sort functions in terms of a `ListNode` type that just
holds the pointer to the next node.  C guarantees you can cast back and forth
between a pointer to a `struct` and a pointer to its first member.  And, C also
guarantees that all pointers to `struct` types have the same representation.

```
// Simple node "base."
typedef struct list_node {
  struct list_node *next;
} ListNode;
```

So, each of the sort functions work on any `struct` data type that has a
`ListNode` as its first member. The sort functions accept a
`ListNodeCompareFxn*` that returns true if the first argument is less than the
second argument.

For benchmarking purposes, I defined two different list types:  `Int64List` and
`CachelineList`.

### `Int64ListNode`

This is a linked list of `int64_t` values.  Each of the values is as big as the
pointer that links them, at least on a 64-bit system.  And, 64-bit systems are
the default these days.

```
// Nodes containing an int64_t.
typedef struct int64_list_node {
  ListNode node;
  int64_t value;
} Int64ListNode;
```

### `CachelineListNode`

This is a linked list of nodes that are 64 bytes long, corresponding to a
typical cacheline size on most systems these days.  The comparison function
performs a lexicographical comparison on the data array.  The benchmark fills
the data array with zeros, except for the last element, to force scanning the
entire array, thereby ensuring it's entirely in the cache.

```
// Nodes containing 64 bytes worth of data (typical cacheline).
enum {
  kCachelineListNodeArrayLen = ((64 - sizeof(ListNode)) / sizeof(int32_t))
};
typedef struct cacheline_list_node {
  ListNode node;
  int32_t data[kCachelineListNodeArrayLen];
} CachelineListNode;
```

## The Benchmark

The benchmark itself is pretty simple.  It allocates a large memory pool once
up front (256MiB, currently).  It reuses this memory pool repeatedly for each
measurement.  Thus, the memory allocator is *not* part of the benchmark.

* Run a warmup on the maximum data size:
    * For each algorithm, do:
        * Initialize Mersenne Twister (64-bit) with a fixed seed.
        * Randomize the set of values.
        * Randomize the order of the nodes, using a Fisher-Yates shuffle.
        * Check that the algorithm returned sorted data, and record a checksum.
        * Report the run-time for the algorithm warmup as part of a CSV record
          on `stdout`.
    * Check that all algorithms returned the same checksum.
* For each data size, do:
    * For 8 different seeds, do:
        * For each algorithm, do:
            * Initialize Mersenne Twister (64-bit) with the seed.
            * Randomize the set of values.
            * Randomize the order of the nodes, using a Fisher-Yates shuffle.
            * Accumulate the time required to sort using the algorithm.
            * Check that the algorithm returned sorted data, and record a
              checksum.
        * Check that all algorithms returned the same checksum.
    * Divide the total run-time for each algorithm by the number of seeds.
    * Output a CSV record with the results for all algorithms to `stdout`.

I run each power-of-2 data size up to the maximum (currently 256MiB).  At each
power of 2, I measure 8 different equally-spaced sizes starting at that power
of 2 and ending just before the next.  For the smaller data sizes, this might
result in a redundant element count, and so I filter out any sizes that are
redundant or result in an empty list.

Each linked list's storage is _dense._  That is, when the benchmark measures a
1MiB linked list, it's measuring a linked list that occupies 1MiB of contiguous
storage with no gaps or memory allocator overhead.  I just randomize the list
element order within that storage and the contents of each list element.  Thus,
this benchmark will give optimistic results as compared to a linked list with a
similar number of elements allocted with separate calls to the stock `malloc()`
(C) or `new` (C++).

### Usage

Here's how I run the benchmarks.  I also sometimes use `taskset` to pin the
benchmark to a single CPU, after setting the CPU's governor to `performance`.

```
./benchmark int64 | tee int64.csv           # run Int64ListNode test
./benchmark cacheline | tee cacheline.csv   # run CachelineListNode test
```

Each benchmark sweep takes hours to run on my machine.  I generally run them
when I won't be at my computer for awhile (e.g. overnight).

### Benchmark Driver Internal Interface

The benchmark driver `benchmark.c` is implemented in a type-agnostic manner
that allows it to measure sort performance on arbitrary `ListNode` types.
To benchmark a new type derived from `ListNode`, supply the following functions
and collect them together in a `ListNodeBenchOps` structure.

From `list_bench.h`:

```
// Treats a buffer as an array of a particular list node type, returning a
// ListNode* to a given index.
typedef ListNode *ListNodeGetFxn(void *buf, size_t index);

// Randomizes the value a list node of a particular type, given a ListNode*.
typedef void ListNodeRandomizeFxn(ListNode *node);

// Returns an index-sensitive checksum for a list node of a particular type.
typedef uint64_t ListNodeChecksumFxn(const ListNode *node, size_t index);

// Returns false if a list node of a particular type violates an internal
// constraint, given a ListNode*.
typedef bool ListNodeValidateFxn(const ListNode *node);

// Provides a set of function pointers for working with list nodes of different
// types in a generic manner.
typedef struct {
  size_t size;  // Holds the size of one element of this type.
  ListNodeGetFxn *get;
  ListNodeRandomizeFxn *randomize;
  ListNodeCompareFxn *compare;
  ListNodeChecksumFxn *checksum;
  ListNodeValidateFxn *validate;
} ListNodeBenchOps;
```

## Caveats

I put this benchmark together in spare time, after work, in the wee hours of
the night.  I've made a reasonable attempt to format the code nicely and apply
consistent coding standards throughout.  I've made a reasonable attempt at
decent code hygiene.  I've also refactored and cleaned up the code a bit since
I first uploaded it.

This is not anywhere close to a definitive set of implementations, or even
necessarily a particularly clever set of implementations.  I have already
thought of potential improvements to most of these.  But, I needed to draw
the line somewhere, so I could publish the data I've collected.  Feel free
(within the bounds of the licenses) to take this code, modify it, adapt it,
extend or improve it, and post your own benchmarks.

This code focuses exclusively on singly linked lists.  Doubly-linked lists
offer more opportunity for cleverness.

---


Copyright 2021, Joe Zbiciak <joe.zbiciak@leftturnonly.info>  
`SPDX-License-Identifier:  CC-BY-SA-4.0`
