// constants.h

#ifndef MYGSL_CONSTANTS_H_
#define MYGSL_CONSTANTS_H_ 1

/// Top-level namespace of the Bayeux/mygsl module library
namespace mygsl {

  /// Nested namespace of the Bayeux/mygsl module library (constants)
  namespace constants {

    //! MKSA system of units and constants
    class mksa
    {
    public:

      static const double acre;
      static const double angstrom;
      static const double astronomical_unit;
      static const double bar;
      static const double barn;
      static const double bohr_magneton;
      static const double bohr_radius;
      static const double boltzmann;
      static const double btu;
      static const double calorie;
      static const double canadian_gallon;
      static const double carat;
      static const double cup;
      static const double curie;
      static const double day;
      static const double debye;
      static const double dyne;
      static const double electron_charge;
      static const double electron_magnetic_moment;
      static const double electron_volt;
      static const double erg;
      static const double faraday;
      static const double fathom;
      static const double fluid_ounce;
      static const double foot;
      static const double footcandle;
      static const double footlambert;
      static const double gauss;
      static const double gram_force;
      static const double grav_accel;
      static const double gravitational_constant;
      static const double hectare;
      static const double horsepower;
      static const double hour;
      static const double inch;
      static const double inch_of_mercury;
      static const double inch_of_water;
      static const double joule;
      static const double kilometers_per_hour;
      static const double kilopound_force;
      static const double knot;
      static const double lambert;
      static const double light_year;
      static const double liter;
      static const double lumen;
      static const double lux;
      static const double mass_electron;
      static const double mass_muon;
      static const double mass_neutron;
      static const double mass_proton;
      static const double meter_of_mercury;
      static const double metric_ton;
      static const double micron;
      static const double mil;
      static const double mile;
      static const double miles_per_hour;
      static const double minute;
      static const double molar_gas;
      static const double nautical_mile;
      static const double newton;
      static const double nuclear_magneton;
      static const double ounce_mass;
      static const double parsec;
      static const double phot;
      static const double pint;
      static const double plancks_constant_h;
      static const double plancks_constant_hbar;
      static const double point;
      static const double poise;
      static const double poundal;
      static const double pound_force;
      static const double pound_mass;
      static const double proton_magnetic_moment;
      static const double psi;
      static const double quart;
      static const double rad;
      static const double roentgen;
      static const double rydberg;
      static const double solar_mass;
      static const double speed_of_light;
      static const double standard_gas_volume;
      static const double std_atmosphere;
      static const double stefan_boltzmann_constant;
      static const double stilb;
      static const double stokes;
      static const double tablespoon;
      static const double teaspoon;
      static const double texpoint;
      static const double therm;
      static const double thomson_cross_section;
      static const double ton;
      static const double torr;
      static const double troy_ounce;
      static const double uk_gallon;
      static const double uk_ton;
      static const double unified_atomic_mass;
      static const double us_gallon;
      static const double vacuum_permeability;
      static const double vacuum_permittivity;
      static const double week;
      static const double yard;
    }; // end of class mksa


    //! Units prefixes and constants
    class num
    {
    public:

      static const double atto;
      static const double avogadro;
      static const double exa;
      static const double femto;
      static const double fine_structure;
      static const double giga;
      static const double kilo;
      static const double mega;
      static const double micro;
      static const double milli;
      static const double nano;
      static const double peta;
      static const double pico;
      static const double tera;
      static const double yocto;
      static const double yotta;
      static const double zepto;
      static const double zetta;
    }; // end of class num

    //! CSGM system of units and constants
    class cgsm
    {
    public:

      static const double acre;
      static const double angstrom;
      static const double astronomical_unit;
      static const double bar;
      static const double barn;
      static const double bohr_magneton;
      static const double bohr_radius;
      static const double boltzmann;
      static const double btu;
      static const double calorie;
      static const double canadian_gallon;
      static const double carat;
      static const double cup;
      static const double curie;
      static const double day;
      static const double dyne;
      static const double electron_charge;
      static const double electron_magnetic_moment;
      static const double electron_volt;
      static const double erg;
      static const double faraday;
      static const double fathom;
      static const double fluid_ounce;
      static const double foot;
      static const double footcandle;
      static const double footlambert;
      // 2009-11623 FM: remove
      // static const double gauss;
      static const double gram_force;
      static const double grav_accel;
      static const double gravitational_constant;
      static const double hectare;
      static const double horsepower;
      static const double hour;
      static const double inch;
      static const double inch_of_mercury;
      static const double inch_of_water;
      static const double joule;
      static const double kilometers_per_hour;
      static const double kilopound_force;
      static const double knot;
      static const double lambert;
      static const double light_year;
      static const double liter;
      static const double lumen;
      static const double lux;
      static const double mass_electron;
      static const double mass_muon;
      static const double mass_neutron;
      static const double mass_proton;
      static const double meter_of_mercury;
      static const double metric_ton;
      static const double micron;
      static const double mil;
      static const double mile;
      static const double miles_per_hour;
      static const double minute;
      static const double molar_gas;
      static const double nautical_mile;
      static const double newton;
      static const double nuclear_magneton;
      static const double ounce_mass;
      static const double parsec;
      static const double phot;
      static const double pint;
      static const double plancks_constant_h;
      static const double plancks_constant_hbar;
      static const double point;
      static const double poise;
      static const double poundal;
      static const double pound_force;
      static const double pound_mass;
      static const double proton_magnetic_moment;
      static const double psi;
      static const double quart;
      static const double rad;
      static const double roentgen;
      static const double rydberg;
      static const double solar_mass;
      static const double speed_of_light;
      static const double standard_gas_volume;
      static const double std_atmosphere;
      static const double stefan_boltzmann_constant;
      static const double stilb;
      static const double stokes;
      static const double tablespoon;
      static const double teaspoon;
      static const double texpoint;
      static const double therm;
      static const double thomson_cross_section;
      static const double ton;
      static const double torr;
      static const double troy_ounce;
      static const double uk_gallon;
      static const double uk_ton;
      static const double unified_atomic_mass;
      static const double us_gallon;
      static const double week;
      static const double yard;
    }; // end of class cgsm

  } // end of namespace constants

} // end of namespace mygsl

#endif // MYGSL_CONSTANTS_H_

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

// end of constants.h
