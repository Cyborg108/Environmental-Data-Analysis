#!/bin/csh -f

#./checklatlong.csh Aqua_Chl > Aqua_Chl.latlon.info

foreach x ( `ls $1` )
    echo $x
    if ( "$x" =~ *.nc ) then
        rm -f t.txt
        ../netcdf-c-4.6.2/ncdump/ncdump -h $1/$x > t.txt
        fgrep "lat = " t.txt |grep -v alt_thresh_lat
        fgrep "lon = " t.txt |grep -v full_latlon
        fgrep "lat:valid_min" t.txt
        fgrep "lat:valid_max" t.txt
        fgrep "lon:valid_min" t.txt
        fgrep "lon:valid_max" t.txt
        fgrep ":data_bins " t.txt

    endif

end
