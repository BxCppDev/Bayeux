// -*- mode: c++ ; -*-
/* gdml_writer.cc
 */

#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include <datatools/units.h>
#include <datatools/exception.h>

#include <geomtools/geomtools_config.h>
#include <geomtools/gdml_writer.h>

namespace geomtools {

  using namespace std;

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

  void gdml_writer::set_using_html_symbols (bool u_)
  {
    _using_html_symbols_ = u_;
    return;
  }

  bool gdml_writer::is_using_html_symbols () const
  {
    return _using_html_symbols_;
  }

  string gdml_writer::to_html(const string & name_,
                              bool using_html_symbols_)
  {
    string str1 = name_;
    if (using_html_symbols_) {
      boost::replace_all (str1, "." , "&#46;");
      boost::replace_all (str1, "[" , "&#91;");
      boost::replace_all (str1, "]" , "&#93;");
      boost::replace_all (str1, "{" , "&#123;");
      boost::replace_all (str1, "}" , "&#124;");
    }
    boost::replace_all (str1, "::" , "__");
    return str1;
  }

  string gdml_writer::to_ascii(const string & name_,
                               bool using_html_symbols_)
  {
    string str1 = name_;
    if (using_html_symbols_) {
      boost::replace_all (str1 , "&#46;"  , ".");
      boost::replace_all (str1 , "&#91;"  , "[");
      boost::replace_all (str1 , "&#93;"  , "]");
      boost::replace_all (str1 , "&#123;" , "{");
      boost::replace_all (str1 , "&#124;" , "}");
    }
    boost::replace_all (str1, "__" , "::");
    return str1;
  }

  const ostringstream & gdml_writer::get_stream (const string & section_) const
  {
    return _get_stream (section_);
  }

  const ostringstream & gdml_writer::_get_stream (const string & section_) const
  {
    streams_col_type::const_iterator i = _streams_.find (section_);
    DT_THROW_IF (i == _streams_.end (),
                 logic_error, "Invalid section '" << section_ << "' !");
    return *(i->second);
  }

  ostringstream & gdml_writer::_get_stream (const string & section_)
  {
    streams_col_type::iterator i = _streams_.find (section_);
    DT_THROW_IF (i == _streams_.end (),
                 logic_error, "Invalid section '" << section_ << "' !");
    return *(i->second);
  }

  bool gdml_writer::has_external_materials_stream () const
  {
    return _external_materials_stream_ != 0;
  }

  void gdml_writer::reset_external_materials_stream ()
  {
    _external_materials_stream_ = 0;
  }

  bool gdml_writer::is_initialized () const
  {
    return _initialized_;
  }

  bool gdml_writer::is_verbose () const
  {
    return _verbose_;
  }

  void gdml_writer::set_verbose (bool v_)
  {
    _verbose_ = v_;
  }

  // ctor:
  gdml_writer::gdml_writer ()
  {
    _initialized_ = false;
    _verbose_ = false;
    _external_materials_stream_ = 0;
    _using_html_symbols_ = false;
    initialize();
  }

  gdml_writer::~gdml_writer ()
  {
    if (_initialized_) {
      reset();
    }
    return;
  }

  void gdml_writer::initialize()
  {
    this->init();
  }

  void gdml_writer::_allocate_streams()
  {
    _streams_[define_section()]    = new ostringstream;
    _streams_[materials_section()] = new ostringstream;
    _streams_[solids_section()]    = new ostringstream;
    _streams_[structure_section()] = new ostringstream;
    _streams_[setup_section()]     = new ostringstream;
    return;
  }

  void gdml_writer::_free_streams()
  {
    if (_streams_.size()) {
      for (streams_col_type::iterator i = _streams_.begin ();
           i != _streams_.end ();
           i++) {
        delete i->second;
      }
      _streams_.clear ();
    }
    return;
  }

  void gdml_writer::init ()
  {
    // if (_initialized_) {
    //     if (is_verbose ()) clog << "WARNING: gdml_writer::init: Already initialized !" << endl;
    //   }
    // DT_THROW_IF(is_initialized(), std::logic_error,
    //             "Already initialized !");
    if (_streams_.size() == 0) {
      _allocate_streams();
    }
    _initialized_ = true;
  }

  void gdml_writer::reset()
  {
    // DT_THROW_IF(! is_initialized(), std::logic_error,
    //             "Not initialized !");
    // if (! _initialized_) {
    //   if (is_verbose ()) clog << "WARNING: gdml_writer::reset: Not initialized !" << endl;
    //   return;
    // }
    if (_streams_.size() > 0) {
      _free_streams();
    }
    reset_external_materials_stream ();
    _initialized_ = false;
  }

  /*** Defines ***/

  void gdml_writer::add_constant (const string & name_,
                                  double value_)
  {
    _get_stream(define_section()) << "<constant"
                                << " name=" << '"' << to_html (name_) << '"'
                                << " value="  << '"';
    _get_stream(define_section()).precision (15);
    _get_stream(define_section()) << value_ << '"';
    _get_stream(define_section()) << " />" << endl;
    _get_stream(define_section()) << endl;
  }

  void gdml_writer::add_quantity (const string & name_,
                                  const string & quantity_type_,
                                  const string & unit_str_,
                                  double value_)
  {
    double unit = datatools::units::get_unit_from (quantity_type_, unit_str_);
    _get_stream (define_section()) << "<quantity"
                                 << " name=" << '"' << to_html (name_) << '"'
                                 << " type=" << '"' << quantity_type_ << '"'
                                 << " value="  << '"';
    _get_stream (define_section()).precision (15);
    _get_stream (define_section()) << (value_ / unit) << '"'
                                 << " unit=" << '"' << unit_str_ << '"'
                                 << " />" << endl;
    _get_stream (define_section()) << endl;
  }

  void gdml_writer::add_variable (const string & name_,
                                  double value_)
  {
    _get_stream (define_section()) << "<variable"
                                 << " name=" << '"' << to_html (name_) << '"'
                                 << " value="  << '"';
    _get_stream (define_section()).precision (15);
    _get_stream (define_section()) << value_ << '"'
                                 << " />" << endl;
    _get_stream (define_section()) << endl;
  }

