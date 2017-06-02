# gnuplot file, template generated automatically with
# /Users/gsalam/scripts/gptemplate.py all-plots.gp

# you can also try replacing "pdfcairo" with "pdf"
set term pdfcairo enhanced font 'Helvetica,12' lw 1.5 size 10cm,8cm

set lt 1 pt 4 ps 0.6 lw 1.5 lc rgb '#f00000'
set lt 2 pt 5 ps 0.6 lw 1.5 lc rgb '#00c000'
set lt 3 pt 6 ps 0.6 lw 1.5 lc rgb '#0000e0'
set lt 4 pt 7 ps 0.6 lw 1.5 lc rgb '#ff8000'
set lt 5 pt 8 ps 0.6 lw 1.5 lc rgb '#5070ff'
set key spacing 1.3

set output 'all-plots.pdf'

datafile='main01.out'

xaxis='m_{jet}'
set title xaxis
set xlabel xaxis
set ylabel 'n_{events} in '.xaxis.' bin'
# index 0 selects the first histogram in the file (etc.)
plot datafile index 0 u 2:4 w histeps t ''

xaxis='m_{mMDT-jet}'
set title xaxis
set xlabel xaxis
set ylabel 'n_{events} in '.xaxis.' bin'
plot datafile index 1 u 2:4 w histeps t ''

# put your code here

set output

