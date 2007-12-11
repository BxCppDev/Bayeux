#!/bin/bash

make demo_fft_real

./demo_fft_real > demo_fft_real.data


(echo "set term postscript solid color"; cat demo_fft_real.gnuplot) \
 | gnuplot > demo_fft_real.ps

gv demo_fft_real.ps &

# end