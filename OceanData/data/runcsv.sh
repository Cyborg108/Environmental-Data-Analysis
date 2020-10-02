#!/bin/csh -f

set tag=3.2.All

date
./process_csv AER_OD  MODAL2_M_AER_OD > AER_OD.$tag.log

date
./process_csv  CO MOP_CO_M > CO.$tag.log

date
./process_csv Chlora MY1DMM_CHLORA > Chlora.$tag.log

date
./process_csv NO2 AURA_NO2_M > NO2.$tag.log 

date
./process_csv  SeaSurfaceTemp MYD28M > SeaSurfaceTemp.$tag.log

date
./process_csv Rainfall TRMM_3B43M > Rainfall.$tag.log
