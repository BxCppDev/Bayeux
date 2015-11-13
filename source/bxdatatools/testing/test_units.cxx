
// Standard library;
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <limits>

// This project:
#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/clhep_units.h>

void test1();
void test2();
void test3();

int main (/*int argc_, char ** argv_*/)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try
    {
      test1();
      test2();
      test3();

      clog << "The end." << endl;

    }
  catch (std::exception & x)
    {
      std::cerr << "error: " << x.what () << std::endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "error: " << "unexpected error!" << std::endl;
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

void test1()
{
  std::clog << std::endl;
  std::clog << "test1..." << std::endl;

  using namespace std;

  clog << "Known unit dimensions : " << endl;
  std::vector<std::string> ulreg;
  datatools::units::registered_unit_dimension_labels(ulreg);

  for (int i = 0; i < (int) ulreg.size (); i++) {
    clog << "Unit dimension label = '" << ulreg[i] << "'" << endl;
  }
  clog << endl;

  string word = "0.1 mm";
  clog << "Parsing expression : '" << word << "'" << endl;

  istringstream iss (word);
  double length;
  iss >> length;
  if (! iss) {
    ostringstream message;
    message << "Format error while reading a double value !";
    throw runtime_error (message.str ());
  }
  string sunit;
  iss >> sunit;
  if (! iss) {
    ostringstream message;
    message << "Format error while reading a word !";
    throw runtime_error (message.str ());
  }
  double lunit = datatools::units::get_length_unit_from (sunit);

  cout << "CLHEP basic units: " << endl;
  cout << "  1 m    ==  " << CLHEP::m << endl;
  cout << "    1 cm   ==  " << CLHEP::cm << endl;
  cout << "    1 mm   ==  " << CLHEP::mm << endl;
  cout << "  1 kg     ==  " << CLHEP::kg << endl;
  cout << "    1 g  ==  " << CLHEP::g << endl;
  cout << "  1 s      ==  " << CLHEP::second << endl;
  cout << "    1 ms   ==  " << CLHEP::millisecond << endl;
  cout << "    1 ns   ==  " << CLHEP::nanosecond << endl;

  // apply the unit to the value:
  length *= lunit;

  cout << "Length is : " << length / CLHEP::um << " um" << endl;
  cout << "Length is : " << length / CLHEP::cm << " cm" << endl;
  cout << "Length is : " << length / CLHEP::parsec << " parsec" << endl;

  double lengthu = datatools::units::get_value_with_unit ("8 inch");
  cout << "Length (2) is : "
       << lengthu / CLHEP::cm << " cm" << endl;

  double temperature = datatools::units::get_value_with_unit ("300 kelvin");
  cout << "Temperature is : "
       << temperature / CLHEP::kelvin << " kelvin" << endl;

  double pressure = datatools::units::get_value_with_unit ("1 millibar");
  cout << "Pressure is : "
       << pressure / CLHEP::atmosphere << " atm" << endl;

  try {
    // this fails:
    double fail = datatools::units::get_value_with_unit ("3.14159 dummy");
    clog << "fail=" << fail << endl;
  }
  catch (exception & x) {
    clog << "As expected, the 'dummy' symbol is not a known unit !" << endl;
  }

  {
    double unit_value;
    string unit_label;
    string unit_str = "mBq/m3";
    if (datatools::units::find_unit (unit_str, unit_value, unit_label))
      {
        clog << "Unit = '" << unit_str << "'\n";
        clog << "  Unit label = '" << unit_label << "'\n";
        clog << "  Unit value = " << unit_value << "\n";
      }
  }

  {
    double mass_activity_value;
    string unit_label;
    string value_str = "10 uBq/kg";
    if (datatools::units::find_value_with_unit (value_str, mass_activity_value, unit_label))
      {
        clog << "Value string = '" << value_str << "'\n";
        clog << "  Mass activity value = " << mass_activity_value << "\n";
        clog << "  Unit label = '" << unit_label << "'\n";
        std::string dus = datatools::units::get_default_unit_symbol_from_label(unit_label);
        clog << "  Value = "
             << mass_activity_value / datatools::units::get_unit(dus)
             << " " << dus << "\n";
      }
    double mass_value;
    if (datatools::units::find_value_with_unit ("2 kg", mass_value, unit_label))
      {
        clog << "Mass value = " << mass_value << "\n";
        clog << "Unit label = '" << unit_label << "'\n";
      }
    double activity = mass_activity_value * mass_value;
    double uBq_unit = 1.e-6 * CLHEP::becquerel;
    clog << "Activity value = " << activity / uBq_unit << " uBq\n";

  }


  {
    double frequency_value;
    string unit_label;
    if (datatools::units::find_value_with_unit ("10.3 kHz", frequency_value, unit_label))
      {
        clog << "Frequency value = " << frequency_value << "\n";
        clog << "Unit label = '" << unit_label << "'\n";
      }
  }

  std::clog << std::endl;
  return;
}

void test2()
{
  std::clog << std::endl;
  std::clog << "test2..." << std::endl;

  datatools::units::unit millimeter("millimeter", "mm", "length;[L]", CLHEP::mm);
  datatools::units::unit centimeter("centimeter", "cm", "length", CLHEP::cm);
  datatools::units::unit meter("meter", "m", "length", CLHEP::meter);
  datatools::units::unit meter_per_second("m/s", "velocity;[L][T-1]", CLHEP::meter / CLHEP::second);
  millimeter.tree_dump(std::clog, "Unit (mm):");
  centimeter.tree_dump(std::clog, "Unit (cm):");
  meter.tree_dump(std::clog, "Unit (m):");
  meter_per_second.tree_dump(std::clog, "Unit (m/s):");
  std::clog << std::endl;

  datatools::units::unit_dimension length("length", "millimeter");
  length.add_unit("centimeter")
    .add_unit("kilometer")
    .add_unit("meter", true)
    .add_unit("parsec");
  length.tree_dump(std::clog, "Unit dimension [L]:");
  std::clog << std::endl;

  const datatools::units::registry & unit_reg = datatools::units::registry::const_system_registry();
  unit_reg.tree_dump(std::clog, "System unit registry:");
  std::clog << std::endl;

  std::vector<std::string> reg_dims;
  datatools::units::registered_unit_dimension_labels(reg_dims);

  double mm = datatools::units::get_length_unit_from("mm");
  std::clog << "mm = " << mm << std::endl;

  double cm = datatools::units::get_length_unit_from("cm");
  std::clog << "cm = " << cm << std::endl;

  double parsec = datatools::units::get_length_unit_from("pc");
  std::clog << "parsec = " << parsec / CLHEP::km << " km" << std::endl;

  try {
    double yy = datatools::units::get_length_unit_from("kg");
    yy = yy*0;
  } catch (std::exception & error) {
    std::cerr << "As expected: " << error.what() << std::endl;
  }
  std::clog << std::endl;

  datatools::units::unit second("second", "s;sec;tick", "time;[T]", CLHEP::second);
  second.tree_dump(std::clog, "Unit (s):");
  std::clog << std::endl;

  datatools::units::unit minute("minute", "M;min;toc", "time;[T]", 60 * CLHEP::second);
  minute.tree_dump(std::clog, "Unit (min):");

  double elapsed_time = 3 * minute;
  std::clog << "elapsed time = " << elapsed_time << " [raw]" << std::endl;
  std::clog << "elapsed time = " << elapsed_time / second << " [sec]" << std::endl;
  std::clog << "elapsed time = " << elapsed_time / minute << " [min]" << std::endl;
  std::clog << std::endl;

  datatools::units::unit_dimension velocity("velocity", "m/s");
  velocity.decode_dimensional_powers("[L][T-1]");
  velocity.add_unit("km/h")
    .add_unit("mph")
    .add_unit("parsec/min");
  velocity.tree_dump(std::clog, "Unit dimension [velocity]:");
  std::clog << std::endl;

  datatools::units::unit_dimension speed("speed;[L][T-1]", "cm/us");
  speed.tree_dump(std::clog, "Unit dimension [velocity]:");
  std::clog << std::endl;


  datatools::units::unit_dimension acceleration("acceleration;[L][T-2]", "m/s2");
  acceleration.tree_dump(std::clog, "Unit dimension [acceleration]:");
  std::clog << std::endl;

  datatools::units::unit_dimension time("time;[T]", "s");
  time.tree_dump(std::clog, "Unit dimension [time]:");
  std::clog << std::endl;

  datatools::units::unit_dimension mass("mass;[M]", "kg");
  //mass.decode_dimensional_powers("[M]");
  mass.add_unit("g")
    .add_unit("dg")
    .add_unit("cg")
    .add_unit("mg")
    .add_unit("ug")
    .add_unit("ng")
    .add_unit("dag")
    .add_unit("hg")
    .add_unit("t");
  mass.tree_dump(std::clog, "Unit dimension [mass]:");
  std::clog << std::endl;

  if (! mass.match_dimensional_powers(velocity)) {
    std::clog << "Unit dimension " << mass.get_label() << " does NOT match " << velocity.get_label() << std::endl;
  }
  if (speed.match_dimensional_powers(velocity)) {
    std::clog << "Unit dimension " << speed.get_label() << " does match " << velocity.get_label() << std::endl;
  }
  std::clog << std::endl;

  datatools::units::unit_dimension foo("foo", "foo");
  foo.decode_dimensional_powers("[1]");
  foo.add_unit("fuu").add_unit("FUU");
  foo.tree_dump(std::clog, "Unit dimension [foo]:");
  std::clog << std::endl;

  datatools::units::unit_dimension gnu("gnu", "gnu");
  gnu.add_unit("Gnu").add_unit("GNU");
  gnu.tree_dump(std::clog, "Unit dimension [gnu]:");
  std::clog << std::endl;

  {
    std::clog << "Register a new unit..." << std::endl;
    datatools::units::registry & mutable_unit_reg = datatools::units::registry::system_registry();
    mutable_unit_reg.registration(datatools::units::unit("kg/s", "mass_flowrate;[M][T-1]", CLHEP::kg / CLHEP::second, true));
    mutable_unit_reg.tree_dump(std::clog, "System unit registry:");
    mutable_unit_reg.get_unit("kg/s").tree_dump(std::clog, "System unit :");
    mutable_unit_reg.get_dimension("mass_flowrate").tree_dump(std::clog, "System unit dimension:");
    std::clog << std::endl;
  }

  {
    const datatools::units::unit & meter = unit_reg.get_unit_from_any("m");
    const datatools::units::unit & meter2 = unit_reg.get_unit_from_any("m2");
    double a = 3.0 * meter;
    double b = 2.0 * meter;
    double area = a * b;
    std::clog << "area = " << area / meter2 << ' ' << meter2.str() << std::endl;
  }
  return;
}

void test3()
{
  std::clog << std::endl;
  std::clog << "test3..." << std::endl;

  const datatools::units::registry & unit_reg = datatools::units::registry::const_system_registry();
  std::cout << "Data storage and data transfert rate units: " << std::endl;
  std::cout << "  1 bit    ==  " << datatools::units::bit << std::endl;
  std::cout << "  1 byte   ==  " << datatools::units::byte << std::endl;
  std::cout << "  1 octet  ==  " << datatools::units::octet << std::endl;
  const datatools::units::unit & kibioctet = unit_reg.get_unit_from_any("KiB");
  kibioctet.tree_dump(std::cout, "Kibioctet: ");
  std::cout << "  1 kibioctet == " << kibioctet.get_value() <<std:: endl;

  const datatools::units::unit & gigabit_per_sec = unit_reg.get_unit_from_any("GB/s");
  gigabit_per_sec.tree_dump(std::cout, "GB/s: ");
  std::cout << "  1 GB/s == " << gigabit_per_sec.get_value() << std::endl;

  return;
}
