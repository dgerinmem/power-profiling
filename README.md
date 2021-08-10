# upmem dimm power profiling


#### dev talk 09/09/2021

* power profiling objectives
* power measurement method
* demo (hw target)
  * single dpu
  * all dpu
  
* power profiling sdk integration

* solved sytem issues 
  * mcu double message handling

* unsolved system issues
  * BIAS : start stop delay
  * BUG : inconsistent signal profile (see on cust 25)

* bugs on hw (labs cust25):

* todo
   * double message handling : add struct mcu * into stuct rank * to maintain mcu state
   * confidence interval **n** parameter computation
   * host error resilience : EC side resilience
   * dpu fault  resilience : EC side resilience :not developped yet



