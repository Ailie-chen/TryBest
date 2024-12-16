#include <algorithm>
#include "dram_controller.h"
#include "cache.h"
#include "ooo_cpu.h"
#include "uncore.h"
#include "util.h"
namespace knob
{
    bool     enable_pseudo_direct_dram_prefetch = false;
    bool     enable_pseudo_direct_dram_prefetch_on_prefetch = false ;
    uint32_t pseudo_direct_dram_prefetch_rob_part_type = 0;;
    extern bool     enable_ddrp;
    uint32_t dram_rq_schedule_type = FR_FCFS;
    bool     dram_force_rq_row_buffer_miss = false;
    bool     dram_cntlr_enable_ddrp_buffer = true;
    uint32_t dram_cntlr_ddrp_buffer_sets = 64;
    uint32_t dram_cntlr_ddrp_buffer_assoc = 16;
    uint32_t dram_cntlr_ddrp_buffer_hash_type = 2;
}
// initialized in main.cc
uint32_t DRAM_MTPS, DRAM_DBUS_RETURN_TIME, DRAM_DBUS_MAX_CAS,
         tRP, tRCD, tCAS;


//Neelu: Adding variables for DRAM busy stats. 
//read_or_write only valid if channel busy is true. last_toggle_ccc contains the current_core_cycle of the last instant when channel_busy was modified. 
uint8_t channel_bank_busy[DRAM_CHANNELS] = {0}, read_or_write[DRAM_CHANNELS] = {0};
uint64_t last_bank_busy_ccc[DRAM_CHANNELS][NUM_CPUS] = {0}, last_bank_busy_cpu[DRAM_CHANNELS] = {0}, sum_cycles_read[DRAM_CHANNELS] = {0}, sum_cycles_write[DRAM_CHANNELS] = {0};

uint64_t banks_busy_read_cycles[DRAM_CHANNELS][DRAM_RANKS][DRAM_BANKS+1] = {0}, banks_busy_write_cycles[DRAM_CHANNELS][DRAM_RANKS][DRAM_BANKS+1] = {0};
//Neelu: Done adding variables for DRAM busy stats.

void MEMORY_CONTROLLER::reset_remain_requests(PACKET_QUEUE *queue, uint32_t channel)
{
    for (uint32_t i=0; i<queue->SIZE; i++) {
        if (queue->entry[i].scheduled) {

            uint64_t op_addr = queue->entry[i].address;
            uint32_t op_cpu = queue->entry[i].cpu,
                     op_channel = dram_get_channel(op_addr), 
                     op_rank = dram_get_rank(op_addr), 
                     op_bank = dram_get_bank(op_addr), 
                     op_row = dram_get_row(op_addr);

#ifdef DEBUG_PRINT
            //uint32_t op_column = dram_get_column(op_addr);
#endif

            // update open row
            if ((bank_request[op_channel][op_rank][op_bank].cycle_available - tCAS) <= current_core_cycle[op_cpu])
                bank_request[op_channel][op_rank][op_bank].open_row = op_row;
            else
                bank_request[op_channel][op_rank][op_bank].open_row = UINT32_MAX;

            // this bank is ready for another DRAM request
            bank_request[op_channel][op_rank][op_bank].request_index = -1;
            bank_request[op_channel][op_rank][op_bank].row_buffer_hit = 0;
            bank_request[op_channel][op_rank][op_bank].working = 0;
            bank_request[op_channel][op_rank][op_bank].cycle_available = current_core_cycle[op_cpu];
            if (bank_request[op_channel][op_rank][op_bank].is_write) {
                scheduled_writes[channel]--;
                bank_request[op_channel][op_rank][op_bank].is_write = 0;
            }
            else if (bank_request[op_channel][op_rank][op_bank].is_read) {
                scheduled_reads[channel]--;
                bank_request[op_channel][op_rank][op_bank].is_read = 0;
            }

            queue->entry[i].scheduled = 0;
            queue->entry[i].event_cycle = current_core_cycle[op_cpu];

            DP ( if (warmup_complete[op_cpu]) {
            cout << queue->NAME << " instr_id: " << queue->entry[i].instr_id << " swrites: " << scheduled_writes[channel] << " sreads: " << scheduled_reads[channel] << endl; });

        }
    }
    
    update_schedule_cycle(&RQ[channel]);
    update_schedule_cycle(&WQ[channel]);
    update_process_cycle(&RQ[channel]);
    update_process_cycle(&WQ[channel]);

#ifdef SANITY_CHECK
    if (queue->is_WQ) {
        if (scheduled_writes[channel] != 0)
            assert(0);
    }
    else {
        if (scheduled_reads[channel] != 0)
            assert(0);
    }
#endif
}