  void gdml_writer::add_variable (const string & name_,
                                  const string & expr_value_)
  {
    _get_stream (define_section()) << "<variable"
                                 << " name=" << '"' << to_html (name_) << '"'
                                 << " value="  << '"' << expr_value_ << '"'
                                 << " />" << endl;
    _get_stream (define_section()) << endl;
  }

  void gdml_writer::add_position (const string & name_,
                                  double x_, double y_, double z_,
                                  const string & unit_str_)
  {
    //clog << "DEVEL: gdml_writer::add_position: Entering..." << endl;
    double unit = datatools::units::get_length_unit_from (unit_str_);
    _get_stream (define_section()) << "<position"
                                 << " name=" << '"' << to_html (name_) << '"'
                                 << " x=" << '"';
    _get_stream (define_section()).precision (15);
    _get_stream (define_section()) << x_ / unit << '"'
                                 << " y=" << '"';
    _get_stream (define_section()).precision (15);
    _get_stream (define_section()) << y_ / unit << '"'
                                 << " z=" << '"';
    _get_stream (define_section()).precision (15);
    _get_stream (define_section()) << z_ / unit << '"'
                                 << " unit=" << '"' << unit_str_ << '"'
                                 << " />" << endl;
    _get_stream (define_section()) << endl;
  }

  void gdml_writer::add_position (const string & name_,
                                  const vector_3d & v_,
                                  const string & unit_str_)
  {
    add_position (name_, v_.x (), v_.y (), v_.z (), unit_str_);
  }

  void gdml_writer::add_rotation (const string & name_,
                                  const string & axis_,
                                  double angle_,
                                  const string & unit_str_)
  {
    double angle_unit = datatools::units::get_angle_unit_from (unit_str_);
    DT_THROW_IF ((axis_ != "x") && (axis_ != "y") && (axis_ != "z"),
                 logic_error,
                 "Invalid rotation axis '" << axis_ << "' for rotation '" << name_ << "' !");
    _get_stream (define_section()) << "<rotation"
                                 << " name=" << '"' << to_html (name_) << '"'
                                 << " " << axis_ << "=" << '"';
    _get_stream (define_section()).precision (15);
    _get_stream (define_section()) << (angle_ / angle_unit) << '"'
                                 << " unit=" << '"' << unit_str_ << '"'
                                 << " />" << endl;
    _get_stream (define_section()) << endl;
  }

  void gdml_writer::add_rotation (const string & name_,
                                  const rotation_3d & rot_,
                                  const string & unit_str_)
  {
    double angle_unit = datatools::units::get_angle_unit_from (unit_str_);
    _get_stream (define_section()) << "<rotation"
                                   << " name=" << '"' << to_html (name_)
                                   << '"';

    double a, b, c;
    extract_xyz_euler_angle_from_rotation (rot_, a, b, c);

    _get_stream (define_section()) << " " << "x" << "=" << '"';
    _get_stream (define_section()).precision (15);
    _get_stream (define_section()) << (-a / angle_unit) << '"';

    _get_stream (define_section()) << " " << "y" << "=" << '"';
    _get_stream (define_section()).precision (15);
    _get_stream (define_section()) << (-b / angle_unit) << '"';

    _get_stream (define_section()) << " " << "z" << "=" << '"';
    _get_stream (define_section()).precision (15);
    _get_stream (define_section()) << (-c / angle_unit) << '"';

    _get_stream (define_section()) << " unit=" << '"' << unit_str_ << '"'
                                 << " />" << endl;
    _get_stream (define_section()) << endl;
  }

  /*** Materials ***/

  void gdml_writer::add_isotope (const string & name_,
                                 size_t atomic_number_,
                                 size_t number_of_nucleons_,
                                 double a_)
  {
    _get_stream (materials_section()) << "<isotope"
                                    << " name=" << '"' << to_html (name_) << '"'
                                    << " Z=" << '"' << atomic_number_ << '"'
                                    << " N=" << '"' << number_of_nucleons_ << '"'
                                    << " >" << endl;
    _get_stream (materials_section()) << "  <atom"
                                    << " type=" << '"' << "A" << '"'
                                    << " value=" << '"';
    _get_stream (materials_section()).precision (15);
    _get_stream (materials_section()) << a_ << '"' << " />" << endl;
    // 2010-10-24 FM: the " />" termination string does not appear in the GDML user manual !
    _get_stream (materials_section()) << "</isotope>" << endl;
    _get_stream (materials_section()) << endl;
  }

  void gdml_writer::add_element (const string & name_,
                                 size_t atomic_number_,
                                 const string & formula_,
                                 double a_)
  {
    _get_stream (materials_section()) << "<element"
                                    << " name=" << '"' << to_html (name_) << '"'
                                    << " Z=" << '"' << atomic_number_ << '"'
                                    << " formula=" << '"' << formula_ << '"'
                                    << " >" << endl;
    _get_stream (materials_section()) << "  <atom"
                                    << " value=" << '"';
    _get_stream (materials_section()).precision (15);
    _get_stream (materials_section()) << a_ << '"' << " />" << endl;
    _get_stream (materials_section()) << "</element>" << endl;
    _get_stream (materials_section()) << endl;
  }

  void gdml_writer::add_element (const string & name_,
                                 const map<string, double> & fractions_)
  {
    ostringstream materials_stream;
    materials_stream << "<element"
                     << " name=" << '"' << to_html (name_) << '"'
                     << " >" << endl;
    double s = 0.0;
    for (map<string, double>::const_iterator i = fractions_.begin ();
         i != fractions_.end ();
         i++) {
      string ref = i->first;
      double n = i->second;
      DT_THROW_IF (n < 0.0 || n > 1.0, logic_error,
                   "Invalid fraction value '" << n << "' for referenced '" << ref << "' in element '"
                   << name_ << "' !");
      s += n;
      materials_stream << "  <fraction"
                       << " ref=" << '"' << to_html (ref) << '"'
                       << " n=" << '"';
      materials_stream.precision (15);
      materials_stream << n << '"' << " />" << endl;
    }
    DT_THROW_IF (s > 1.000000000001, logic_error,
                 "Invalid fraction sum in element '" << name_ << "' !");
    materials_stream << "</element>" << endl;
    _get_stream (materials_section()) << materials_stream.str ();
    _get_stream (materials_section()) << endl;
  }

