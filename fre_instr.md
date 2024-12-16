## 重命名
find ./outputsum/output1212/spec2k17/ -type f -name "-baseline-no---*" | while read file; do
    new_name=$(echo "$file" | sed 's/-baseline-no/-no-no/')
    mv "$file" "$new_name"
done

find ./outputsum/output1213/spec2k17/ -type f -name "-hyperion_hpc_2table_UTBh1_buffer8_8-no---*" | while read file; do
    new_name=$(echo "$file" | sed 's/-hyperion_hpc_2table_UTBh1_buffer8_8-no/-hyperion-no/')
    mv "$file" "$new_name"
done

find ./outputsum/output1214/spec2k17/ -type f -name "-no-no---*" | while read file; do
    new_name=$(echo "$file" | sed 's/-no-no/-hermes-no/')
    mv "$file" "$new_name"
done

find ./outputsum/output1215/spec2k17/ -type f -name "-hyperion_hpc_2table_UTBh1_buffer8_8-no---*" | while read file; do
    new_name=$(echo "$file" | sed 's/-hyperion_hpc_2table_UTBh1_buffer8_8-no/-hyperion_hermes-no/')
    mv "$file" "$new_name"
done

find ./outputsum/output1214/parsec/ -type f -name "-no-no---*" | while read file; do
    new_name=$(echo "$file" | sed 's/-no-no/-hermes-no/')
    mv "$file" "$new_name"
done