# dpu programm used as power measurement references

# build and load dpu mdd

dgerin@dgerinpc:~/dpu_tools/mdd/modules

sudo rmmod dpu.ko && sudo insmod dpu.ko


# on cloud machine, need to insmod in /tmp to avoid nfs issue
# on dgerin@upmemcloud5
mv dpu.ko /tmp && cd /tmp && sudo rmmod dpu.ko && sudo insmod dpu.ko


# ADC VDD range value
ioctl return 1151 -> 1.15 V
unit is mV

# ADC MA range value
ioctl return MA : 13458
unit is mA


# measurement principle

-> inner Averaging of mA, mV is computed over N_SHORT_AVG_MEAN measurement,
   points measured on ADC at the STM maximal speed.

-> outer averaging of mA, mV is made with N_LONG_AVG_MEAN measurement,
   points measured taken from inner Averaging.


N_LONG_AVG_MEAN needs to be computed depending on the 
runtime of the application to measure.

N_SHORT_AVG_MEAN needs to be computed from charactesisation results.

Charactesisation councerns the current / voltage profile.

N_SHORT_AVG_MEAN needs to be long enough to give a good estimation of the 
average current / voltage. Also, it must be choosen as short as possible
to avoid STM CPU overloading.

# test profiling API (with statistics mode)
dpu-profiling dpu-statistics -- ./power_prof_testing

# test profiling API (with power mode)
dpu-profiling dpu-power -- ./power_prof_testing
dpu-profiling --save-temps dpu-statistics -- ./power_prof_testing