void MEMORY_CONTROLLER::operate()
{

	//Neelu: Capturing DRAM busy stats.
	if(all_warmup_complete >= NUM_CPUS)
	//if(warmup_complete[0])
	{	
		uint8_t bank_busy_in_this_iter = 0;
		for (uint32_t i=0; i<DRAM_CHANNELS; i++) {
			if(bank_busy_in_this_iter)
				break;
			for (uint32_t j=0; j<DRAM_RANKS; j++) {	
				if(bank_busy_in_this_iter)
					break;
				int num_of_banks_busy = 0;
				for (uint32_t k=0; k<DRAM_BANKS; k++) {
					if(bank_request[i][j][k].working)
					{
						num_of_banks_busy++;
						/*if(bank_request[i][j][k].is_read)
						{
							sum_cycles_read[i]++;
							break;
						}
						else if(bank_request[i][j][k].is_write)
						{
							sum_cycles_write[i]++;
							break;
						}*/
						
						//Bank is busy.
						bank_busy_in_this_iter = 1;
						
						/*cout << "Bank busy: " << unsigned(bank_request[i][j][k].working) << ", read: " << unsigned(bank_request[i][j][k].is_read) << ", write: " << unsigned(bank_request[i][j][k].is_write) << " write_mode: " << unsigned(write_mode[i]) << endl;

						if(!channel_bank_busy[i])
						{
							channel_bank_busy[i] = 1;
							if(bank_request[i][j][k].is_read)
								read_or_write[i] = 0;
							else if(bank_request[i][j][k].is_write)
								read_or_write[i] = 1;
							else
								assert(0);

							//Note down the corresponding cpu.
							if(read_or_write[i] == 0)
								last_bank_busy_cpu[i] = RQ[i].entry[bank_request[i][j][k].request_index].cpu;
							else if(read_or_write[i] == 1)
								last_bank_busy_cpu[i] = WQ[i].entry[bank_request[i][j][k].request_index].cpu;
							else
								assert(0);

							//Note ccc for all CPUs.
							for(int temp_cpu = 0; temp_cpu < NUM_CPUS; temp_cpu++)
								last_bank_busy_ccc[i][temp_cpu] = current_core_cycle[temp_cpu];
								
						}
						else
						{
							//If channel_bank_busy is already set, update request cpu
							if(bank_request[i][j][k].is_read && !read_or_write[i])
							{
								//Capture cycles for write and update read_or_write.
								//sum_cycles_write[i] += current_core_cycle[last_bank_busy_cpu[i]] - last_bank_busy_ccc[i][last_bank_busy_cpu[i]];
								read_or_write[i] = 0;
								for(int temp_cpu = 0; temp_cpu < NUM_CPUS; temp_cpu++)
									last_bank_busy_ccc[i][temp_cpu] = current_core_cycle[temp_cpu];
							}
                                                        else if(bank_request[i][j][k].is_write && !read_or_write[i])
							{
								//sum_cycles_read[i] += current_core_cycle[last_bank_busy_cpu[i]] - last_bank_busy_ccc[i][last_bank_busy_cpu[i]];	
                                                                read_or_write[i] = 1;
								for(int temp_cpu = 0; temp_cpu < NUM_CPUS; temp_cpu++)
									last_bank_busy_ccc[i][temp_cpu] = current_core_cycle[temp_cpu];
							}

                                                        //Note down the corresponding cpu.
                                                        if(read_or_write[i] == 0)
                                                                last_bank_busy_cpu[i] = RQ[i].entry[bank_request[i][j][k].request_index].cpu;
                                                        else if(read_or_write[i] == 1)
                                                                last_bank_busy_cpu[i] = WQ[i].entry[bank_request[i][j][k].request_index].cpu;
                                                        else
                                                                assert(0);
	


						}
						break;*/
					}
				}
				//if(num_of_banks_busy == 0)
				//	cout << "curr core cycle: " << current_core_cycle[0] << endl;
				if(write_mode[i])
					banks_busy_write_cycles[i][j][num_of_banks_busy]++;
				else
					banks_busy_read_cycles[i][j][num_of_banks_busy]++;
				//cout << "curr core cycle: " << current_core_cycle[0] << " num_of_banks_busy: " << num_of_banks_busy << endl; 
			}
		}
		/*if(!bank_busy_in_this_iter)
		{
			//Check if channel bank busy if set or not.
			for (uint32_t i=0; i<DRAM_CHANNELS; i++) 
			{
				if(channel_bank_busy[i])
                                {
	                                channel_bank_busy[i] = 0;
                                        if(!read_or_write[i])
                              	        	sum_cycles_read[i] += current_core_cycle[last_bank_busy_cpu[i]] - last_bank_busy_ccc[i][last_bank_busy_cpu[i]];
					else if(read_or_write[i])
                                        	sum_cycles_write[i] += current_core_cycle[last_bank_busy_cpu[i]] - last_bank_busy_ccc[i][last_bank_busy_cpu[i]];
					else
                                                assert(0);
                                }

			}	
		}*/			
	}

	//Neelu: Done capturing DRAM busy stats.


    for (uint32_t i=0; i<DRAM_CHANNELS; i++) {
        //if ((write_mode[i] == 0) && (WQ[i].occupancy >= DRAM_WRITE_HIGH_WM)) {
      if ((write_mode[i] == 0) && ((WQ[i].occupancy >= DRAM_WRITE_HIGH_WM) || ((RQ[i].occupancy == 0) && (WQ[i].occupancy > 0)))) { // use idle cycles to perform writes
            write_mode[i] = 1;

            // reset scheduled RQ requests
            reset_remain_requests(&RQ[i], i);
            // add data bus turn-around time
            dbus_cycle_available[i] += DRAM_DBUS_TURN_AROUND_TIME;
        } else if (write_mode[i]) {

            if (WQ[i].occupancy == 0)
                write_mode[i] = 0;
            else if (RQ[i].occupancy && (WQ[i].occupancy < DRAM_WRITE_LOW_WM))
                write_mode[i] = 0;

            if (write_mode[i] == 0) {
                // reset scheduled WQ requests
                reset_remain_requests(&WQ[i], i);
                // add data bus turnaround time
                dbus_cycle_available[i] += DRAM_DBUS_TURN_AROUND_TIME;
            }
        }

        // handle write
        // schedule new entry
        if (write_mode[i] && (WQ[i].next_schedule_index < WQ[i].SIZE)) {
            if (WQ[i].next_schedule_cycle <= current_core_cycle[WQ[i].entry[WQ[i].next_schedule_index].cpu])
                schedule(&WQ[i]);
        }

        // process DRAM requests
        if (write_mode[i] && (WQ[i].next_process_index < WQ[i].SIZE)) {
            if (WQ[i].next_process_cycle <= current_core_cycle[WQ[i].entry[WQ[i].next_process_index].cpu])
                process(&WQ[i]);
        }

        // handle read
        // schedule new entry
        if ((write_mode[i] == 0) && (RQ[i].next_schedule_index < RQ[i].SIZE)) {
            if (RQ[i].next_schedule_cycle <= current_core_cycle[RQ[i].entry[RQ[i].next_schedule_index].cpu])
                schedule(&RQ[i]);
        }

        // process DRAM requests
        if ((write_mode[i] == 0) && (RQ[i].next_process_index < RQ[i].SIZE)) {
            if (RQ[i].next_process_cycle <= current_core_cycle[RQ[i].entry[RQ[i].next_process_index].cpu])
                process(&RQ[i]);
        }
    }
}

