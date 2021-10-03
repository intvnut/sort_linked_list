# Makefile for the Linked List Sorting Benchmark.
#
# Author:  Joe Zbiciak <joe.zbiciak@leftturnonly.info>
# SPDX-License-Identifier:  CC-BY-SA-4.0
CC = gcc-9.2.0
CFLAGS = -O3 -flto -Wall -W -Wextra -DUSE_MEMALIGN
LFLAGS = -lrt

COMMON_SRCS += list_sort.c
COMMON_SRCS += list_types.c
COMMON_SRCS += mt19937-64.c
COMMON_SRCS += benchmark.c
COMMON_SRCS += bui1_merge_sort.c
COMMON_SRCS += bui2_merge_sort.c
COMMON_SRCS += tdi1_merge_sort.c
COMMON_SRCS += tdi2_merge_sort.c
COMMON_SRCS += tdr1_merge_sort.c
COMMON_SRCS += tdr2_merge_sort.c
COMMON_SRCS += tdr3_merge_sort.c
COMMON_SRCS += tdq1_quick_sort.c


COMMON_HDRS += list_node.h
COMMON_HDRS += list_sort.h
COMMON_HDRS += list_types.h
COMMON_HDRS += mt64.h
COMMON_HDRS += bui1_merge_sort.h
COMMON_HDRS += bui2_merge_sort.h
COMMON_HDRS += tdi1_merge_sort.h
COMMON_HDRS += tdi2_merge_sort.h
COMMON_HDRS += tdr1_merge_sort.h
COMMON_HDRS += tdr2_merge_sort.h
COMMON_HDRS += tdr3_merge_sort.h
COMMON_HDRS += tdq1_quick_sort.h

all: benchmark

benchmark: $(COMMON_SRCS) $(COMMON_HDRS)
	$(CC) -o benchmark $(CFLAGS) $(COMMON_SRCS) $(LFLAGS)

clean:
	rm benchmark
