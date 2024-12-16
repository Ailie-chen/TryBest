#!/bin/bash



truncate -s 0 deltas_out.txt
traces_all=false
run_now=false
NUM_THREAD=54
DATE_SET="1216"
prefs=( "hyperion_hpc_2table_UTBh1_buffer8_8")
# prefs=( "no" )
output_base="outputsum/output${DATE_SET}/"
res_test_name="debug"
tmp_par_path="${DATE_SET}tmp_par.out"
traces=( "parsec" "gap" )
trace_base_path="~/workpath/prefetch/Berti-Artifact/traces/"
# 设置参数
# warmupInstructions=1400000
# simulationInstructions=10000
warmupInstructions=20000000
simulationInstructions=80000000

spec2k17_traces=(
    # "403.gcc-17B.champsimtrace.xz"
             "401.bzip2-277B.champsimtrace.xz"
            # "401.bzip2-7B.champsimtrace.xz"
            # "471.omnetpp-188B.champsimtrace.xz"
            # "400.perlbench-41B.champsimtrace.xz"
            # "654.roms_s-1007B.champsimtrace.xz"
            # "602.gcc_s-734B.champsimtrace.xz"
            # "401.bzip2-277B.champsimtrace.xz"
            # "621.wrf_s-6673B.champsimtrace.xz"
            "620.omnetpp_s-141B.champsimtrace.xz"
             )
