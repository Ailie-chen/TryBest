import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os
import matplotlib
import math
from matplotlib.font_manager import FontProperties
figure_res = 'analysis_py/evaluation2/mtps/'
date='0110'
traces=["spec2k17","gap","ligra","cs"]
dic_suite={
    'spec2k17':'SPEC',
    'gap':"GAP",
    'ligra':"Ligra",
    'cs':"CloudSuite"
}
def get_col_data(data,metric):
    num_prefs = data.shape[0]-1
    prefs_name = data.iloc[1:,1].tolist()
    metric_data = data.loc[:,data.loc[0, :].str.contains(metric)]
    first_column_array = np.array(metric_data.iloc[:, 0])
    metric_data = metric_data.iloc[np.arange(1,num_prefs+1), :]
    # first_row_array = np.array(metric_data.iloc[0, :])
    first_column_array = np.array(metric_data.iloc[:, 0])
    first_column_array = np.where(first_column_array == '-', '0', first_column_array)
    # Convert the array back to its original numeric type if necessary  
    print(first_column_array)
    return first_column_array

data_file_path = 'evaluationmemtense/'+ traces[0] + '/'+'all_results/'
data_pref = pd.read_csv(data_file_path + date + '.csv', header=None)
prefs = get_col_data(data_pref,'Prefetcher')
num_prefs = len(prefs)

Xlabel = ['6400', '3200','1600']
fig, axes = plt.subplots(1,4, figsize=(7.0, 1.3))
plt.subplots_adjust(left=0, right=1.0, bottom=0, top=1.0, wspace=0.3, hspace=0.37)
ax_idx = 0
for trace in traces:
    ax2 = axes[ax_idx]
    data_all = np.zeros((num_prefs, 1))
    data_file_path='evaluationmemtense/'+ trace + '/'+'all_results/'
    data = pd.read_csv(data_file_path + date + '.csv', header=None)
    i = 0    
    data_all[0:num_prefs,i] = get_col_data(data,'IPCI')


    A1data = data_all[1:4,0]
    A2data = data_all[4:7,0]
    A3data = data_all[7:10,0]
    A4data = data_all[10:13,0]


    left_bar_pos = np.arange(len(Xlabel))
    left_bar_pos1 = np.arange(1,len(Xlabel),1)
    ax2.plot(left_bar_pos, A1data, linestyle='-', marker='o', markerfacecolor='none',color='grey', label='MLOP',linewidth=1.5,markersize=4,zorder=2) 
    ax2.plot(left_bar_pos, A2data, linestyle='-', marker='s', markerfacecolor='none',color='grey', label='IPCP',linewidth=1.5,markersize=4,zorder=2)
    ax2.plot(left_bar_pos, A3data, linestyle='-', marker='o', markerfacecolor='black',color='black', label='Berti',linewidth=1.5,markersize=4,zorder=2) 
    ax2.plot(left_bar_pos, A4data, linestyle='-', marker='s', markerfacecolor='black',color='black', label='Hyperion',linewidth=1.5,markersize=4,zorder=2)

    if(ax_idx == 0):
        min_v = 1.0
        max_v = 1.80

        ax2_yticks = np.around(np.arange(min_v, max_v+0.01, 0.2), decimals=2)
        ax2_yticks1 = np.around(np.arange(min_v, max_v+0.01, 0.1), decimals=3)
    elif(ax_idx == 1):
        min_v = 1.0
        max_v = 1.8

        ax2_yticks = np.around(np.arange(min_v, max_v+0.01, 0.2), decimals=2)
        ax2_yticks1 = np.around(np.arange(min_v, max_v+0.01, 0.1), decimals=3)
    elif(ax_idx == 2):
        min_v = 1.0
        max_v = 1.8

        ax2_yticks = np.around(np.arange(min_v, max_v+0.01, 0.2), decimals=2)
        ax2_yticks1 = np.around(np.arange(min_v, max_v+0.01, 0.1), decimals=3)
    elif(ax_idx == 3):
        min_v = 1.0
        max_v = 1.8

        ax2_yticks = np.around(np.arange(min_v, max_v+0.01, 0.2), decimals=2)
        ax2_yticks1 = np.around(np.arange(min_v, max_v+0.01, 0.1), decimals=3)

    ax2.set_xticks(left_bar_pos)  
    ax2.set_yticks(ax2_yticks)
    ax2.set_xticklabels(Xlabel,fontsize=9,ha='center')
    ax2.set_yticklabels(ax2_yticks,fontsize=9,ha='right')

    # plt.xticks(left_bar_pos, Xlabel,  fontsize=9,ha='center')#rotation=45,
    # plt.yticks([0,0.25,0.5,0.75,1.00], ['0%','25%','50%','75%','100%'], fontsize=9,ha='right')
    # for label in ax.get_yticklabels():
    #     label.set_fontsize(16)
    ax2.set_title(dic_suite[trace], y=-0.45, fontsize=9) 
    ax2.text(0.75,0.8, 'MTPS', fontsize=9)
    ax2.set_ylim(min_v,max_v)
    ax2_idx = 0
    for item in ax2_yticks1:
        if(ax2_idx % 2 == 0):
            ax2.axhline(float(item), color='lightgray', linestyle='-', linewidth=1.0,zorder=1)
        else:
            ax2.axhline(float(item), color='lightgray', linestyle='--', linewidth=1.0,zorder=1)
        ax2_idx += 1
    ax_idx += 1

    if(ax_idx == 2):        
        ax2.legend(loc='upper center', bbox_to_anchor=(1.15, 1.30), ncol=4,fontsize=9,
            handlelength=1.2,     # 控制图例句柄的长度
                handletextpad=0.2,  # 控制图例句柄和文本之间的间距
                columnspacing=0.5,
                edgecolor='black'   )



plt.savefig(os.path.join(figure_res,'mtps.png'),dpi=800, format="png", bbox_inches='tight')
plt.savefig(os.path.join(figure_res,'mtps.pdf'),dpi=800, format="pdf", bbox_inches='tight')

