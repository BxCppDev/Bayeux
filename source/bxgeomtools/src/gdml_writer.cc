// gdml_writer.cc

// Standard library:
#include <stdexcept>
#include <set>

// Third party:
// - Boost:
#include <boost/algorithm/string.hpp>
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/units.h>
#include <datatools/exception.h>

// This project:
#include <geomtools/geomtools_config.h>
#include <geomtools/gdml_writer.h>

#include <geomtools/box.h>
#include <geomtools/right_circular_conical_frustrum.h>
#include <geomtools/ellipsoid.h>
#include <geomtools/elliptical_cylinder.h>
#include <geomtools/sphere.h>
#include <geomtools/polycone.h>
#include <geomtools/polyhedra.h>
#include <geomtools/right_polygonal_frustrum.h>
#include <geomtools/cylinder.h>
#include <geomtools/tube.h>
#include <geomtools/tessellation.h>
#include <geomtools/union_3d.h>
#include <geomtools/subtraction_3d.h>
#include <geomtools/intersection_3d.h>

namespace geomtools {

  // using namespace std;

  // statics
  const std::string & gdml_writer::default_xml_version()
  {
    static std::string label;
    if (label.empty()) {
      label = "1.0";
    }
    return label;
  }

  const std::string & gdml_writer::default_xml_encoding()
  {
    static std::string label;
    if (label.empty()) {
      label = "UTF-8";
    }
    return label;
  }

  const std::string & gdml_writer::default_xsi()
  {
    static std::string label;
    if (label.empty()) {
      label = "http://www.w3.org/2001/XMLSchema-instance";
    }
    return label;
  }

  const std::string & gdml_writer::default_gdml_schema()
  {
    static std::string label;
    if (label.empty()) {
      label = "gdml.xsd";
    }
    return label;
  }

  const std::string & gdml_writer::default_remote_gdml_schema()
  {
    static std::string label;
    if (label.empty()) {
      label = "http://service-spi.web.cern.ch/service-spi/app/releases/GDML/schema/gdml.xsd";
    }
    return label;
  }

  const std::string & gdml_writer::define_section()
  {
    static std::string label;
    if (label.empty()) {
      label = "define";
    }
    return label;
  }

  const std::string & gdml_writer::materials_section()
  {
    static std::string label;
    if (label.empty()) {
      label = "materials";
    }
    return label;
  }

  const std::string & gdml_writer::solids_section()
  {
    static std::string label;
    if (label.empty()) {
      label = "solids";
    }
    return label;
  }

  const std::string & gdml_writer::structure_section()
  {
    static std::string label;
    if (label.empty()) {
      label = "structure";
    }
    return label;
  }

  const std::string & gdml_writer::setup_section()
  {
    static std::string label;
    if (label.empty()) {
      label = "setup";
    }
    return label;
  }

  // static
  const std::string & gdml_writer::state_convert(::materials::material::state_type state_)
  {
    static std::string _solid_label("solid");
    static std::string _liquid_label("liquid");
    static std::string _gas_label("gas");
    static std::string _unknown_label("unknown");
    // See file: gdml_schema/gdml_materials.xsd
    switch(state_) {
    case ::materials::material::STATE_SOLID: return _solid_label;
    case ::materials::material::STATE_LIQUID: return _liquid_label;
    case ::materials::material::STATE_GAS: return _gas_label;
    default: return _unknown_label;
    }
  }

  void gdml_writer::set_using_html_symbols(bool u_)
  {
    _using_html_symbols_ = u_;
    return;
  }

  bool gdml_writer::is_using_html_symbols() const
  {
    return _using_html_symbols_;
  }

  std::string gdml_writer::to_html(const std::string & name_,
                              bool using_html_symbols_)
  {
    std::string str1 = name_;
    if (using_html_symbols_) {
      boost::replace_all(str1, "." , "&#46;");
      boost::replace_all(str1, "[" , "&#91;");
      boost::replace_all(str1, "]" , "&#93;");
      boost::replace_all(str1, "{" , "&#123;");
      boost::replace_all(str1, "}" , "&#124;");
    }
    boost::replace_all(str1, "::" , "__");
    return str1;
  }

  std::string gdml_writer::to_ascii(const std::string & name_,
                               bool using_html_symbols_)
  {
    std::string str1 = name_;
    if (using_html_symbols_) {
      boost::replace_all(str1 , "&#46;"  , ".");
      boost::replace_all(str1 , "&#91;"  , "[");
      boost::replace_all(str1 , "&#93;"  , "]");
      boost::replace_all(str1 , "&#123;" , "{");
      boost::replace_all(str1 , "&#124;" , "}");
    }
    boost::replace_all(str1, "__" , "::");
    return str1;
  }

  const std::ostringstream & gdml_writer::get_stream(const std::string & section_) const
  {
    return _get_stream(section_);
  }

  const std::ostringstream & gdml_writer::_get_stream(const std::string & section_) const
  {
    streams_col_type::const_iterator i = _streams_.find(section_);
    DT_THROW_IF (i == _streams_.end(),
                 std::logic_error, "Invalid section '" << section_ << "' !");
    return *(i->second);
  }

  std::ostringstream & gdml_writer::_get_stream(const std::string & section_)
  {
    streams_col_type::iterator i = _streams_.find(section_);
    DT_THROW_IF (i == _streams_.end(),
                 std::logic_error, "Invalid section '" << section_ << "' !");
    return *(i->second);
  }

  bool gdml_writer::has_external_materials_stream() const
  {
    return _external_materials_stream_ != 0;
  }

  void gdml_writer::reset_external_materials_stream()
  {
    _external_materials_stream_ = 0;
    return;
  }

  bool gdml_writer::is_initialized() const
  {
    return _initialized_;
  }

  bool gdml_writer::is_verbose() const
  {
    return _verbose_;
  }

  void gdml_writer::set_verbose(bool v_)
  {
    _verbose_ = v_;
    return;
  }

  gdml_writer::gdml_writer()
  {
    _initialized_ = false;
    _verbose_ = false;
    _external_materials_stream_ = 0;
    _using_html_symbols_ = false;
    initialize();
    return;
  }

  gdml_writer::~gdml_writer()
  {
    if (_initialized_) {
      reset();
    }
    return;
  }

  void gdml_writer::initialize()
  {
    this->init();
    return;
  }

  void gdml_writer::_allocate_streams()
  {
    _streams_[define_section()]    = new std::ostringstream;
    _streams_[materials_section()] = new std::ostringstream;
    _streams_[solids_section()]    = new std::ostringstream;
    _streams_[structure_section()] = new std::ostringstream;
    _streams_[setup_section()]     = new std::ostringstream;
    return;
  }

  void gdml_writer::_free_streams()
  {
    if (_streams_.size()) {
      for(streams_col_type::iterator i = _streams_.begin();
          i != _streams_.end();
          i++) {
        delete i->second;
      }
      _streams_.clear();
    }
    return;
  }

  void gdml_writer::init()
  {
    // if (_initialized_) {
    //     if (is_verbose()) clog << "WARNING: gdml_writer::init: Already initialized !" << std::endl;
    //   }
    // DT_THROW_IF (is_initialized(), std::logic_error,
    //             "Already initialized !");
    if (_streams_.size() == 0) {
      _allocate_streams();
    }
    _initialized_ = true;
    return;
  }

  void gdml_writer::reset()
  {
    // DT_THROW_IF (! is_initialized(), std::logic_error,
    //             "Not initialized !");
    // if (! _initialized_) {
    //   if (is_verbose()) clog << "WARNING: gdml_writer::reset: Not initialized !" << std::endl;
    //   return;
    // }
    if (_streams_.size() > 0) {
      _free_streams();
    }
    reset_external_materials_stream();
    _initialized_ = false;
    return;
  }

  /*  Defines  */

  void gdml_writer::add_constant(const std::string & name_,
                                 double value_)
  {
    _get_stream(define_section()) << "<constant"
                                  << " name=" << '"' << to_html(name_) << '"'
                                  << " value="  << '"';
    _get_stream(define_section()).precision(15);
    _get_stream(define_section()) << value_ << '"';
    _get_stream(define_section()) << " />" << std::endl;
    _get_stream(define_section()) << std::endl;
    return;
  }

  void gdml_writer::add_quantity(const std::string & name_,
                                 const std::string & quantity_type_,
                                 const std::string & unit_str_,
                                 double value_)
  {
    double unit = datatools::units::get_unit_from(quantity_type_, unit_str_);
    _get_stream(define_section()) << "<quantity"
                                  << " name=" << '"' << to_html(name_) << '"'
                                  << " type=" << '"' << quantity_type_ << '"'
                                  << " value="  << '"';
    _get_stream(define_section()).precision(15);
    _get_stream(define_section()) <<(value_ / unit) << '"'
                                  << " unit=" << '"' << unit_str_ << '"'
                                  << " />" << std::endl;
    _get_stream(define_section()) << std::endl;
    return;
  }