void MEMORY_CONTROLLER::schedule(PACKET_QUEUE *queue)
{
    uint64_t read_addr;
    uint32_t read_channel, read_rank, read_bank, read_row;
    uint8_t  row_buffer_hit = 0;

    int oldest_index = -1;
    uint64_t oldest_cycle = UINT64_MAX;

    // first, search for the oldest open row hit
    for (uint32_t i=0; i<queue->SIZE; i++) {

        // already scheduled
        if (queue->entry[i].scheduled) 
            continue;

        // empty entry
        read_addr = queue->entry[i].address;
        if (read_addr == 0) 
            continue;

        read_channel = dram_get_channel(read_addr);
        read_rank = dram_get_rank(read_addr);
        read_bank = dram_get_bank(read_addr);

        // bank is busy
        if (bank_request[read_channel][read_rank][read_bank].working) { // should we check this or not? how do we know if bank is busy or not for all requests in the queue?

            //DP ( if (warmup_complete[0]) {
            //cout << queue->NAME << " " << __func__ << " instr_id: " << queue->entry[i].instr_id << " bank is busy";
            //cout << " swrites: " << scheduled_writes[channel] << " sreads: " << scheduled_reads[channel];
            //cout << " write: " << +bank_request[read_channel][read_rank][read_bank].is_write << " read: " << +bank_request[read_channel][read_rank][read_bank].is_read << hex;
            //cout << " address: " << queue->entry[i].address << dec << " channel: " << read_channel << " rank: " << read_rank << " bank: " << read_bank << endl; });

            continue;
        }

        read_row = dram_get_row(read_addr);
        //read_column = dram_get_column(read_addr);

        // check open row
        if (bank_request[read_channel][read_rank][read_bank].open_row != read_row) {

            /*
            DP ( if (warmup_complete[0]) {
            cout << queue->NAME << " " << __func__ << " instr_id: " << queue->entry[i].instr_id << " row is inactive";
            cout << " swrites: " << scheduled_writes[channel] << " sreads: " << scheduled_reads[channel];
            cout << " write: " << +bank_request[read_channel][read_rank][read_bank].is_write << " read: " << +bank_request[read_channel][read_rank][read_bank].is_read << hex;
            cout << " address: " << queue->entry[i].address << dec << " channel: " << read_channel << " rank: " << read_rank << " bank: " << read_bank << endl; });
            */

            continue;
        }

        // select the oldest entry
        if (queue->entry[i].event_cycle < oldest_cycle) {
            oldest_cycle = queue->entry[i].event_cycle;
            oldest_index = i;
            row_buffer_hit = 1;
        }	  
    }

    if (oldest_index == -1) { // no matching open_row (row buffer miss)

        oldest_cycle = UINT64_MAX;
        for (uint32_t i=0; i<queue->SIZE; i++) {

            // already scheduled
            if (queue->entry[i].scheduled)
                continue;

            // empty entry
            read_addr = queue->entry[i].address;
            if (read_addr == 0) 
                continue;

            // bank is busy
            read_channel = dram_get_channel(read_addr);
            read_rank = dram_get_rank(read_addr);
            read_bank = dram_get_bank(read_addr);
            if (bank_request[read_channel][read_rank][read_bank].working) 
                continue;

            //read_row = dram_get_row(read_addr);
            //read_column = dram_get_column(read_addr);

            // select the oldest entry
            if (queue->entry[i].event_cycle < oldest_cycle) {
                oldest_cycle = queue->entry[i].event_cycle;
                oldest_index = i;
            }
        }
    }
    if(queue->is_RQ && knob::dram_force_rq_row_buffer_miss)
    {
        row_buffer_hit = 0;
    }
    // at this point, the scheduler knows which bank to access and if the request is a row buffer hit or miss
    if (oldest_index != -1) { // scheduler might not find anything if all requests are already scheduled or all banks are busy

        

        uint64_t op_addr = queue->entry[oldest_index].address;
        uint32_t op_cpu = queue->entry[oldest_index].cpu,
                 op_rob_pos = queue->entry[oldest_index].rob_position,
                 op_channel = dram_get_channel(op_addr), 
                 op_rank = dram_get_rank(op_addr), 
                 op_bank = dram_get_bank(op_addr), 
                 op_row = dram_get_row(op_addr);
#ifdef DEBUG_PRINT
        uint32_t op_column = dram_get_column(op_addr);

#endif 
        uint64_t LATENCY = 0;
        if (row_buffer_hit)  
            LATENCY = tCAS;
        else 
            LATENCY = tRP + tRCD + tCAS;

        // model pseudo direct DRAM prefetch
        if(knob::enable_pseudo_direct_dram_prefetch && queue->entry[oldest_index].is_data)
        {
            // model only for loads, and also for prefetch requests if enabled
            if(queue->entry[oldest_index].type == LOAD || (queue->entry[oldest_index].type == PREFETCH && knob::enable_pseudo_direct_dram_prefetch_on_prefetch))
            {
                uint8_t op_rob_part_type = ooo_cpu[op_cpu].rob_pos_get_part_type(op_rob_pos);
                if(knob::pseudo_direct_dram_prefetch_rob_part_type == NUM_PARTITION_TYPES 
                    || (uint32_t)op_rob_part_type == knob::pseudo_direct_dram_prefetch_rob_part_type)
                {
                    uint64_t on_chip_cache_lookup_lat = L1D_LATENCY + L2C_LATENCY + LLC_LATENCY;
                    if(LATENCY > on_chip_cache_lookup_lat)
                    {
                        LATENCY = LATENCY - on_chip_cache_lookup_lat;
                        stats.pseudo_direct_dram_prefetch.reduced_lat++;
                    }
                    else
                    {
                        // the real headroom might be even higher than this
                        // when total on-chip cache lookup latency will be much higher 
                        // than the row buffer hit latency
                        LATENCY = 0;
                        stats.pseudo_direct_dram_prefetch.zero_lat++;
                    }
                }
            }
        }

        // this bank is now busy
        bank_request[op_channel][op_rank][op_bank].working = 1;
        bank_request[op_channel][op_rank][op_bank].working_type = queue->entry[oldest_index].type;
        bank_request[op_channel][op_rank][op_bank].cycle_available = current_core_cycle[op_cpu] + LATENCY;

        bank_request[op_channel][op_rank][op_bank].request_index = oldest_index;
        bank_request[op_channel][op_rank][op_bank].row_buffer_hit = row_buffer_hit;
        if (queue->is_WQ) {
            bank_request[op_channel][op_rank][op_bank].is_write = 1;
            bank_request[op_channel][op_rank][op_bank].is_read = 0;
            scheduled_writes[op_channel]++;
        }
        else {
            bank_request[op_channel][op_rank][op_bank].is_write = 0;
            bank_request[op_channel][op_rank][op_bank].is_read = 1;
            scheduled_reads[op_channel]++;
        }

        // update open row
        bank_request[op_channel][op_rank][op_bank].open_row = op_row;

        queue->entry[oldest_index].scheduled = 1;
        queue->entry[oldest_index].event_cycle = current_core_cycle[op_cpu] + LATENCY;
        if(queue->entry[oldest_index].is_data && queue->entry[oldest_index].type == LOAD)
        {
            stats.data_loads.total_loads++;
            uint8_t op_rob_part_type = ooo_cpu[op_cpu].rob_pos_get_part_type(op_rob_pos);
            stats.data_loads.load_cat[op_rob_part_type]++;
        }

        update_schedule_cycle(queue);
        update_process_cycle(queue);

        DP (if (warmup_complete[op_cpu]) {
        cout << "[" << queue->NAME << "] " <<  __func__ << " instr_id: " << queue->entry[oldest_index].instr_id;
        cout << " row buffer: " << (row_buffer_hit ? (int)bank_request[op_channel][op_rank][op_bank].open_row : -1) << hex;
        cout << " address: " << queue->entry[oldest_index].address << " full_addr: " << queue->entry[oldest_index].full_addr << dec;
        cout << " index: " << oldest_index << " occupancy: " << queue->occupancy;
        cout << " ch: " << op_channel << " rank: " << op_rank << " bank: " << op_bank; // wrong from here
        cout << " row: " << op_row << " col: " << op_column;
        cout << " current: " << current_core_cycle[op_cpu] << " event: " << queue->entry[oldest_index].event_cycle << endl; });
    }
}

