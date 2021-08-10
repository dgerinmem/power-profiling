# build
dpu-upmem-dpurte-clang trivial_checksum_example.c -o trivial_checksum_example
dpu-upmem-dpurte-clang -pg trivial_checksum_example.c -o trivial_checksum_example_prof
gcc -O3 --std=c99 -o trivial_checksum_example_host trivial_checksum_example_host.c -g `dpu-pkg-config --cflags --libs dpu`
gcc -O3 --std=c99 -o power_prof_ticker power_profile_ticker.c -g `dpu-pkg-config --cflags --libs dpu`
gcc -O3 --std=c99 -o power_prof_testing power_prof_testing.c -g `dpu-pkg-config --cflags --libs dpu`