spec2k17_traces1=(
            "401.bzip2-277B.champsimtrace.xz"
            "403.gcc-17B.champsimtrace.xz"
            "410.bwaves-1963B.champsimtrace.xz"
            "410.bwaves-2097B.champsimtrace.xz"
            "429.mcf-184B.champsimtrace.xz"
            "429.mcf-192B.champsimtrace.xz"
            "429.mcf-217B.champsimtrace.xz"
            "429.mcf-22B.champsimtrace.xz"
            "429.mcf-51B.champsimtrace.xz"
            "433.milc-127B.champsimtrace.xz"
            "433.milc-274B.champsimtrace.xz"
            "433.milc-337B.champsimtrace.xz"
            "434.zeusmp-10B.champsimtrace.xz"
            "436.cactusADM-1804B.champsimtrace.xz"
            "437.leslie3d-134B.champsimtrace.xz"
            "437.leslie3d-149B.champsimtrace.xz"
            "437.leslie3d-232B.champsimtrace.xz"
            "437.leslie3d-265B.champsimtrace.xz"
            "437.leslie3d-271B.champsimtrace.xz"
            "437.leslie3d-273B.champsimtrace.xz"
            "450.soplex-247B.champsimtrace.xz"
            "450.soplex-92B.champsimtrace.xz"
            "459.GemsFDTD-1169B.champsimtrace.xz"
            "459.GemsFDTD-1211B.champsimtrace.xz"
            "459.GemsFDTD-1320B.champsimtrace.xz"
            "459.GemsFDTD-1418B.champsimtrace.xz"
            "459.GemsFDTD-1491B.champsimtrace.xz"
            "459.GemsFDTD-765B.champsimtrace.xz"
            "462.libquantum-1343B.champsimtrace.xz"
            "462.libquantum-714B.champsimtrace.xz"
            "470.lbm-1274B.champsimtrace.xz"
            "471.omnetpp-188B.champsimtrace.xz"
            "473.astar-359B.champsimtrace.xz"
            "473.astar-42B.champsimtrace.xz"
            "481.wrf-1254B.champsimtrace.xz"
            "481.wrf-1281B.champsimtrace.xz"
            "481.wrf-196B.champsimtrace.xz"
            "481.wrf-455B.champsimtrace.xz"
            "481.wrf-816B.champsimtrace.xz"
            "482.sphinx3-1100B.champsimtrace.xz"
            "482.sphinx3-1297B.champsimtrace.xz"
            "482.sphinx3-1395B.champsimtrace.xz"
            "482.sphinx3-1522B.champsimtrace.xz"
            "482.sphinx3-234B.champsimtrace.xz"
            "482.sphinx3-417B.champsimtrace.xz"
            "483.xalancbmk-127B.champsimtrace.xz"
            "602.gcc_s-1850B.champsimtrace.xz"
            "602.gcc_s-2226B.champsimtrace.xz"
            "602.gcc_s-734B.champsimtrace.xz"
            "603.bwaves_s-1740B.champsimtrace.xz"
            "603.bwaves_s-2609B.champsimtrace.xz"
            "603.bwaves_s-2931B.champsimtrace.xz"
            "603.bwaves_s-891B.champsimtrace.xz"
            "605.mcf_s-1152B.champsimtrace.xz"
            "605.mcf_s-1536B.champsimtrace.xz"
            "605.mcf_s-1554B.champsimtrace.xz"
            "605.mcf_s-1644B.champsimtrace.xz"
            "605.mcf_s-472B.champsimtrace.xz"
            "605.mcf_s-484B.champsimtrace.xz"
            "605.mcf_s-665B.champsimtrace.xz"
            "605.mcf_s-782B.champsimtrace.xz"
            "605.mcf_s-994B.champsimtrace.xz"
            "607.cactuBSSN_s-2421B.champsimtrace.xz"
            "607.cactuBSSN_s-3477B.champsimtrace.xz"
            "607.cactuBSSN_s-4004B.champsimtrace.xz"
            "619.lbm_s-2676B.champsimtrace.xz"
            "619.lbm_s-2677B.champsimtrace.xz"
            "619.lbm_s-3766B.champsimtrace.xz"
            "619.lbm_s-4268B.champsimtrace.xz"
            "620.omnetpp_s-141B.champsimtrace.xz"
            "620.omnetpp_s-874B.champsimtrace.xz"
            "621.wrf_s-6673B.champsimtrace.xz"
            "621.wrf_s-8065B.champsimtrace.xz"
            "623.xalancbmk_s-10B.champsimtrace.xz"
            "623.xalancbmk_s-165B.champsimtrace.xz"
            "623.xalancbmk_s-202B.champsimtrace.xz"
            "625.x264_s-20B.champsimtrace.xz"
            "627.cam4_s-490B.champsimtrace.xz"
            "628.pop2_s-17B.champsimtrace.xz"
            "649.fotonik3d_s-10881B.champsimtrace.xz"
            "649.fotonik3d_s-1176B.champsimtrace.xz"
            "649.fotonik3d_s-7084B.champsimtrace.xz"
            "649.fotonik3d_s-8225B.champsimtrace.xz"
            "654.roms_s-1007B.champsimtrace.xz"
            "654.roms_s-1070B.champsimtrace.xz"
            "654.roms_s-1390B.champsimtrace.xz"
            "654.roms_s-1613B.champsimtrace.xz"
            "654.roms_s-293B.champsimtrace.xz"
            "654.roms_s-294B.champsimtrace.xz"
            "654.roms_s-523B.champsimtrace.xz"
            "657.xz_s-2302B.champsimtrace.xz"
)
spec2k17_traces_all=(
  "400.perlbench-41B.champsimtrace.xz"
  "400.perlbench-50B.champsimtrace.xz"
  "401.bzip2-226B.champsimtrace.xz"
  "401.bzip2-277B.champsimtrace.xz"
  "401.bzip2-38B.champsimtrace.xz"
  "401.bzip2-7B.champsimtrace.xz"
  "403.gcc-16B.champsimtrace.xz"
  "403.gcc-17B.champsimtrace.xz"
  "403.gcc-48B.champsimtrace.xz"
  "410.bwaves-1963B.champsimtrace.xz"
  "410.bwaves-2097B.champsimtrace.xz"
  "410.bwaves-945B.champsimtrace.xz"
  "416.gamess-875B.champsimtrace.xz"
  "429.mcf-184B.champsimtrace.xz"
  "429.mcf-192B.champsimtrace.xz"
  "429.mcf-217B.champsimtrace.xz"
  "429.mcf-22B.champsimtrace.xz"
  "429.mcf-51B.champsimtrace.xz"
  "433.milc-127B.champsimtrace.xz"
  "433.milc-274B.champsimtrace.xz"
  "433.milc-337B.champsimtrace.xz"
  "434.zeusmp-10B.champsimtrace.xz"
  "435.gromacs-111B.champsimtrace.xz"
  "435.gromacs-134B.champsimtrace.xz"
  "435.gromacs-226B.champsimtrace.xz"
  "435.gromacs-228B.champsimtrace.xz"
  "436.cactusADM-1804B.champsimtrace.xz"
  "437.leslie3d-134B.champsimtrace.xz"
  "437.leslie3d-149B.champsimtrace.xz"
  "437.leslie3d-232B.champsimtrace.xz"
  "437.leslie3d-265B.champsimtrace.xz"
  "437.leslie3d-271B.champsimtrace.xz"
  "437.leslie3d-273B.champsimtrace.xz"
  "444.namd-120B.champsimtrace.xz"
  "444.namd-166B.champsimtrace.xz"
  "444.namd-23B.champsimtrace.xz"
  "444.namd-321B.champsimtrace.xz"
  "444.namd-33B.champsimtrace.xz"
  "444.namd-426B.champsimtrace.xz"
  "444.namd-44B.champsimtrace.xz"
  "445.gobmk-17B.champsimtrace.xz"
  "445.gobmk-2B.champsimtrace.xz"
  "445.gobmk-30B.champsimtrace.xz"
  "445.gobmk-36B.champsimtrace.xz"
  "447.dealII-3B.champsimtrace.xz"
  "450.soplex-247B.champsimtrace.xz"
  "450.soplex-92B.champsimtrace.xz"
  "453.povray-252B.champsimtrace.xz"
  "453.povray-576B.champsimtrace.xz"
  "453.povray-800B.champsimtrace.xz"
  "453.povray-887B.champsimtrace.xz"
  "454.calculix-104B.champsimtrace.xz"
  "454.calculix-460B.champsimtrace.xz"
  "456.hmmer-191B.champsimtrace.xz"
  "456.hmmer-327B.champsimtrace.xz"
  "456.hmmer-88B.champsimtrace.xz"
  "458.sjeng-1088B.champsimtrace.xz"
  "458.sjeng-283B.champsimtrace.xz"
  "458.sjeng-31B.champsimtrace.xz"
  "458.sjeng-767B.champsimtrace.xz"
  "459.GemsFDTD-1169B.champsimtrace.xz"
  "459.GemsFDTD-1211B.champsimtrace.xz"
  "459.GemsFDTD-1320B.champsimtrace.xz"
  "459.GemsFDTD-1418B.champsimtrace.xz"
  "459.GemsFDTD-1491B.champsimtrace.xz"
  "459.GemsFDTD-765B.champsimtrace.xz"
  "462.libquantum-1343B.champsimtrace.xz"
  "462.libquantum-714B.champsimtrace.xz"
  "464.h264ref-30B.champsimtrace.xz"
  "464.h264ref-57B.champsimtrace.xz"
  "464.h264ref-64B.champsimtrace.xz"
  "464.h264ref-97B.champsimtrace.xz"
  "465.tonto-1914B.champsimtrace.xz"
  "465.tonto-44B.champsimtrace.xz"
  "470.lbm-1274B.champsimtrace.xz"
  "471.omnetpp-188B.champsimtrace.xz"
  "473.astar-153B.champsimtrace.xz"
  "473.astar-359B.champsimtrace.xz"
  "473.astar-42B.champsimtrace.xz"
  "481.wrf-1170B.champsimtrace.xz"
  "481.wrf-1254B.champsimtrace.xz"
  "481.wrf-1281B.champsimtrace.xz"
  "481.wrf-196B.champsimtrace.xz"
  "481.wrf-455B.champsimtrace.xz"
  "481.wrf-816B.champsimtrace.xz"
  "482.sphinx3-1100B.champsimtrace.xz"
  "482.sphinx3-1297B.champsimtrace.xz"
  "482.sphinx3-1395B.champsimtrace.xz"
  "482.sphinx3-1522B.champsimtrace.xz"
  "482.sphinx3-234B.champsimtrace.xz"
  "482.sphinx3-417B.champsimtrace.xz"
  "483.xalancbmk-127B.champsimtrace.xz"
  "483.xalancbmk-716B.champsimtrace.xz"
  "600.perlbench_s-1273B.champsimtrace.xz"
  "600.perlbench_s-210B.champsimtrace.xz"
  "600.perlbench_s-570B.champsimtrace.xz"
  "602.gcc_s-1850B.champsimtrace.xz"
  "602.gcc_s-2226B.champsimtrace.xz"
  "602.gcc_s-2375B.champsimtrace.xz"
  "602.gcc_s-734B.champsimtrace.xz"
  "603.bwaves_s-1080B.champsimtrace.xz"
  "603.bwaves_s-1740B.champsimtrace.xz"
  "603.bwaves_s-2609B.champsimtrace.xz"
  "603.bwaves_s-2931B.champsimtrace.xz"
  "603.bwaves_s-3699B.champsimtrace.xz"
  "603.bwaves_s-5359B.champsimtrace.xz"
  "603.bwaves_s-891B.champsimtrace.xz"
  "605.mcf_s-1152B.champsimtrace.xz"
  "605.mcf_s-1536B.champsimtrace.xz"
  "605.mcf_s-1554B.champsimtrace.xz"
  "605.mcf_s-1644B.champsimtrace.xz"
  "605.mcf_s-472B.champsimtrace.xz"
  "605.mcf_s-484B.champsimtrace.xz"
  "605.mcf_s-665B.champsimtrace.xz"
  "605.mcf_s-782B.champsimtrace.xz"
  "605.mcf_s-994B.champsimtrace.xz"
  "607.cactuBSSN_s-2421B.champsimtrace.xz"
  "607.cactuBSSN_s-3477B.champsimtrace.xz"
  "607.cactuBSSN_s-4004B.champsimtrace.xz"
  "607.cactuBSSN_s-4248B.champsimtrace.xz"
  "619.lbm_s-2676B.champsimtrace.xz"
  "619.lbm_s-2677B.champsimtrace.xz"
  "619.lbm_s-3766B.champsimtrace.xz"
  "619.lbm_s-4268B.champsimtrace.xz"
  "620.omnetpp_s-141B.champsimtrace.xz"
  "620.omnetpp_s-874B.champsimtrace.xz"
  "621.wrf_s-575B.champsimtrace.xz"
  "621.wrf_s-6673B.champsimtrace.xz"
  "621.wrf_s-8065B.champsimtrace.xz"
  "621.wrf_s-8100B.champsimtrace.xz"
  "623.xalancbmk_s-10B.champsimtrace.xz"
  "623.xalancbmk_s-165B.champsimtrace.xz"
  "623.xalancbmk_s-202B.champsimtrace.xz"
  "623.xalancbmk_s-325B.champsimtrace.xz"
  "623.xalancbmk_s-592B.champsimtrace.xz"
  "623.xalancbmk_s-700B.champsimtrace.xz"
  "625.x264_s-12B.champsimtrace.xz"
  "625.x264_s-18B.champsimtrace.xz"
  "625.x264_s-20B.champsimtrace.xz"
  "625.x264_s-33B.champsimtrace.xz"
  "625.x264_s-39B.champsimtrace.xz"
  "627.cam4_s-490B.champsimtrace.xz"
  "627.cam4_s-573B.champsimtrace.xz"
  "628.pop2_s-17B.champsimtrace.xz"
  "631.deepsjeng_s-928B.champsimtrace.xz"
  "638.imagick_s-10316B.champsimtrace.xz"
  "638.imagick_s-4128B.champsimtrace.xz"
  "638.imagick_s-824B.champsimtrace.xz"
  "641.leela_s-1052B.champsimtrace.xz"
  "641.leela_s-1083B.champsimtrace.xz"
  "641.leela_s-149B.champsimtrace.xz"
  "641.leela_s-334B.champsimtrace.xz"
  "641.leela_s-602B.champsimtrace.xz"
  "641.leela_s-800B.champsimtrace.xz"
  "641.leela_s-862B.champsimtrace.xz"
  "644.nab_s-12459B.champsimtrace.xz"
  "644.nab_s-12521B.champsimtrace.xz"
  "644.nab_s-5853B.champsimtrace.xz"
  "644.nab_s-7928B.champsimtrace.xz"
  "644.nab_s-9322B.champsimtrace.xz"
  "648.exchange2_s-1227B.champsimtrace.xz"
  "648.exchange2_s-1247B.champsimtrace.xz"
  "648.exchange2_s-1511B.champsimtrace.xz"
  "648.exchange2_s-1699B.champsimtrace.xz"
  "648.exchange2_s-1712B.champsimtrace.xz"
  "648.exchange2_s-210B.champsimtrace.xz"
  "648.exchange2_s-353B.champsimtrace.xz"
  "648.exchange2_s-387B.champsimtrace.xz"
  "648.exchange2_s-584B.champsimtrace.xz"
  "648.exchange2_s-72B.champsimtrace.xz"
  "649.fotonik3d_s-10881B.champsimtrace.xz"
  "649.fotonik3d_s-1176B.champsimtrace.xz"
  "649.fotonik3d_s-1B.champsimtrace.xz"
  "649.fotonik3d_s-7084B.champsimtrace.xz"
  "649.fotonik3d_s-8225B.champsimtrace.xz"
  "654.roms_s-1007B.champsimtrace.xz"
  "654.roms_s-1021B.champsimtrace.xz"
  "654.roms_s-1070B.champsimtrace.xz"
  "654.roms_s-1390B.champsimtrace.xz"
  "654.roms_s-1613B.champsimtrace.xz"
  "654.roms_s-293B.champsimtrace.xz"
  "654.roms_s-294B.champsimtrace.xz"
  "654.roms_s-523B.champsimtrace.xz"
  "654.roms_s-842B.champsimtrace.xz"
  "657.xz_s-2302B.champsimtrace.xz"
  "657.xz_s-3167B.champsimtrace.xz"
  "657.xz_s-4994B.champsimtrace.xz"
)
spec2k17_traces2=(
          "401.bzip2-7B.champsimtrace.xz"
          "401.bzip2-38B.champsimtrace.xz"
          "401.bzip2-226B.champsimtrace.xz"
          "401.bzip2-277B.champsimtrace.xz"
          "403.gcc-17B.champsimtrace.xz"
          "410.bwaves-1963B.champsimtrace.xz"
          "410.bwaves-2097B.champsimtrace.xz"
          "429.mcf-22B.champsimtrace.xz"
          "429.mcf-51B.champsimtrace.xz"
          "429.mcf-184B.champsimtrace.xz"
          "429.mcf-192B.champsimtrace.xz"
          "429.mcf-217B.champsimtrace.xz"
          "433.milc-127B.champsimtrace.xz"
          "433.milc-274B.champsimtrace.xz"
          "433.milc-337B.champsimtrace.xz"
          "434.zeusmp-10B.champsimtrace.xz"
          "435.gromacs-111B.champsimtrace.xz"
          "435.gromacs-134B.champsimtrace.xz"
          "435.gromacs-226B.champsimtrace.xz"
          "435.gromacs-228B.champsimtrace.xz"
          "436.cactusADM-1804B.champsimtrace.xz"
          "437.leslie3d-134B.champsimtrace.xz"
          "444.namd-23B.champsimtrace.xz"
          "444.namd-33B.champsimtrace.xz"
          "444.namd-44B.champsimtrace.xz"
          "444.namd-120B.champsimtrace.xz"
          "444.namd-166B.champsimtrace.xz"
          "444.namd-321B.champsimtrace.xz"
          "444.namd-426B.champsimtrace.xz"
          "445.gobmk-2B.champsimtrace.xz"
          "445.gobmk-36B.champsimtrace.xz"
          "450.soplex-92B.champsimtrace.xz"
          "450.soplex-247B.champsimtrace.xz"
          "453.povray-252B.champsimtrace.xz"
          "453.povray-576B.champsimtrace.xz"
          "453.povray-800B.champsimtrace.xz"
          "453.povray-887B.champsimtrace.xz"
          "454.calculix-460B.champsimtrace.xz"
          "459.GemsFDTD-765B.champsimtrace.xz"
          "459.GemsFDTD-1169B.champsimtrace.xz"
          "459.GemsFDTD-1211B.champsimtrace.xz"
          "459.GemsFDTD-1320B.champsimtrace.xz"
          "459.GemsFDTD-1491B.champsimtrace.xz"
          "462.libquantum-714B.champsimtrace.xz"
          "462.libquantum-1343B.champsimtrace.xz"
          "470.lbm-1274B.champsimtrace.xz"
          "471.omnetpp-188B.champsimtrace.xz"
          "473.astar-42B.champsimtrace.xz"
          "473.astar-153B.champsimtrace.xz"
          "473.astar-359B.champsimtrace.xz"
          "481.wrf-1170B.champsimtrace.xz"
          "482.sphinx3-234B.champsimtrace.xz"
          "482.sphinx3-417B.champsimtrace.xz"
          "482.sphinx3-1100B.champsimtrace.xz"
          "482.sphinx3-1297B.champsimtrace.xz"
          "482.sphinx3-1395B.champsimtrace.xz"
          "482.sphinx3-1522B.champsimtrace.xz"
          "483.xalancbmk-127B.champsimtrace.xz"
          "483.xalancbmk-716B.champsimtrace.xz"
          "600.perlbench_s-570B.champsimtrace.xz"
          "602.gcc_s-734B.champsimtrace.xz"
          "602.gcc_s-1850B.champsimtrace.xz"
          "602.gcc_s-2226B.champsimtrace.xz"
          "602.gcc_s-2375B.champsimtrace.xz"
          "603.bwaves_s-891B.champsimtrace.xz"
          "603.bwaves_s-1740B.champsimtrace.xz"
          "603.bwaves_s-2609B.champsimtrace.xz"
          "603.bwaves_s-2931B.champsimtrace.xz"
          "605.mcf_s-472B.champsimtrace.xz"
          "605.mcf_s-484B.champsimtrace.xz"
          "605.mcf_s-665B.champsimtrace.xz"
          "605.mcf_s-782B.champsimtrace.xz"
          "605.mcf_s-994B.champsimtrace.xz"
          "605.mcf_s-1152B.champsimtrace.xz"
          "605.mcf_s-1536B.champsimtrace.xz"
          "605.mcf_s-1554B.champsimtrace.xz"
          "605.mcf_s-1644B.champsimtrace.xz"
          "607.cactuBSSN_s-2421B.champsimtrace.xz"
          "607.cactuBSSN_s-3477B.champsimtrace.xz"
          "607.cactuBSSN_s-4004B.champsimtrace.xz"
          "619.lbm_s-2676B.champsimtrace.xz"
          "619.lbm_s-2677B.champsimtrace.xz"
          "619.lbm_s-3766B.champsimtrace.xz"
          "619.lbm_s-4268B.champsimtrace.xz"
          "620.omnetpp_s-141B.champsimtrace.xz"
          "620.omnetpp_s-874B.champsimtrace.xz"
          "621.wrf_s-6673B.champsimtrace.xz"
          "621.wrf_s-8065B.champsimtrace.xz"
          "623.xalancbmk_s-10B.champsimtrace.xz"
          "623.xalancbmk_s-165B.champsimtrace.xz"
          "623.xalancbmk_s-202B.champsimtrace.xz"
          "623.xalancbmk_s-325B.champsimtrace.xz"
          "623.xalancbmk_s-592B.champsimtrace.xz"
          "623.xalancbmk_s-700B.champsimtrace.xz"
          "625.x264_s-20B.champsimtrace.xz"
          "627.cam4_s-490B.champsimtrace.xz"
          "627.cam4_s-573B.champsimtrace.xz"
          "628.pop2_s-17B.champsimtrace.xz"
          "641.leela_s-334B.champsimtrace.xz"
          "641.leela_s-602B.champsimtrace.xz"
          "641.leela_s-800B.champsimtrace.xz"
          "641.leela_s-862B.champsimtrace.xz"
          "641.leela_s-1052B.champsimtrace.xz"
          "641.leela_s-1083B.champsimtrace.xz"
          "644.nab_s-5853B.champsimtrace.xz"
          "644.nab_s-7928B.champsimtrace.xz"
          "644.nab_s-9322B.champsimtrace.xz"
          "644.nab_s-12459B.champsimtrace.xz"
          "644.nab_s-12521B.champsimtrace.xz"
          "649.fotonik3d_s-1176B.champsimtrace.xz"
          "649.fotonik3d_s-8225B.champsimtrace.xz"
          "649.fotonik3d_s-10881B.champsimtrace.xz"
          "654.roms_s-523B.champsimtrace.xz"
          "654.roms_s-1070B.champsimtrace.xz"
          "654.roms_s-1390B.champsimtrace.xz"
          "657.xz_s-2302B.champsimtrace.xz"
          "657.xz_s-3167B.champsimtrace.xz"
)
cs_traces=(
  "cassandra_phase0_core0.trace.xz"
)
gap_traces=(
  "sssp-3.trace.gz"
)
parsec_traces1=(
        "parsec_2.1.canneal.simlarge.prebuilt.drop_0M.length_250M.champsimtrace.xz"
        "parsec_2.1.canneal.simlarge.prebuilt.drop_500M.length_250M.champsimtrace.xz"
        "parsec_2.1.canneal.simlarge.prebuilt.drop_1250M.length_250M.champsimtrace.xz"
        "parsec_2.1.canneal.simlarge.prebuilt.drop_3000M.length_250M.champsimtrace.xz"
        "parsec_2.1.canneal.simlarge.prebuilt.drop_4500M.length_250M.champsimtrace.xz"
        "parsec_2.1.canneal.simlarge.prebuilt.drop_4750M.length_250M.champsimtrace.xz"
        "parsec_2.1.canneal.simlarge.prebuilt.drop_5000M.length_15M.champsimtrace.xz"
        "parsec_2.1.streamcluster.simlarge.prebuilt.drop_0M.length_250M.champsimtrace.xz"
        "parsec_2.1.streamcluster.simlarge.prebuilt.drop_250M.length_250M.champsimtrace.xz"
        "parsec_2.1.streamcluster.simlarge.prebuilt.drop_4750M.length_250M.champsimtrace.xz"
        "parsec_2.1.streamcluster.simlarge.prebuilt.drop_6250M.length_250M.champsimtrace.xz"
        "parsec_2.1.streamcluster.simlarge.prebuilt.drop_14750M.length_250M.champsimtrace.xz"
        "parsec_2.1.facesim.simlarge.prebuilt.drop_1500M.length_250M.champsimtrace.xz"
        "parsec_2.1.facesim.simlarge.prebuilt.drop_21500M.length_250M.champsimtrace.xz"
        "parsec_2.1.raytrace.simlarge.prebuilt.drop_23500M.length_250M.champsimtrace.xz"
        "parsec_2.1.raytrace.simlarge.prebuilt.drop_23750M.length_250M.champsimtrace.xz"
  
)
cs_traces1=(
    "cassandra_phase0_core0.trace.xz"
    "cassandra_phase0_core1.trace.xz"
    "cassandra_phase0_core2.trace.xz"
    "cassandra_phase0_core3.trace.xz"
    "cassandra_phase1_core0.trace.xz"
    "cassandra_phase1_core1.trace.xz"
    "cassandra_phase1_core2.trace.xz"
    "cassandra_phase1_core3.trace.xz"
    "cassandra_phase2_core0.trace.xz"
    "cassandra_phase2_core1.trace.xz"
    "cassandra_phase2_core2.trace.xz"
    "cassandra_phase2_core3.trace.xz"
    "cassandra_phase3_core0.trace.xz"
    "cassandra_phase3_core1.trace.xz"
    "cassandra_phase3_core2.trace.xz"
    "cassandra_phase3_core3.trace.xz"
    "cassandra_phase4_core0.trace.xz"
    "cassandra_phase4_core1.trace.xz"
    "cassandra_phase4_core2.trace.xz"
    "cassandra_phase4_core3.trace.xz"
    "cassandra_phase5_core0.trace.xz"
    "cassandra_phase5_core1.trace.xz"
    "cassandra_phase5_core2.trace.xz"
    "cassandra_phase5_core3.trace.xz"
    "classification_phase0_core0.trace.xz"
    "classification_phase0_core1.trace.xz"
    "classification_phase0_core2.trace.xz"
    "classification_phase0_core3.trace.xz"
    "classification_phase1_core0.trace.xz"
    "classification_phase1_core1.trace.xz"
    "classification_phase1_core2.trace.xz"
    "classification_phase1_core3.trace.xz"
    "classification_phase2_core0.trace.xz"
    "classification_phase2_core1.trace.xz"
    "classification_phase2_core2.trace.xz"
    "classification_phase2_core3.trace.xz"
    "classification_phase3_core0.trace.xz"
    "classification_phase3_core1.trace.xz"
    "classification_phase3_core2.trace.xz"
    "classification_phase3_core3.trace.xz"
    "classification_phase4_core0.trace.xz"
    "classification_phase4_core1.trace.xz"
    "classification_phase4_core2.trace.xz"
    "classification_phase4_core3.trace.xz"
    "classification_phase5_core0.trace.xz"
    "classification_phase5_core1.trace.xz"
    "classification_phase5_core2.trace.xz"
    "classification_phase5_core3.trace.xz"
    "cloud9_phase0_core0.trace.xz"
    "cloud9_phase0_core1.trace.xz"
    "cloud9_phase0_core2.trace.xz"
    "cloud9_phase0_core3.trace.xz"
    "cloud9_phase1_core0.trace.xz"
    "cloud9_phase1_core1.trace.xz"
    "cloud9_phase1_core2.trace.xz"
    "cloud9_phase1_core3.trace.xz"
    "cloud9_phase2_core0.trace.xz"
    "cloud9_phase2_core1.trace.xz"
    "cloud9_phase2_core2.trace.xz"
    "cloud9_phase2_core3.trace.xz"
    "cloud9_phase3_core0.trace.xz"
    "cloud9_phase3_core1.trace.xz"
    "cloud9_phase3_core2.trace.xz"
    "cloud9_phase3_core3.trace.xz"
    "cloud9_phase4_core0.trace.xz"
    "cloud9_phase4_core1.trace.xz"
    "cloud9_phase4_core2.trace.xz"
    "cloud9_phase4_core3.trace.xz"
    "cloud9_phase5_core0.trace.xz"
    "cloud9_phase5_core1.trace.xz"
    "cloud9_phase5_core2.trace.xz"
    "cloud9_phase5_core3.trace.xz"
    "nutch_phase0_core0.trace.xz"
    "nutch_phase0_core1.trace.xz"
    "nutch_phase0_core2.trace.xz"
    "nutch_phase0_core3.trace.xz"
    "nutch_phase1_core0.trace.xz"
    "nutch_phase1_core1.trace.xz"
    "nutch_phase1_core2.trace.xz"
    "nutch_phase1_core3.trace.xz"
    "nutch_phase2_core0.trace.xz"
    "nutch_phase2_core1.trace.xz"
    "nutch_phase2_core2.trace.xz"
    "nutch_phase2_core3.trace.xz"
    "nutch_phase3_core0.trace.xz"
    "nutch_phase3_core1.trace.xz"
    "nutch_phase3_core2.trace.xz"
    "nutch_phase3_core3.trace.xz"
    "nutch_phase4_core0.trace.xz"
    "nutch_phase4_core1.trace.xz"
    "nutch_phase4_core2.trace.xz"
    "nutch_phase4_core3.trace.xz"
    "nutch_phase5_core0.trace.xz"
    "nutch_phase5_core1.trace.xz"
    "nutch_phase5_core2.trace.xz"
    "nutch_phase5_core3.trace.xz"
    "streaming_phase0_core0.trace.xz"
    "streaming_phase0_core1.trace.xz"
    "streaming_phase0_core2.trace.xz"
    "streaming_phase0_core3.trace.xz"
    "streaming_phase1_core0.trace.xz"
    "streaming_phase1_core1.trace.xz"
    "streaming_phase1_core2.trace.xz"
    "streaming_phase1_core3.trace.xz"
    "streaming_phase2_core0.trace.xz"
    "streaming_phase2_core1.trace.xz"
    "streaming_phase2_core2.trace.xz"
    "streaming_phase2_core3.trace.xz"
    "streaming_phase3_core0.trace.xz"
    "streaming_phase3_core1.trace.xz"
    "streaming_phase3_core2.trace.xz"
    "streaming_phase3_core3.trace.xz"
    "streaming_phase4_core0.trace.xz"
    "streaming_phase4_core1.trace.xz"
    "streaming_phase4_core2.trace.xz"
    "streaming_phase4_core3.trace.xz"
    "streaming_phase5_core0.trace.xz"
    "streaming_phase5_core1.trace.xz"
    "streaming_phase5_core2.trace.xz"
    "streaming_phase5_core3.trace.xz"
)

