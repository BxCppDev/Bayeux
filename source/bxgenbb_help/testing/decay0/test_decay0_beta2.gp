

set grid
set title "Test beta2"

! make test_beta2
! ./test_beta2 > test_beta2.data
! cat test_beta2.data | grep "beta2-" | cut -d' ' -f2 | gsl-histogram 0 2 50 > test_beta2m.his

plot \
     'test_beta2m.his' using 1:3  title "Beta2-" with histeps
pause -1 "Hit [Enter]..."

rm -f test_beta2.data
rm -f test_beta2m.his

# end