  void gdml_writer::add_variable(const std::string & name_,
                                 double value_)
  {
    _get_stream(define_section()) << "<variable"
                                  << " name=" << '"' << to_html(name_) << '"'
                                  << " value="  << '"';
    _get_stream(define_section()).precision(15);
    _get_stream(define_section()) << value_ << '"'
                                  << " />" << std::endl;
    _get_stream(define_section()) << std::endl;
    return;
  }

  void gdml_writer::add_variable(const std::string & name_,
                                 const std::string & expr_value_)
  {
    _get_stream(define_section()) << "<variable"
                                  << " name=" << '"' << to_html(name_) << '"'
                                  << " value="  << '"' << expr_value_ << '"'
                                  << " />" << std::endl;
    _get_stream(define_section()) << std::endl;
    return;
  }

  void gdml_writer::add_position(const std::string & name_,
                                 double x_, double y_, double z_,
                                 const std::string & unit_str_)
  {
    //clog << "DEVEL: gdml_writer::add_position: Entering..." << std::endl;
    double unit = datatools::units::get_length_unit_from(unit_str_);
    _get_stream(define_section()) << "<position"
                                  << " name=" << '"' << to_html(name_) << '"'
                                  << " x=" << '"';
    _get_stream(define_section()).precision(15);
    _get_stream(define_section()) << x_ / unit << '"'
                                  << " y=" << '"';
    _get_stream(define_section()).precision(15);
    _get_stream(define_section()) << y_ / unit << '"'
                                  << " z=" << '"';
    _get_stream(define_section()).precision(15);
    _get_stream(define_section()) << z_ / unit << '"'
                                  << " unit=" << '"' << unit_str_ << '"'
                                  << " />" << std::endl;
    _get_stream(define_section()) << std::endl;
    return;
  }

  void gdml_writer::add_position(const std::string & name_,
                                 const vector_3d & v_,
                                 const std::string & unit_str_)
  {
    add_position(name_, v_.x(), v_.y(), v_.z(), unit_str_);
    return;
  }

  void gdml_writer::add_rotation(const std::string & name_,
                                 const std::string & axis_,
                                 double angle_,
                                 const std::string & unit_str_)
  {
    double angle_unit = datatools::units::get_angle_unit_from(unit_str_);
    DT_THROW_IF ((axis_ != "x") &&(axis_ != "y") &&(axis_ != "z"),
                 std::logic_error,
                 "Invalid rotation axis '" << axis_ << "' for rotation '" << name_ << "' !");
    _get_stream(define_section()) << "<rotation"
                                  << " name=" << '"' << to_html(name_) << '"'
                                  << " " << axis_ << "=" << '"';
    _get_stream(define_section()).precision(15);
    _get_stream(define_section()) <<(angle_ / angle_unit) << '"'
                                  << " unit=" << '"' << unit_str_ << '"'
                                  << " />" << std::endl;
    _get_stream(define_section()) << std::endl;
    return;
  }

  void gdml_writer::add_rotation(const std::string & name_,
                                 const rotation_3d & rot_,
                                 const std::string & unit_str_)
  {
    double angle_unit = datatools::units::get_angle_unit_from(unit_str_);
    _get_stream(define_section()) << "<rotation"
                                  << " name=" << '"' << to_html(name_)
                                  << '"';

    double a, b, c;
    extract_xyz_euler_angle_from_rotation(rot_, a, b, c);

    _get_stream(define_section()) << " " << "x" << "=" << '"';
    _get_stream(define_section()).precision(15);
    _get_stream(define_section()) <<(-a / angle_unit) << '"';

    _get_stream(define_section()) << " " << "y" << "=" << '"';
    _get_stream(define_section()).precision(15);
    _get_stream(define_section()) <<(-b / angle_unit) << '"';

    _get_stream(define_section()) << " " << "z" << "=" << '"';
    _get_stream(define_section()).precision(15);
    _get_stream(define_section()) <<(-c / angle_unit) << '"';

    _get_stream(define_section()) << " unit=" << '"' << unit_str_ << '"'
                                  << " />" << std::endl;
    _get_stream(define_section()) << std::endl;
    return;
  }

  void gdml_writer::add_scale(const std::string & name_,
                              double x_, double y_, double z_)
  {
    _get_stream(define_section()) << "<scale"
                                  << " name=" << '"' << to_html(name_)
                                  << '"';

    _get_stream(define_section()) << " " << "x" << "=" << '"';
    _get_stream(define_section()).precision(15);
    _get_stream(define_section()) << x_ << '"';

    _get_stream(define_section()) << " " << "y" << "=" << '"';
    _get_stream(define_section()).precision(15);
    _get_stream(define_section()) << y_ << '"';

    _get_stream(define_section()) << " " << "z" << "=" << '"';
    _get_stream(define_section()).precision(15);
    _get_stream(define_section()) << z_ << '"';

    _get_stream(define_section()) << " />" << std::endl;

    return;
  }

  /*  Materials */

  void gdml_writer::add_isotope(const std::string & name_,
                                size_t atomic_number_,
                                size_t number_of_nucleons_,
                                double a_)
  {
    _get_stream(materials_section()) << "<isotope"
                                     << " name=" << '"' << to_html(name_) << '"'
                                     << " Z=" << '"' << atomic_number_ << '"'
                                     << " N=" << '"' << number_of_nucleons_ << '"'
                                     << " >" << std::endl;
    _get_stream(materials_section()) << "  <atom"
                                     << " type=" << '"' << "A" << '"'
                                     << " value=" << '"';
    _get_stream(materials_section()).precision(15);
    _get_stream(materials_section()) << a_ << '"' << " />" << std::endl;
    // 2010-10-24 FM: the " />" termination std::string does not appear in the GDML user manual !
    _get_stream(materials_section()) << "</isotope>" << std::endl;
    _get_stream(materials_section()) << std::endl;
  }

  void gdml_writer::add_element(const std::string & name_,
                                size_t atomic_number_,
                                const std::string & formula_,
                                double a_)
  {
    _get_stream(materials_section()) << "<element"
                                     << " name=" << '"' << to_html(name_) << '"'
                                     << " Z=" << '"' << atomic_number_ << '"'
                                     << " formula=" << '"' << formula_ << '"'
                                     << " >" << std::endl;
    _get_stream(materials_section()) << "  <atom"
                                     << " value=" << '"';
    _get_stream(materials_section()).precision(15);
    _get_stream(materials_section()) << a_ << '"' << " />" << std::endl;
    _get_stream(materials_section()) << "</element>" << std::endl;
    _get_stream(materials_section()) << std::endl;
  }

  void gdml_writer::add_element(const std::string & name_,
                                const std::map<std::string, double> & fractions_)
  {
    std::ostringstream materials_stream;
    materials_stream << "<element"
                     << " name=" << '"' << to_html(name_) << '"'
                     << " >" << std::endl;
    double s = 0.0;
    for(std::map<std::string, double>::const_iterator i = fractions_.begin();
        i != fractions_.end();
        i++) {
      std::string ref = i->first;
      double n = i->second;
      DT_THROW_IF (n < 0.0 || n > 1.0, std::logic_error,
                   "Invalid fraction value '" << n << "' for referenced '" << ref << "' in element '"
                   << name_ << "' !");
      s += n;
      materials_stream << "  <fraction"
                       << " ref=" << '"' << to_html(ref) << '"'
                       << " n=" << '"';
      materials_stream.precision(15);
      materials_stream << n << '"' << " />" << std::endl;
    }
    DT_THROW_IF (s > 1.000000000001, std::logic_error,
                 "Invalid fraction sum in element '" << name_ << "' !");
    materials_stream << "</element>" << std::endl;
    _get_stream(materials_section()) << materials_stream.str();
    _get_stream(materials_section()) << std::endl;
  }

  void gdml_writer::add_element(const std::string & name_,
                                const std::string & ref_)
  {
    std::map<std::string, double> m;
    m[ref_] = 1.0;
    add_element(name_, m);
  }

  void gdml_writer::add_element(const std::string & name_,
                                const std::string & ref1_, double fraction1_,
                                const std::string & ref2_, double fraction2_)
  {
    std::map<std::string, double> m;
    m[ref1_] = fraction1_;
    m[ref2_] = fraction2_;
    add_element(name_, m);
  }

  void gdml_writer::add_element(const std::string & name_,
                                const std::string & ref1_, double fraction1_,
                                const std::string & ref2_, double fraction2_,
                                const std::string & ref3_, double fraction3_)
  {
    std::map<std::string, double> m;
    m[ref1_] = fraction1_;
    m[ref2_] = fraction2_;
    m[ref3_] = fraction3_;
    add_element(name_, m);
  }

  void gdml_writer::add_element(const std::string & name_,
                                const std::string & ref1_, double fraction1_,
                                const std::string & ref2_, double fraction2_,
                                const std::string & ref3_, double fraction3_,
                                const std::string & ref4_, double fraction4_)
  {
    std::map<std::string, double> m;
    m[ref1_] = fraction1_;
    m[ref2_] = fraction2_;
    m[ref3_] = fraction3_;
    m[ref4_] = fraction4_;
    add_element(name_, m);
    return;
  }

