# Makefile for the Linked List Sorting Benchmark.
#
# Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
# SPDX-License-Identifier:  CC-BY-SA-4.0
CC = gcc-9.2.0
CFLAGS = -O3 -flto -Wall -W -Wextra -DUSE_MEMALIGN
LFLAGS = -lrt

COMMON_SRCS += list_node.c
COMMON_SRCS += mt19937-64.c
COMMON_SRCS += bui1_merge_sort.c
COMMON_SRCS += bui2_merge_sort.c
COMMON_SRCS += tdi1_merge_sort.c
COMMON_SRCS += tdi2_merge_sort.c
COMMON_SRCS += tdr1_merge_sort.c
COMMON_SRCS += tdr2_merge_sort.c
COMMON_SRCS += tdr3_quick_sort.c

BMK_INT64_SRCS += benchmark_int64.c
BMK_CACHELINE_SRCS += benchmark_cacheline.c

COMMON_HDRS += list_node.h
COMMON_HDRS += mt64.h
COMMON_HDRS += bui1_merge_sort.h
COMMON_HDRS += bui2_merge_sort.h
COMMON_HDRS += tdi1_merge_sort.h
COMMON_HDRS += tdi2_merge_sort.h
COMMON_HDRS += tdr1_merge_sort.h
COMMON_HDRS += tdr2_merge_sort.h
COMMON_HDRS += tdr3_quick_sort.h

all: benchmark_cacheline benchmark_int64

benchmark_cacheline: $(COMMON_SRCS) $(COMMON_HDRS) $(BMK_CACHELINE_SRCS)
	$(CC) -o benchmark_cacheline $(CFLAGS) $(COMMON_SRCS) $(BMK_CACHELINE_SRCS) $(LFLAGS)

benchmark_int64: $(COMMON_SRCS) $(COMMON_HDRS) $(BMK_INT64_SRCS)
	$(CC) -o benchmark_int64 $(CFLAGS) $(COMMON_SRCS) $(BMK_INT64_SRCS) $(LFLAGS)

clean:
	rm benchmark_int64 benchmark_cacheline