  void gdml_writer::add_element (const string & name_,
                                 const string & ref_)
  {
    map<string, double> m;
    m[ref_] = 1.0;
    add_element (name_, m);
  }

  void gdml_writer::add_element (const string & name_,
                                 const string & ref1_, double fraction1_,
                                 const string & ref2_, double fraction2_)
  {
    map<string, double> m;
    m[ref1_] = fraction1_;
    m[ref2_] = fraction2_;
    add_element (name_, m);
  }

  void gdml_writer::add_element (const string & name_,
                                 const string & ref1_, double fraction1_,
                                 const string & ref2_, double fraction2_,
                                 const string & ref3_, double fraction3_)
  {
    map<string, double> m;
    m[ref1_] = fraction1_;
    m[ref2_] = fraction2_;
    m[ref3_] = fraction3_;
    add_element (name_, m);
  }

  void gdml_writer::add_element (const string & name_,
                                 const string & ref1_, double fraction1_,
                                 const string & ref2_, double fraction2_,
                                 const string & ref3_, double fraction3_,
                                 const string & ref4_, double fraction4_)
  {
    map<string, double> m;
    m[ref1_] = fraction1_;
    m[ref2_] = fraction2_;
    m[ref3_] = fraction3_;
    m[ref4_] = fraction4_;
    add_element (name_, m);
  }

  void gdml_writer::add_element (const string & name_,
                                 const string & ref1_, double fraction1_,
                                 const string & ref2_, double fraction2_,
                                 const string & ref3_, double fraction3_,
                                 const string & ref4_, double fraction4_,
                                 const string & ref5_, double fraction5_)
  {
    map<string, double> m;
    m[ref1_] = fraction1_;
    m[ref2_] = fraction2_;
    m[ref3_] = fraction3_;
    m[ref4_] = fraction4_;
    m[ref5_] = fraction5_;
    add_element (name_, m);
  }

  void gdml_writer::add_material (const string & name_,
                                  double atomic_number_,
                                  double density_,
                                  double a_)
  {
    _get_stream (materials_section()) << "<material"
                                    << " name=" << '"' << to_html (name_) << '"'
                                    << " Z=" << '"';
    _get_stream (materials_section()).precision (15);
    _get_stream (materials_section()) << atomic_number_ << '"' << " >" << endl;

    _get_stream (materials_section()) << "  <D"
                                    << " value=" << '"';
    _get_stream (materials_section()).precision (15);
    _get_stream (materials_section()) << (density_ / (CLHEP::g / CLHEP::cm3))
                                    << '"'
                                    << " unit=" << '"' << "g/cm3" << '"'
                                    << " />" << endl;

    _get_stream (materials_section()) << "  <atom"
                                    << " value=" << '"';
    _get_stream (materials_section()).precision (15);
    _get_stream (materials_section()) << a_ << '"' << " />" << endl;

    _get_stream (materials_section()) << "</material >" << endl;
    _get_stream (materials_section()) << endl;
  }

  void gdml_writer::add_material (const string & name_,
                                  const string & formula_,
                                  double density_,
                                  const map<string, double> & fractions_)
  {
    ostringstream materials_stream;
    materials_stream << "<material"
                     << " name=" << '"' << to_html (name_) << '"'
                     << " formula=" << '"' << formula_ << '"'
                     << " >" << endl;

    materials_stream << "  <D"
                     << " value=" << '"';
    materials_stream.precision (15);
    materials_stream << (density_ / (CLHEP::g / CLHEP::cm3))
                     << '"'
                     << " unit=" << '"' << "g/cm3" << '"'
                     << " />" << endl;

    double s = 0.0;
    for (map<string, double>::const_iterator i = fractions_.begin ();
         i != fractions_.end ();
         i++)
      {
        string ref = i->first;
        double n = i->second;
        DT_THROW_IF (n < 0.0 || n > 1.0, logic_error,
                     "Invalid fraction value '" << n
                     << "' for referenced '" << ref << "' in element '"
                     << name_ << "' !");
        s += n;

        materials_stream << "  <fraction"
                         << " ref=" << '"' << to_html (ref) << '"'
                         << " n=" << '"';
        materials_stream.precision (15);
        materials_stream << n << '"' << " />" << endl;
      }
    DT_THROW_IF (s > 1.000000000001, logic_error,
                 "Invalid fraction sum in material '" << name_ << "' !");
    materials_stream << "</material>" << endl;
    _get_stream (materials_section()) << materials_stream.str ();
    _get_stream (materials_section()) << endl;
  }


  void gdml_writer::add_material (const string & name_,
                                  const string & formula_,
                                  double density_,
                                  const map<string, size_t> & composites_)
  {
    ostringstream materials_stream;
    materials_stream << "<material"
                     << " name=" << '"' << to_html (name_) << '"'
                     << " formula=" << '"' << formula_ << '"'
                     << " >" << endl;

    materials_stream << "  <D"
                     << " value=" << '"';
    materials_stream.precision (15);
    materials_stream << (density_ / (CLHEP::g / CLHEP::cm3))
                     << '"'
                     << " unit=" << '"' << "g/cm3" << '"'
                     << " />" << endl;

    for (map<string, size_t>::const_iterator i = composites_.begin ();
         i != composites_.end ();
         i++)
      {
        string ref = i->first;
        size_t n = i->second;
        DT_THROW_IF (n == 0, logic_error,
                     "Invalid composite value '" << n
                     << "' for referenced '" << ref << "' in element '"
                     << name_ << "' !");
        materials_stream << "  <composite"
                         << " ref=" << '"' << to_html (ref) << '"'
                         << " n=" << '"' << n << '"' << " />" << endl;;
      }
    materials_stream << "</material>" << endl;
    _get_stream (materials_section()) << materials_stream.str ();
    _get_stream (materials_section()) << endl;
  }

