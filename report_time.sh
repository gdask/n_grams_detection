#!/bin/sh
#All datasets' folders should be placed at the parent directory
make
#Runs n_gram for small datasets
./n_gram -i ../small_static/small_static.init -q ../small_static/small_static.work > res_small_static
diff -q ../small_static/small_static.result res_small_static
echo ''
./n_gram -i ../small_dynamic/small_dynamic.init -q ../small_dynamic/small_dynamic.work > res_small_dynamic
diff -q ../small_dynamic/small_dynamic.result res_small_dynamic
echo ''
#Runs n_gram for medium datasets
./n_gram -i ../medium_static/medium_static.init -q ../medium_static/medium_static.work > res_medium_static
diff -q ../medium_static/medium_static.result res_medium_static
echo ''
./n_gram -i ../medium_dynamic/medium_dynamic.init -q ../medium_dynamic/medium_dynamic.work > res_medium_dynamic
diff -q ../medium_dynamic/medium_dynamic.result res_medium_dynamic
echo ''
#Runs n_gram for large datasets
./n_gram -i ../large_static/large_static.init -q ../large_static/large_static.work > res_large_static
diff -q ../large_static/large_static.result res_large_static
echo ''
./n_gram -i ../large_dynamic/large_dynamic.init -q ../large_dynamic/large_dynamic.work > res_large_dynamic
diff -q ../large_dynamic/large_dynamic.result res_large_dynamic
echo ''

rm res_*
make clean