#!/bin/csh -f

set tag=3.2_All

date
./process_data Aqua_Chl > Aqua_Chl.$tag.log
./process_log  Aqua_Chl.$tag.log > Aqua_Chl.$tag.extract.txt

date
./process_data Aqua_nflh > Aqua_nflh.$tag.log
./process_log   Aqua_nflh.$tag.log > Aqua_nflh.$tag.extract.txt

date
./process_data Terra_Chl > Terra_Chl.$tag.log
./process_log  Terra_Chl.$tag.log > Terra_Chl.$tag.extract.txt

date
./process_data Terra_nflh > Terra_nflh.$tag.log
./process_log  Terra_nflh.$tag.log > Terra_nflh.$tag.extract.txt
date
