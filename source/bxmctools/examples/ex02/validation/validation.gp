# Validation of the tracking of arbitrarily charged ions in a constant electric field
# in the Bayeux/mctools Geant4 module.
# Author: F.Mauger
# Date: 2015-09-09

# Units and constants:
C=1.0       # Coulomb
e=1.602176462e-19 * C # Elementary electric charge
V=1.0       # Volt
m=1.0       # meter
s=1.0       # second
ns=1e-9*s   # nanosecond
mm=1e-3*m   # millimeter
eV=e*V      # eV
keV=1e3*eV  # keV
MeV=1e3*keV # MeV
print "keV=", keV
c=2.99792458e8 * m / s
c2=c*c
c4=c2*c2

# Ion parameters: C12(2+)
M = 11176.90633818 * MeV / c2 # mass
Q = +2 * e # effective charge

print "Q = ", Q / e , " [e]"
# px=7.3057090503407576 * MeV/c
# py=-1.6783885887453207 * MeV/c
# pz=-0.63448324811959944 * MeV/c

# Ion generation from Bayeux:
px = 14.951194827290559 * (MeV/c)
py =  0.0 * (MeV/c)
pz =  9.1549664443349236e-16 * (MeV/c)

print "M = ", M * c2 / MeV , " [MeV]"
p2 = px**2 + py**2 + pz**2
p  = sqrt(p2)
W2 = p2 * c2 + M**2 * c4
W  = sqrt(W2)
print "W = ", W/MeV , " [MeV]"
K  = W - M * c2
print "K = ", K/keV , " [keV]"


####################
# C12(2+) @ 10 keV #
####################

# Ion initial conditions:
Ec0 = 10.0 * keV
W0 = Ec0 + M * c2
print "W0 = ", W0 / MeV , " [MeV]"

p0 = sqrt(Ec0*(Ec0+2*M*c2))/c
print "p0    = ", p0 / (MeV/c), " [MeV]"

p0b = sqrt(2*M*c2*Ec0)*(1.0+0.5*(Ec0/(2*M*c2)))/c
print "p0b   = ", p0b / (MeV/c), " [MeV] (NR)"
p0c = sqrt(2*M*c2*Ec0)/c
print "p0c   = ", p0c / (MeV/c), " [MeV] (NR)"

beta0 = p0 * c / W0
print "beta0 = ", beta0

vx0 = beta0 * c
print "vx0   = ", vx0 / c, " [c]"

vx0b = sqrt(2 * Ec0 / M) # Non relativistic
print "vx0b  = ", vx0b /c , " [c] (NR)"
vy0 = 0.0 * m/s
print "vy0   = ", vy0 /c , " [c]"

px0 = M*vx0
py0 = M*vy0
p0d  = sqrt(px0**2 + py0**2)
print "p0d   = ", p0d / (MeV/c), " [MeV] (NR)"

# Gun position:
x0  = -350.0 * mm
y0  =    0.0 * mm
z0  =    0.0 * mm

# Electric field (Y axis):
Ey0 = 2500.0 * V / m

# Equation of motion :
#  x : time (t)
#  y : ion charge (Q)
# Spacial coordinates versus time:
fx(x)   = x0 + vx0 * x
fy(x,y) = y0 + vy0 * x + 0.5 * (x**2) * (y * Ey0) / M
# Velocity coordinates versus time:
fvx(x)   = vx0
fvy(x,y) = vy0 + x * (y * Ey0) / M
print "fvy(  0 ns,2+)   = ", fvy(  0.0 * ns,Q) /c , " [c]"
print "fvy(100 ns,2+)   = ", fvy(100.0 * ns,Q) /c , " [c]"
# Velocity versus time:
fv(x,y) = sqrt(fvx(x)**2 + fvy(x,y)**2)
# Momentum versus time (NR):
fp(x,y) = M * fv(x,y)
# Kinetic energy versus time (NR):
fK(x,y) = 0.5 * M * fv(x,y)**2