  void gdml_writer::add_element(const std::string & name_,
                                const std::string & ref1_, double fraction1_,
                                const std::string & ref2_, double fraction2_,
                                const std::string & ref3_, double fraction3_,
                                const std::string & ref4_, double fraction4_,
                                const std::string & ref5_, double fraction5_)
  {
    std::map<std::string, double> m;
    m[ref1_] = fraction1_;
    m[ref2_] = fraction2_;
    m[ref3_] = fraction3_;
    m[ref4_] = fraction4_;
    m[ref5_] = fraction5_;
    add_element(name_, m);
    return;
  }

  void gdml_writer::add_material(const std::string & name_,
                                 double atomic_number_,
                                 double density_,
                                 double a_,
                                 const std::string & state_label_,
                                 double /*temperature_*/,
                                 double /*pressure_*/
                                 )
  {
    std::ostringstream materials_stream;
    materials_stream << "<material"
                     << " name=" << '"' << to_html(name_) << '"'
                     << " Z=" << '"';
    materials_stream.precision(15);
    materials_stream << atomic_number_ << '"';

    if (!state_label_.empty()) {
      materials_stream << " state=" << '"' << state_label_ << '"';
    }

    materials_stream << " >" << std::endl;

    materials_stream << "  <D"
                     << " value=" << '"';
    materials_stream.precision(15);
    materials_stream <<(density_ /(CLHEP::g / CLHEP::cm3))
                     << '"'
                     << " unit=" << '"' << "g/cm3" << '"'
                     << " />" << std::endl;

    //>>> Add additional parameters(state/temperature/pressure):
    // see Geant4 source: ./persistency/gdml/src/G4GDMLReadMaterials.cc
    /*
    if (datatools::is_valid(temperature_)) {
      materials_stream << "  <T"
                       << " value=" << '"';
      materials_stream.precision(15);
      materials_stream <<(temperature_ / CLHEP::kelvin)
                       << '"'
                       << " unit=" << '"' << "K" << '"'
                       << " />" << std::endl;
    }
    if (datatools::is_valid(temperature_)) {
      materials_stream << "  <P"
                       << " value=" << '"';
      materials_stream.precision(15);
      materials_stream <<(pressure_ / CLHEP::pascal)
                       << '"'
                       << " unit=" << '"' << "Pa" << '"'
                       << " />" << std::endl;
    }
    */
    //<<<

    materials_stream << "  <atom"
                     << " value=" << '"';
    materials_stream.precision(15);
    materials_stream << a_ << '"' << " />" << std::endl;

    materials_stream << "</material >" << std::endl;
    materials_stream << std::endl;

    _get_stream(materials_section()) << materials_stream.str();
    _get_stream(materials_section()) << std::endl;
    return;
  }

  void gdml_writer::add_material(const std::string & name_,
                                 const std::string & formula_,
                                 double density_,
                                 const std::map<std::string, double> & fractions_,
                                 const std::string & state_label_,
                                 double /*temperature_*/,
                                 double /*pressure_*/
                                 )
  {
    std::ostringstream materials_stream;
    materials_stream << "<material"
                     << " name=" << '"' << to_html(name_) << '"';

    if (!formula_.empty()) {
      materials_stream << " formula=" << '"' << formula_ << '"';
    }

    if (!state_label_.empty()) {
      materials_stream << " state=" << '"' << state_label_ << '"';
    }

    materials_stream << " >" << std::endl;

    materials_stream << "  <D"
                     << " value=" << '"';
    materials_stream.precision(15);
    materials_stream <<(density_ /(CLHEP::g / CLHEP::cm3))
                     << '"'
                     << " unit=" << '"' << "g/cm3" << '"'
                     << " />" << std::endl;

    //>>> Add additional parameters(state/temperature/pressure):
    // see Geant4 source: ./persistency/gdml/src/G4GDMLReadMaterials.cc
    /*
    if (datatools::is_valid(temperature_)) {
      materials_stream << "  <T"
                       << " value=" << '"';
      materials_stream.precision(15);
      materials_stream <<(temperature_ / CLHEP::kelvin)
                       << '"'
                       << " unit=" << '"' << "K" << '"'
                       << " />" << std::endl;
    }
    if (datatools::is_valid(temperature_)) {
      materials_stream << "  <P"
                       << " value=" << '"';
      materials_stream.precision(15);
      materials_stream <<(pressure_ / CLHEP::pascal)
                       << '"'
                       << " unit=" << '"' << "Pa" << '"'
                       << " />" << std::endl;
    }
    */
    //<<<

    double s = 0.0;
    for(std::map<std::string, double>::const_iterator i = fractions_.begin();
        i != fractions_.end();
        i++)
      {
        std::string ref = i->first;
        double n = i->second;
        DT_THROW_IF (n < 0.0 || n > 1.0, std::logic_error,
                     "Invalid fraction value '" << n
                     << "' for referenced '" << ref << "' in element '"
                     << name_ << "' !");
        s += n;

        materials_stream << "  <fraction"
                         << " ref=" << '"' << to_html(ref) << '"'
                         << " n=" << '"';
        materials_stream.precision(15);
        materials_stream << n << '"' << " />" << std::endl;
      }
    DT_THROW_IF (s > 1.000000000001, std::logic_error,
                 "Invalid fraction sum in material '" << name_ << "' !");
    materials_stream << "</material>" << std::endl;
    _get_stream(materials_section()) << materials_stream.str();
    _get_stream(materials_section()) << std::endl;
  }


  void gdml_writer::add_material(const std::string & name_,
                                 const std::string & formula_,
                                 double density_,
                                 const std::map<std::string, size_t> & composites_,
                                 const std::string & state_label_,
                                 double /*temperature_*/,
                                 double /*pressure_*/
                                 )
  {
    std::ostringstream materials_stream;
    materials_stream << "<material"
                     << " name=" << '"' << to_html(name_) << '"';

    if (!formula_.empty()) {
      materials_stream << " formula=" << '"' << formula_ << '"';
    }

    if (!state_label_.empty()) {
      materials_stream << " state=" << '"' << state_label_ << '"';
    }

    materials_stream << " >" << std::endl;

    materials_stream << "  <D"
                     << " value=" << '"';
    materials_stream.precision(15);
    materials_stream <<(density_ /(CLHEP::g / CLHEP::cm3))
                     << '"'
                     << " unit=" << '"' << "g/cm3" << '"'
                     << " />" << std::endl;

    //>>> Add additional parameters(state/temperature/pressure):
    // see Geant4 source: ./persistency/gdml/src/G4GDMLReadMaterials.cc
    /*
    if (datatools::is_valid(temperature_)) {
      materials_stream << "  <T"
                       << " value=" << '"';
      materials_stream.precision(15);
      materials_stream <<(temperature_ / CLHEP::kelvin)
                       << '"'
                       << " unit=" << '"' << "K" << '"'
                       << " />" << std::endl;
    }
    if (datatools::is_valid(temperature_)) {
      materials_stream << "  <P"
                       << " value=" << '"';
      materials_stream.precision(15);
      materials_stream <<(pressure_ / CLHEP::pascal)
                       << '"'
                       << " unit=" << '"' << "Pa" << '"'
                       << " />" << std::endl;
    }
    */
    //<<<

    for(std::map<std::string, size_t>::const_iterator i = composites_.begin();
        i != composites_.end();
        i++)
      {
        std::string ref = i->first;
        size_t n = i->second;
        DT_THROW_IF (n == 0, std::logic_error,
                     "Invalid composite value '" << n
                     << "' for referenced '" << ref << "' in element '"
                     << name_ << "' !");
        materials_stream << "  <composite"
                         << " ref=" << '"' << to_html(ref) << '"'
                         << " n=" << '"' << n << '"' << " />" << std::endl;;
      }
    materials_stream << "</material>" << std::endl;
    _get_stream(materials_section()) << materials_stream.str();
    _get_stream(materials_section()) << std::endl;
  }

  /*  solid */

  bool gdml_writer::solid_type_is_supported(const std::string & solid_type_)
  {
    boost::scoped_ptr<std::set<std::string> > _supported_solid_types;
    if (!_supported_solid_types) {
      _supported_solid_types.reset(new std::set<std::string>);
      _supported_solid_types->insert("box");
      _supported_solid_types->insert("sphere");
      _supported_solid_types->insert("tube");
      _supported_solid_types->insert("cylinder");
      _supported_solid_types->insert("right_circular_conical_frustrum");
      _supported_solid_types->insert("ellipsoid");
      _supported_solid_types->insert("elliptical_cylinder");
      _supported_solid_types->insert("polycone");
      // _supported_solid_types->insert("right_polygonal_frustrum");
      _supported_solid_types->insert("polyhedra");
      _supported_solid_types->insert("tessellated");
      _supported_solid_types->insert("union_3d");
      _supported_solid_types->insert("subtraction_3d");
      _supported_solid_types->insert("intersection_3d");
    }
    return _supported_solid_types->count(solid_type_) == 1;
  }