void MEMORY_CONTROLLER::process(PACKET_QUEUE *queue)
{
    uint32_t request_index = queue->next_process_index;

    // sanity check
    if (request_index == queue->SIZE)
        assert(0);

    uint8_t  op_type = queue->entry[request_index].type;
    uint64_t op_addr = queue->entry[request_index].address;
    uint32_t op_cpu = queue->entry[request_index].cpu,
             op_channel = dram_get_channel(op_addr), 
             op_rank = dram_get_rank(op_addr), 
             op_bank = dram_get_bank(op_addr);
#ifdef DEBUG_PRINT
    uint32_t op_row = dram_get_row(op_addr), 
             op_column = dram_get_column(op_addr);
#endif

    // sanity check
    if (bank_request[op_channel][op_rank][op_bank].request_index != (int)request_index) {
        assert(0);
    }

    // paid all DRAM access latency, data is ready to be processed
    if (bank_request[op_channel][op_rank][op_bank].cycle_available <= current_core_cycle[op_cpu]) {

        // check if data bus is available
        if (dbus_cycle_available[op_channel] <= current_core_cycle[op_cpu]) {

            if (queue->is_WQ) {
                // update data bus cycle time
                dbus_cycle_available[op_channel] = current_core_cycle[op_cpu] + DRAM_DBUS_RETURN_TIME;

                if (bank_request[op_channel][op_rank][op_bank].row_buffer_hit)
                    queue->ROW_BUFFER_HIT++;
                else
                    queue->ROW_BUFFER_MISS++;

                // this bank is ready for another DRAM request
                bank_request[op_channel][op_rank][op_bank].request_index = -1;
                bank_request[op_channel][op_rank][op_bank].row_buffer_hit = 0;
                bank_request[op_channel][op_rank][op_bank].working = false;
                bank_request[op_channel][op_rank][op_bank].is_write = 0;
                bank_request[op_channel][op_rank][op_bank].is_read = 0;

                scheduled_writes[op_channel]--;
            } else {
                // update data bus cycle time
                dbus_cycle_available[op_channel] = current_core_cycle[op_cpu] + DRAM_DBUS_RETURN_TIME;
                queue->entry[request_index].event_cycle = dbus_cycle_available[op_channel]; 

                DP ( if (warmup_complete[op_cpu]) {
                cout << "[" << queue->NAME << "] " <<  __func__ << " return data" << hex;
                cout << " address: " << queue->entry[request_index].address << " full_addr: " << queue->entry[request_index].full_addr << dec;
                cout << " occupancy: " << queue->occupancy << " channel: " << op_channel << " rank: " << op_rank << " bank: " << op_bank;
                cout << " row: " << op_row << " column: " << op_column;
                cout << " current_cycle: " << current_core_cycle[op_cpu] << " event_cycle: " << queue->entry[request_index].event_cycle << endl; });

                // send data back to the core cache hierarchy
                if(queue->entry[request_index].fill_level < FILL_DDRP)
                {                
                    upper_level_dcache[op_cpu]->return_data(&queue->entry[request_index]);
                    stats.dram_process.returned[queue->entry[request_index].type]++;

                    DDRP_DP ( if (warmup_complete[op_cpu]) {
                    cout << "[" << queue->NAME << "] " <<  __func__ << " return data";
                    cout << " instr_id: " << queue->entry[request_index].instr_id << " address: " << hex << queue->entry[request_index].address << dec;
                    cout << " full_addr: " << hex << queue->entry[request_index].full_addr << dec << " type: " << +queue->entry[request_index].type << " fill_level: " << queue->entry[request_index].fill_level;
                    cout << " current_cycle: " << current_core_cycle[op_cpu] << " event_cycle: " << queue->entry[request_index].event_cycle << endl; });
                }
                else
                {
                    assert(queue->entry[request_index].type == PREFETCH); // this has to be a DDRP prefetch
                    // if DDRP buffer is enabled, put this otherwise wasted request there
                    if(knob::dram_cntlr_enable_ddrp_buffer)
                    {
                        stats.dram_process.buffered++;
                        insert_ddrp_buffer(op_addr);
                        DDRP_DP ( if (warmup_complete[op_cpu]) {
                        cout << "[" << queue->NAME << "] " <<  __func__ << " buffering_data";
                        cout << " instr_id: " << queue->entry[request_index].instr_id << " address: " << hex << queue->entry[request_index].address << dec;
                        cout << " full_addr: " << hex << queue->entry[request_index].full_addr << dec << " type: " << +queue->entry[request_index].type << " fill_level: " << queue->entry[request_index].fill_level;
                        cout << " current_cycle: " << current_core_cycle[op_cpu] << " event_cycle: " << queue->entry[request_index].event_cycle << endl; });
                    }
                    else
                    {
                        // request is wasted
                        stats.dram_process.not_returned++;
                    }
                }

                if (bank_request[op_channel][op_rank][op_bank].row_buffer_hit)
                    queue->ROW_BUFFER_HIT++;
                else
                    queue->ROW_BUFFER_MISS++;

                // this bank is ready for another DRAM request
                bank_request[op_channel][op_rank][op_bank].request_index = -1;
                bank_request[op_channel][op_rank][op_bank].row_buffer_hit = 0;
                bank_request[op_channel][op_rank][op_bank].working = false;
                bank_request[op_channel][op_rank][op_bank].is_write = 0;
                bank_request[op_channel][op_rank][op_bank].is_read = 0;

                scheduled_reads[op_channel]--;
            }

            // remove the oldest entry
            queue->remove_queue(&queue->entry[request_index]);
            update_process_cycle(queue);
        }
        else { // data bus is busy, the available bank cycle time is fast-forwarded for faster simulation

#if 0
            // TODO: what if we can service prefetching request without dbus congestion?
            // can we have more timely prefetches and improve performance?
            if ((op_type == PREFETCH) || (op_type == LOAD)) {
                // just magically return prefetch request (no need to update data bus cycle time)
                /*
                dbus_cycle_available[op_channel] = current_core_cycle[op_cpu] + DRAM_DBUS_RETURN_TIME;
                queue->entry[request_index].event_cycle = dbus_cycle_available[op_channel]; 

                DP ( if (warmup_complete[op_cpu]) {
                cout << "[" << queue->NAME << "] " <<  __func__ << " return data" << hex;
                cout << " address: " << queue->entry[request_index].address << " full_addr: " << queue->entry[request_index].full_addr << dec;
                cout << " occupancy: " << queue->occupancy << " channel: " << op_channel << " rank: " << op_rank << " bank: " << op_bank;
                cout << " row: " << op_row << " column: " << op_column;
                cout << " current_cycle: " << current_core_cycle[op_cpu] << " event_cycle: " << queue->entry[request_index].event_cycle << endl; });
                */

                // send data back to the core cache hierarchy
                upper_level_dcache[op_cpu]->return_data(&queue->entry[request_index]);

                if (bank_request[op_channel][op_rank][op_bank].row_buffer_hit)
                    queue->ROW_BUFFER_HIT++;
                else
                    queue->ROW_BUFFER_MISS++;

                // this bank is ready for another DRAM request
                bank_request[op_channel][op_rank][op_bank].request_index = -1;
                bank_request[op_channel][op_rank][op_bank].row_buffer_hit = 0;
                bank_request[op_channel][op_rank][op_bank].working = false;
                bank_request[op_channel][op_rank][op_bank].is_write = 0;
                bank_request[op_channel][op_rank][op_bank].is_read = 0;

                scheduled_reads[op_channel]--;

                // remove the oldest entry
                queue->remove_queue(&queue->entry[request_index]);
                update_process_cycle(queue);

                return;
            }
#endif

            dbus_cycle_congested[op_channel] += (dbus_cycle_available[op_channel] - current_core_cycle[op_cpu]);
            bank_request[op_channel][op_rank][op_bank].cycle_available = dbus_cycle_available[op_channel];
            dbus_congested[NUM_TYPES][NUM_TYPES]++;
            dbus_congested[NUM_TYPES][op_type]++;
            dbus_congested[bank_request[op_channel][op_rank][op_bank].working_type][NUM_TYPES]++;
            dbus_congested[bank_request[op_channel][op_rank][op_bank].working_type][op_type]++;

            DP ( if (warmup_complete[op_cpu]) {
            cout << "[" << queue->NAME << "] " <<  __func__ << " dbus_occupied" << hex;
            cout << " address: " << queue->entry[request_index].address << " full_addr: " << queue->entry[request_index].full_addr << dec;
            cout << " occupancy: " << queue->occupancy << " channel: " << op_channel << " rank: " << op_rank << " bank: " << op_bank;
            cout << " row: " << op_row << " column: " << op_column;
            cout << " current_cycle: " << current_core_cycle[op_cpu] << " event_cycle: " << bank_request[op_channel][op_rank][op_bank].cycle_available << endl; });
        }
    }
}

