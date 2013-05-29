! make test_fermi


set grid
set title "Test fermi"

! ./test_fermi > test_fermi.data

plot \
     'test_fermi.data' u 1:2 title "Fermi" with lines , \
     'test_fermi.data' u 1:3 title "Fermi NR" with lines , \
     'test_fermi.data' u 1:4 title "Fermi w/o L0" with lines 

pause -1 "Hit [Enter]..."

rm -f test_fermi.data

# end
