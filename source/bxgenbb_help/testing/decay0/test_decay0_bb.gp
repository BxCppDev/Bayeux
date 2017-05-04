

set grid
set title "Test bb"

! make test_bb
! ./test_bb > test_bb.data
! cat test_bb.data | cut -d' ' -f1 | gsl-histogram 0 4 100 > test_bb_e1.his
! cat test_bb.data | cut -d' ' -f2 | gsl-histogram 0 4 100 > test_bb_e2.his
! cat test_bb.data | cut -d' ' -f3 | gsl-histogram 0 4 100 > test_bb_esum.his

plot [:][0:2500]\
     'test_bb_e1.his'   using 1:3  title "Beta 1" with histeps, \
     'test_bb_e2.his'   using 1:3  title "Beta 2" with histeps, \
     'test_bb_esum.his' using 1:3  title "Sum"    with histeps
pause -1 "Hit [Enter]..."

rm -f test_bb_e1.his
rm -f test_bb_e2.his
rm -f test_bb_esum.his
rm -f test_bb.data

# end
