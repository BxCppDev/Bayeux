//! \file datatools/units.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-18
 * Last modified: 2015-10-18
 *
 * License: GPL3
 *
 * Description:
 *
 *   Utilities for units based on the CLHEP library's implementation
  *
 */
#ifndef DATATOOLS_UNITS_H
#define DATATOOLS_UNITS_H

// Standard Library:
#include <string>
#include <vector>
#include <set>
#include <map>
#include <limits>
#include <list>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>

// This Project:
#include <datatools/exception.h>
#include <datatools/i_tree_dump.h>

#ifndef Q_MOC_RUN
#include <datatools/reflection_interface.h>
#endif // Q_MOC_RUN

namespace datatools {

  /*! \brief Utilities for units based on the CLHEP library's implementation.
   *
   *  The units namespace provides some methods to manipulate
   *  units values through strings.
   *  It is based on the system of units in the CLHEP package.
   *  A system registry of units (class datatools::units::unit) is provided
   *  to register arbitrary SI based units.
   *  Extension tools are provided beyond the CLHEP material.
   */
  namespace units {

    //! SI powers of ten
    enum power_of_ten {
      YOCTO = -24,
      ZEPTO = -21,
      ATTO  = -18,
      FEMTO = -15,
      PICO  = -12,
      NANO  =  -9,
      MICRO =  -6,
      MILLI =  -3,
      CENTI =  -2,
      DECI  =  -1,
      UNIT_POWER_OF_TEN = 0,
      DECA  =   1,
      HECTO =   2,
      KILO  =   3,
      MEGA  =   6,
      GIGA  =   9,
      TERA  =  12,
      PETA  =  15,
      EXA   =  18,
      ZETTA =  21,
      YOTTA =  24
    };

    double yocto();
    double zepto();
    double atto();
    double femto();
    double pico();
    double nano();
    double micro();
    double milli();
    double centi();
    double deci();
    double deca();
    double hecto();
    double kilo();
    double mega();
    double giga();
    double tera();
    double peta();
    double exa();
    double zetta();
    double yotta();

    //! Return the SI prefix factor associated to a given SI power of ten
    double power_of_ten_to_factor(power_of_ten);

    //! Return the SI name associated to a given SI power of ten
    std::string power_of_ten_to_name(power_of_ten);

    //! Return the SI symbol associated to a given SI power of ten
    std::string power_of_ten_to_symbol(power_of_ten);

    //! IEC powers of two (binary prefixes)
    enum power_of_two {
      UNIT_POWER_OF_TWO = 0,
      KIBI = 10,
      MEBI = 20,
      GIBI = 30,
      TEBI = 40,
      PEBI = 50,
      EXBI = 60,
      ZEBI = 70,
      YOBI = 80
    };

    //! Return the IEC 80000-13 byte (octet 8 bits) to bit factor
    double byte_to_bit_factor();

    static const double bit   = 1.0;
    static const double octet = 8.0 * bit;
    static const double byte  = octet;

    //! Return the IEC binary prefix approximated factor associated to a given IEC power of two
    double power_of_two_to_factor(power_of_two);

    //! Return the IEC binary prefix name associated to a given IEC power of two
    std::string power_of_two_to_name(power_of_two);

    //! Return the IEC binary prefix symbol associated to a given IEC power of two
    std::string power_of_two_to_symbol(power_of_two);

    //! Build the list of system registered dimension labels
    //! @param dimension_labels_ a vector of string to be filled by unit dimension labels
    //! @return the number of unit dimension labels
    size_t registered_unit_dimension_labels(std::vector<std::string> & dimension_labels_);

    //! Build the list of system registered unit names
    //! @param unit_names_ a vector of string to be filled by unit names
    //! @return the number of unit names
    size_t registered_unit_names(std::vector<std::string> & unit_names_);

    //! Build the list of system registered unit symbols
    //! @param unit_symbols_ a vector of string to be filled by unit symbols
    //! @return the number of unit symbols
    size_t registered_unit_symbols(std::vector<std::string> & unit_symbols_);

