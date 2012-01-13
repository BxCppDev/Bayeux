# -*- mode :gnuplot; -*-

set title "Decay0 reference plots -- Se82 2b0n"
set grid
set xlabel "E (MeV)"
set ylabel "dN/dE (/MeV)"
plot [0:+4][0:3000] \
     'Se82_2b0n_e1.his'   using 1:3 title "E1" with histeps lw 3,  \
     'Se82_2b0n_e2.his'   using 1:3 title "E2" with histeps lw 3 ,  \
     'Se82_2b0n_esum.his' using 1:3 title "Esum" with histeps  lw 3
pause -1 'Hit [Enter]'

set xlabel "cost"
set ylabel "dN/dcost"
plot [-1.25:+1.25][0:*] 'Se82_2b0n_cost.his' using 1:3 with histeps lw 3
pause -1 'Hit [Enter]'

set title "Decay0 reference plots -- Se82 2b2n"
set grid
set xlabel "E (MeV)"
set ylabel "dN/dE (/MeV)"
plot [0:+4] \
     'Se82_2b2n_e1.his'   using 1:3 title "E1" with histeps lw 3 ,  \
     'Se82_2b2n_e2.his'   using 1:3 title "E2" with histeps lw 3 ,  \
     'Se82_2b2n_esum.his' using 1:3 title "Esum" with histeps lw 3 
pause -1 'Hit [Enter]'

set xlabel "cost"
set ylabel "dN/dcost"
plot [-1.25:+1.25][0:*] 'Se82_2b2n_cost.his' using 1:3 with histeps lw 3
pause -1 'Hit [Enter]'

# end