  /*** solid ***/

  bool gdml_writer::solid_type_is_supported (const string & solid_type_)
  {
    if (solid_type_ == "box")
      {
        return true;
      }
    if (solid_type_ == "cylinder")
      {
        return true;
      }
    if (solid_type_ == "tube")
      {
        return true;
      }
    if (solid_type_ == "sphere")
      {
        return true;
      }
    if (solid_type_ == "polycone")
      {
        return true;
      }
    if (solid_type_ == "polyhedra")
      {
        return true;
      }

    // combination:
    if (solid_type_ == "union_3d")
      {
        return true;
      }
    if (solid_type_ == "subtraction_3d")
      {
        return true;
      }
    if (solid_type_ == "intersection_3d")
      {
        return true;
      }
    return false;
  };

  bool gdml_writer::solid_type_is_valid (const string & solid_type_)
  {
    //list<string> supported_solid_types;
    if (solid_type_ == "box")
      {
        return true;
      }
    if (solid_type_ == "sphere")
      {
        return true;
      }
    if (solid_type_ == "tube")
      {
        return true;
      }
    if (solid_type_ == "cylinder")
      {
        return true;
      }
    if (solid_type_ == "cone")
      {
        return true;
      }
    if (solid_type_ == "ellipsoid")
      {
        return true;
      }
    if (solid_type_ == "eltube")
      {
        return true;
      }
    if (solid_type_ == "elcone")
      {
        return true;
      }
    if (solid_type_ == "orb")
      {
        return true;
      }
    if (solid_type_ == "para")
      {
        return true;
      }
    if (solid_type_ == "polycone")
      {
        return true;
      }
    if (solid_type_ == "polyhedra")
      {
        return true;
      }
    if (solid_type_ == "torus")
      {
        return true;
      }
    if (solid_type_ == "trd")
      {
        return true;
      }
    if (solid_type_ == "trap")
      {
        return true;
      }
    if (solid_type_ == "hype")
      {
        return true;
      }
    if (solid_type_ == "twistedbox")
      {
        return true;
      }
    if (solid_type_ == "twistedtrd")
      {
        return true;
      }
    if (solid_type_ == "twistedtrap")
      {
        return true;
      }
    if (solid_type_ == "twistedtube")
      {
        return true;
      }
    if (solid_type_ == "xtru")
      {
        return true;
      }
    if (solid_type_ == "arb8")
      {
        return true;
      }
    if (solid_type_ == "tessellated")
      {
        return true;
      }
    if (solid_type_ == "tet")
      {
        return true;
      }
    if (solid_type_ == "union_3d")
      {
        return true;
      }
    if (solid_type_ == "subtraction_3d")
      {
        return true;
      }
    if (solid_type_ == "intersection_3d")
      {
        return true;
      }
    return false;
  }

  void gdml_writer::add_gdml_box (const string & name_,
                                  double x_, double y_, double z_,
                                  const string & lunit_str_)
  {
    double lunit = datatools::units::get_length_unit_from (lunit_str_);

    ostringstream solids_stream;
    solids_stream << "<" <<  "box"
                  << " name=" << '"' << to_html (name_) << '"';

    solids_stream << " x=" << '"';
    solids_stream.precision (15);
    solids_stream << x_ / lunit << '"';

    solids_stream << " y=" << '"';
    solids_stream.precision (15);
    solids_stream << y_ / lunit << '"';

    solids_stream << " z=" << '"';
    solids_stream.precision (15);
    solids_stream << z_ / lunit << '"';

    solids_stream << " lunit=" << '"' << lunit_str_ << '"';
    solids_stream << " />" << endl << endl;

    _get_stream (solids_section()) << solids_stream.str ();
  }


  void gdml_writer::add_box (const string & name_,
                             const box & b_,
                             const string & lunit_str_)
  {
    add_gdml_box (name_, b_.get_x (), b_.get_y (), b_.get_z (), lunit_str_);
  }


  void gdml_writer::add_gdml_orb (const string & name_,
                                  double r_,
                                  const string & lunit_str_)
  {
    double lunit = datatools::units::get_length_unit_from (lunit_str_);

    ostringstream solids_stream;
    solids_stream << "<" <<  "orb"
                  << " name=" << '"' << to_html (name_) << '"';

    solids_stream << " r=" << '"';
    solids_stream.precision (15);
    solids_stream << r_ / lunit << '"';

    solids_stream << " lunit=" << '"' << lunit_str_ << '"';

    solids_stream << " />" << endl << endl;
    _get_stream (solids_section()) << solids_stream.str ();
  }


  void gdml_writer::add_gdml_sphere (const string & name_,
                                     double rmin_, double rmax_,
                                     double start_phi_, double delta_phi_,
                                     double start_theta_, double delta_theta_,
                                     const string & lunit_str_,
                                     const string & aunit_str_)
  {
    double lunit = datatools::units::get_length_unit_from (lunit_str_);
    double aunit = datatools::units::get_angle_unit_from (aunit_str_);

    ostringstream solids_stream;
    solids_stream << "<" <<  "sphere"
                  << " name=" << '"' << to_html (name_) << '"';

    if (rmin_ > 0.0)
      {
        solids_stream << " rmin=" << '"';
        solids_stream.precision (15);
        solids_stream << rmin_ / lunit << '"';
      }

    solids_stream << " rmax=" << '"';
    solids_stream.precision (15);
    solids_stream << rmax_ / lunit << '"';

    if (start_phi_ > 0.0)
      {
        solids_stream << " startphi=" << '"';
        solids_stream.precision (15);
        solids_stream << start_phi_ / aunit << '"';
      }

    solids_stream << " deltaphi=" << '"';
    solids_stream.precision (15);
    solids_stream << delta_phi_ / aunit << '"';

    if (start_theta_ > 0.0)
      {
        solids_stream << " starttheta=" << '"';
        solids_stream.precision (15);
        solids_stream << start_theta_ / aunit << '"';
      }

    solids_stream << " deltatheta=" << '"';
    solids_stream.precision (15);
    solids_stream << delta_theta_ / aunit << '"';

    solids_stream << " lunit=" << '"' << lunit_str_ << '"';
    solids_stream << " aunit=" << '"' << aunit_str_ << '"';

    solids_stream << " />" << endl << endl;
    _get_stream (solids_section()) << solids_stream.str ();
  }


