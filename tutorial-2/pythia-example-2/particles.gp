set xrange [-20:20]
set yrange [-20:20]
set zrange [-20:20]
set xlabel 'p_x [GeV]'
set ylabel 'p_y [GeV]'
set zlabel 'p_z [GeV]'
splot 'main01.particles' u 1:2:3 w l t 'particles'