  bool gdml_writer::solid_type_is_valid(const std::string & /*solid_type_*/)
  {
    // Not implemented yet
    return true;
  }

  void gdml_writer::add_gdml_box(const std::string & name_,
                                 double x_, double y_, double z_,
                                 const std::string & lunit_str_)
  {
    double lunit = datatools::units::get_length_unit_from(lunit_str_);

    std::ostringstream solids_stream;
    solids_stream << "<" <<  "box"
                  << " name=" << '"' << to_html(name_) << '"';

    solids_stream << " x=" << '"';
    solids_stream.precision(15);
    solids_stream << x_ / lunit << '"';

    solids_stream << " y=" << '"';
    solids_stream.precision(15);
    solids_stream << y_ / lunit << '"';

    solids_stream << " z=" << '"';
    solids_stream.precision(15);
    solids_stream << z_ / lunit << '"';

    solids_stream << " lunit=" << '"' << lunit_str_ << '"';
    solids_stream << " />" << std::endl << std::endl;

    _get_stream(solids_section()) << solids_stream.str();
   return;
  }

  void gdml_writer::add_box(const std::string & name_,
                            const box & b_,
                            const std::string & lunit_str_)
  {
    add_gdml_box(name_, b_.get_x(), b_.get_y(), b_.get_z(), lunit_str_);
   return;
  }

  void gdml_writer::add_gdml_cone_segment(const std::string & name_,
                                          double rmin1_, double rmax1_,
                                          double rmin2_, double rmax2_,
                                          double z_,
                                          double start_phi_, double delta_phi_,
                                          const std::string & lunit_str_,
                                          const std::string & aunit_str_)
  {
    double lunit = datatools::units::get_length_unit_from(lunit_str_);
    double aunit = datatools::units::get_angle_unit_from(aunit_str_);

    std::ostringstream solids_stream;
    solids_stream << "<" <<  "cone"
                  << " name=" << '"' << to_html(name_) << '"';

    if (datatools::is_valid(rmin1_) && rmin1_ >= 0.0) {
      solids_stream << " rmin1=" << '"';
      solids_stream.precision(15);
      solids_stream << rmin1_ / lunit << '"';
    }

    solids_stream << " rmax1=" << '"';
    solids_stream.precision(15);
    solids_stream << rmax1_ / lunit << '"';

    if (datatools::is_valid(rmin2_) && rmin2_ >= 0.0) {
      solids_stream << " rmin2=" << '"';
      solids_stream.precision(15);
      solids_stream << rmin2_ / lunit << '"';
    }

    solids_stream << " rmax2=" << '"';
    solids_stream.precision(15);
    solids_stream << rmax2_ / lunit << '"';

    solids_stream << " z=" << '"';
    solids_stream.precision(15);
    solids_stream << z_ / lunit << '"';

    double startphi = 0.0;
    double deltaphi = 2 * M_PI;
    if (datatools::is_valid(start_phi_) && start_phi_ >= 0.0) {
      startphi = start_phi_;
    }
    solids_stream << " startphi=" << '"';
    solids_stream.precision(15);
    solids_stream << startphi / aunit << '"';

    if (datatools::is_valid(delta_phi_)) {
      DT_THROW_IF(delta_phi_ == 0.0, std::domain_error,
                  "Invalid zero delta phi!");
      DT_THROW_IF(startphi + delta_phi_ > 2 * M_PI, std::domain_error,
                  "Delta phi is too large!");
      deltaphi = delta_phi_;
    }
    solids_stream << " deltaphi=" << '"';
    solids_stream.precision(15);
    solids_stream << deltaphi / aunit << '"';

    solids_stream << " aunit=" << '"' << aunit_str_ << '"';
    solids_stream << " lunit=" << '"' << lunit_str_ << '"';

    solids_stream << " />" << std::endl << std::endl;

   _get_stream(solids_section()) << solids_stream.str();
    return;
  }


  void gdml_writer::add_cone_segment(const std::string & name_,
                                     const right_circular_conical_frustrum & cf_,
                                     const std::string & lunit_str_,
                                     const std::string & aunit_str_)
  {
    add_gdml_cone_segment(name_,
                          cf_.get_inner_bottom_radius(),
                          cf_.get_outer_bottom_radius(),
                          cf_.get_inner_top_radius(),
                          cf_.get_outer_top_radius(),
                          cf_.get_z(),
                          cf_.get_start_angle(),
                          cf_.get_delta_angle(),
                          lunit_str_, aunit_str_);
   return;
  }

  void gdml_writer::add_gdml_ellipsoid(const std::string & name_,
                                       double ax_, double by_, double cz_,
                                       double bottom_z_, double top_z_,
                                       const std::string & lunit_str_,
                                       const std::string & /*aunit_str_*/)
  {
    double lunit = datatools::units::get_length_unit_from(lunit_str_);
    //double aunit = datatools::units::get_angle_unit_from(aunit_str_);

    std::ostringstream solids_stream;
    solids_stream << "<" <<  "ellipsoid"
                  << " name=" << '"' << to_html(name_) << '"';

    solids_stream << " ax=" << '"';
    solids_stream.precision(15);
    solids_stream << ax_ / lunit << '"';

    solids_stream << " by=" << '"';
    solids_stream.precision(15);
    solids_stream << by_ / lunit << '"';

    solids_stream << " cz=" << '"';
    solids_stream.precision(15);
    solids_stream << cz_ / lunit << '"';

    if (datatools::is_valid(bottom_z_)) {
      solids_stream << " zcut1=" << '"';
      solids_stream.precision(15);
      solids_stream << (-cz_ + bottom_z_) / lunit << '"';
    }

    if (datatools::is_valid(top_z_)) {
      solids_stream << " zcut2=" << '"';
      solids_stream.precision(15);
      solids_stream << (cz_ - top_z_) / lunit << '"';
    }

    solids_stream << " lunit=" << '"' << lunit_str_ << '"';

    solids_stream << " />" << std::endl << std::endl;
    _get_stream(solids_section()) << solids_stream.str();
   return;
  }

  void gdml_writer::add_ellipsoid(const std::string & name_,
                                  const ellipsoid & e_,
                                  const std::string & lunit_str_,
                                  const std::string & aunit_str_)
  {
    add_gdml_ellipsoid(name_,
                       e_.get_x_radius(),
                       e_.get_y_radius(),
                       e_.get_z_radius(),
                       e_.get_bottom_z_cut(),
                       e_.get_top_z_cut(),
                       lunit_str_,
                       aunit_str_);
   return;
  }

  void gdml_writer::add_gdml_elliptical_tube(const std::string & name_,
                                             double dx_, double dy_, double dz_,
                                             const std::string & lunit_str_,
                                             const std::string & /*aunit_str_*/)
  {
    double lunit = datatools::units::get_length_unit_from(lunit_str_);
    //double aunit = datatools::units::get_angle_unit_from(aunit_str_);

    std::ostringstream solids_stream;
    solids_stream << "<" <<  "eltube"
                  << " name=" << '"' << to_html(name_) << '"';

    solids_stream << " dx=" << '"';
    solids_stream.precision(15);
    solids_stream << dx_ / lunit << '"';

    solids_stream << " dy=" << '"';
    solids_stream.precision(15);
    solids_stream << dy_ / lunit << '"';

    solids_stream << " dz=" << '"';
    solids_stream.precision(15);
    solids_stream << dz_ / lunit << '"';

    solids_stream << " lunit=" << '"' << lunit_str_ << '"';
    // solids_stream << " aunit=" << '"' << aunit_str_ << '"';

    solids_stream << " />" << std::endl << std::endl;
    _get_stream(solids_section()) << solids_stream.str();
   return;
  }

  void gdml_writer::add_elliptical_tube(const std::string & name_,
                                        const elliptical_cylinder & t_,
                                        const std::string & lunit_str_,
                                        const std::string & aunit_str_)
  {
    add_gdml_elliptical_tube(name_,
                             t_.get_x_radius(),
                             t_.get_y_radius(),
                             0.5 * t_.get_z(),
                             lunit_str_,
                             aunit_str_);
   return;
  }

  void gdml_writer::add_gdml_orb(const std::string & name_,
                                 double r_,
                                 const std::string & lunit_str_)
  {
    double lunit = datatools::units::get_length_unit_from(lunit_str_);

    std::ostringstream solids_stream;
    solids_stream << "<" <<  "orb"
                  << " name=" << '"' << to_html(name_) << '"';

    solids_stream << " r=" << '"';
    solids_stream.precision(15);
    solids_stream << r_ / lunit << '"';

    solids_stream << " lunit=" << '"' << lunit_str_ << '"';

    solids_stream << " />" << std::endl << std::endl;
    _get_stream(solids_section()) << solids_stream.str();
   return;
  }


