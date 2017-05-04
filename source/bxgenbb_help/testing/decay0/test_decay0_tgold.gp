

set grid
set title "Test tgold"

! make test_tgold
! __build-Linux-x86_64/testing/test_decay0_tgold > test_decay0_tgold.data

plot \
     'test_decay0_tgold.data' with lines

pause -1 "Hit [Enter]..."

rm -f test_tgold.data

# end
