# build
dpu-upmem-dpurte-clang trivial_checksum_example.c -o trivial_checksum_example
dpu-upmem-dpurte-clang trivial_checksum_example_long.c -o trivial_checksum_example_long
gcc -O3 --std=c99 -o trivial_checksum_example_host trivial_checksum_example_host.c -g `dpu-pkg-config --cflags --libs dpu`
gcc -O3 --std=c99 -o trivial_checksum_example_host_multirank trivial_checksum_example_host_multirank.c -g `dpu-pkg-config --cflags --libs dpu`

gcc -O3 --std=c99 -o trivial_checksum_example_host_long trivial_checksum_example_host_long.c -g `dpu-pkg-config --cflags --libs dpu`
gcc -O3 --std=c99 -o trivial_checksum_example_host_long_multirank trivial_checksum_example_host_long_multirank.c -g `dpu-pkg-config --cflags --libs dpu`

# short
# dpu-profiling --save-temps dpu-power ./trivial_checksum_example_host
# dpu-profiling --save-temps dpu-power ./trivial_checksum_example_host_multirank

# long
# dpu-profiling --save-temps dpu-power ./trivial_checksum_example_host_long
# dpu-profiling --save-temps dpu-power ./trivial_checksum_example_host_long_multirank