int MEMORY_CONTROLLER::add_rq(PACKET *packet)
{
    bool return_data_to_core = true;
    if(knob::enable_ddrp && packet->fill_level >= FILL_DDRP)
    {
        return_data_to_core = false;
    }

    // simply return read requests with dummy response before the warmup
    if (all_warmup_complete < NUM_CPUS  && return_data_to_core) {
        if (packet->instruction) 
            upper_level_icache[packet->cpu]->return_data(packet);
        if (packet->is_data)
            upper_level_dcache[packet->cpu]->return_data(packet);
        
        DDRP_DP ( if(warmup_complete[packet->cpu]) {
        cout << "[" << NAME << "_RQ] " <<  __func__ << " instr_id: " << packet->instr_id << " address: " << hex << packet->address;
        cout << " full_addr: " << packet->full_addr << dec << " type: " << +packet->type << " fill_level: " << packet->fill_level << " return_data_before_warmup" << endl; });
        return -1;
    }

    // check for the latest wirtebacks in the write queue
    uint32_t channel = dram_get_channel(packet->address);
    int wq_index = check_dram_queue(&WQ[channel], packet);
    if (wq_index != -1) {
        if(return_data_to_core)
        {
        
        // no need to check fill level
        //if (packet->fill_level < fill_level) {

            packet->data = WQ[channel].entry[wq_index].data;
            if (packet->instruction) 
                upper_level_icache[packet->cpu]->return_data(packet);
            if (packet->is_data) 
                upper_level_dcache[packet->cpu]->return_data(packet);
        }
        //}

        DP ( if (packet->cpu) {
        cout << "[" << NAME << "_RQ] " << __func__ << " instr_id: " << packet->instr_id << " found recent writebacks";
        cout << hex << " read: " << packet->address << " writeback: " << WQ[channel].entry[wq_index].address << dec << endl; });

        ACCESS[1]++;
        HIT[1]++;

        WQ[channel].FORWARD++;
        RQ[channel].ACCESS++;
        //assert(0);
        DDRP_DP ( if(warmup_complete[packet->cpu]) {
        cout << "[" << NAME << "_RQ] " <<  __func__ << " instr_id: " << packet->instr_id << " address: " << hex << packet->address;
        cout << " full_addr: " << packet->full_addr << dec << " DRAM_WQ_" << channel << "_FORWARD"; });
        return -1;
    }
    // check for possible hit in DDRP buffer
    if(knob::dram_cntlr_enable_ddrp_buffer)
    {
        bool hit = lookup_ddrp_buffer(packet->address);
        if(hit)
        {
            // RBERA_TODO: do we want to model latency here?
            if (return_data_to_core) 
            {
                // RBERA_TODO: what should be the data payload of the packet?
                if (packet->instruction) 
                    upper_level_icache[packet->cpu]->return_data(packet);
                if (packet->is_data) 
                    upper_level_dcache[packet->cpu]->return_data(packet);

                stats.dram_process.returned[packet->type]++;
            }

            DDRP_DP ( if(warmup_complete[packet->cpu]) {
            cout << "[" << NAME << "_RQ] " <<  __func__ << " instr_id: " << packet->instr_id << " address: " << hex << packet->address;
            cout << " full_addr: " << packet->full_addr << dec << " DDRP_BUFFER_FORWARD"; });

            if(packet->fill_level <= FILL_LLC)
            {
                stats.ddrp.llc_miss.ddrp_buffer_hit[packet->type]++;
                stats.ddrp.llc_miss.total[packet->type]++;
            }
            else if(packet->fill_level == FILL_DDRP)
            {
                stats.ddrp.ddrp_req.ddrp_buffer_hit++;
                stats.ddrp.ddrp_req.total++;
            }

            return -1;
        }
    }
    // check for duplicates in the read queue
    int index = check_dram_queue(&RQ[channel], packet);
    // request should not merge in DRAM's RQ, unless DDRP is turned on
    assert(index == -1 || knob::enable_ddrp);
    if (index != -1)
    {
        DDRP_DP ( if(warmup_complete[packet->cpu]) {
        cout << "[" << NAME << "_RQ] " <<  __func__ << " instr_id: " << packet->instr_id << " address: " << hex << packet->address;
        cout << " full_addr: " << packet->full_addr << dec << " LLC_MISS_MERGE_IN_RQ";
        cout << " occupancy: " << RQ[channel].occupancy << " current: " << current_core_cycle[packet->cpu] << " event: " << packet->event_cycle << endl; });

        stats.rq_merged++;
        if(RQ[channel].entry[index].fill_level == FILL_DDRP)
        {
            if(packet->fill_level <= FILL_LLC) // incoming LLC miss
            {
                // RBERA_TODO: properly merge the incoming LLC miss to
                // the exsisting DDRP request so that we can correctly
                // return data back to the core
                uint8_t tmp_scheduled = RQ[channel].entry[index].scheduled;
                uint64_t tmp_event_cycle = RQ[channel].entry[index].event_cycle;
                uint64_t tmp_enque_cycle = RQ[channel].entry[index].enque_cycle[IS_DRAM][IS_RQ];
                RQ[channel].entry[index] = *packet; // merge
                RQ[channel].entry[index].scheduled = tmp_scheduled;
                RQ[channel].entry[index].event_cycle = tmp_event_cycle;
                RQ[channel].entry[index].enque_cycle[IS_DRAM][IS_RQ] = tmp_enque_cycle;
                stats.ddrp.llc_miss.rq_hit[RQ[channel].entry[index].type]++;
                stats.ddrp.llc_miss.total[RQ[channel].entry[index].type]++;
            }
            else if(packet->fill_level == FILL_DDRP) // incoming DDRP request
            {
                ; // no need to do anything, as
                // this is a DDRP request hitting
                // another DDRP request in RQ
                stats.ddrp.ddrp_req.rq_hit[0]++;
                stats.ddrp.ddrp_req.total++;
            }
            else{
                assert(false);
                return -2;
            }
        }
        else if(RQ[channel].entry[index].fill_level <= FILL_LLC)
        {
            if(packet->fill_level <= FILL_LLC) // incoming LLC miss
            {
                // the incoming packet and RQ's packet both cannot be LLC miss requests
                assert(false);
                return -2;
            }
            else if(packet->fill_level == FILL_DDRP) // incoming DDRP request
            {
                ; // no need to upgrade the request, 
                // as the request that already went
                // t0 DRAM is requested by core.
                stats.ddrp.ddrp_req.rq_hit[1]++;
                stats.ddrp.ddrp_req.total++;
            }
            else{
                assert(false);
                return -2;
            }
        }
        else{
            assert(false);
            return -2;
        }

        return index; // merged index
    }

    // search for the empty index
    for (index=0; index<DRAM_RQ_SIZE; index++) {
        if (RQ[channel].entry[index].address == 0) {
            
            RQ[channel].entry[index] = *packet;
            RQ[channel].occupancy++;
            RQ[channel].entry[index].enque_cycle[IS_DRAM][IS_RQ] = uncore.cycle;
            rq_enqueue_count++;

            // cout << "[ENQUEUE_" << RQ[channel].NAME << "] " << " id: " << packet->id << " cpu: " << packet->cpu << " instr_id: " << packet->instr_id;
            // cout << " address: " << hex << packet->address << " full_addr: " << packet->full_addr << dec;
            // cout << " instruction: " << (uint32_t)packet->instruction << " is_data: " << (uint32_t)packet->is_data; 
            // cout << " timestamp: " << uncore.cycle;
            // cout << " packet_enq_timestamp: " << packet->enque_cycle[IS_DRAM][IS_RQ] << " packet_deq_timestamp: " << packet->deque_cycle[IS_DRAM][IS_RQ];
            // cout << " entry_enq_timestamp: " << RQ[channel].entry[index].enque_cycle[IS_DRAM][IS_RQ] << " entry_deq_timestamp: " << RQ[channel].entry[index].deque_cycle[IS_DRAM][IS_RQ] << endl;

#ifdef DEBUG_PRINT
            uint32_t channel = dram_get_channel(packet->address),
                     rank = dram_get_rank(packet->address),
                     bank = dram_get_bank(packet->address),
                     row = dram_get_row(packet->address),
                     column = dram_get_column(packet->address); 
#endif

#ifdef PRINT_QUEUE_TRACE
           if(packet->instr_id == QTRACE_INSTR_ID)
            {
            uint32_t channel = dram_get_channel(packet->address),
                     rank = dram_get_rank(packet->address),
                     bank = dram_get_bank(packet->address),
                     row = dram_get_row(packet->address),
                     column = dram_get_column(packet->address);
            cout << "[" << NAME << "_RQ] " <<  __func__ << " instr_id: " << packet->instr_id << " address: " << hex << packet->address;
            cout << " full_addr: " << packet->full_addr << dec << " ch: " << channel;
            cout << " rank: " << rank << " bank: " << bank << " row: " << row << " col: " << column;
            cout << " occupancy: " << RQ[channel].occupancy << " current: " << current_core_cycle[packet->cpu] << " event: " << packet->event_cycle << " cpu: "<<packet->cpu<<endl;
    }
#endif


            DP ( if(warmup_complete[packet->cpu]) {
            cout << "[" << NAME << "_RQ] " <<  __func__ << " instr_id: " << packet->instr_id << " address: " << hex << packet->address;
            cout << " full_addr: " << packet->full_addr << dec << " ch: " << channel;
            cout << " rank: " << rank << " bank: " << bank << " row: " << row << " col: " << column;
            cout << " occupancy: " << RQ[channel].occupancy << " current: " << current_core_cycle[packet->cpu] << " event: " << packet->event_cycle << endl; });

            break;
        }
    }
    if(packet->fill_level <= FILL_LLC)
    {
        stats.ddrp.llc_miss.went_to_dram[packet->type]++;
        stats.ddrp.llc_miss.total[packet->type]++;
    }
    else if(packet->fill_level == FILL_DDRP)
    {
        stats.ddrp.ddrp_req.went_to_dram++;
        stats.ddrp.ddrp_req.total++;
    }
    update_schedule_cycle(&RQ[channel]);

    return -1;
}

