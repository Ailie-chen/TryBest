#ifndef DRAM_H
#define DRAM_H

#include "memory_class.h"
typedef enum
{
    FR_FCFS = 0,        // 0
    FRONTAL_FR_FCFS,    // 1
    FR_FRONTAL_FCFS,    // 2
    ROB_PART_FR_FCFS,   // 3
    FR_ROB_PART_FCFS,   // 4
    CR_FCFS,            // 5

    NumMemControllerScheduleType
} mem_cntlr_schedule_type_t;
extern string MemControllerScheduleTypeString[NumMemControllerScheduleType];

// DRAM configuration
#define DRAM_CHANNEL_WIDTH 8 // 8B
#define DRAM_WQ_SIZE 64
#define DRAM_RQ_SIZE 64

#define tRP_DRAM_NANOSECONDS  12.5
#define tRCD_DRAM_NANOSECONDS 12.5
#define tCAS_DRAM_NANOSECONDS 12.5

// the data bus must wait this amount of time when switching between reads and writes, and vice versa
#define DRAM_DBUS_TURN_AROUND_TIME ((15*CPU_FREQ)/2000) // 7.5 ns 
extern uint32_t DRAM_MTPS, DRAM_DBUS_RETURN_TIME,DRAM_DBUS_MAX_CAS ;

// these values control when to send out a burst of writes
#define DRAM_WRITE_HIGH_WM    ((DRAM_WQ_SIZE*7)>>3) // 7/8th
#define DRAM_WRITE_LOW_WM     ((DRAM_WQ_SIZE*3)>>2) // 6/8th
#define MIN_DRAM_WRITES_PER_SWITCH (DRAM_WQ_SIZE*1/4)

// DRAM
class MEMORY_CONTROLLER : public MEMORY {
  public:
    const string NAME;

    DRAM_ARRAY dram_array[DRAM_CHANNELS][DRAM_RANKS][DRAM_BANKS];
    uint64_t dbus_cycle_available[DRAM_CHANNELS], dbus_cycle_congested[DRAM_CHANNELS], dbus_congested[NUM_TYPES+1][NUM_TYPES+1];
    uint64_t bank_cycle_available[DRAM_CHANNELS][DRAM_RANKS][DRAM_BANKS];
    uint8_t  do_write, write_mode[DRAM_CHANNELS]; 
    uint32_t processed_writes, scheduled_reads[DRAM_CHANNELS], scheduled_writes[DRAM_CHANNELS];
    int fill_level;

    BANK_REQUEST bank_request[DRAM_CHANNELS][DRAM_RANKS][DRAM_BANKS];

    // queues
    PACKET_QUEUE WQ[DRAM_CHANNELS], RQ[DRAM_CHANNELS];
    // DDRP buffer
    vector<deque<uint64_t> > ddrp_buffer;
    // to measure bandwidth
    uint64_t rq_enqueue_count, last_enqueue_count, epoch_enqueue_count, next_bw_measure_cycle;
    uint8_t bw;
    uint64_t total_bw_epochs;
    uint64_t bw_level_hist[DRAM_BW_LEVELS];
struct
    {
        uint64_t rq_merged;

        struct
        {
            uint64_t total_loads;
            uint64_t load_cat[NUM_PARTITION_TYPES];
        } data_loads;

        struct
        {
            uint64_t reduced_lat;
            uint64_t zero_lat;
        } pseudo_direct_dram_prefetch;

        struct
        {
            struct
            {
                uint64_t total;
                uint64_t went_to_dram;
                uint64_t rq_hit[2];
                uint64_t ddrp_buffer_hit;
            } ddrp_req;

            struct
            {
                uint64_t total[NUM_TYPES];
                uint64_t went_to_dram[NUM_TYPES];
                uint64_t rq_hit[NUM_TYPES];
                uint64_t ddrp_buffer_hit[NUM_TYPES];
            } llc_miss;
        } ddrp;

        struct
        {
            uint64_t returned[NUM_TYPES];
            uint64_t buffered;
            uint64_t not_returned;
        } dram_process;

        struct
        {
            struct
            {
                uint64_t called;
                uint64_t hit;
                uint64_t evict;
                uint64_t insert;
            } insert;

