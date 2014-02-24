// -*- mode: c++ ; -*-
//! \file datatools/units.h
/* units.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-18
 * Last modified: 2014-02-24
 *
 * License:
 *
 * Description:
 *
 *   Utilities for units (CLHEP)
 *
 * History:
 *
 */
#ifndef DATATOOLS_UNITS_H_
#define DATATOOLS_UNITS_H_
// Standard Library
#include <string>
#include <vector>
#include <limits>

// Third Party
// - A

// This Project
#include <datatools/exception.h>

namespace datatools {

/*! \brief Utilities for units (CLHEP).
 *
 *  The units class provides some static methods to manipulate
 *  units values through strings.
 *  It is based on the system of units in the CLHEP package.
 *
 */
class units {
 public:
  //! Get the CLHEP length unit from a string.
  static double get_length_unit_from(const std::string&);

  //! Get the CLHEP surface unit from a string.
  static double get_surface_unit_from(const std::string&);

  //! Get the CLHEP volume unit from a string.
  static double get_volume_unit_from(const std::string&);

  //! Get the CLHEP time unit from a string.
  static double get_time_unit_from(const std::string&);

  //! Get the CLHEP angle unit from a string.
  static double get_angle_unit_from(const std::string&);

  //! Get the CLHEP solid angle unit from a string.
  static double get_solid_angle_unit_from(const std::string&);

  //! Get the CLHEP energy unit from a string.
  static double get_energy_unit_from(const std::string&);

  //! Get the CLHEP mass unit from a string.
  static double get_mass_unit_from(const std::string&);

  //! Get the CLHEP pressure unit from a string.
  static double get_pressure_unit_from(const std::string&);

  //! Get the CLHEP magnetic field unit from a string.
  static double get_magnetic_field_unit_from(const std::string&);

  //! Get the CLHEP electric field unit from a string.
  static double get_electric_field_unit_from(const std::string&);

  //! Get the CLHEP electric tension unit from a string.
  static double get_electric_tension_unit_from(const std::string&);

  //! Get the CLHEP temperature unit from a string.
  static double get_temperature_unit_from(const std::string&);

  //! Get the CLHEP density unit from a string.
  static double get_density_unit_from(const std::string&);

  //! Get the CLHEP activity unit from a string.
  static double get_activity_unit_from(const std::string&);

  //! Get the CLHEP surface activity unit from a string.
  static double get_surface_activity_unit_from(const std::string&);

  //! Get the CLHEP volume activity unit from a string.
  static double get_volume_activity_unit_from(const std::string&);

  //! Get the CLHEP mass activity unit from a string.
  static double get_mass_activity_unit_from(const std::string&);

  //! Get the frequency unit from a string.
  static double get_frequency_unit_from(const std::string&);

  //! Get the electric charge unit from a string.
  static double get_electric_charge_unit_from(const std::string&);

  //! Get the electric current unit from a string.
  static double get_electric_current_unit_from(const std::string&);

  //! Get the speed unit from a string.
  static double get_speed_unit_from(const std::string&);

  //! Get the CLHEP unit from a string specifiying the unit type.
  /**
   * \b Example:
   *
   *   \code
   *   double lu = datatools::units::get_unit_from("length", "cm");
   *   \endcode
   */
  static double get_unit_from(const std::string& unit_type,
                              const std::string& unit_str);

  //! Get the CLHEP unspecified unit from a string.
  /**
   * \b Example:
   *
   *   \code
   *   double u = datatools::units::get_unit ("cm");
   *   \endcode
   */
  static double get_unit(const std::string& unit_str);

  //! Return an array containing the labels associated to all type of units supported  by the datatools::units class.
  static const std::vector<std::string>& get_unit_labels_registry();

  //! Check if a unit type with a given label is supported by the datatools::units class.
  /**
   * \b Example:
   *
   *   \code
   *   if (datatools::units::is_unit_label_valid ("length"))
   *     {
   *       std::cout << "A unit of type 'length' is possible." << std::endl;
   *     }
   *   if (! datatools::units::is_unit_label_valid ("megafoo"))
   *     {
   *       std::cout << "A unit of type 'megafoo' is not supported." << std::endl;
   *     }
   *   \endcode
   */
  static bool is_unit_label_valid(const std::string & unit_label);

  //! Return the symbol of the default unit associated to a unit label supported by the datatools::units class.
  static std::string get_default_unit_symbol_from_label(const std::string & unit_label);

  //! Find the CLHEP unspecified unit and the associated unit label from a string.
  /**
   * \b Example:
   *
   *   \code
   *   double unit_value;
   *   string unit_label;
   *   bool ok = datatools::units::find_unit("cm", unit_value, unit_label);
   *   cout << "Unit label = '" << unit_label << "'\n";
   *   cout << "Unit value = " << unit_value << "\n";
   *   \endcode
   *
   */
  static bool find_unit(const std::string& unit_str,
                        double& unit_value, std::string& unit_label);

  //! Get a value from a string taking into account the unit symbol.
  /**
   * \b Example:
   *
   *   \code
   *   double val = datatools::units::get_value_with_unit("2.54 cm");
   *   \endcode
   */
  static double get_value_with_unit(const std::string& word);

  //! Get a value from a string taking into account the unit symbol.
  /**
   * \b Example:
   *
   *   \code
   *   double value;
   *   string unit_label;
   *   bool ok = datatools::units::find_value_with_unit("2.54 cm", value, unit_label);
   *   \endcode
   */
  static bool find_value_with_unit(const std::string& word,
                                   double& value,
                                   std::string& unit_label,
                                   double default_unit_ = std::numeric_limits<double>::quiet_NaN());

};

} // end of namespace datatools

//! \def DT_THROW_BAD_UNIT(UnitType,UnitSymbol)
//! \brief Throw a special exception when \c UnitSymbol does not match \c UnitType.
#define DT_THROW_BAD_UNIT(UnitType,UnitSymbol) \
  DT_THROW_IF(true,std::logic_error, \
              "Invalid '" << UnitType << "' unit for symbol '" << UnitSymbol << "' !")
/**/

#endif // DATATOOLS_UNITS_H_