int MEMORY_CONTROLLER::add_wq(PACKET *packet)
{
    // simply drop write requests before the warmup
    if (all_warmup_complete < NUM_CPUS)
        return -1;

    // check for duplicates in the write queue
    uint32_t channel = dram_get_channel(packet->address);
    int index = check_dram_queue(&WQ[channel], packet);
    if (index != -1)
        return index; // merged index

    // search for the empty index
    for (index=0; index<DRAM_WQ_SIZE; index++) {
        if (WQ[channel].entry[index].address == 0) {
            
            WQ[channel].entry[index] = *packet;
            WQ[channel].occupancy++;
            WQ[channel].entry[index].enque_cycle[IS_DRAM][IS_WQ] = uncore.cycle;

#ifdef DEBUG_PRINT
            uint32_t channel = dram_get_channel(packet->address),
                     rank = dram_get_rank(packet->address),
                     bank = dram_get_bank(packet->address),
                     row = dram_get_row(packet->address),
                     column = dram_get_column(packet->address); 
#endif


#ifdef PRINT_QUEUE_TRACE
            if(packet->instr_id == QTRACE_INSTR_ID)
            {
            uint32_t channel = dram_get_channel(packet->address),
                     rank = dram_get_rank(packet->address),
                     bank = dram_get_bank(packet->address),
                     row = dram_get_row(packet->address),
                     column = dram_get_column(packet->address);
                  cout << "[" << NAME << "_WQ] " <<  __func__ << " instr_id: " << packet->instr_id << " address: " << hex << packet->address;
            cout << " full_addr: " << packet->full_addr << dec << " ch: " << channel;
            cout << " rank: " << rank << " bank: " << bank << " row: " << row << " col: " << column;
            cout << " occupancy: " << WQ[channel].occupancy << " current: " << current_core_cycle[packet->cpu] << " event: " << packet->event_cycle << " cpu: "<<packet->cpu<<endl;
    }
#endif


            DP ( if(warmup_complete[packet->cpu]) {
            cout << "[" << NAME << "_WQ] " <<  __func__ << " instr_id: " << packet->instr_id << " address: " << hex << packet->address;
            cout << " full_addr: " << packet->full_addr << dec << " ch: " << channel;
            cout << " rank: " << rank << " bank: " << bank << " row: " << row << " col: " << column;
            cout << " occupancy: " << WQ[channel].occupancy << " current: " << current_core_cycle[packet->cpu] << " event: " << packet->event_cycle << endl; });

            break;
        }
    }

    update_schedule_cycle(&WQ[channel]);

    return -1;
}