    //! Check if a unit value given by id (name or symbol) is in a dimension
    //! @param unit_id_ the name or the symbol of the unit
    //! @param unit_dimension_label_ the label of the unit dimension
    //! @return true is the unit symbol is associated to the unit dimension
    //! \code
    //! is_unit_in_dimension_from("mm", "length"); // true
    //! is_unit_in_dimension_from("kg", "length"); // false
    //! is_unit_in_dimension_from("kg", "mass");   // true
    //! \endcode
    bool is_unit_in_dimension_from(const std::string & unit_id_,
                                   const std::string & unit_dimension_label_);

    //! Return the unit value associated to an unit given its id (name or symbol) and dimension
    //! @param unit_id_ the name or the symbol of the unit
    //! @param unit_dimension_label_ the label of the unit dimension
    //! @return the value associated to the unit
    //! \code
    //! double mm = get_unit_in_dimension_from("mm", "length"); // success
    //! double xx = get_unit_in_dimension_from("kg", "length"); // throw an exception
    //! double kg = get_unit_in_dimension_from("kg", "mass");   // success
    //! \endcode
    double get_unit_in_dimension_from(const std::string & unit_id_,
                                      const std::string & unit_dimension_label_);

    //! Get the length unit value from a string.
    //! @param unit_id_ the name or the symbol of the unit
    //! @return the value associated to the unit
    //! \code
    //! double mm = get_length_unit_from("mm"); // success
    //! double xx = get_length_unit_from("kg"); // throw an exception
    //! double yy = get_length_unit_from("yy"); // throw an exception
    //! \endcode
    double get_length_unit_from(const std::string & unit_id_);

    //! Get the surface unit value from a string.
    double get_surface_unit_from(const std::string &);

    //! Get the volume unit value from a string.
    double get_volume_unit_from(const std::string &);

    //! Get the time unit value from a string.
    double get_time_unit_from(const std::string &);

    //! Get the angle unit value from a string.
    double get_angle_unit_from(const std::string &);

    //! Get the solid angle unit value from a string.
    double get_solid_angle_unit_from(const std::string &);

    //! Get the energy unit from a string.
    double get_energy_unit_from(const std::string &);

    //! Get the mass unitvalue  from a string.
    double get_mass_unit_from(const std::string &);

    //! Get the pressure unit value from a string.
    double get_pressure_unit_from(const std::string &);

    //! Get the magnetic field unit value from a string.
    double get_magnetic_flux_density_unit_from(const std::string &);

    //! @deprecated Get the magnetic field unit value from a string.
    double get_magnetic_field_unit_from(const std::string &);

    //! Get the electric field unit value from a string.
    double get_electric_field_unit_from(const std::string &);

    //! Get the electric potential unit value from a string.
    double get_electric_potential_unit_from(const std::string &);

    //! @deprecated Get the electric tension unit value from a string.
    double get_electric_tension_unit_from(const std::string &);

    //! Get the temperature unit value from a string.
    double get_temperature_unit_from(const std::string &);

    //! Get the density unit value from a string.
    double get_density_unit_from(const std::string &);

    //! Get the activity unit value from a string.
    double get_activity_unit_from(const std::string &);

    //! Get the surface activity unit value from a string.
    double get_surface_activity_unit_from(const std::string &);

    //! Get the volume activity unit value from a string.
    double get_volume_activity_unit_from(const std::string &);

    //! Get the mass activity unit value from a string.
    double get_mass_activity_unit_from(const std::string &);

    //! Get the frequency unit value from a string.
    double get_frequency_unit_from(const std::string &);

    //! Get the electric charge unit value from a string.
    double get_electric_charge_unit_from(const std::string &);

    //! Get the electric current unit value from a string.
    double get_electric_current_unit_from(const std::string &);

    //! Get the velocity unit value from a string.
    double get_velocity_unit_from(const std::string &);

    //! @deprecated Get the speed unit value from a string.
    double get_speed_unit_from(const std::string &);


    //! Get the CLHEP unit value from a string specifiying the unit type.
    /**
     * \b Example:
     *
     *   \code
     *   double lu = datatools::units::get_unit_from("length", "cm");
     *   \endcode
     */
    double get_unit_from(const std::string & unit_type_,
                         const std::string & unit_str_);

    //! Get the CLHEP unspecified unit value from a string.
    /**
     * \b Example:
     *
     *   \code
     *   double u = datatools::units::get_unit("cm");
     *   \endcode
     */
    double get_unit(const std::string & unit_str_, bool throw_ = false);

    //! Return an array containing the labels associated to all type of units supported  by the datatools::units class.
    // const std::vector<std::string>& get_unit_labels_registry();

