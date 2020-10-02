wget -v -d -i urllist.txt
wget -v -d --user=fire --password=burnt ftp://fuoco.geog.umd.edu/modis/C5/mcd14ml/MCD14ML.200012.005.01.asc.g
z

wget -v -d http://acdb-ext.gsfc.nasa.gov/Data_services/cloud_slice/data_monthly/tco_oct04_to_dec13.csv

#count number of columns:
head -n 1 AURA_OZONE_M_2004-10.CSV | awk -F ',' '{print NF-1}'