            struct
            {
                uint64_t called;
                uint64_t hit;
                uint64_t miss;
            } lookup;
        } ddrp_buffer;
        
    } stats;
    // constructor
    MEMORY_CONTROLLER(string v1) : NAME (v1) {
        for (uint32_t i=0; i<NUM_TYPES+1; i++) {
            for (uint32_t j=0; j<NUM_TYPES+1; j++) {
                dbus_congested[i][j] = 0;
            }
        }
        do_write = 0;
        processed_writes = 0;
        for (uint32_t i=0; i<DRAM_CHANNELS; i++) {
            dbus_cycle_available[i] = 0;
            dbus_cycle_congested[i] = 0;
            write_mode[i] = 0;
            scheduled_reads[i] = 0;
            scheduled_writes[i] = 0;

            for (uint32_t j=0; j<DRAM_RANKS; j++) {
                for (uint32_t k=0; k<DRAM_BANKS; k++)
                    bank_cycle_available[i][j][k] = 0;
            }

            WQ[i].NAME = "DRAM_WQ" + to_string(i);
            WQ[i].SIZE = DRAM_WQ_SIZE;
            WQ[i].entry = new PACKET [DRAM_WQ_SIZE];

            RQ[i].NAME = "DRAM_RQ" + to_string(i);
            RQ[i].SIZE = DRAM_RQ_SIZE;
            RQ[i].entry = new PACKET [DRAM_RQ_SIZE];
        }

        fill_level = FILL_DRAM;
    };

    // destructor
    ~MEMORY_CONTROLLER() {

    };
void reset_stats()
    {
        // reset DRAM stats
        for (uint32_t i=0; i<DRAM_CHANNELS; i++) 
        {
            RQ[i].ROW_BUFFER_HIT = 0;
            RQ[i].ROW_BUFFER_MISS = 0;
            WQ[i].ROW_BUFFER_HIT = 0;
            WQ[i].ROW_BUFFER_MISS = 0;
        }
        bzero(&stats, sizeof(stats));
    }
    // functions
    int  add_rq(PACKET *packet),
         add_wq(PACKET *packet),
         add_pq(PACKET *packet);

    void return_data(PACKET *packet),
         operate(),
         increment_WQ_FULL(uint64_t address);

    uint32_t get_occupancy(uint8_t queue_type, uint64_t address),
             get_size(uint8_t queue_type, uint64_t address);

    void schedule(PACKET_QUEUE *queue), process(PACKET_QUEUE *queue),
         update_schedule_cycle(PACKET_QUEUE *queue),
         update_process_cycle(PACKET_QUEUE *queue),
         reset_remain_requests(PACKET_QUEUE *queue, uint32_t channel),
	 print_DRAM_busy_stats();

     int find_next_request(PACKET_QUEUE *queue, uint8_t& row_buffer_hit),
        schedule_FR_FCFS(PACKET_QUEUE *queue, uint8_t& row_buffer_hit),
        schedule_FRONTAL_FR_FCFS(PACKET_QUEUE *queue, uint8_t& row_buffer_hit),
        schedule_FR_FRONTAL_FCFS(PACKET_QUEUE *queue, uint8_t& row_buffer_hit),
        schedule_ROB_PART_FR_FCFS(PACKET_QUEUE *queue, uint8_t& row_buffer_hit),
        schedule_FR_ROB_PART_FCFS(PACKET_QUEUE *queue, uint8_t& row_buffer_hit),
        schedule_CR_FCFS(PACKET_QUEUE *queue, uint8_t& row_buffer_hit);

    uint32_t dram_get_channel(uint64_t address),
             dram_get_rank   (uint64_t address),
             dram_get_bank   (uint64_t address),
             dram_get_row    (uint64_t address),
             dram_get_column (uint64_t address),
             drc_check_hit (uint64_t address, uint32_t cpu, uint32_t channel, uint32_t rank, uint32_t bank, uint32_t row);

    uint64_t get_bank_earliest_cycle();

    int check_dram_queue(PACKET_QUEUE *queue, PACKET *packet);
    // DDRP buffer
    void init_ddrp_buffer();
    void insert_ddrp_buffer(uint64_t address);
    bool lookup_ddrp_buffer(uint64_t address);
    uint32_t get_ddrp_buffer_set_index(uint64_t address);
};
void print_dram_config();


#endif
