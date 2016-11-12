f2r1(x) = 1.0
f2r2(x) = 1.03961212/(1.0-0.00600069867/x**2)
f2r4(x) = 0.231792344/(1.0-0.0200179144/x**2)
f2r6(x) = 1.01046945/(1.0-103.560653/x**2)
f2r(x) = f2r1(x) + f2r2(x) + f2r4(x) + f2r6(x)
fr(x) = sqrt(f2r(x))

um = 1.0
l1 = 0.3 * um
l2 = 2.5 * um
set title "http://refractiveindex.info - N-BK7"
set grid mxtics mytics back
set xlabel "Wavelength [um]"
set ylabel "Refractive index"

l = 0.5 * um
print "lambda  = ", l / um, " [um] -> n=", fr(l)
print "term[1] = ", f2r1(l / um)
print "term[2] = ", f2r2(l / um)
print "term[4] = ", f2r4(l / um)
print "term[6] = ", f2r6(l / um)
print "n2      = ", f2r(l / um)
print "n       = ", fr(l / um)

plot [l1:l2] fr(x) notitle with lines lt rgbcolor "magenta" lw 2
pause -1 "Hit [Enter]..."

# end
