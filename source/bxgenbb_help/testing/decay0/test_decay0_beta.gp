

set grid
set title "Test beta"

! make test_beta
! ./test_beta > test_beta.data
! cat test_beta.data | grep "beta-" | cut -d' ' -f2 | gsl-histogram 0 2 50 > test_betam.his
! cat test_beta.data | grep "beta+" | cut -d' ' -f2 | gsl-histogram 0 2 50 > test_betap.his

plot \
     'test_betam.his' using 1:3  title "Beta-" with histeps, \
     'test_betap.his' using 1:3  title "Beta+" with histeps
pause -1 "Hit [Enter]..."

rm -f test_beta.data
rm -f test_betam.his
rm -f test_betap.his

# end