ligra_traces1=(
    "ligra_BFSCC.com-lj.ungraph.gcc_6.3.0_O3.drop_18750M.length_250M.champsimtrace.xz"
    "ligra_CF.com-lj.ungraph.gcc_6.3.0_O3.drop_184750M.length_250M.champsimtrace.xz"
    "ligra_PageRankDelta.com-lj.ungraph.gcc_6.3.0_O3.drop_24500M.length_250M.champsimtrace.xz"
    "ligra_PageRank.com-lj.ungraph.gcc_6.3.0_O3.drop_79500M.length_250M.champsimtrace.xz"
    "ligra_Triangle.com-lj.ungraph.gcc_6.3.0_O3.drop_10250M.length_250M.champsimtrace.xz"
    "ligra_MIS.com-lj.ungraph.gcc_6.3.0_O3.drop_11000M.length_250M.champsimtrace.xz"
    "ligra_BFSCC.com-lj.ungraph.gcc_6.3.0_O3.drop_5000M.length_250M.champsimtrace.xz"
    "ligra_BFSCC.com-lj.ungraph.gcc_6.3.0_O3.drop_15500M.length_250M.champsimtrace.xz"
    "ligra_PageRank.com-lj.ungraph.gcc_6.3.0_O3.drop_60750M.length_250M.champsimtrace.xz"
    "ligra_Components-Shortcut.com-lj.ungraph.gcc_6.3.0_O3.drop_5000M.length_250M.champsimtrace.xz"
    "ligra_BFS.com-lj.ungraph.gcc_6.3.0_O3.drop_20250M.length_250M.champsimtrace.xz"
    "ligra_Radii.com-lj.ungraph.gcc_6.3.0_O3.drop_32000M.length_250M.champsimtrace.xz"
    "ligra_CF.com-lj.ungraph.gcc_6.3.0_O3.drop_2500M.length_250M.champsimtrace.xz"
    "ligra_PageRank.com-lj.ungraph.gcc_6.3.0_O3.drop_21750M.length_250M.champsimtrace.xz"
    "ligra_PageRank.com-lj.ungraph.gcc_6.3.0_O3.drop_18500M.length_250M.champsimtrace.xz"
    "ligra_Radii.com-lj.ungraph.gcc_6.3.0_O3.drop_18000M.length_250M.champsimtrace.xz"
    "ligra_BFSCC.com-lj.ungraph.gcc_6.3.0_O3.drop_17000M.length_250M.champsimtrace.xz"
    "ligra_Components-Shortcut.com-lj.ungraph.gcc_6.3.0_O3.drop_17000M.length_250M.champsimtrace.xz"
    "ligra_MIS.com-lj.ungraph.gcc_6.3.0_O3.drop_15750M.length_250M.champsimtrace.xz"
    "ligra_BC.com-lj.ungraph.gcc_6.3.0_O3.drop_5000M.length_250M.champsimtrace.xz"
    "ligra_Components.com-lj.ungraph.gcc_6.3.0_O3.drop_23750M.length_250M.champsimtrace.xz"
    "ligra_BC.com-lj.ungraph.gcc_6.3.0_O3.drop_15750M.length_250M.champsimtrace.xz"
    "ligra_BFS-Bitvector.com-lj.ungraph.gcc_6.3.0_O3.drop_23000M.length_250M.champsimtrace.xz"
    "ligra_Components.com-lj.ungraph.gcc_6.3.0_O3.drop_750M.length_250M.champsimtrace.xz"
    "ligra_Radii.com-lj.ungraph.gcc_6.3.0_O3.drop_5000M.length_250M.champsimtrace.xz"
    "ligra_BC.com-lj.ungraph.gcc_6.3.0_O3.drop_26750M.length_250M.champsimtrace.xz"
    "ligra_PageRankDelta.com-lj.ungraph.gcc_6.3.0_O3.drop_6000M.length_250M.champsimtrace.xz"
    "ligra_Radii.com-lj.ungraph.gcc_6.3.0_O3.drop_36000M.length_250M.champsimtrace.xz"
    "ligra_BFSCC.com-lj.ungraph.gcc_6.3.0_O3.drop_750M.length_250M.champsimtrace.xz"
    "ligra_Components-Shortcut.com-lj.ungraph.gcc_6.3.0_O3.drop_21000M.length_250M.champsimtrace.xz"
    "ligra_Components.com-lj.ungraph.gcc_6.3.0_O3.drop_3500M.length_250M.champsimtrace.xz"
    "ligra_Radii.com-lj.ungraph.gcc_6.3.0_O3.drop_750M.length_250M.champsimtrace.xz"
    "ligra_PageRank.com-lj.ungraph.gcc_6.3.0_O3.drop_10750M.length_250M.champsimtrace.xz"
    "ligra_BellmanFord.com-lj.ungraph.gcc_6.3.0_O3.drop_33750M.length_250M.champsimtrace.xz"
    "ligra_Components-Shortcut.com-lj.ungraph.gcc_6.3.0_O3.drop_750M.length_250M.champsimtrace.xz"
    "ligra_PageRankDelta.com-lj.ungraph.gcc_6.3.0_O3.drop_1250M.length_250M.champsimtrace.xz"
    "ligra_BFS.com-lj.ungraph.gcc_6.3.0_O3.drop_17000M.length_250M.champsimtrace.xz"
    "ligra_BC.com-lj.ungraph.gcc_6.3.0_O3.drop_500M.length_250M.champsimtrace.xz"
    "ligra_BFS.com-lj.ungraph.gcc_6.3.0_O3.drop_3500M.length_250M.champsimtrace.xz"
    "ligra_BellmanFord.com-lj.ungraph.gcc_6.3.0_O3.drop_4000M.length_250M.champsimtrace.xz"
    "ligra_BFSCC.com-lj.ungraph.gcc_6.3.0_O3.drop_6750M.length_250M.champsimtrace.xz"
    "ligra_Components.com-lj.ungraph.gcc_6.3.0_O3.drop_22750M.length_250M.champsimtrace.xz"
    "ligra_BC.com-lj.ungraph.gcc_6.3.0_O3.drop_17000M.length_250M.champsimtrace.xz"
    "ligra_BFSCC.com-lj.ungraph.gcc_6.3.0_O3.drop_22000M.length_250M.champsimtrace.xz"
    "ligra_Radii.com-lj.ungraph.gcc_6.3.0_O3.drop_3500M.length_250M.champsimtrace.xz"
    "ligra_BFS-Bitvector.com-lj.ungraph.gcc_6.3.0_O3.drop_500M.length_250M.champsimtrace.xz"
    "ligra_BC.com-lj.ungraph.gcc_6.3.0_O3.drop_11500M.length_250M.champsimtrace.xz"
    "ligra_BC.com-lj.ungraph.gcc_6.3.0_O3.drop_15500M.length_250M.champsimtrace.xz"
    "ligra_Triangle.com-lj.ungraph.gcc_6.3.0_O3.drop_25250M.length_8M.champsimtrace.xz"
    "ligra_MIS.com-lj.ungraph.gcc_6.3.0_O3.drop_750M.length_250M.champsimtrace.xz"
    "ligra_PageRank.com-lj.ungraph.gcc_6.3.0_O3.drop_5000M.length_250M.champsimtrace.xz"
    "ligra_BellmanFord.com-lj.ungraph.gcc_6.3.0_O3.drop_1750M.length_250M.champsimtrace.xz"
    "ligra_Triangle.com-lj.ungraph.gcc_6.3.0_O3.drop_18000M.length_250M.champsimtrace.xz"
    "ligra_BellmanFord.com-lj.ungraph.gcc_6.3.0_O3.drop_7500M.length_250M.champsimtrace.xz"
    "ligra_BFS.com-lj.ungraph.gcc_6.3.0_O3.drop_11500M.length_250M.champsimtrace.xz"
    "ligra_Radii.com-lj.ungraph.gcc_6.3.0_O3.drop_12500M.length_250M.champsimtrace.xz"
    "ligra_BellmanFord.com-lj.ungraph.gcc_6.3.0_O3.drop_24750M.length_250M.champsimtrace.xz"
    "ligra_PageRankDelta.com-lj.ungraph.gcc_6.3.0_O3.drop_24000M.length_250M.champsimtrace.xz"
    "ligra_BellmanFord.com-lj.ungraph.gcc_6.3.0_O3.drop_14000M.length_250M.champsimtrace.xz"
    "ligra_BFS-Bitvector.com-lj.ungraph.gcc_6.3.0_O3.drop_9750M.length_250M.champsimtrace.xz"
    "ligra_Components-Shortcut.com-lj.ungraph.gcc_6.3.0_O3.drop_3500M.length_250M.champsimtrace.xz"
    "ligra_Triangle.com-lj.ungraph.gcc_6.3.0_O3.drop_25000M.length_250M.champsimtrace.xz"
    "ligra_Triangle.com-lj.ungraph.gcc_6.3.0_O3.drop_750M.length_250M.champsimtrace.xz"
    "ligra_PageRankDelta.com-lj.ungraph.gcc_6.3.0_O3.drop_56500M.length_208M.champsimtrace.xz"
    "ligra_PageRank.com-lj.ungraph.gcc_6.3.0_O3.drop_51000M.length_250M.champsimtrace.xz"
    "ligra_MIS.com-lj.ungraph.gcc_6.3.0_O3.drop_5000M.length_250M.champsimtrace.xz"
    "ligra_PageRankDelta.com-lj.ungraph.gcc_6.3.0_O3.drop_17000M.length_250M.champsimtrace.xz"
    "ligra_BC.com-lj.ungraph.gcc_6.3.0_O3.drop_3500M.length_250M.champsimtrace.xz"
    "ligra_Components-Shortcut.com-lj.ungraph.gcc_6.3.0_O3.drop_22000M.length_250M.champsimtrace.xz"
    "ligra_BFS.com-lj.ungraph.gcc_6.3.0_O3.drop_500M.length_250M.champsimtrace.xz"
    "ligra_CF.com-lj.ungraph.gcc_6.3.0_O3.drop_154750M.length_250M.champsimtrace.xz"
    "ligra_Components.com-lj.ungraph.gcc_6.3.0_O3.drop_15750M.length_250M.champsimtrace.xz"
    "ligra_Components.com-lj.ungraph.gcc_6.3.0_O3.drop_12500M.length_250M.champsimtrace.xz"
    "ligra_Triangle.com-lj.ungraph.gcc_6.3.0_O3.drop_3500M.length_250M.champsimtrace.xz"
    "ligra_BFS.com-lj.ungraph.gcc_6.3.0_O3.drop_21500M.length_250M.champsimtrace.xz"
    "ligra_BellmanFord.com-lj.ungraph.gcc_6.3.0_O3.drop_22250M.length_250M.champsimtrace.xz"
    "ligra_Components.com-lj.ungraph.gcc_6.3.0_O3.drop_18000M.length_250M.champsimtrace.xz"
    "ligra_CF.com-lj.ungraph.gcc_6.3.0_O3.drop_10750M.length_250M.champsimtrace.xz"
    "ligra_Radii.com-lj.ungraph.gcc_6.3.0_O3.drop_25000M.length_250M.champsimtrace.xz"
    "ligra_PageRank.com-lj.ungraph.gcc_6.3.0_O3.drop_500M.length_250M.champsimtrace.xz"
    "ligra_BFSCC.com-lj.ungraph.gcc_6.3.0_O3.drop_11250M.length_250M.champsimtrace.xz"
    "ligra_Triangle.com-lj.ungraph.gcc_6.3.0_O3.drop_6000M.length_250M.champsimtrace.xz"
    "ligra_BFS.com-lj.ungraph.gcc_6.3.0_O3.drop_5000M.length_250M.champsimtrace.xz"
    "ligra_BFS-Bitvector.com-lj.ungraph.gcc_6.3.0_O3.drop_18000M.length_250M.champsimtrace.xz"
    "ligra_Components.com-lj.ungraph.gcc_6.3.0_O3.drop_6250M.length_250M.champsimtrace.xz"
    "ligra_Components.com-lj.ungraph.gcc_6.3.0_O3.drop_5000M.length_250M.champsimtrace.xz"
    "ligra_MIS.com-lj.ungraph.gcc_6.3.0_O3.drop_17000M.length_250M.champsimtrace.xz"
    "ligra_MIS.com-lj.ungraph.gcc_6.3.0_O3.drop_3500M.length_250M.champsimtrace.xz"
    "ligra_MIS.com-lj.ungraph.gcc_6.3.0_O3.drop_21250M.length_250M.champsimtrace.xz"
    "ligra_Triangle.com-lj.ungraph.gcc_6.3.0_O3.drop_24000M.length_250M.champsimtrace.xz"
    "ligra_BellmanFord.com-lj.ungraph.gcc_6.3.0_O3.drop_5500M.length_250M.champsimtrace.xz"
    "ligra_PageRankDelta.com-lj.ungraph.gcc_6.3.0_O3.drop_52000M.length_250M.champsimtrace.xz"
    "ligra_BFS-Bitvector.com-lj.ungraph.gcc_6.3.0_O3.drop_2500M.length_250M.champsimtrace.xz"
    "ligra_PageRankDelta.com-lj.ungraph.gcc_6.3.0_O3.drop_3500M.length_250M.champsimtrace.xz"
    "ligra_PageRankDelta.com-lj.ungraph.gcc_6.3.0_O3.drop_11750M.length_250M.champsimtrace.xz"
    "ligra_PageRankDelta.com-lj.ungraph.gcc_6.3.0_O3.drop_35250M.length_250M.champsimtrace.xz"
    "ligra_Components-Shortcut.com-lj.ungraph.gcc_6.3.0_O3.drop_11500M.length_250M.champsimtrace.xz"
    "ligra_BFSCC.com-lj.ungraph.gcc_6.3.0_O3.drop_3500M.length_250M.champsimtrace.xz"
    "ligra_Radii.com-lj.ungraph.gcc_6.3.0_O3.drop_24500M.length_250M.champsimtrace.xz"
    "ligra_BFS.com-lj.ungraph.gcc_6.3.0_O3.drop_15750M.length_250M.champsimtrace.xz"
)
ligra_traces=()
gap_traces1=(
  "sssp-5.trace.gz"
  "bfs-14.trace.gz"
  "sssp-3.trace.gz"
  "cc-14.trace.gz"
  "pr-3.trace.gz"
  "sssp-14.trace.gz"
  "pr-10.trace.gz"
  "cc-5.trace.gz"
  "bc-5.trace.gz"
  "pr-5.trace.gz"
  "bc-3.trace.gz"
  "bfs-3.trace.gz"
  "cc-6.trace.gz"
  "cc-13.trace.gz"
  "sssp-10.trace.gz"
  "bfs-8.trace.gz"
  "bc-0.trace.gz"
  "bfs-10.trace.gz"
  "bc-12.trace.gz"
  "pr-14.trace.gz"
)