  void gdml_writer::add_orb(const std::string & name_,
                            const sphere & s_,
                            const std::string & lunit_str_,
                            const std::string & /*aunit_str_*/)
  {
    add_gdml_orb(name_, s_.get_r(), lunit_str_);
    return;
  }

  /*
   * <polycone name = "thepolycone"
   *  startphi="1" deltaphi="4" aunit="rad" lunit= "mm">
   *    <zplane rmin="1" rmax="9" z="10" />
   *    <zplane rmin="3" rmax="5" z="12" />
   * </polycone>
   */
  void gdml_writer::add_gdml_polycone(const std::string & name_,
                                      std::map<double, std::pair<double, double> > zplanes_,
                                      double start_phi_, double delta_phi_,
                                      const std::string & lunit_str_,
                                      const std::string & aunit_str_)
  {
    double lunit = datatools::units::get_length_unit_from(lunit_str_);
    double aunit = datatools::units::get_angle_unit_from(aunit_str_);

    std::ostringstream solids_stream;
    solids_stream << "<" <<  "polycone"
                  << " name=" << '"' << to_html(name_) << '"';

    double startphi = 0.0;
    double deltaphi = 2 * M_PI;
    if (datatools::is_valid(start_phi_) && start_phi_ >= 0.0) {
      startphi = start_phi_;
    }
    solids_stream << " startphi=" << '"';
    solids_stream.precision(15);
    solids_stream << startphi / aunit << '"';

    if (datatools::is_valid(delta_phi_)) {
      DT_THROW_IF(delta_phi_ == 0.0, std::domain_error,
                  "Invalid zero delta phi!");
      DT_THROW_IF(startphi + delta_phi_ > 2 * M_PI, std::domain_error,
                  "Delta theta is too large!");
      deltaphi = delta_phi_;
    }
    solids_stream << " deltaphi=" << '"';
    solids_stream.precision(15);
    solids_stream << deltaphi / aunit << '"';

    solids_stream << " lunit=" << '"' << lunit_str_ << '"';
    solids_stream << " aunit=" << '"' << aunit_str_ << '"';

    solids_stream << " >" << std::endl << std::endl;

    for (std::map<double, std::pair<double, double> >::const_iterator i = zplanes_.begin();
        i != zplanes_.end();
        i++) {
        double z = i->first;
        double rmin = i->second.first;
        double rmax = i->second.second;
        solids_stream << "  " << "<zplane ";
        if (rmin != 0.0) {
          solids_stream << "rmin=\"" << rmin / lunit << "\" ";
        } else {
          solids_stream << "rmin=\"0\" ";
        }
        solids_stream << "rmax=\"" << rmax / lunit << "\" ";
        solids_stream << "z=\"" << z / lunit << "\" ";
        solids_stream << "/>" << std::endl;
      }
    solids_stream << "</polycone>" << std::endl << std::endl;
    _get_stream(solids_section()) << solids_stream.str();
    return;
  }

  void gdml_writer::add_polycone(const std::string & name_,
                                 const polycone & p_,
                                 const std::string & lunit_str_,
                                 const std::string & aunit_str_)
  {
    std::map<double, std::pair<double, double> > zplanes;
    for(polycone::rz_col_type::const_iterator i = p_.points().begin();
        i != p_.points().end();
        i++) {
      double z = i->first;
      std::pair<double, double> rminmax;
      rminmax.first = i->second.rmin;
      rminmax.second = i->second.rmax;
      zplanes[z] = rminmax;
    }
    add_gdml_polycone(name_,
                      zplanes,
                      p_.get_start_angle(),
                      p_.get_delta_angle(),
                      lunit_str_,
                      aunit_str_);
    return;
  }

  /*
   * <polycone name = "thepolycone"
   *  startphi="1" deltaphi="4" aunit="rad" lunit= "mm">
   *    <zplane rmin="1" rmax="9" z="10" />
   *    <zplane rmin="3" rmax="5" z="12" />
   * </polycone>
   */
  void gdml_writer::add_gdml_generic_polycone(const std::string & name_,
                                              std::map<double, double> rzpoints_,
                                              double start_phi_, double delta_phi_,
                                              const std::string & lunit_str_,
                                              const std::string & aunit_str_)
  {
    double lunit = datatools::units::get_length_unit_from(lunit_str_);
    double aunit = datatools::units::get_angle_unit_from(aunit_str_);

    std::ostringstream solids_stream;
    solids_stream << "<" <<  "genericPolycone"
                  << " name=" << '"' << to_html(name_) << '"';

    double startphi = 0.0;
    double deltaphi = 2 * M_PI;
    if (datatools::is_valid(start_phi_) && start_phi_ >= 0.0) {
      startphi = start_phi_;
    }
    solids_stream << " startphi=" << '"';
    solids_stream.precision(15);
    solids_stream << startphi / aunit << '"';

    if (datatools::is_valid(delta_phi_)) {
      DT_THROW_IF(delta_phi_ == 0.0, std::domain_error,
                  "Invalid zero delta phi!");
      DT_THROW_IF(startphi + delta_phi_ > 2 * M_PI, std::domain_error,
                  "Delta theta is too large!");
      deltaphi = delta_phi_;
    }
    solids_stream << " deltaphi=" << '"';
    solids_stream.precision(15);
    solids_stream << deltaphi / aunit << '"';

    solids_stream << " lunit=" << '"' << lunit_str_ << '"';
    solids_stream << " aunit=" << '"' << aunit_str_ << '"';

    solids_stream << " >" << std::endl << std::endl;

    for (std::map<double, double>::const_iterator i = rzpoints_.begin();
        i != rzpoints_.end();
        i++) {
        double z = i->first;
        double r = i->second;
        solids_stream << "  " << "<rzpoint ";
        solids_stream << "r=\"" << r / lunit << "\" ";
        solids_stream << "z=\"" << z / lunit << "\" ";
        solids_stream << "/>" << std::endl;
      }
    solids_stream << "</genericPolycone>" << std::endl << std::endl;
    _get_stream(solids_section()) << solids_stream.str();
    return;
  }


  /*
   * <polyhedra name= "thepolyhedra"
   *  startphi="1" deltaphi="4" numsides="10" aunit="rad"
   *  lunit= "mm">
   *   <zplane rmin="1" rmax="9" z="10" />
   *   <zplane rmin="3" rmax="5" z="12" />
   * </polyhedra>
   */
  void gdml_writer::add_gdml_polyhedra(const std::string & name_,
                                       size_t num_sides_,
                                       std::map<double, std::pair<double, double> > zplanes_,
                                       double start_phi_, double delta_phi_,
                                       const std::string & lunit_str_,
                                       const std::string & aunit_str_)
  {
    double lunit = datatools::units::get_length_unit_from(lunit_str_);
    double aunit = datatools::units::get_angle_unit_from(aunit_str_);

    std::ostringstream solids_stream;
    solids_stream << "<" <<  "polyhedra"
                  << " name=" << '"' << to_html(name_) << '"';

    solids_stream << " numsides=" << '"' << num_sides_ << '"';

    double startphi = 0.0;
    double deltaphi = 2 * M_PI;
    if (datatools::is_valid(start_phi_) && start_phi_ >= 0.0) {
      startphi = start_phi_;
    }
    solids_stream << " startphi=" << '"';
    solids_stream.precision(15);
    solids_stream << startphi / aunit << '"';

    if (datatools::is_valid(delta_phi_)) {
      DT_THROW_IF(delta_phi_ == 0.0, std::domain_error,
                  "Invalid zero delta phi!");
      DT_THROW_IF(startphi + delta_phi_ > 2 * M_PI, std::domain_error,
                  "Delta theta is too large!");
      deltaphi = delta_phi_;
    }
    solids_stream << " deltaphi=" << '"';
    solids_stream.precision(15);
    solids_stream << deltaphi / aunit << '"';

    solids_stream << " lunit=" << '"' << lunit_str_ << '"';
    solids_stream << " aunit=" << '"' << aunit_str_ << '"';

    solids_stream << " >" << std::endl << std::endl;

    for(std::map<double, std::pair<double, double> >::const_iterator i = zplanes_.begin();
        i != zplanes_.end();
        i++) {
        double z = i->first;
        double rmin = i->second.first;
        double rmax = i->second.second;
        solids_stream << "  " << "<zplane ";
        if (datatools::is_valid(rmin) && rmin != 0.0) {
          solids_stream << "rmin=\"" << rmin / lunit << "\" ";
        } else {
          solids_stream << "rmin=\"0\" ";
        }
        solids_stream << "rmax=\"" << rmax / lunit << "\" ";
        solids_stream << "z=\"" << z / lunit << "\" ";
        solids_stream << "/>" << std::endl;
      }
    solids_stream << "<" <<  "/polyhedra>" << std::endl << std::endl;
    _get_stream(solids_section()) << solids_stream.str();
    return;
  }