int MEMORY_CONTROLLER::add_pq(PACKET *packet)
{
    return -1;
}

void MEMORY_CONTROLLER::return_data(PACKET *packet)
{

}

void MEMORY_CONTROLLER::update_schedule_cycle(PACKET_QUEUE *queue)
{
    // update next_schedule_cycle
    uint64_t min_cycle = UINT64_MAX;
    uint32_t min_index = queue->SIZE;
    for (uint32_t i=0; i<queue->SIZE; i++) {
        /*
        DP (if (warmup_complete[queue->entry[min_index].cpu]) {
        cout << "[" << queue->NAME << "] " <<  __func__ << " instr_id: " << queue->entry[i].instr_id;
        cout << " index: " << i << " address: " << hex << queue->entry[i].address << dec << " scheduled: " << +queue->entry[i].scheduled;
        cout << " event: " << queue->entry[i].event_cycle << " min_cycle: " << min_cycle << endl;
        });
        */

        if (queue->entry[i].address && (queue->entry[i].scheduled == 0) && (queue->entry[i].event_cycle < min_cycle)) {
            min_cycle = queue->entry[i].event_cycle;
            min_index = i;
        }
    }
    
    queue->next_schedule_cycle = min_cycle;
    queue->next_schedule_index = min_index;
    if (min_index < queue->SIZE) {

        DP (if (warmup_complete[queue->entry[min_index].cpu]) {
        cout << "[" << queue->NAME << "] " <<  __func__ << " instr_id: " << queue->entry[min_index].instr_id;
        cout << " address: " << hex << queue->entry[min_index].address << " full_addr: " << queue->entry[min_index].full_addr;
        cout << " data: " << queue->entry[min_index].data << dec;
        cout << " event: " << queue->entry[min_index].event_cycle << " current: " << current_core_cycle[queue->entry[min_index].cpu] << " next: " << queue->next_schedule_cycle << endl; });
    }
}

void MEMORY_CONTROLLER::update_process_cycle(PACKET_QUEUE *queue)
{
    // update next_process_cycle
    uint64_t min_cycle = UINT64_MAX;
    uint32_t min_index = queue->SIZE;
    for (uint32_t i=0; i<queue->SIZE; i++) {
        if (queue->entry[i].scheduled && (queue->entry[i].event_cycle < min_cycle)) {
            min_cycle = queue->entry[i].event_cycle;
            min_index = i;
        }
    }
    
    queue->next_process_cycle = min_cycle;
    queue->next_process_index = min_index;
    if (min_index < queue->SIZE) {

        DP (if (warmup_complete[queue->entry[min_index].cpu]) {
        cout << "[" << queue->NAME << "] " <<  __func__ << " instr_id: " << queue->entry[min_index].instr_id;
        cout << " address: " << hex << queue->entry[min_index].address << " full_addr: " << queue->entry[min_index].full_addr;
        cout << " data: " << queue->entry[min_index].data << dec << " num_returned: " << queue->num_returned;
        cout << " event: " << queue->entry[min_index].event_cycle << " current: " << current_core_cycle[queue->entry[min_index].cpu] << " next: " << queue->next_process_cycle << endl; });
    }
}

