! BuildProducts/bin/bxtests/mctools-test_random_event_sequence_model > t.data

############################################
! cut -d' ' -f1 t.data > tall.data
! cat tall.data | gsl-histogram 0 5 100 > tall.his
set grid
uA=450.0
A=1.0
tau=1.0
uTau=1.0
plot [0:][0:] \
     'tall.his' u (column(1)):(column(3))  title "Data" w histeps, \
     ''  u (column(1)):(column(3)):(sqrt(column(3))) with errorbars
pause -1 "Hit [Enter]..."

f(x)=A*uA*exp(-x/(tau*uTau))
plot [0:4] \
     f(x) title "Guess", \
     'tall.his' u (column(1)):(column(3)) title "Data" w histeps
pause -1 "Hit [Enter]..."

FIT_LIMIT=1e-6
fit [0:1.5] f(x) 'tall.his' u (column(1)):(column(3)):(sqrt(column(3))) via A,tau
plot [0:2] \
     f(x) title "Fit", \
     'tall.his' u (column(1)):(column(3)) title "Data" w histeps, \
     '' u (column(1)):(column(3)):(sqrt(column(3))) notitle with errorbars
pause -1 "Hit [Enter]..."

# end