  void gdml_writer::add_gdml_tube (const string & name_,
                                   double rmin_, double rmax_, double z_,
                                   double start_phi_, double delta_phi_,
                                   const string & lunit_str_,
                                   const string & aunit_str_)
  {
    double lunit = datatools::units::get_length_unit_from (lunit_str_);
    double aunit = datatools::units::get_angle_unit_from (aunit_str_);

    ostringstream solids_stream;
    solids_stream << "<" <<  "tube"
                  << " name=" << '"' << to_html (name_) << '"';

    if (rmin_ > 0.0)
      {
        solids_stream << " rmin=" << '"';
        solids_stream.precision (15);
        solids_stream << rmin_ / lunit << '"';
      }
    else
      {
        solids_stream << " rmin=" << '"' << "0" << '"';
      }

    solids_stream << " rmax=" << '"';
    solids_stream.precision (15);
    solids_stream << rmax_ / lunit << '"';

    solids_stream << " z=" << '"';
    solids_stream.precision (15);
    solids_stream << z_ / lunit << '"';

    if (start_phi_ != 0.0)
      {
        solids_stream << " startphi=" << '"';
        solids_stream.precision (15);
        solids_stream << start_phi_ / aunit << '"';
      }

    solids_stream << " deltaphi=" << '"';
    solids_stream.precision (15);
    solids_stream << delta_phi_ / aunit << '"';

    solids_stream << " lunit=" << '"' << lunit_str_ << '"';
    solids_stream << " aunit=" << '"' << aunit_str_ << '"';

    solids_stream << " />" << endl << endl;
    _get_stream (solids_section()) << solids_stream.str ();
  }

  /*
   * <polycone name = "thepolycone"
   startphi="1" deltaphi="4" aunit="rad" lunit= "mm">
   * <zplane rmin="1" rmax="9" z="10" />
   * <zplane rmin="3" rmax="5" z="12" />
   * </polycone>
   */
  void gdml_writer::add_gdml_polycone (const string & name_,
                                       map<double, pair<double, double> > zplanes_,
                                       double start_phi_, double delta_phi_,
                                       const string & lunit_str_,
                                       const string & aunit_str_)
  {
    double lunit = datatools::units::get_length_unit_from (lunit_str_);
    double aunit = datatools::units::get_angle_unit_from (aunit_str_);

    ostringstream solids_stream;
    solids_stream << "<" <<  "polycone"
                  << " name=" << '"' << to_html (name_) << '"';

    if (start_phi_ != 0.0)
      {
        solids_stream << " startphi=" << '"';
        solids_stream.precision (15);
        solids_stream << start_phi_ / aunit << '"';
      }

    solids_stream << " deltaphi=" << '"';
    solids_stream.precision (15);
    solids_stream << delta_phi_ / aunit << '"';

    solids_stream << " lunit=" << '"' << lunit_str_ << '"';
    solids_stream << " aunit=" << '"' << aunit_str_ << '"';

    solids_stream << " >" << endl << endl;

    for (map<double, pair<double, double> >::const_iterator i = zplanes_.begin ();
         i != zplanes_.end ();
         i++)
      {
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
        solids_stream << "/>" << endl;
      }
    solids_stream << "<" <<  "/polycone>" << endl << endl;
    _get_stream (solids_section()) << solids_stream.str ();
    return;
  }

  /*
   * <polyhedra name= "thepolyhedra" startphi="1" deltaphi="4" numsides="10" aunit="rad"
   lunit= "mm">
   * <zplane rmin="1" rmax="9" z="10" />
   * <zplane rmin="3" rmax="5" z="12" />
   * </polyhedra>
   */
  void gdml_writer::add_gdml_polyhedra (const string & name_,
                                        size_t num_sides_,
                                        map<double, pair<double, double> > zplanes_,
                                        double start_phi_, double delta_phi_,
                                        const string & lunit_str_,
                                        const string & aunit_str_)
  {
    double lunit = datatools::units::get_length_unit_from (lunit_str_);
    double aunit = datatools::units::get_angle_unit_from (aunit_str_);

    ostringstream solids_stream;
    solids_stream << "<" <<  "polyhedra"
                  << " name=" << '"' << to_html (name_) << '"';

    solids_stream << " numsides=" << '"' << num_sides_ << '"';

    // if (start_phi_ != 0.0)
    // {
    solids_stream << " startphi=" << '"';
    solids_stream.precision (15);
    solids_stream << start_phi_ / aunit << '"';
    // }

    solids_stream << " deltaphi=" << '"';
    solids_stream.precision (15);
    solids_stream << delta_phi_ / aunit << '"';

    solids_stream << " lunit=" << '"' << lunit_str_ << '"';
    solids_stream << " aunit=" << '"' << aunit_str_ << '"';

    solids_stream << " >" << endl << endl;

    for (map<double, pair<double, double> >::const_iterator i = zplanes_.begin ();
         i != zplanes_.end ();
         i++)
      {
        double z = i->first;
        double rmin = i->second.first;
        double rmax = i->second.second;
        solids_stream << "  " << "<zplane ";
        if (rmin != 0.0)
          {
            solids_stream << "rmin=\"" << rmin / lunit << "\" ";
          }
        else
          {
            solids_stream << "rmin=\"0\" ";
          }
        solids_stream << "rmax=\"" << rmax / lunit << "\" ";
        solids_stream << "z=\"" << z / lunit << "\" ";
        solids_stream << "/>" << endl;
      }
    solids_stream << "<" <<  "/polyhedra>" << endl << endl;
    _get_stream (solids_section()) << solids_stream.str ();
    return;
  }

