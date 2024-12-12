#include <iostream>
#include "ooo_cpu.h"
#include "offchip_pred_base.h"
#include "offchip_pred_perc.h"

namespace knob
{
    string offchip_pred_type = "perc";
    extern bool  offchip_pred_mark_merged_load ;
}

void O3_CPU::initialize_offchip_predictor(uint64_t seed)
{
        cout << "Adding Offchip predictor: perceptron-based" << endl;
        offchip_pred = (OffchipPredPerc*) new OffchipPredPerc(cpu, knob::offchip_pred_type, seed);
}

void O3_CPU::print_config_offchip_predictor()
{
    cout << "offchip_pred_type " << knob::offchip_pred_type << endl
         << "offchip_pred_mark_merged_load " << knob::offchip_pred_mark_merged_load << endl
         << endl;

    offchip_pred->print_config();
}

void O3_CPU::dump_stats_offchip_predictor()
{
    // float precision = (float)stats.offchip_pred.true_pos / (stats.offchip_pred.true_pos + stats.offchip_pred.false_pos),
    //       recall = (float)stats.offchip_pred.true_pos / (stats.offchip_pred.true_pos + stats.offchip_pred.false_neg);
          

    // cout << "Core_" << cpu << "_offchip_pred_true_pos " << stats.offchip_pred.true_pos << endl
    //      << "Core_" << cpu << "_offchip_pred_false_pos " << stats.offchip_pred.false_pos << endl
    //      << "Core_" << cpu << "_offchip_pred_false_neg " << stats.offchip_pred.false_neg << endl
    //      << "Core_" << cpu << "_offchip_pred_precision " << precision*100 << endl
    //      << "Core_" << cpu << "_offchip_pred_recall " << recall*100 << endl
    //      << endl;

    offchip_pred->dump_stats();
}

void O3_CPU::offchip_predictor_update_dram_bw(uint8_t dram_bw)
{
    if(offchip_pred) offchip_pred->update_dram_bw(dram_bw);
}

/* This function is called at every LLC eviction.
 * Cache-level prediction mechanisms that operate as tag-tracking mechanisms
 * need to track LLC evictions to make accurate off-chip predictions.
 * This function is written to track such evictions */
void O3_CPU::offchip_predictor_track_llc_eviction(uint32_t set, uint32_t way, uint64_t address)
{
	// if(!knob::offchip_pred_type.compare("ttp"))
	// {
	// 	OffchipPredTTP *ocp_lp = (OffchipPredTTP*) offchip_pred;
	// 	ocp_lp->track_llc_eviction(address);
	// }
}