  void gdml_writer::add_polyhedra(const std::string & name_,
                                  const polyhedra & p_,
                                  const std::string & lunit_str_,
                                  const std::string & aunit_str_)
  {
    std::map<double, std::pair<double, double> > zplanes;
    for(polyhedra::rz_col_type::const_iterator i = p_.points().begin();
        i != p_.points().end();
        i++) {
        double z = i->first;
        std::pair<double, double> rminmax;
        rminmax.first =  i->second.rmin;
        rminmax.second = i->second.rmax;
        zplanes[z] = rminmax;
      }
    add_gdml_polyhedra(name_,
                       p_.get_n_sides(),
                       zplanes,
                       0.0,
                       2. * M_PI * CLHEP::radian,
                       lunit_str_,
                       aunit_str_);
    return;
  }


  void gdml_writer::add_gdml_generic_polyhedra(const std::string & name_,
                                               size_t num_sides_,
                                               std::map<double, double> rzpoints_,
                                               double start_phi_, double delta_phi_,
                                               const std::string & lunit_str_,
                                               const std::string & aunit_str_)
  {
    double lunit = datatools::units::get_length_unit_from(lunit_str_);
    double aunit = datatools::units::get_angle_unit_from(aunit_str_);

    std::ostringstream solids_stream;
    solids_stream << "<" <<  "<genericPolyhedra"
                  << " name=" << '"' << to_html(name_) << '"';

    solids_stream << " numsides=" << '"' << num_sides_ << '"';

    double startphi = 0.0;
    double deltaphi = 2 * M_PI;
    if (datatools::is_valid(start_phi_) && start_phi_ != 0.0) {
      startphi = start_phi_;
    }
    solids_stream << " startphi=" << '"';
    solids_stream.precision(15);
    solids_stream << startphi / aunit << '"';

    if (datatools::is_valid(delta_phi_)) {
      DT_THROW_IF(delta_phi_ == 0.0, std::domain_error,
                  "Invalid zero delta phi!");
      DT_THROW_IF(startphi + delta_phi_ > 2 * M_PI, std::domain_error,
                  "Delta phi is too large!");
      deltaphi = delta_phi_;
    }
    solids_stream << " deltaphi=" << '"';
    solids_stream.precision(15);
    solids_stream << deltaphi / aunit << '"';

    solids_stream << " lunit=" << '"' << lunit_str_ << '"';
    solids_stream << " aunit=" << '"' << aunit_str_ << '"';

    solids_stream << " >" << std::endl << std::endl;

    for(std::map<double, double>::const_iterator i = rzpoints_.begin();
        i != rzpoints_.end();
        i++) {
        double z = i->first;
        double r = i->second;
        solids_stream << "  " << "<<rzpoint ";
        solids_stream << "r=\"" << r / lunit << "\" ";
        solids_stream << "z=\"" << z / lunit << "\" ";
        solids_stream << "/>" << std::endl;
      }
    solids_stream << "<" <<  "/genericPolyhedra>" << std::endl << std::endl;
    _get_stream(solids_section()) << solids_stream.str();
    return;
  }


  void gdml_writer::add_gdml_sphere(const std::string & name_,
                                    double rmin_, double rmax_,
                                    double start_phi_, double delta_phi_,
                                    double start_theta_, double delta_theta_,
                                    const std::string & lunit_str_,
                                    const std::string & aunit_str_)
  {
    double lunit = datatools::units::get_length_unit_from(lunit_str_);
    double aunit = datatools::units::get_angle_unit_from(aunit_str_);

    std::ostringstream solids_stream;
    solids_stream << "<" <<  "sphere"
                  << " name=" << '"' << to_html(name_) << '"';

    if (datatools::is_valid(rmin_) && rmin_ > 0.0) {
      solids_stream << " rmin=" << '"';
      solids_stream.precision(15);
      solids_stream << rmin_ / lunit << '"';
    }

    solids_stream << " rmax=" << '"';
    solids_stream.precision(15);
    solids_stream << rmax_ / lunit << '"';

    double startphi = 0.0;
    double deltaphi = 2 * M_PI;
    if (datatools::is_valid(start_phi_) && start_phi_ >= 0.0) {
      startphi = start_phi_;
    }
    solids_stream << " startphi=" << '"';
    solids_stream.precision(15);
    solids_stream << startphi / aunit << '"';

    if (datatools::is_valid(delta_phi_)) {
      DT_THROW_IF(delta_phi_ == 0.0, std::domain_error,
                  "Invalid zero delta phi!");
      DT_THROW_IF(startphi + delta_phi_ > 2 * M_PI, std::domain_error,
                  "Delta phi is too large!");
      deltaphi = delta_phi_;
    }
    solids_stream << " deltaphi=" << '"';
    solids_stream.precision(15);
    solids_stream << deltaphi / aunit << '"';

    double starttheta = 0.0;
    double deltatheta = M_PI;
    if (datatools::is_valid(start_theta_) && start_theta_ >= 0.0) {
      starttheta = start_theta_;
    }
    solids_stream << " starttheta=" << '"';
    solids_stream.precision(15);
    solids_stream << starttheta / aunit << '"';

    if (datatools::is_valid(delta_theta_)) {
      DT_THROW_IF(delta_theta_ == 0.0, std::domain_error,
                  "Invalid zero delta theta!");
      DT_THROW_IF(starttheta + delta_theta_ > M_PI, std::domain_error,
                  "Delta theta is too large!");
      deltatheta = delta_theta_;
    }
    solids_stream << " deltatheta=" << '"';
    solids_stream.precision(15);
    solids_stream << deltatheta / aunit << '"';

    solids_stream << " lunit=" << '"' << lunit_str_ << '"';
    solids_stream << " aunit=" << '"' << aunit_str_ << '"';

    solids_stream << " />" << std::endl << std::endl;
    _get_stream(solids_section()) << solids_stream.str();
    return;
  }

  void gdml_writer::add_sphere(const std::string & name_,
                               const sphere & s_,
                               const std::string & lunit_str_,
                               const std::string & aunit_str_)
  {
    if (s_.is_orb()) {
      add_gdml_orb(name_,
                   s_.get_r(),
                   lunit_str_);
    } else {
      add_gdml_sphere(name_,
                      s_.get_r_min(),
                      s_.get_r_max(),
                      s_.get_start_phi(),
                      s_.get_delta_phi(),
                      s_.get_start_theta(),
                      s_.get_delta_theta(),
                      lunit_str_,
                      aunit_str_);
    }
    return;
  }

  void gdml_writer::add_gdml_tube(const std::string & name_,
                                  double rmin_, double rmax_, double z_,
                                  double start_phi_, double delta_phi_,
                                  const std::string & lunit_str_,
                                  const std::string & aunit_str_)
  {
    double lunit = datatools::units::get_length_unit_from(lunit_str_);
    double aunit = datatools::units::get_angle_unit_from(aunit_str_);

    std::ostringstream solids_stream;
    solids_stream << "<" <<  "tube"
                  << " name=" << '"' << to_html(name_) << '"';

    if (datatools::is_valid(rmin_) && rmin_ > 0.0) {
      solids_stream << " rmin=" << '"';
      solids_stream.precision(15);
      solids_stream << rmin_ / lunit << '"';
    } else {
      solids_stream << " rmin=" << '"' << "0" << '"';
    }

    solids_stream << " rmax=" << '"';
    solids_stream.precision(15);
    solids_stream << rmax_ / lunit << '"';

    solids_stream << " z=" << '"';
    solids_stream.precision(15);
    solids_stream << z_ / lunit << '"';

    double startphi = 0.0;
    double deltaphi = 2 * M_PI;
    if (datatools::is_valid(start_phi_) && start_phi_ != 0.0) {
      startphi = start_phi_;
    }
    solids_stream << " startphi=" << '"';
    solids_stream.precision(15);
    solids_stream << startphi / aunit << '"';

    if (datatools::is_valid(delta_phi_)) {
      DT_THROW_IF(delta_phi_ == 0.0, std::domain_error,
                  "Invalid zero delta phi!");
      DT_THROW_IF(startphi + delta_phi_ > 2 * M_PI, std::domain_error,
                  "Delta phi is too large!");
      deltaphi = delta_phi_;
    }
    solids_stream << " deltaphi=" << '"';
    solids_stream.precision(15);
    solids_stream << deltaphi / aunit << '"';

    solids_stream << " lunit=" << '"' << lunit_str_ << '"';
    solids_stream << " aunit=" << '"' << aunit_str_ << '"';

    solids_stream << " />" << std::endl << std::endl;
    _get_stream(solids_section()) << solids_stream.str();
    return;
  }


  void gdml_writer::add_tube(const std::string & name_,
                             const tube & t_,
                             const std::string & lunit_str_,
                             const std::string & aunit_str_)
  {
    add_gdml_tube(name_,
                  t_.get_inner_r(),
                  t_.get_outer_r(),
                  t_.get_z(),
                  t_.get_start_phi(),
                  t_.get_delta_phi(),
                  lunit_str_,
                  aunit_str_);
    return;
  }


