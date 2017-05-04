! make test_plog69


set grid
set title "Test plog69"

! ./test_plog69 > test_plog69.data

fpe(x)=log(x)
#set yrange[-5:+5]
plot \
     'test_plog69.data' index 0 with points pt 6 ps 1. , \
     'test_plog69.data' index 1 with lines 

pause -1 "Hit [Enter]..."

rm -f test_plog69.data

# end
