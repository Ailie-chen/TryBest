#ifndef CHAMPSIM_COMMONS_H
#define CHAMPSIM_COMMONS_H

// CACHE ACCESS TYPE
#define LOAD      0
#define RFO       1
#define PREFETCH  2
#define WRITEBACK 3
#define LOAD_TRANSLATION 4
#define PREFETCH_TRANSLATION 5  //Requests coming from TLB prefetcher
#define TRANSLATION_FROM_L1D 6	//Translation requests coming to STLB when the translation of data prefetch request is missing in DTLB
#define NUM_TYPES 7

// QUEUE TYPES
#define IS_RQ           0
#define IS_WQ           1
#define IS_PQ           2
#define IS_MSHR         3
#define IS_PROCESSED    4
#define IS_DRAM_QUEUE   5
#define IS_DRAM_CONTROLLER_QUEUE 6
#define NUM_QUEUE_TYPES 7

// special registers that help us identify branches
#define REG_STACK_POINTER 6
#define REG_FLAGS 25
#define REG_INSTRUCTION_POINTER 26

// branch types
#define NOT_BRANCH           0
#define BRANCH_DIRECT_JUMP   1
#define BRANCH_INDIRECT      2
#define BRANCH_CONDITIONAL   3
#define BRANCH_DIRECT_CALL   4
#define BRANCH_INDIRECT_CALL 5
#define BRANCH_RETURN        6
#define BRANCH_OTHER         7

// dependent instruction types
#define DEP_INSTR_BRANCH_MISPRED    0
#define DEP_INSTR_BRANCH_CORRECT    1
#define DEP_INSTR_LOAD              2
#define DEP_INSTR_TYPES             3

// some more parameters for stat collection
#define DRAM_BW_LEVELS 4
#define CACHE_ACC_LEVELS 10

// ROB PARTITION TYPES
#define FRONTAL             0
#define NONE                1
#define DORSAL              2
#define NUM_PARTITION_TYPES 3

// MODULE TYPE
#define IS_ITLB 0
#define IS_DTLB 1
#define IS_STLB 2
#define IS_L1I  3
#define IS_L1D  4
#define IS_L2C  5
#define IS_LLC  6
#define IS_DRAM 7
#define IS_PSCL5 8
#define IS_PSCL4 9
#define IS_PSCL3 10
#define IS_PSCL2 11
#define IS_DRAM_CONTROLLER 12
#define NUM_MODULE_TYPES 13

#endif /* CHAMPSIM_COMMONS_H */