int MEMORY_CONTROLLER::check_dram_queue(PACKET_QUEUE *queue, PACKET *packet)
{
    // search write queue
    for (uint32_t index=0; index<queue->SIZE; index++) {
        if (queue->entry[index].address == packet->address) {
            
            DP ( if (warmup_complete[packet->cpu]) {
            cout << "[" << queue->NAME << "] " << __func__ << " same entry instr_id: " << packet->instr_id << " prior_id: " << queue->entry[index].instr_id;
            cout << " address: " << hex << packet->address << " full_addr: " << packet->full_addr << dec << endl; });

            return index;
        }
    }

    DP ( if (warmup_complete[packet->cpu]) {
    cout << "[" << queue->NAME << "] " << __func__ << " new address: " << hex << packet->address;
    cout << " full_addr: " << packet->full_addr << dec << endl; });

    DP ( if (warmup_complete[packet->cpu] && (queue->occupancy == queue->SIZE)) {
    cout << "[" << queue->NAME << "] " << __func__ << " mshr is full";
    cout << " instr_id: " << packet->instr_id << " mshr occupancy: " << queue->occupancy;
    cout << " address: " << hex << packet->address;
    cout << " full_addr: " << packet->full_addr << dec;
    cout << " cycle: " << current_core_cycle[packet->cpu] << endl; });

    return -1;
}

uint32_t MEMORY_CONTROLLER::dram_get_channel(uint64_t address)
{
    if (LOG2_DRAM_CHANNELS == 0)
        return 0;

    int shift = 0;

    return (uint32_t) (address >> shift) & (DRAM_CHANNELS - 1);
}

uint32_t MEMORY_CONTROLLER::dram_get_bank(uint64_t address)
{
    if (LOG2_DRAM_BANKS == 0)
        return 0;

    int shift = LOG2_DRAM_CHANNELS;

    return (uint32_t) (address >> shift) & (DRAM_BANKS - 1);
}

uint32_t MEMORY_CONTROLLER::dram_get_column(uint64_t address)
{
    if (LOG2_DRAM_COLUMNS == 0)
        return 0;

    int shift = LOG2_DRAM_BANKS + LOG2_DRAM_CHANNELS;

    return (uint32_t) (address >> shift) & (DRAM_COLUMNS - 1);
}

uint32_t MEMORY_CONTROLLER::dram_get_rank(uint64_t address)
{
    if (LOG2_DRAM_RANKS == 0)
        return 0;

    int shift = LOG2_DRAM_COLUMNS + LOG2_DRAM_BANKS + LOG2_DRAM_CHANNELS;

    return (uint32_t) (address >> shift) & (DRAM_RANKS - 1);
}

uint32_t MEMORY_CONTROLLER::dram_get_row(uint64_t address)
{
    if (LOG2_DRAM_ROWS == 0)
        return 0;

    int shift = LOG2_DRAM_RANKS + LOG2_DRAM_COLUMNS + LOG2_DRAM_BANKS + LOG2_DRAM_CHANNELS;

    return (uint32_t) (address >> shift) & (DRAM_ROWS - 1);
}

uint32_t MEMORY_CONTROLLER::get_occupancy(uint8_t queue_type, uint64_t address)
{
    uint32_t channel = dram_get_channel(address);
    if (queue_type == 1)
        return RQ[channel].occupancy;
    else if (queue_type == 2)
        return WQ[channel].occupancy;

    return 0;
}

uint32_t MEMORY_CONTROLLER::get_size(uint8_t queue_type, uint64_t address)
{
    uint32_t channel = dram_get_channel(address);
    if (queue_type == 1)
        return RQ[channel].SIZE;
    else if (queue_type == 2)
        return WQ[channel].SIZE;

    return 0;
}

void MEMORY_CONTROLLER::increment_WQ_FULL(uint64_t address)
{
    uint32_t channel = dram_get_channel(address);
    WQ[channel].FULL++;
}

void MEMORY_CONTROLLER::print_DRAM_busy_stats()
{
	cout << " All warmup complete: " << unsigned(all_warmup_complete) << endl;
	for(uint32_t i = 0; i < DRAM_CHANNELS; i++)
	{
		cout << "Channel " << i << " Bank busy for read cycles: " << sum_cycles_read[i] << endl;
		cout << "Channel " << i << " Bank busy for write cycles: " << sum_cycles_write[i] << endl;
	}

	for(uint32_t i = 0; i < DRAM_CHANNELS; i++)
	{
		cout << "Channel " << i << endl;
		for(uint32_t j = 0; j < DRAM_RANKS; j++)
		{
			cout << "Rank " << j << endl;
			for(uint32_t k = 0; k <= DRAM_BANKS; k++)
			{
				cout << k << "banks busy for read cycles: " << banks_busy_read_cycles[i][j][k] << endl;
				cout << k << "banks busy for write cycles: " << banks_busy_write_cycles[i][j][k] << endl;
			}
		}
	}
}

/* DDRP BUFFER */

void MEMORY_CONTROLLER::init_ddrp_buffer()
{
    // init buffer
    ddrp_buffer.clear();
    deque<uint64_t> d;
    ddrp_buffer.resize(knob::dram_cntlr_ddrp_buffer_sets, d);
}

void MEMORY_CONTROLLER::insert_ddrp_buffer(uint64_t addr)
{
    stats.ddrp_buffer.insert.called++;
    uint32_t set = get_ddrp_buffer_set_index(addr);
    auto it = find_if(ddrp_buffer[set].begin(), ddrp_buffer[set].end(), [addr](uint64_t m_addr){return m_addr == addr;});
    if(it != ddrp_buffer[set].end())
    {
        ddrp_buffer[set].erase(it);
        ddrp_buffer[set].push_back(addr);
        stats.ddrp_buffer.insert.hit++;
    }
    else
    {
        if(ddrp_buffer[set].size() >= knob::dram_cntlr_ddrp_buffer_assoc)
        {
            ddrp_buffer[set].pop_front();
            stats.ddrp_buffer.insert.evict++;
        }
        ddrp_buffer[set].push_back(addr);
        stats.ddrp_buffer.insert.insert++;
    }
}

bool MEMORY_CONTROLLER::lookup_ddrp_buffer(uint64_t addr)
{
    stats.ddrp_buffer.lookup.called++;
    uint32_t set = get_ddrp_buffer_set_index(addr);
    auto it = find_if(ddrp_buffer[set].begin(), ddrp_buffer[set].end(), [addr](uint64_t m_addr){return m_addr == addr;});
    if(it != ddrp_buffer[set].end())
    {
        stats.ddrp_buffer.lookup.hit++;
        return true;
    }
    else
    {
        stats.ddrp_buffer.lookup.miss++;
        return false;
    }
}

uint32_t MEMORY_CONTROLLER::get_ddrp_buffer_set_index(uint64_t address)
{
    uint32_t hash = folded_xor(address, 2);
    hash = HashZoo::getHash(knob::dram_cntlr_ddrp_buffer_hash_type, hash);
    return (hash % knob::dram_cntlr_ddrp_buffer_sets);
}