    //! Check if a unit type with a given label is supported by the datatools::units class.
    /**
     * \b Example:
     *
     *   \code
     *   if (datatools::units::is_unit_label_valid ("length"))
     *     {
     *       std::cout << "A unit of type 'length' is possible." << std::endl;
     *     }
     *   if (! datatools::units::is_unit_label_valid ("color"))
     *     {
     *       std::cout << "A unit of type 'color' is not supported." << std::endl;
     *     }
     *   \endcode
     */
    bool is_unit_label_valid(const std::string & unit_label_);

    //! Return the symbol of the default unit associated to a unit label supported by the datatools::units class.
    std::string get_default_unit_symbol_from_label(const std::string & unit_label_);

    //! Find the specified unit and the associated unit label from a string.
    /**
     * \b Example:
     *
     *   \code
     *   double unit_value;
     *   string unit_label;
     *
     *   bool ok = datatools::units::find_unit("cm", unit_value, unit_label);
     *   cout << "Unit value = " << unit_value << "\n";
     *   cout << "Unit label = '" << unit_label << "'\n";
     *
     *   bool ok2 = datatools::units::find_unit("centimeter", unit_value, unit_label);
     *   cout << "Unit value = " << unit_value << "\n";
     *   cout << "Unit label = '" << unit_label << "'\n";
     *   \endcode
     *
     */
    bool find_unit(const std::string & unit_str_,
                   double & unit_value_,
                   std::string & unit_label_);

    //! Get a value from a string taking into account the unit symbol.
    /**
     * \b Example:
     *
     *   \code
     *   double val = datatools::units::get_value_with_unit("2.54 cm");
     *   \endcode
     */
    double get_value_with_unit(const std::string & word_);

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
    bool find_value_with_unit(const std::string & word_,
                              double & value_,
                              std::string & unit_label_,
                              double default_unit_ = std::numeric_limits<double>::quiet_NaN());

    //! Parse a value from a string taking into account the unit symbol, set the value, the unit symbol and unit label.
    /**
     * \b Example:
     *
     *   \code
     *   double value;
     *   string unit_symbol;
     *   string unit_label;
     *   bool ok = datatools::units::parse_value_with_unit("2.54 cm", value, unit_symbol, unit_label);
     *   \endcode
     *   This will output:
     *     value       = 25.4  // because native unit for length is "mm"
     *     unit_symbol = "cm"
     *     unit_label  = "length"
     */
    bool parse_value_with_unit(const std::string & word,
                               double& value,
                               std::string & unit_symbol,
                               std::string & unit_label,
                               uint32_t flags = 0);

    //! \brief The description of an unit
    class unit
      : public i_tree_dumpable
    {

    public:

      //! Default constructor
      unit();

      //! Constructor
      unit(const std::string & name_,
           const std::string & dimension_desc_,
           double value_,
           bool SI_main_ = false);

      //! Constructor
      unit(const std::string & name_,
           const std::string & symbol_,
           const std::string & dimension_desc_,
           double value_,
           bool SI_main_ = false);

      //! Constructor
      unit(const std::string & name_,
           const std::string & main_symbol_,
           const std::string & alt_symbols_,
           const std::string & dimension_desc_,
           double value_,
           bool SI_main_ = false);

      //! Destructor
      ~unit() override;

      //! Check validity
      bool is_valid() const;

      //! Reset
      void reset();

      //! Check the name
      bool has_name() const;

      //! Return the name
      const std::string & get_name() const;

      //! Set the name
      unit & set_name(const std::string &);

      //! Add a symbol
      unit & add_symbol(const std::string & symbol_, bool main_ = false);

      //! Check if a given symbol is defined
      bool has_symbol(const std::string & symbol_) const;

      //! Check if a given id matches the name or one of the symbol
      bool match(const std::string & id_) const;

      //! Build the set of associated symbols
      const std::set<std::string> & get_symbols() const;

      //! Check main symbol
      bool has_main_symbol() const;

      //! Return the symbol
      const std::string & get_main_symbol() const;

      //! Set the symbol
      unit & set_main_symbol(const std::string &);

      //! Check dimension label
      bool has_dimension_label() const;

      //! Return the dimension label
      const std::string & get_dimension_label() const;

      //! Set the dimension label
      unit & set_dimension_label(const std::string &);

      //! Check dimension meta
      bool has_dimension_meta() const;

      //! Return the dimension meta
      const std::string & get_dimension_meta() const;

      //! Set the dimension meta
      unit & set_dimension_meta(const std::string &);

      //! Set the dimension infos
      unit & set_dimension_infos(const std::string &);

      //! Check if the dimension matches a given label
      bool is_dimension(const std::string &) const;

      //! Check if the unit is the main SI unit for its unit dimension
      bool is_SI_main() const;

      //! Set the main SI unit flag
      unit & set_SI_main(bool);

      //! Return the value
      double get_value() const;

      //! Set the value
      unit & set_value(double);

      //! Conversion to double
      operator double() const;

      //! Shortcut for the symbol
      const std::string & str() const;

      // Implement a manipulator for "cout << meter(23.1) << endl;" printing "0.0231" ???
      // std::ostream & operator()(std::ostream & out_, double x_) const;

      //! Smart print
      void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const override;

    protected:

      //! Set default attributes
      void _set_default();

    private:

      std::string           _name_;            //!< Unit name
      std::set<std::string> _symbols_;         //!< Unit symbols
      std::string           _main_symbol_;     //!< The main unit symbol
      std::string           _dimension_label_; //!< Label of the unit dimension
      std::string           _dimension_meta_;  //!< Meta information about the unit dimension
      bool                  _SI_main_;         //!< Flag the SI main unit
      double                _value_;           //!< Unit value (CLHEP convention)

    };