  void gdml_writer::add_gdml_union (const string & name_,
                                    const string & first_ref_,
                                    const string & second_ref_,
                                    const string & position_ref_,
                                    const string & rotation_ref_)
  {
    gdml_writer::add_gdml_boolean (name_, "union", first_ref_, second_ref_,
                                   position_ref_, rotation_ref_);
  }

  void gdml_writer::add_gdml_subtraction (const string & name_,
                                          const string & first_ref_,
                                          const string & second_ref_,
                                          const string & position_ref_,
                                          const string & rotation_ref_)
  {
    gdml_writer::add_gdml_boolean (name_, "subtraction", first_ref_, second_ref_,
                                   position_ref_, rotation_ref_);
  }

  void gdml_writer::add_gdml_intersection (const string & name_,
                                           const string & first_ref_,
                                           const string & second_ref_,
                                           const string & position_ref_,
                                           const string & rotation_ref_)
  {
    gdml_writer::add_gdml_boolean (name_, "intersection", first_ref_, second_ref_,
                                   position_ref_, rotation_ref_);
  }

  void gdml_writer::add_gdml_boolean (const string & name_,
                                      const string & boolean_type_,
                                      const string & first_ref_,
                                      const string & second_ref_,
                                      const string & position_ref_,
                                      const string & rotation_ref_)
  {
    DT_THROW_IF ((boolean_type_ != "union")
                 && (boolean_type_ != "subtraction") && (boolean_type_ != "intersection"),
                 logic_error,
                 "Invalid boolean solid type '" << boolean_type_
                 << "' for solid '" << name_ << "' !");
    ostringstream solids_stream;
    solids_stream << "<" <<  boolean_type_
                  << " name=" << '"' << to_html (name_) << '"'
                  << " >" << endl;

    solids_stream << "  <first ref=" << '"' <<  to_html (first_ref_) << '"' << " />" << endl;
    solids_stream << "  <second ref=" << '"' <<  to_html (second_ref_) << '"' << " />" << endl;
    solids_stream << "  <positionref ref=" << '"' <<  to_html (position_ref_) << '"' << " />" << endl;
    solids_stream << "  <rotationref ref=" << '"' <<  to_html (rotation_ref_) << '"' << " />" << endl;

    solids_stream << "</" <<  boolean_type_ << ">" << endl << endl;
    _get_stream (solids_section()) << solids_stream.str ();
  }

  void gdml_writer::add_cylinder (const string & name_,
                                  const cylinder & c_,
                                  const string & lunit_str_,
                                  const string & aunit_str_)
  {
    add_gdml_tube (name_,
                   0.0,
                   c_.get_radius (),
                   c_.get_z (),
                   0.0,
                   2. * M_PI * CLHEP::radian,
                   lunit_str_,
                   aunit_str_);
  }


  void gdml_writer::add_tube (const string & name_,
                              const tube & t_,
                              const string & lunit_str_,
                              const string & aunit_str_)
  {
    add_gdml_tube (name_,
                   t_.get_inner_r (),
                   t_.get_outer_r (),
                   t_.get_z (),
                   0.0,
                   2. * M_PI * CLHEP::radian,
                   lunit_str_,
                   aunit_str_);
  }

  void gdml_writer::add_orb (const string & name_,
                             const sphere & s_,
                             const string & lunit_str_,
                             const string & /*aunit_str_*/)
  {
    add_gdml_orb (name_,
                  s_.get_r (),
                  lunit_str_);
  }

  void gdml_writer::add_sphere (const string & name_,
                                const sphere & s_,
                                const string & lunit_str_,
                                const string & /*aunit_str_*/)
  {
    add_gdml_orb (name_,
                  s_.get_r (),
                  lunit_str_);
  }

  void gdml_writer::add_polycone (const string & name_,
                                  const polycone & p_,
                                  const string & lunit_str_,
                                  const string & aunit_str_)
  {
    map<double, pair<double, double> > zplanes;
    for (polycone::rz_col_type::const_iterator i = p_.points ().begin ();
         i != p_.points ().end ();
         i++)
      {
        double z = i->first;
        pair<double, double> rminmax;
        rminmax.first = i->second.rmin;
        rminmax.second = i->second.rmax;
        zplanes[z] = rminmax;
      }
    add_gdml_polycone (name_,
                       zplanes,
                       0.0,
                       2. * M_PI * CLHEP::radian,
                       lunit_str_,
                       aunit_str_);
    return;
  }

  void gdml_writer::add_polyhedra (const string & name_,
                                   const polyhedra & p_,
                                   const string & lunit_str_,
                                   const string & aunit_str_)
  {
    map<double, pair<double, double> > zplanes;
    for (polyhedra::rz_col_type::const_iterator i = p_.points ().begin ();
         i != p_.points ().end ();
         i++)
      {
        double z = i->first;
        pair<double, double> rminmax;
        rminmax.first =  i->second.rmin;
        rminmax.second = i->second.rmax;
        zplanes[z] = rminmax;
      }
    add_gdml_polyhedra (name_,
                        p_.get_n_sides (),
                        zplanes,
                        0.0,
                        2. * M_PI * CLHEP::radian,
                        lunit_str_,
                        aunit_str_);
    return;
  }

  /*** Structure ***/

