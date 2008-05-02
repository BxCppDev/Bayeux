
set origin 0,0
set size 1,1

set autoscale
set grid
visu_shift=0.005

set title "Test program for FFT GSL wrapper"
set xlabel "time (ns)"
set ylabel "ADC (mV)"
plot  \
     'test_fft_real.data' u 1:2 index 0 title "Original signal" with lines, \
     'test_fft_real.data' u 1:(($2)+visu_shift) index 1 title "FFT-restored signal" with lines

pause -1 "Hit return..."

set origin 0.0,0.0
set size 1,1
set xlabel "frequency (GHz)"
set ylabel "Amplitude"
freq_min=-0.005
freq_max=0.25
set xrange [freq_min:freq_max]
set yrange [-50:+120]
#set yrange [*:*]
plot \
     'test_fft_real.data' index 2 title "FFT-frequency spectrum" with impulses lw 2
pause -1 "Hit return..."

set multiplot

set origin 0.0,0.0
set size 1,1
set xlabel "frequency (GHz)"
set ylabel "Amplitude"
freq_min=-0.005
freq_max=0.25
set xrange [freq_min:freq_max]
set yrange [-50:+120]
#set yrange [*:*]
plot \
     'test_fft_real.data' index 2 title "FFT-frequency spectrum" with impulses lw 2

#pause -1 "Hit return..."
set title ""
set origin 0.25,0.35
set size 0.7,0.5
clear
set xlabel ""
set ylabel ""
freq_max=0.035
set xrange [freq_min:freq_max]
set yrange [*:*]
plot \
     'test_fft_real.data' index 2 title "" with impulses lw 2
set nomultiplot


# end