# 检查是否定义了编译参数，并且据此对champsim进行编译
function compile_champsim
{
  ##########参数说明
  # - $1: 预取器名称
  ##########
  cd ChampSim/TryBest
  # if [ "$1" = "vberti" ] || [ "$1" = "vbertim" ] ; then
  #   cd ChampSim/Berti
  # else
  #   cd ChampSim/Other_PF
  # fi
  
  # 执行构建命令
  ./build_champsim.sh hashed_perceptron no $1 no no no no no lru lru lru srrip drrip lru lru lru 1 no $DATE_SET

  # 返回上级目录
  cd ../..
}

function get_ChampSimPath
{
  ##########参数说明
  # - $1: 预取器名称
  ##########
  champSimPath="./ChampSim/TryBest/bin/${DATE_SET}hashed_perceptron-no-$1-no-no-no-no-no-lru-lru-lru-srrip-drrip-lru-lru-lru-1core-no"

  # if [ "$1" = "vberti" ] || [ "$1" = "vbertim" ] ; then
  #   champSimPath="./ChampSim/Berti/bin/${DATE_SET}hashed_perceptron-no-$1-no-no-no-no-no-lru-lru-lru-srrip-drrip-lru-lru-lru-1core-no"
  # else
  #   champSimPath="./ChampSim/Other_PF/bin/${DATE_SET}hashed_perceptron-no-$1-no-no-no-no-no-lru-lru-lru-srrip-drrip-lru-lru-lru-1core-no"
  # fi
  echo "$champSimPath"
}

