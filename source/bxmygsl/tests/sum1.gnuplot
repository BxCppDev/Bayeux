# -*- mode: gnuplot; -*-

set grid

set title "Compton spectrum"

set xlabel "energy (keV)"
set ylabel "dN/dx (AU)"

e1=975.
e2=475.
plot [0:1500][0:5] \
      'a_975.data' index 1 w li , \
      'a_475.data' index 1 #using ($1*e1/e2):($2*e2/e1)
pause -1


set title "Integral of convoluted Compton spectrum"
set grid
set xlabel "resolution r"
set ylabel "Integral"

yref=438.013
yref2=605.085
ymin=-0.05
ymax=+0.05
plot [-0.05:0.25][ymin:ymax] \
  'sccs_975.data' using 1:(($2-yref)/yref) title "975 keV" with points ps 3, \
  'sccs_475.data' using 1:(($2-yref2)/yref2) title "475 keV" with points ps 3
pause -1 "Hit return..."

a975=0.1
f975(x)=a975*x**2

set fit errorvariables
FIT_LIMIT = 1e-6

fit f975(x) 'sccs_975.data' using 1:(($2-yref)/yref) via a975

plot [-0.05:0.25][ymin:ymax] \
  'sccs_975.data' using 1:(($2-yref)/yref) title "975 keV" with points ps 3, \
	f975(x) with lines lt 3	

print 'a975 = ' , a975, ' +/- ', a975_err

pause -1 "Hit return..."

exit 1

#