  void gdml_writer::add_cylinder(const std::string & name_,
                                 const cylinder & c_,
                                 const std::string & lunit_str_,
                                 const std::string & aunit_str_)
  {
    add_gdml_tube(name_,
                  0.0,
                  c_.get_radius(),
                  c_.get_z(),
                  0.0,
                  2. * M_PI * CLHEP::radian,
                  lunit_str_,
                  aunit_str_);
    return;
  }

  void gdml_writer::add_tessellated(const std::string & name_,
                                    const tessellated_solid & t_,
                                    const std::string & lunit_str_)
  {
    // Registration of vertices:
    for (tessellated_solid::vertices_col_type::const_iterator i
           = t_.vertices().begin();
         i != t_.vertices().end();
         i++) {
      std::ostringstream vtx_key_oss;
      vtx_key_oss << name_<< ".vtx_" << i->first;
      add_position(vtx_key_oss.str(), i->second.get_position(), lunit_str_);
    }

    // Open the solid:
    std::ostringstream solids_stream;
    solids_stream << "<" <<  "tessellated"
                  << " name=" << '"' << to_html(name_) << '"';
    solids_stream << " >" << std::endl << std::endl;

    // Registration of facets:
    for (tessellated_solid::facets_col_type::const_iterator i
           = t_.facets().begin();
         i != t_.facets().end();
         i++) {
       // unsigned int facet_index = i->first;
       const facet34 & facet = i->second;

       solids_stream << "<";
       if (facet.is_triangle()) {
         solids_stream << "triangular";
       } else {
         solids_stream << "quadrangular";
       }
       solids_stream << " ";
       for (int i = 0; i < (int) facet.get_number_of_vertices(); i++) {
         int32_t vtx_key = facet.get_vertex_key(i);
         solids_stream << "vertex" << (i+1) << "=\"" << name_ << ".vtx_" << vtx_key << "\" ";
       }
       solids_stream << "type=\"" << "ABSOLUTE" << "\"";
       solids_stream << " />" << std::endl;

     }
     // Close the solid:
     solids_stream << "</tessellated>" << std::endl << std::endl;

     _get_stream(solids_section()) << solids_stream.str();
     return;
  }

  void gdml_writer::add_gdml_boolean(const std::string & name_,
                                     const std::string & boolean_type_,
                                     const std::string & first_ref_,
                                     const std::string & second_ref_,
                                     const std::string & position_ref_,
                                     const std::string & rotation_ref_)
  {
    DT_THROW_IF ((boolean_type_ != "union")
                 && (boolean_type_ != "subtraction")
                 && (boolean_type_ != "intersection"),
                 std::logic_error,
                 "Invalid boolean solid type '" << boolean_type_
                 << "' for solid '" << name_ << "' !");
    std::ostringstream solids_stream;
    solids_stream << "<" <<  boolean_type_
                  << " name=" << '"' << to_html(name_) << '"'
                  << " >" << std::endl;
    DT_THROW_IF (first_ref_.empty(), std::logic_error,
                 "Missing first reference in boolean '" << name_ << "' of type '" << boolean_type_ << "'!");
    DT_THROW_IF (second_ref_.empty(), std::logic_error,
                 "Missing second reference in boolean '" << name_ << "' of type '" << boolean_type_ << "'!");

    solids_stream << "  <first ref=" << '"' <<  to_html(first_ref_) << '"' << " />" << std::endl;
    solids_stream << "  <second ref=" << '"' <<  to_html(second_ref_) << '"' << " />" << std::endl;
    solids_stream << "  <positionref ref=" << '"' <<  to_html(position_ref_) << '"' << " />" << std::endl;
    solids_stream << "  <rotationref ref=" << '"' <<  to_html(rotation_ref_) << '"' << " />" << std::endl;

    solids_stream << "</" <<  boolean_type_ << ">" << std::endl << std::endl;
    _get_stream(solids_section()) << solids_stream.str();
    return;
  }

  void gdml_writer::add_gdml_union(const std::string & name_,
                                   const std::string & first_ref_,
                                   const std::string & second_ref_,
                                   const std::string & position_ref_,
                                   const std::string & rotation_ref_)
  {
    gdml_writer::add_gdml_boolean(name_, "union", first_ref_, second_ref_,
                                  position_ref_, rotation_ref_);
    return;
  }

  void gdml_writer::add_gdml_subtraction(const std::string & name_,
                                         const std::string & first_ref_,
                                         const std::string & second_ref_,
                                         const std::string & position_ref_,
                                         const std::string & rotation_ref_)
  {
    gdml_writer::add_gdml_boolean(name_, "subtraction", first_ref_, second_ref_,
                                  position_ref_, rotation_ref_);
    return;
  }

  void gdml_writer::add_gdml_intersection(const std::string & name_,
                                          const std::string & first_ref_,
                                          const std::string & second_ref_,
                                          const std::string & position_ref_,
                                          const std::string & rotation_ref_)
  {
    gdml_writer::add_gdml_boolean(name_, "intersection", first_ref_, second_ref_,
                                  position_ref_, rotation_ref_);
    return;
  }

  /* Structure */

  gdml_writer::physvol::physvol(const std::string & volumeref_,
                                const std::string & positionref_,
                                const std::string & rotationref_,
                                const std::string & scaleref_)
  {
    volumeref = volumeref_;
    positionref = positionref_;
    rotationref = rotationref_;
    scaleref = scaleref_;
    return;
  }

  // static
  const std::string & gdml_writer::replicavol::replicated_along_axis()
  {
    static std::string label;
    if (label.empty()) {
      label = "replicated_along_axis";
    }
    return label;
  }

  void gdml_writer::add_replica_volume(const std::string & name_,
                                       const std::string & material_ref_,
                                       const std::string & solid_ref_,
                                       const replicavol & replicavol_,
                                       const std::string & lunit_str_,
                                       const std::string & /*aunit_str_*/,
                                       const std::map<std::string, std::string> & aux_)
  {
    double lunit = datatools::units::get_length_unit_from(lunit_str_);
    _get_stream(structure_section()) << "<volume" << " name=" << '"' << to_html(name_) << '"' << " >" << std::endl;
    _get_stream(structure_section()) << "  <materialref" << " ref=" << '"' << to_html(material_ref_) << '"' << " />" << std::endl;
    _get_stream(structure_section()) << "  <solidref   " << " ref=" << '"' << to_html(solid_ref_) << '"' << " />" << std::endl;

    _get_stream(structure_section()) << "  <replicavol number=" << '"' << replicavol_.number << '"' << " >" << std::endl;
    _get_stream(structure_section()) << "    <volumeref  " << " ref=" << '"' << to_html(replicavol_.volumeref) << '"' << " />" << std::endl;

    _get_stream(structure_section()) << "    <" << replicavol_.mode << ">" << std::endl;

    _get_stream(structure_section()) << "      <direction  "
                                     << replicavol_.direction<< "=" << '"' << "1" << '"' << " />" << std::endl;

    _get_stream(structure_section()) << "      <width"
                                     << " value="
                                     << '"' << replicavol_.width / lunit << '"'
                                     << " unit="
                                     << '"' << lunit_str_ << '"'
                                     << " />" << std::endl;

    _get_stream(structure_section()) << "      <offset"
                                     << " value="
                                     << '"' << replicavol_.offset / lunit << '"'
                                     << " unit="
                                     << '"' << lunit_str_ << '"'
                                     << " />" << std::endl;

    _get_stream(structure_section()) << "    </" << replicavol_.mode << ">" << std::endl;
    _get_stream(structure_section()) << "  </replicavol >" << std::endl << std::endl;

    add_volume_auxiliaries(aux_);

    _get_stream(structure_section()) << "</volume>" << std::endl << std::endl;
    return;
  }

  void gdml_writer::add_volume_auxiliaries(const std::map<std::string, std::string> & aux_)
  {
    _get_stream(structure_section()) << std::endl;
    for(std::map<std::string, std::string>::const_iterator i = aux_.begin();
        i != aux_.end();
        i++) {
        _get_stream(structure_section()) << "  <auxiliary "
                                         << "auxtype=\""
                                         << i->first
                                         << "\" "
                                         << " auxvalue=\""
                                         << i->second
                                         << "\" />" << std::endl;
      }
    _get_stream(structure_section()) << std::endl;
    return;
  }