# Ion charge:
Q = +2 * e

x_det = 200.0 * mm

# Ion impact on the front surface of the detector:
t_ion_in_detector = (x_det - x0) / vx0
x_ion_in_detector = fx(t_ion_in_detector)
y_ion_in_detector = fy(t_ion_in_detector, Q)
k_ion_in_detector = fK(t_ion_in_detector, Q)
print "T_ion(det.)     = ", t_ion_in_detector / ns , " [ns]"
print "X_ion(det.)     = ", x_ion_in_detector / mm , " [mm]"
print "Y_ion(det.)     = ", y_ion_in_detector / mm , " [mm]"
print "K_ion(det.)     = ", k_ion_in_detector / keV , " [keV]"

t_ion_in_detector_g4  = 1371.3490495142335 * ns # From G4
x_ion_in_detector_g4  = 200.0 * mm
y_ion_in_detector_g4  = 37.800411028266218 * mm
z_ion_in_detector_g4  = 0.018294153176661901 * mm
px_ion_in_detector_g4 = 14.951299878558425 * (MeV / c)
py_ion_in_detector_g4 = 2.0546549286114155 * (MeV / c)
pz_ion_in_detector_g4 = 0.00095849149237041951 * (MeV / c)
p_ion_in_detector_g4  = sqrt(px_ion_in_detector_g4**2 + py_ion_in_detector_g4**2 + pz_ion_in_detector_g4**2)
k_ion_in_detector_g4  = 0.5 * p_ion_in_detector_g4**2 / M

dk_ion_in_detector_g4_step1 = 0.0039391319239116844 * MeV
dk_ion_in_detector_g4_step2 = 0.0062498615711687805 * MeV
dk_ion_in_detector_g4       = dk_ion_in_detector_g4_step1 + dk_ion_in_detector_g4_step2

print "T_ion(det., G4)  = ", t_ion_in_detector_g4 / ns , " [ns]"
print "P_ion(det., G4)  = ", p_ion_in_detector_g4 / (MeV/c) , " [MeV/c]"
print "K_ion(det., G4)  = ", k_ion_in_detector_g4 / keV , " [keV]"
print "X_ion(det., G4)  = ", x_ion_in_detector_g4 / mm , " [mm]"
print "Y_ion(det., G4)  = ", y_ion_in_detector_g4 / mm , " [mm]"
print "DK_ion(det., G4) = ", dk_ion_in_detector_g4 / MeV , " [MeV]"

set title "Geant4 tracking of C12(2+) 10 keV ions \nin a 2500 V/m transverse electric field\nComparison with theoretical prediction"
tmin =    0.0 * ns
tmax = 2000.0 * ns
bmin =   -0.0001
bmax =    0.002
set xlabel "t (ns)"
set ylabel "V_x/V_y (c)"
set key outside box
set grid front
set xrange [tmin / ns: tmax / ns]
set yrange [bmin:bmax]
plot \
     fvx(x * ns) / c title "C12(2+) 10keV ion beta_x", \
     fvy(x * ns, Q) / c title "beta_y", \
     fv(x * ns, Q) / c title "beta"
pause -1 "Hit [Enter]..."

set ylabel "Ec (keV)"
set xtics 250
#set ytics 0.1
set key outside box
set xrange [tmin / ns: tmax / ns]
set yrange [0.:+20]
plot [0:*][0:*] fK(x * ns, Q) / keV title "C12(2+) 1keV ion"
pause -1 "Hit [Enter]..."

# World:
set object 1 rectangle center 0,0 size 1000,1000
set object 1 rectangle fc rgb "white"

# Lab:
set object 2 rectangle center 0,0 size 900,900
set object 2 rectangle fc rgb "cyan" fillstyle solid 0.2

# Detector:
set object 3 rectangle center +300,0 size 200,+100
set object 3 rectangle fc rgb "blue" fillstyle solid 0.2