    //! \brief The description of an unit dimension
    class unit_dimension
      : public i_tree_dumpable
    {

    public:

      //! \brief Base dimension index
      enum base_dimension_type {
        BD_INVALID              =  0,
        BD_MASS                 =  1, // [M]
        BD_LENGTH               =  2, // [L]
        BD_TIME                 =  3, // [T]
        BD_ELECTRIC_CURRENT     =  4, // [I]
        BD_TEMPERATURE          =  5, // [theta]
        BD_ABSOLUTE_TEMPERATURE =  5, // [theta]
        BD_AMOUNT               =  6, // [N]
        BD_AMOUNT_OF_SUBSTANCE  =  6, // [N]
        BD_LUMINOUS_INTENSITY   =  7, // [J]
        BD_PROCEDURE_DEFINED    =  8, // [?] For "arbitrary/procedure defined" units
        BD_LAST                 =  BD_PROCEDURE_DEFINED + 1
      };

      static std::string base_dimension_to_symbol(base_dimension_type);

      static base_dimension_type symbol_to_base_dimension(const std::string &);

      static std::string base_dimension_to_label(base_dimension_type);

      static base_dimension_type label_to_base_dimension(const std::string &);

      //! Validate a unit dimension label
      static bool validate_dimension_label(const std::string & name_);

      //! Default constructor
      unit_dimension();

      //! Default constructor
      unit_dimension(const std::string & label_,
                     const std::string & default_unit_symbol_);

      //! Destructor
      ~unit_dimension() override;

      //! Check validity
      bool is_valid() const;

      //! Reset
      void reset();

      //! Check the label
      bool has_label() const;

      //! Set the label of the unit dimension
      unit_dimension & set_label(const std::string &);

      //! Return the label of the unit dimension
      const std::string & get_label() const;

      //! Check if a unit is known by name
      bool has_unit(const std::string & unit_name_) const;

      //! Add an unit by name
      unit_dimension & add_unit(const std::string & unit_name_,
                                bool default_unit_ = false);

      //! Remove an unit by name
      void remove_unit(const std::string & unit_name_);

      //! Return the set of unit names
      const std::set<std::string> & get_unit_names() const;

      //! Check if a default unit is defined
      bool has_default_unit() const;

      //! Set the name of the default unit
      unit_dimension & set_default_unit_name(const std::string & unit_name_);

      //! Return the name of the default unit
      const std::string & get_default_unit_name() const;

      //! Check if the dimensional analysis information is available
      bool has_dimensional_powers() const;

      //! Reset the the dimensional analysis information
      void reset_dimensional_powers();

      //! Decode and set the dimensional powers from a character string
      bool decode_dimensional_powers(const std::string & word_);

      //! Encode the set of dimensional powers
      void encode_dimensional_powers(std::string & word_) const;

      //! Check if unit dimension is dimensionless
      bool is_dimensionless() const;

      //! Check if the dimensional powers match another unit dimension
      bool match_dimensional_powers(const unit_dimension & ud_) const;

      //! Smart print
      void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const override;

    protected:

      //! Set default attributes
      void _set_default();

    private:

      std::string           _label_;                 //!< Label of the unit dimension
      std::set<std::string> _unit_names_;            //!< Set of names of the units in this dimension
      std::string           _default_unit_name_;     //!< Default unit name
      int                   _dimensional_powers_[9]; //!< Powers of base units (dimensional analysis)

    };

