// test_tessellated_solid.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Geant4:
#include <globals.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4GDMLParser.hh>

/*
  export LD_LIBRARY_PATH="/data/sw/Cadfael/Binary/Cadfael-trunk/Install-Linux-x86_64/lib:${LD_LIBRARY_PATH}"

  LD_LIBRARY_PATH="/data/sw/Cadfael/Binary/Cadfael-trunk/Install-Linux-x86_64/lib:${LD_LIBRARY_PATH}" \
  g++ \
  -I/data/sw/Cadfael/Binary/Cadfael-trunk/Install-Linux-x86_64/include \
  -I/data/sw/Cadfael/Binary/Cadfael-trunk/Install-Linux-x86_64/include/Geant4 \
  test_g4_gdml.cxx \
  -L/data/sw/Cadfael/Binary/Cadfael-trunk/Install-Linux-x86_64/lib \
  -lG4persistency \
  -lG4geometry    \
  -lG4graphics_reps       \
  -lG4intercoms   \
  -lG4materials   \
  -lG4event   \
  -lG4run   \
  -lG4global      \
  -lxerces-c \
  -lCLHEP

  LD_LIBRARY_PATH="/data/sw/Cadfael/Binary/Cadfael-trunk/Install-Linux-x86_64/lib:${LD_LIBRARY_PATH}" \
  ./a.out data/test_tessellated_solid_1.gdml


*/

int main(int argc_, char * argv_[])
{
  try {
    // G4ExceptionDescription desc;
    // desc << "Test";
    // std::clog << "Test = '" << desc << "'" << std::endl;
    // G4Exception("here", "mock", FatalException, "test");


    std::string gdml_file = "data/test_tessellated_solid_1.gdml";
    if (argc_ > 1) gdml_file = argv_[1];
    std::clog << "GDML file = '" << gdml_file << "'" << std::endl;
    G4VPhysicalVolume * world_physical_volume = 0;
    std::setlocale(LC_NUMERIC, "C");
    G4GDMLParser parser;
    parser.SetStripFlag(false);
    G4String s(gdml_file.c_str());
    // parser.Read(s, true);
    parser.Read(s, false);
    world_physical_volume = parser.GetWorldVolume();
    // G4LogicalVolumeStore::Clean();
    G4LogicalVolumeStore * g4_LV_store = G4LogicalVolumeStore::GetInstance();
    std::clog << "Geant4 logical volumes: " << std::endl;
    for (G4LogicalVolumeStore::const_iterator i = g4_LV_store->begin();
         i != g4_LV_store->end();
         ++i) {
      G4LogicalVolume * g4_log = *i;
      std::clog << " --> Geant4 logical: '" << g4_log->GetName() << "'" << std::endl;
    }
  }
  catch (std::exception & error) {
    std::cerr << "ERROR: " << error.what() << std::endl;
    return 1;
  }
  catch (...) {
    std::cerr << "ERROR: " << "Unexpected error!" << std::endl;
    return 1;
  }
  return 0;
}