# 判断结果路径是否存在，如果不存在则创建结果路径
function res_path_exist
{
  ##########参数说明
  # - $1: res_output_path
  ##########
  if [ ! -d "$1" ]; then
      mkdir -p "$1"
  fi
}



# 运行ChampSim命令，针对每个跟踪文件进行循环生成对应的模拟器bin文件运行trace的指令
function generate_par
{
  ##########参数说明
  # - $1: trace_file_path
  # - $2: res_output_path
  # - $3: prefetcher
  # - $4: champSimPath
  # - $5: trace
  ##########
  if [ "$traces_all" = "true" ]; then
    case "$5" in
      "spec2k17")
        traceFiles=("${spec2k17_traces1[@]}")
        ;;
      "cs")
        traceFiles=("${cs_traces1[@]}")
        ;;
      "ligra")
        traceFiles=("${ligra_traces1[@]}")
        ;;
      "parsec")
        traceFiles=("${parsec_traces1[@]}")
        ;;
      "gap")
        traceFiles=("${gap_traces1[@]}")
        ;;

    *)
        traceFiles=()
        while IFS= read -r line; do
          traceFiles+=("$line")
        done < "$trace_base_path$5_name.txt"
        ;;
    esac
      # traceFiles=()
      # while IFS= read -r line; do
      #   traceFiles+=("$line")
      # done < "$trace_base_path$5_name.txt"
  else
    case "$5" in
      "spec2k17")
        traceFiles=("${spec2k17_traces[@]}");;
      "cs")
        traceFiles=("${cs_traces[@]}");;
      "gap")
        traceFiles=("${gap_traces[@]}");;
      "ligra")
      traceFiles=("${ligra_traces[@]}");;
    esac 
  fi
  for traceFile in "${traceFiles[@]}"
  do
    # 设置输出文件名
    traceFilebase=$(basename "$traceFile")
    # outputFile="$2/hashed_perceptron-no-${3}-no-no-no-no-no-lru-lru-lru-srrip-drrip-lru-lru-lru-1core-no---$traceFilebase"
    outputFile="$2/-${3}-no---$traceFilebase"
    # outputFile="$2/$traceFilebase"    
    # 运行ChampSim命令
    if [[ $output_base == *"test"* ]]; then
      outputFile="$2/$res_test_name$traceFilebase"
    fi
    if [[ $5 == "cs" ]];then
      echo "$4 -warmup_instructions $warmupInstructions -simulation_instructions $simulationInstructions -cloudsuite -traces $1$traceFilebase > $outputFile 2>/dev/null" >> ${tmp_par_path}
    else
      echo "$4 -warmup_instructions $warmupInstructions -simulation_instructions $simulationInstructions -traces $1$traceFilebase > $outputFile 2>&1" >> ${tmp_par_path}
    fi
  done
}
# 2>&1

main()
{
  ########参数说明
  # - $1 是否compile
  truncate -s 0 $tmp_par_path
  for pref in "${prefs[@]}"
  do
    #判断是否需要编译
    if [ "$1" == "compile" ]; then
      compile_champsim $pref
    fi
    
    #根据trace获取tmp_par
    # 设置ChampSim的bin路径
    champSimPath=$(get_ChampSimPath $pref)
    # 清空tmp_par文件
    
    
    for trace in "${traces[@]}"
    do
        trace_file_path="$trace_base_path$trace/"
        res_output_path="$output_base$trace"
        res_path_exist $res_output_path  
        generate_par $trace_file_path $res_output_path $pref $champSimPath $trace
    done
  done

  if [ "$run_now" = "true" ]; then
  cat $tmp_par_path | xargs -I CMD -P $NUM_THREAD bash -c CMD
  fi
}

main $1
