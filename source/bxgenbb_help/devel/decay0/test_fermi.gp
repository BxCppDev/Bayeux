# -*- mode : gnuplot; -*-

set title "Decay0/genbb_help : Fermi function test plots for Se82 (Z=34, E <= 3MeV)"
set grid
set xlabel "E (MeV)"
set ylabel "F"
plot [0:+3][0:6] \
     'test_fermi.data' using 1:2 title "Decay0 Fermi function" with lines lw 8, \
     'test_fermi_val.data' using 1:2 title "genbb_help Fermi function" with lines lw 4

 
# plot [0:+3][0:10] \
#      'test_fermi.data' using 1:2 title "Decay0 Fermi function" with lines, \
#      'test_fermi_val.data' using 1:2 title "genbb_help Fermi function 1" with lines, \
#      'test_fermi_val.data' using 1:3 title "genbb_help Fermi function 2" with lines, \
#      'test_fermi_val.data' using 1:4 title "genbb_help Fermi function *" with lines, \
#      'test_fermi_val.data' using 1:5 title "genbb_help Fermi function 4" with lines
pause -1 'Hit [Enter]'

# end