  gdml_writer::physvol::physvol (const string & volumeref_,
                                 const string & positionref_,
                                 const string & rotationref_,
                                 const string & scaleref_)
  {
    volumeref = volumeref_;
    positionref = positionref_;
    rotationref = rotationref_;
    scaleref = scaleref_;
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

  void gdml_writer::add_replica_volume (const string & name_,
                                        const string & material_ref_,
                                        const string & solid_ref_,
                                        const replicavol & replicavol_,
                                        const string & lunit_str_,
                                        const string & /*aunit_str_*/,
                                        const map<string, string> & aux_)
  {
    double lunit = datatools::units::get_length_unit_from (lunit_str_);
    _get_stream (structure_section()) << "<volume" << " name=" << '"' << to_html (name_) << '"' << " >" << endl;
    _get_stream (structure_section()) << "  <materialref" << " ref=" << '"' << to_html (material_ref_) << '"' << " />" << endl;
    _get_stream (structure_section()) << "  <solidref   " << " ref=" << '"' << to_html (solid_ref_) << '"' << " />" << endl;

    _get_stream (structure_section()) << "  <replicavol number=" << '"' << replicavol_.number << '"' << " >" << endl;
    _get_stream (structure_section()) << "    <volumeref  " << " ref=" << '"' << to_html (replicavol_.volumeref) << '"' << " />" << endl;

    _get_stream (structure_section()) << "    <" << replicavol_.mode << ">" << endl;

    _get_stream (structure_section()) << "      <direction  "
                                    << replicavol_.direction<< "=" << '"' << "1" << '"' << " />" << endl;

    _get_stream (structure_section()) << "      <width"
                                    << " value="
                                    << '"' << replicavol_.width / lunit << '"'
                                    << " unit="
                                    << '"' << lunit_str_ << '"'
                                    << " />" << endl;

    _get_stream (structure_section()) << "      <offset"
                                    << " value="
                                    << '"' << replicavol_.offset / lunit << '"'
                                    << " unit="
                                    << '"' << lunit_str_ << '"'
                                    << " />" << endl;

    _get_stream (structure_section()) << "    </" << replicavol_.mode << ">" << endl;
    _get_stream (structure_section()) << "  </replicavol >" << endl << endl;

    add_volume_auxiliaries (aux_);

    _get_stream (structure_section()) << "</volume>" << endl << endl;
  }

  void gdml_writer::add_volume_auxiliaries (const map<string,string> & aux_)
  {
    _get_stream (structure_section()) << endl;
    for (map<string,string>::const_iterator i = aux_.begin ();
         i != aux_.end ();
         i++)
      {
        _get_stream (structure_section()) << "  <auxiliary "
                                        << "auxtype=\""
                                        << i->first
                                        << "\" "
                                        << " auxvalue=\""
                                        << i->second
                                        << "\" />" << endl;
      }
    _get_stream (structure_section()) << endl;
    return;
  }

  void gdml_writer::add_volume (const string & name_,
                                const string & material_ref_,
                                const string & solid_ref_,
                                const list<physvol> & phys_vols_,
                                const map<string, string> & aux_)
  {
    _get_stream (structure_section()) << "<volume" << " name=" << '"' << to_html (name_) << '"' << " >" << endl;
    _get_stream (structure_section()) << "  <materialref" << " ref=" << '"' << to_html (material_ref_) << '"' << " />" << endl;
    _get_stream (structure_section()) << "  <solidref   " << " ref=" << '"' << to_html (solid_ref_) << '"' << " />" << endl;

    if ( phys_vols_.size ())
      {
        _get_stream (structure_section()) << endl;
      }
    for (list<physvol>::const_iterator i = phys_vols_.begin ();
         i != phys_vols_.end ();
         i++)
      {
        _get_stream (structure_section()) << "  <physvol>" << endl;
        _get_stream (structure_section()) << "    <volumeref  " << " ref=" << '"' << to_html (i->volumeref) << '"' << " />" << endl;
        _get_stream (structure_section()) << "    <positionref" << " ref=" << '"' << to_html (i->positionref) << '"' << " />" << endl;
        if (! i->rotationref.empty ())
          {
            _get_stream (structure_section()) << "    <rotationref" << " ref=" << '"' << to_html (i->rotationref) << '"' << " />" << endl;
          }
        if (! i->scaleref.empty ())
          {
            _get_stream (structure_section()) << "    <scaleref  " << " ref=" << '"' << to_html (i->scaleref) << '"' << " />" << endl;
          }
        _get_stream (structure_section()) << "  </physvol>" << endl << endl;
      }

    add_volume_auxiliaries (aux_);

    _get_stream (structure_section()) << "</volume>" << endl << endl;
  }

  void gdml_writer::add_volume (const string & name_,
                                const string & material_ref_,
                                const string & solid_ref_)
  {
    map<string, string> aux;
    add_volume (name_, material_ref_, solid_ref_, aux);
  }

  void gdml_writer::add_volume (const string & name_,
                                const string & material_ref_,
                                const string & solid_ref_,
                                const map<string, string> & aux_)
  {
    //cerr << "DEVEL: gdml_writer::add_volume: " << "Entering..." << endl;
    list<physvol> empty_phys_vols;
    add_volume (name_, material_ref_, solid_ref_, empty_phys_vols, aux_);
    //cerr << "DEVEL: gdml_writer::add_volume: " << "Exiting." << endl;
  }

  void gdml_writer::add_volume (const string & name_,
                                const string & material_ref_,
                                const string & solid_ref_,
                                const list<physvol> & phys_vols_)
  {
    map<string, string> aux;
    add_volume (name_,
                material_ref_,
                solid_ref_,
                phys_vols_,
                aux);
  }

  void gdml_writer::add_replica_volume (const string & name_,
                                        const string & material_ref_,
                                        const string & solid_ref_,
                                        const replicavol & replicavol_,
                                        const string & lunit_str_,
                                        const string & aunit_str_)
  {
    map<string, string> aux;
    add_replica_volume (name_,
                        material_ref_,
                        solid_ref_,
                        replicavol_,
                        lunit_str_,
                        aunit_str_,
                        aux);
  }

  /*** Setup ***/

  void gdml_writer::add_setup (const string & name_,
                               const string & world_ref_,
                               const string & version_)
  {
    _get_stream (setup_section()) << "<setup"
                                << " name=" << '"' << to_html (name_) << '"'
                                << " version=" << '"' << version_ << '"'
                                << " >" << endl;
    _get_stream (setup_section()) << "  <world ref=" << '"' << to_html (world_ref_) << '"'
                                << " />" << endl;
    _get_stream (setup_section()) << "</setup>" << endl << endl;
  }


  /*** Section ***/

  void gdml_writer::xml_header (ostream & out_,
                                const string & xml_version_,
                                const string & xml_encoding_,
                                bool standalone_)
  {
    std::string xml_version = xml_version_;
    if (xml_version.empty()) xml_version = default_xml_version();
    std::string xml_encoding = xml_encoding_;
    if (xml_encoding.empty()) xml_encoding = default_xml_encoding();

    out_ << "<?xml version=" << '"' << xml_version << '"';
    out_ << " encoding=" << '"' << xml_encoding << '"';
    if (standalone_)
      {
        out_ << " standalone=" << '"' << "yes" << '"';
      }
    else
      {
        out_ << " standalone=" << '"' << "no" << '"';
      }
    out_ << " ?>" << endl;
    return;
  }


  /*

    <?xml version="1.0" encoding="UTF-8" standalone="no" ?>
    <gdml xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://service-spi.web.cern.ch/service-spi/app/releases/GDML/schema/gdml.xsd">

  */
  void gdml_writer::gdml_begin (ostream & out_,
                                const string & schema_,
                                const string & xsi_)
  {
    std::string schema = schema_;
    if (schema.empty()) schema = default_gdml_schema();
    std::string xsi = xsi_;
    if (! xsi.empty() && xsi == "<default>") xsi = default_xsi();
    out_ << "<gdml ";
    if (! xsi.empty ())
      {
        out_ << "xmlns:xsi="
             << '"' << xsi_ << '"'
             << " ";

      }
    out_ << "xsi:noNamespaceSchemaLocation="
         << '"' << schema << '"'
         << " ";

    out_ << " >" << endl;

    return;
  }

  void gdml_writer::gdml_end (ostream & out_)
  {
    out_ << "</gdml>" << endl;
  }

  void gdml_writer::gdml_section_begin (ostream & out_,
                                        const string & section_)
  {
    out_ << "<" << section_ << ">" << endl << endl;
  }

  void gdml_writer::gdml_section_end (ostream & out_,
                                      const string & section_)
  {
    out_ << "</" << section_ << ">" << endl << endl;
  }

  /*** Utilities ***/

  void gdml_writer::full_write (ostream & out_,
                                const string & xml_version_,
                                const string & xml_encoding_,
                                const string & gdml_schema_,
                                const string & xsi_)
  {
    std::string xml_version = xml_version_;
    if (xml_version.empty()) xml_version = default_xml_version();
    std::string xml_encoding = xml_encoding_;
    if (xml_encoding.empty()) xml_encoding = default_xml_encoding();
    std::string gdml_schema = gdml_schema_;
    if (gdml_schema.empty()) gdml_schema = default_remote_gdml_schema();
    std::string xsi = xsi_;

    DT_THROW_IF (! out_, logic_error, "Output stream is invalid !");
    bool standalone = false;
    xml_header (out_, xml_version, xml_encoding, standalone);
    gdml_begin (out_, gdml_schema, xsi);
    out_ << endl;

    gdml_section_begin (out_, define_section());
    out_ << _get_stream (define_section()).str ();
    gdml_section_end (out_, define_section());
    out_ << endl;

    gdml_section_begin (out_, materials_section());
    if (has_external_materials_stream ()) {
      out_ << _external_materials_stream_->str ();
    }
    out_ << _get_stream (materials_section()).str ();
    gdml_section_end (out_, materials_section());
    out_ << endl;

    gdml_section_begin (out_, solids_section());
    out_ << _get_stream (solids_section()).str ();
    gdml_section_end (out_, solids_section());
    out_ << endl;

    gdml_section_begin (out_, structure_section());
    out_ << _get_stream (structure_section()).str ();
    gdml_section_end (out_, structure_section());
    out_ << endl;

    //gdml_section_begin (out_, setup_section());
    out_ << _get_stream (setup_section()).str ();
    //gdml_section_end (out_, setup_section());
    out_ << endl;

    gdml_end (out_);
    out_ << endl;

    return;
  }

  void gdml_writer::attach_external_materials (const ostringstream & oss_)
  {
    _external_materials_stream_ = &oss_;
    return;
  }

  void gdml_writer::save_file (const string & filename_,
                               const string & xml_version_,
                               const string & xml_encoding_,
                               const string & gdml_schema_,
                               const string & xsi_)
  {
    std::string xml_version = xml_version_;
    if (xml_version.empty()) xml_version = default_xml_version();
    std::string xml_encoding = xml_encoding_;
    if (xml_encoding.empty()) xml_encoding = default_xml_encoding();
    std::string gdml_schema = gdml_schema_;
    if (gdml_schema.empty()) gdml_schema = default_remote_gdml_schema();
    std::string xsi = xsi_;
    ofstream fout;
    string filename = filename_;
    datatools::fetch_path_with_env(filename);
    fout.open(filename.c_str ());
    DT_THROW_IF(! fout, std::runtime_error,
                "Cannot open output GDML file '" << filename << "' !");
    full_write (fout, xml_version, xml_encoding, gdml_schema, xsi);
    return;
  }

  void gdml_writer::dump (ostream & out_) const
  {
    out_ << "gdml_writer::dump: " << endl;
    out_ << endl << "*** Define section *** " << endl;
    out_ << _get_stream(define_section()).str ();
    out_ << endl << "*** Materials section *** " << endl;
    out_ << _get_stream(materials_section()).str ();
    out_ << endl << "*** Solids section *** " << endl;
    out_ << _get_stream(solids_section()).str ();
    out_ << endl << "*** Structure section *** " << endl;
    out_ << _get_stream(structure_section()).str ();
    out_ << endl << "*** Setup section *** " << endl;
    out_ << _get_stream(setup_section()).str ();
    out_ << endl;
  }

} // end of namespace geomtools

// end of gdml_writer.cc
