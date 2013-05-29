

set grid
set title "Test beta1"

! make test_beta1
! ./test_beta1 > test_beta1.data
! cat test_beta1.data | grep "beta1-" | cut -d' ' -f2 | gsl-histogram 0 0.2 50 > test_beta1m.his

plot \
     'test_beta1m.his' using 1:3  title "Beta1-" with histeps
pause -1 "Hit [Enter]..."

rm -f test_beta1.data
rm -f test_beta1m.his

# end