# Beam pipe:
set object 4 rectangle center -400,0 size 100,+50
set object 4 rectangle fc rgb "orange" fillstyle solid 0.2

# G4 final impact:
szImpact = 5
xG4 = x_ion_in_detector_g4 / mm
yG4 = y_ion_in_detector_g4 / mm
set object 5 rectangle from xG4-0.5*szImpact,yG4-0.5*szImpact to xG4+0.5*szImpact,yG4+0.5*szImpact
set object 5 rectangle fc rgb "red" fillstyle solid 0.3

szImpact = 2.5
xTh = x_ion_in_detector / mm
yTh = y_ion_in_detector / mm
set object 6 rectangle from xTh-0.5*szImpact,yTh-0.5*szImpact to xTh+0.5*szImpact,yTh+0.5*szImpact
set object 6 rectangle fc rgb "orange" fillstyle solid 1.0

# Electric field:
set arrow 2 from 0,-300 to 0,-100 head filled front lt 3 lw 2

set parametric
set size ratio -1
set xlabel "x (mm)"
set ylabel "y (mm)"
set xrange [-600:+600]
set yrange [-600:+600]
set samples 10000
plot [0.0:t_ion_in_detector / ns] fx(t * ns) / mm,fy(t * ns, Q) / mm title "Trajectory"
pause -1 "Hit [Enter]..."
pause -1 "Hit [Enter]..."

set terminal pdfcairo enhanced color solid font ",10"
set output "mctools_g4_ex02_C12_ion_10keV_traj.pdf"
replot
set output
set terminal wxt

unset parametric
unset object


###################
# C12(2+) @ 1 keV #
###################

# Ion initial conditions:
Ec0 = 1.0 * keV
W0 = Ec0 + M * c2
print "W0 = ", W0 / MeV , " [MeV]"
p0 = sqrt(Ec0*(Ec0+2*M*c2))/c
print "p0    = ", p0 / (MeV/c), " [MeV]"
beta0 = p0 * c / W0
print "beta0 = ", beta0
vx0 = beta0 * c
print "vx0   = ", vx0 / c, " [c]"
vy0 = 0.0 * m/s
print "vy0   = ", vy0 /c , " [c]"

_ftrajy(x,y) = y0 + vy0 * (x - x0)/vx0 + 0.5 * (y*Ey0)/M * ((x - x0)/vx0)**2
ftrajy(x,y) = (x < x0) || (x > 500*mm) ? NaN : _ftrajy(x,y) > +500*mm ? NaN : _ftrajy(x,y)

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
set object 1 rectangle center 0,0 size 1000*mm,1000*mm back
set object 1 rectangle fc rgb "white"

# Lab:
set object 2 rectangle center 0,0 size 900*mm,900*mm back
set object 2 rectangle fc rgb "cyan" fillstyle solid 0.2

# Detector:
set object 3 rectangle center +300*mm,0*mm size 200*mm,+100*mm back
set object 3 rectangle fc rgb "blue" fillstyle solid 0.2

# Beam pipe:
set object 4 rectangle center -400*mm,0*mm size 100*mm,+50*mm back
set object 4 rectangle fc rgb "orange" fillstyle solid 0.2

# Geant4 results:
# C12(1+) (outlab: x=450 y=400.213842382928)  (outworld: x=500 y=450.236578882539)
# C12(2+) (outlab: x=249.921370408207 y=450)  (outworld: x=283.258150429337 y=500)
# C12(3+) (outlab: x=140.063510327711 y=450)  (outworld: x=167.291671046043 y=500)
# C12(4+) (outlab: x=74.3687389125986 y=450)  (outworld: x=97.9470085285827 y=500)
# C12(5+) (outlab: x=29.6383493298782 y=450)  (outworld: x=50.7279153904333 y=500)
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
set output "mctools_g4_ex02_C12_ions_1keV_traj.pdf"
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