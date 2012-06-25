// -*- mode: c++ ; -*- 
//! \file datatools/utils/units.h
/* units.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-18
 * Last modified: 2011-03-25
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
#ifndef DATATOOLS_UTILS_UNITS_H_
#define DATATOOLS_UTILS_UNITS_H_
// Standard Library
#include <string>
#include <vector>

// Third Party
// - A

// This Project

namespace datatools {
namespace utils {

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

  //! Get the CLHEP unit from a string specifiying the unit type.
  /**
   * \b Example:
   *
   *   \code
   *   double lu = datatools::utils::units::get_unit_from("length", "cm");
   *   \endcode
   */
  static double get_unit_from(const std::string& unit_type, 
                              const std::string& unit_str);

  //! Get the CLHEP unspecified unit from a string.
  /**
   * \b Example:
   *
   *   \code
   *   double u = datatools::utils::units::get_unit ("cm");
   *   \endcode
   */
  static double get_unit(const std::string& unit_str);

  static const std::vector<std::string>& get_unit_labels_registry();

  //! Find the CLHEP unspecified unit and the associated unit label from a string.
  /**
   * \b Example:
   *
   *   \code
   *   double unit_value;
   *   string unit_label;
   *   bool ok = datatools::utils::units::find_unit("cm", unit_value, unit_label);
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
   *   double val = datatools::utils::units::get_value_with_unit("2.54 cm");
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
   *   bool ok = datatools::utils::units::find_value_with_unit("2.54 cm", value, unit_label);
   *   \endcode
   */
  static bool find_value_with_unit(const std::string& word, 
                                   double& value, 
                                   std::string& unit_label);

 protected:
  //! Exception thrown in case of unrecognized unit symbol.
  static void throw_bad_unit(const std::string& type, 
                             const std::string& unit_str);
};

} // end of namespace utils
} // end of namespace datatools

#endif // DATATOOLS_UTILS_UNITS_H_