    //! \brief A registry for units
    class registry
      : public i_tree_dumpable
    {

    public:

      enum init_flag_type {
        NO_STANDARD_UNITS = 0x1
      };

      //! Type of dictionary of unit
      typedef std::map<std::string, unit> unit_dict_type;

      //! Type of dictionary of unit
      typedef std::map<std::string, const unit *> symbol_dict_type;

      //! Type of dictionary of dimension
      typedef std::map<std::string, unit_dimension> dimension_dict_type;

      //! Default constructor
      registry(uint32_t flags_ = 0);

      //! Destructor
      ~registry() override;

      //! Registration of an unit
      void registration(const unit & unit_, bool default_in_dimension_ = false);

      //! Unregistration of an unit
      void unregistration(const std::string & unit_name_);

      //! Check unit
      bool has_unit(const std::string & unit_name_) const;

      //! Return a registered unit
      const unit & get_unit(const std::string & unit_name_) const;

      //! Check unit symbol
      bool has_symbol(const std::string & unit_symbol_) const;

      //! Check unit name or symbol
      bool has_unit_from_any(const std::string & unit_label_) const;

      //! Return a registered unit per symbol
      const unit & get_unit_from_symbol(const std::string & unit_symbol_) const;

      //! Return a registered unit by name or symbol
      const unit & get_unit_from_any(const std::string & unit_id_) const;

      //! Check dimension
      bool has_dimension(const std::string & dimension_label_) const;

      //! Return a registered unit dimension
      const unit_dimension & get_dimension(const std::string & dimension_label_) const;

      //! Clear the registry
      void clear();

      //! Return the non mutable dictionary of registered units
      const unit_dict_type & get_units() const;

      //! Return the non mutable dictionary of registered unit symbols
      const symbol_dict_type & get_symbols() const;

      //! Return the non mutable dictionary of registered unit dimensions
      const dimension_dict_type & get_dimensions() const;

      //! Build the list of unit symbols associated to a given dimension with units ordered by values (i.e. for the "time" dimension: "fs", "ps", "ns", "us", "ms", "s", ..., "year", "century"... "eon" ...)
      bool build_ordered_unit_symbols(const std::string & dimension_label_,
                                      std::list<std::string> & symbols_) const;

      //! Register standard units
      void register_standard_units();

      //! Smart print
      void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const override;

    protected:

      //! Return a mutable registered unit
      unit & _grab_unit(const std::string & unit_name_);

      //! Return a mutable registered unit dimension
      unit_dimension & _grab_dimension(const std::string & dimension_label_);

    private:

      unit_dict_type      _units_;      //!< Dictionary of units keyed by name
      symbol_dict_type    _symbols_;    //!< Dictionary of units keyed by unit symbol
      dimension_dict_type _dimensions_; //!< Dictionary of unit dimensions keyed by label

    public:

      //! Access to the mutable system singleton registry
      static registry & system_registry();

      //! Access to the non mutable system singleton registry
      static const registry & const_system_registry();

    private:

      //! Private access to the mutable system singleton registry
      static registry & _access_system_registry_();

    };

  } // end of namespace units

} // end of namespace datatools

#ifndef Q_MOC_RUN
// Activate reflection layer:
DR_TYPE_INIT(::datatools::units::power_of_ten)
#endif // Q_MOC_RUN

//! \def DT_THROW_BAD_UNIT(UnitType,UnitSymbol)
//! \brief Throw a special exception when \c UnitSymbol does not match \c UnitType.
#define DT_THROW_BAD_UNIT(UnitType,UnitSymbol)                          \
  DT_THROW_IF(true,std::logic_error,                                    \
              "Invalid '" << UnitType << "' unit for symbol '" << UnitSymbol << "' !")
/**/

#endif // DATATOOLS_UNITS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
