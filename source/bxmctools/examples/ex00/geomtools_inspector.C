#include <iostream>
void geomtools_inspector() {
TGeoManager * geo = new TGeoManager("geo","Virtual geometry setup 'mctools_ex00-1.0'");
TGeoManager * g2 = geo->Import("mctools_ex00-1.0.gdml");
g2->SetVisOption(0);
g2->SetVisLevel(100);
g2->GetMasterVolume()->Draw();
std::cout << "Enter '.q' to quit...
";
}
