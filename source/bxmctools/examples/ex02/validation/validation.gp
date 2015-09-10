# Validation of the tracking of arbitrarily charged ions in a constant electric field
# in the Bayeux/mctools Geant4 module.
# Author: F.Mauger
# Date: 2015-09-09

# Units and constants:
C=1.0     # Coulomb
e=1.602176462e-19 * C # Elementary electric charge
V=1.0     # Volt
m=1.0     # meter
s=1.0     # second
ns=1e-9*s # nanosecond
mm=1e-3*m # millimeter
eV=e*V      # eV
keV=1e3*eV  # keV
MeV=1e3*keV # MeV
c=2.99792458e8 * m / s
c2=c*c

# Ion parameters:
M = 11176.90633818 * MeV / c2 # mass
Q = +2 * e # effective charge

# Ion initial conditions:
Ec0 = 1.0 * keV
W0 = Ec0 + M * c2

p0 = sqrt(Ec0*(Ec0+2*M*c2))/c
print "p0    = ", p0 / (MeV/c), " MeV"

p0b = sqrt(2*M*c2*Ec0)*(1.0+0.5*(Ec0/(2*M*c2)))/c
print "p0b   = ", p0b / (MeV/c), " MeV (NR)"
p0c = sqrt(2*M*c2*Ec0)/c
print "p0c   = ", p0c / (MeV/c), " MeV (NR)"

beta0 = p0 * c / W0
print "beta0 = ", beta0

vx0 = beta0 * c
print "vx0   = ", vx0 / c, " c"

vx0b = sqrt(2 * Ec0 / M) # Non relativistic
print "vx0   = ", vx0b /c , " c (NR)"
vy0 = 0.0 * m/s

px0 = M*vx0b
py0 = 0.0
p0d  = sqrt(px0**2+py0**2)
print "p0d   = ", p0d / (MeV/c), " MeV (NR)"

x0  = -350.0*mm
y0  = 0.0*mm

# Electric field:
Ey0 = 2500.0 * V/m

# Equation of motion (x==time):
fx(x) = x0 + vx0 * x
# y == Q
fy(x,y) = y0 + vy0 * x + (y*Ey0)/M * x**2

# y == Q
_ftrajy(x,y) = y0 + vy0 * (x - x0)/vx0 + 0.5 * (y*Ey0)/M * ((x - x0)/vx0)**2

# y == Q
ftrajy(x,y) = (x < x0) || (x > 500*mm) ? NaN : _ftrajy(x,y) > +500*mm ? NaN : _ftrajy(x,y)

# y0 + vy0 * (x - x0)/vx0 + (Q*Ey0)/M * ((x - x0)/vx0)**2

xG4 = 283.343297124014 * mm
yG4 = 500.0 * mm
szImpact=10*mm

set title "Geant4 tracking of C12(N+) 1 keV ions \nin a 2500 V/m transverse electric field\nComparison with theoretical prediction"
set xlabel "x (m)"
set ylabel "y (m)"
set xtics 100*mm
set ytics 100*mm
set size ratio -1
set key outside box
set xrange [-600*mm:+600*mm]
set yrange [-600*mm:+600*mm]

# World:
set object 1 rectangle center 0,0 size 1000*mm,1000*mm
set object 1 rectangle fc rgb "white"

# Lab:
set object 2 rectangle center 0,0 size 900*mm,900*mm
set object 2 rectangle fc rgb "cyan" fillstyle solid 0.2

# Detector:
set object 3 rectangle center +300*mm,0*mm size 200*mm,+100*mm
set object 3 rectangle fc rgb "blue" fillstyle solid 0.2

# Beam pipe:
set object 4 rectangle center -400*mm,0*mm size 100*mm,+50*mm
set object 4 rectangle fc rgb "orange" fillstyle solid 0.2

# Geant4 results:
# C12(1+) (outlab: x=450 y=400.213842382928) (outworld: x=500 y=450.236578882539)
# C12(2+) (outlab: x=249.921370408207 y=450) (outworld: x=283.258150429337 y=500)
# C12(3+) (outlab: x=140.063510327711 y=450) (outworld: x=167.291671046043 y=500)
# C12(4+) (outlab: x=74.3687389125986 y=450) (outworld: x=97.9470085285827 y=500)
# C12(5+) (outlab: x=29.6383493298782 y=450) (outworld: x=50.7279153904333 y=500)
# C12(6+) (outlab: x=-3.62801052396418 y=450) (outworld: x=15.600893981946 y=500)
#

# G4 final impact:
set object 5 rectangle from xG4-0.5*szImpact,yG4-0.5*szImpact  to xG4+0.5*szImpact,yG4+0.5*szImpact
set object 5 rectangle fc rgb "red"

# Beam emission direction:
set arrow 1 from x0,y0 to x0+150*mm,y0 head filled front lt 4 lw 2

# Electric field:
set arrow 2 from 0,-300*mm to 0,-100*mm head filled front lt 3 lw 2

set label 1 "Gun"  at -400*mm,+50*mm center front
set label 2 "C12(N+)"  at x0+75*mm,y0 center front textcolor rgb "magenta" offset 0,-1
set label 3 "Detector" at +300*mm,+75*mm center front
set label 4 "Electric field" at +0*mm,-200*mm left front offset 1
set grid front
set samples 1000

plot \
     ftrajy(x,+1*e) title "C12(1+) 1keV ion trajectory", \
     ftrajy(x,+2*e) title "C12(2+) 1keV ion trajectory", \
     ftrajy(x,+3*e) title "C12(3+) 1keV ion trajectory", \
     ftrajy(x,+4*e) title "C12(4+) 1keV ion trajectory", \
     ftrajy(x,+5*e) title "C12(5+) 1keV ion trajectory", \
     ftrajy(x,+6*e) title "C12(6+) 1keV ion trajectory", \
     "g4_impacts.data" using (($2)*mm):(($3)*mm) title "G4 results (out lab)"   with points pt 6 ps 0.35 , \
     "g4_impacts.data" using (($4)*mm):(($5)*mm) title "G4 results (out world)" with points pt 6 ps 0.35
pause -1 "Hit [Enter]..."

set terminal pdfcairo enhanced color solid font ",10"
set output "mctools_g4_ex02_C12_ion_traj.pdf"
replot
set output
set terminal wxt

# set parametric
# plot [0.0:4700*ns] fx(t),fy(t)
# pause -1 "Hit [Enter]..."
# unset parametric

unset object
unset label

# end