  void gdml_writer::add_volume(const std::string & name_,
                               const std::string & material_ref_,
                               const std::string & solid_ref_,
                               const std::list<physvol> & phys_vols_,
                               const std::map<std::string, std::string> & aux_)
  {
    _get_stream(structure_section()) << "<volume" << " name=" << '"' << to_html(name_) << '"' << " >" << std::endl;
    _get_stream(structure_section()) << "  <materialref" << " ref=" << '"' << to_html(material_ref_) << '"' << " />" << std::endl;
    _get_stream(structure_section()) << "  <solidref   " << " ref=" << '"' << to_html(solid_ref_) << '"' << " />" << std::endl;

    if ( phys_vols_.size())
      {
        _get_stream(structure_section()) << std::endl;
      }
    for(std::list<physvol>::const_iterator i = phys_vols_.begin();
        i != phys_vols_.end();
        i++)
      {
        _get_stream(structure_section()) << "  <physvol>" << std::endl;
        _get_stream(structure_section()) << "    <volumeref  " << " ref=" << '"' << to_html(i->volumeref) << '"' << " />" << std::endl;
        _get_stream(structure_section()) << "    <positionref" << " ref=" << '"' << to_html(i->positionref) << '"' << " />" << std::endl;
        if (! i->rotationref.empty())
          {
            _get_stream(structure_section()) << "    <rotationref" << " ref=" << '"' << to_html(i->rotationref) << '"' << " />" << std::endl;
          }
        if (! i->scaleref.empty())
          {
            _get_stream(structure_section()) << "    <scaleref  " << " ref=" << '"' << to_html(i->scaleref) << '"' << " />" << std::endl;
          }
        _get_stream(structure_section()) << "  </physvol>" << std::endl << std::endl;
      }

    add_volume_auxiliaries(aux_);

    _get_stream(structure_section()) << "</volume>" << std::endl << std::endl;
    return;
  }

  void gdml_writer::add_volume(const std::string & name_,
                               const std::string & material_ref_,
                               const std::string & solid_ref_)
  {
    std::map<std::string, std::string> aux;
    add_volume(name_, material_ref_, solid_ref_, aux);
    return;
  }

  void gdml_writer::add_volume(const std::string & name_,
                               const std::string & material_ref_,
                               const std::string & solid_ref_,
                               const std::map<std::string, std::string> & aux_)
  {
    std::list<physvol> empty_phys_vols;
    add_volume(name_, material_ref_, solid_ref_, empty_phys_vols, aux_);
    return;
  }

  void gdml_writer::add_volume(const std::string & name_,
                               const std::string & material_ref_,
                               const std::string & solid_ref_,
                               const std::list<physvol> & phys_vols_)
  {
    std::map<std::string, std::string> aux;
    add_volume(name_,
               material_ref_,
               solid_ref_,
               phys_vols_,
               aux);
    return;
  }

  void gdml_writer::add_replica_volume(const std::string & name_,
                                       const std::string & material_ref_,
                                       const std::string & solid_ref_,
                                       const replicavol & replicavol_,
                                       const std::string & lunit_str_,
                                       const std::string & aunit_str_)
  {
    std::map<std::string, std::string> aux;
    add_replica_volume(name_,
                       material_ref_,
                       solid_ref_,
                       replicavol_,
                       lunit_str_,
                       aunit_str_,
                       aux);
    return;
  }

  /* Setup */

  void gdml_writer::add_setup(const std::string & name_,
                              const std::string & world_ref_,
                              const std::string & version_)
  {
    _get_stream(setup_section()) << "<setup"
                                 << " name=" << '"' << to_html(name_) << '"'
                                 << " version=" << '"' << version_ << '"'
                                 << " >" << std::endl;
    _get_stream(setup_section()) << "  <world ref=" << '"' << to_html(world_ref_) << '"'
                                 << " />" << std::endl;
    _get_stream(setup_section()) << "</setup>" << std::endl << std::endl;
  }


  /*** Section ***/

  void gdml_writer::xml_header(std::ostream & out_,
                               const std::string & xml_version_,
                               const std::string & xml_encoding_,
                               bool standalone_)
  {
    std::string xml_version = xml_version_;
    if (xml_version.empty()) xml_version = default_xml_version();
    std::string xml_encoding = xml_encoding_;
    if (xml_encoding.empty()) xml_encoding = default_xml_encoding();

    out_ << "<?xml version=" << '"' << xml_version << '"';
    out_ << " encoding=" << '"' << xml_encoding << '"';
    if (standalone_) {
      out_ << " standalone=" << '"' << "yes" << '"';
    } else {
      out_ << " standalone=" << '"' << "no" << '"';
    }
    out_ << " ?>" << std::endl;
    return;
  }


  /*

    <?xml version="1.0" encoding="UTF-8" standalone="no" ?>
    <gdml xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://service-spi.web.cern.ch/service-spi/app/releases/GDML/schema/gdml.xsd">

  */
  void gdml_writer::gdml_begin(std::ostream & out_,
                               const std::string & schema_,
                               const std::string & xsi_)
  {
    std::string schema = schema_;
    if (schema.empty()) schema = default_gdml_schema();
    std::string xsi = xsi_;
    if (! xsi.empty() && xsi == "<default>") xsi = default_xsi();
    out_ << "<gdml ";
    if (! xsi.empty())
      {
        out_ << "xmlns:xsi="
             << '"' << xsi_ << '"'
             << " ";

      }
    out_ << "xsi:noNamespaceSchemaLocation="
         << '"' << schema << '"'
         << " ";

    out_ << " >" << std::endl;

    return;
  }

  void gdml_writer::gdml_end(std::ostream & out_)
  {
    out_ << "</gdml>" << std::endl;
    return;
  }

  void gdml_writer::gdml_section_begin(std::ostream & out_,
                                       const std::string & section_)
  {
    out_ << "<" << section_ << ">" << std::endl << std::endl;
    return;
  }

  void gdml_writer::gdml_section_end(std::ostream & out_,
                                     const std::string & section_)
  {
    out_ << "</" << section_ << ">" << std::endl << std::endl;
    return;
  }

  /*** Utilities ***/

  void gdml_writer::full_write(std::ostream & out_,
                               const std::string & xml_version_,
                               const std::string & xml_encoding_,
                               const std::string & gdml_schema_,
                               const std::string & xsi_)
  {
    std::string xml_version = xml_version_;
    if (xml_version.empty()) xml_version = default_xml_version();
    std::string xml_encoding = xml_encoding_;
    if (xml_encoding.empty()) xml_encoding = default_xml_encoding();
    std::string gdml_schema = gdml_schema_;
    if (gdml_schema.empty()) gdml_schema = default_remote_gdml_schema();
    std::string xsi = xsi_;

    DT_THROW_IF (! out_, std::logic_error, "Output stream is invalid !");
    bool standalone = false;
    xml_header(out_, xml_version, xml_encoding, standalone);
    gdml_begin(out_, gdml_schema, xsi);
    out_ << std::endl;

    gdml_section_begin(out_, define_section());
    out_ << _get_stream(define_section()).str();
    gdml_section_end(out_, define_section());
    out_ << std::endl;

    gdml_section_begin(out_, materials_section());
    if (has_external_materials_stream()) {
      out_ << _external_materials_stream_->str();
    }
    out_ << _get_stream(materials_section()).str();
    gdml_section_end(out_, materials_section());
    out_ << std::endl;

    gdml_section_begin(out_, solids_section());
    out_ << _get_stream(solids_section()).str();
    gdml_section_end(out_, solids_section());
    out_ << std::endl;

    gdml_section_begin(out_, structure_section());
    out_ << _get_stream(structure_section()).str();
    gdml_section_end(out_, structure_section());
    out_ << std::endl;

    //gdml_section_begin(out_, setup_section());
    out_ << _get_stream(setup_section()).str();
    //gdml_section_end(out_, setup_section());
    out_ << std::endl;

    gdml_end(out_);
    out_ << std::endl;

    return;
  }

  void gdml_writer::attach_external_materials(const std::ostringstream & oss_)
  {
    _external_materials_stream_ = &oss_;
    return;
  }

  void gdml_writer::save_file(const std::string & filename_,
                              const std::string & xml_version_,
                              const std::string & xml_encoding_,
                              const std::string & gdml_schema_,
                              const std::string & xsi_)
  {
    std::string xml_version = xml_version_;
    if (xml_version.empty()) xml_version = default_xml_version();
    std::string xml_encoding = xml_encoding_;
    if (xml_encoding.empty()) xml_encoding = default_xml_encoding();
    std::string gdml_schema = gdml_schema_;
    if (gdml_schema.empty()) gdml_schema = default_remote_gdml_schema();
    std::string xsi = xsi_;
    std::ofstream fout;
    std::string filename = filename_;
    datatools::fetch_path_with_env(filename);
    fout.open(filename.c_str());
    DT_THROW_IF (! fout, std::runtime_error,
                 "Cannot open output GDML file '" << filename << "' !");
    full_write(fout, xml_version, xml_encoding, gdml_schema, xsi);
    return;
  }

  void gdml_writer::dump(std::ostream & out_) const
  {
    out_ << "gdml_writer::dump: " << std::endl;
    out_ << std::endl << "*** Define section *** " << std::endl;
    out_ << _get_stream(define_section()).str();
    out_ << std::endl << "*** Materials section *** " << std::endl;
    out_ << _get_stream(materials_section()).str();
    out_ << std::endl << "*** Solids section *** " << std::endl;
    out_ << _get_stream(solids_section()).str();
    out_ << std::endl << "*** Structure section *** " << std::endl;
    out_ << _get_stream(structure_section()).str();
    out_ << std::endl << "*** Setup section *** " << std::endl;
    out_ << _get_stream(setup_section()).str();
    out_ << std::endl;
    return;
  }

} // end of namespace geomtools
