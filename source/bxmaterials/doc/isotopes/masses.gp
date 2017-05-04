# A Gnuplot macro to test various methods to compute
# the mass of a nucleus given AME2012 data.
# Author: F. Mauger
# Data from:
# - PDG
# - M.Wang et al., The AME2012 atomic mass evaluation

#############################################################
# Units:
keV=1.0
eV=1e-3 * keV
MeV=1000 * keV
GeV=1000 * MeV
u=931.494028 * MeV

#############################################################
# Constants:
me=0.510998910 * MeV
mp=938.272013 * MeV
mn=939.56536 * MeV

#############################################################
# x == Z
Bel(x)=(14.4381*x**2.39 + 1.55468e-6*x**5.35) * eV
set grid
Zmin=0
Zmax=100
set title "Atomic electron binding energies"
set xlabel "Z"
set ylabel "Bel (eV)"
plot [Zmin:Zmax] Bel(x)/eV notitle with lines
print "Bel(Z=1)  = " , Bel(1)/eV , " eV"
print "Bel(Z=92) = " , Bel(92)/keV , " keV"
pause -1 "Hit [Enter] to continue..."

#############################################################
# Compute the nucleus mass from A and the mass excess
# x == A
# y == Z
# z == mass excess
compute_nucleus_mass(x,y,z)= x * u - y * me + z

#############################################################
# Compute the nucleus mass from A, Z and the binding energy
# per nucleon
# x == A
# y == Z
# z == binding energy/nucleon
compute_nucleus_mass_A_Z_BA(x,y,z)= y * mp + (x - y) * mn - x * z

#############################################################
# Compute the nucleus mass from Z and the atomic mass
# x == atomic mass
# y == Z
compute_nucleus_mass_MA_Z(x,y) = x - y * me + Bel(y)

# proton:
Mx_p=7288.970591 * keV
B_p=0.0*keV
MA_p=1.00782503223 * u
print ""
print "Proton mass  = " , mp / MeV , " MeV"
print "Proton mass  = " , compute_nucleus_mass(1, 1, Mx_p) / MeV , " MeV "
print "Proton mass  = " , compute_nucleus_mass_A_Z_BA(1, 1, B_p) / MeV , " MeV "
print "Proton mass  = " , compute_nucleus_mass_MA_Z(MA_p, 1) / MeV , " MeV"

# neutron:
Mx_n=8071.317144 * keV
B_n=0.0*keV
MA_n=1.0086649158 * u
print ""
print "Neutron mass  = " , mn / MeV , " MeV"
print "Neutron mass  = " , compute_nucleus_mass(1, 0, Mx_n) / MeV , " MeV "
print "Neutron mass  = " , compute_nucleus_mass_A_Z_BA(1, 0, B_n) / MeV , " MeV "
print "Neutron mass  = " , compute_nucleus_mass_MA_Z(MA_n, 0) / MeV , " MeV"

# deuteron:
md=1875.612793 * MeV
Mx_d=13135.72174 * keV
B_d=1112.283 * keV
MA_d=2.01410177812 * u
print ""
print "Deuteron mass  = " , md / MeV , " MeV"
print "Deuteron mass  = " , compute_nucleus_mass(2, 1, Mx_d) / MeV , " MeV "
print "Deuteron mass  = " , compute_nucleus_mass_A_Z_BA(2, 1, B_d) / MeV , " MeV "
print "Deuteron mass  = " , compute_nucleus_mass_MA_Z(MA_d, 1) / MeV , " MeV"

# triton:
mt=2808.921 * MeV
Mx_t=14949.8061 * keV
B_t=2827.266 * keV
MA_t=3.0160492779 * u
print ""
print "Triton mass  = " , mt / MeV , " MeV"
print "Triton mass  = " , compute_nucleus_mass(3, 1, Mx_t) / MeV , " MeV "
print "Triton mass  = " , compute_nucleus_mass_A_Z_BA(3, 1, B_t) / MeV , " MeV "
print "Triton mass  = " , compute_nucleus_mass_MA_Z(MA_t, 1) / MeV , " MeV"

# He3
mHe3=2.808391 * GeV
Mx_He3=14931.2155 * keV
B_He3=2572.681 * keV
MA_He3=3.0160293201 * u
print ""
print "He3 mass  = " , mHe3 / MeV , " MeV"
print "He3 mass  = " , compute_nucleus_mass(3, 2, Mx_He3) / MeV , " MeV "
print "He3 mass  = " , compute_nucleus_mass_A_Z_BA(3, 2, B_He3) / MeV , " MeV "
print "He3 mass  = " , compute_nucleus_mass_MA_Z(MA_He3, 2) / MeV , " MeV"

# alpha:
ma=3.727379 * GeV
Mx_a=2424.91561 * keV
B_a=7073.915 * keV
MA_a=4.00260325413 * u
print ""
print "Alpha mass  = " , ma / MeV , " MeV"
print "Alpha mass  = " , compute_nucleus_mass(4, 2, Mx_a) / MeV , " MeV "
print "Alpha mass  = " , compute_nucleus_mass_A_Z_BA(4, 2, B_a) / MeV , " MeV "
print "Alpha mass  = " , compute_nucleus_mass_MA_Z(MA_a, 2) / MeV , " MeV"

# end