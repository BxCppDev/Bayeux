/// isotope.cc

// Ourselves:
#include <materials/isotope.h>

// Standard Library:
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <limits>

// Third Party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/clhep_units.h>

// This Project:
#include <materials/resource.h>
#include <materials/materials_config.h>
#include <materials/chemical_symbol.h>

// Parsing functions:
bool token_to_double(const std::string & token_, double & val_);
bool token_to_int(const std::string & token_, int min_, int max_, int & val_);
bool token_to_string(const std::string & token_, std::string & val_);

namespace materials {

  // static
  int isotope::max_number_of_protons()
  {
    return chemical_symbol::nb_chemical_symbols() - 1;
  }

  // static
  int isotope::undefined_number_of_nucleons()
  {
    return 0;
  }

  // static
  int isotope::max_number_of_nucleons()
  {
    return 293;
  }

  isotope::id::id()
  {
    reset();
    return;
  }

  isotope::id::id(int z_, int a_)
  {
    set(z_, a_);
    return;
  }

  void isotope::id::reset()
  {
    _z_ = chemical_symbol::Z_UNDEFINED;
    _a_ = undefined_number_of_nucleons();
    return;
  }

  void isotope::id::set(int z_, int a_)
  {
    set_z(z_);
    set_a(a_);
    return;
  }

  void isotope::id::set_z(int z_)
  {
    if (z_ < 0) {
      _z_ = chemical_symbol::Z_UNDEFINED;
    } else {
      _z_ = z_;
    }
    return;
  }

  int isotope::id::get_z() const
  {
    return _z_;
  }

  void isotope::id::set_a(int a_)
  {
    if (a_ <= 0) {
      _a_ = undefined_number_of_nucleons();
    } else {
      _a_ = a_;
    }
    return;
  }

  int isotope::id::get_a() const
  {
    return _a_;
  }

  bool isotope::id::is_valid() const
  {
    return _z_ != chemical_symbol::Z_UNDEFINED
      && _z_ < (int) chemical_symbol::nb_chemical_symbols()
               && _a_ != undefined_number_of_nucleons()
               && _a_ >= _z_;
  }

  std::string isotope::id::to_string() const
  {
    std::string sid;
    to_string(sid);
    return sid;
  }

  void isotope::id::to_string(std::string & name_) const
  {
    std::ostringstream iss;
    if (! is_valid()) {
      iss << "?";
    } else {
      iss << chemical_symbol::symbol_from_z(_z_);
      if (_z_ > 0) { // Not a neutron
        iss << '-' << _a_;
      }
    }
    name_ = iss.str();
    return;
  }

  bool isotope::id::from_string(const std::string & token_)
  {
    std::istringstream iss(token_);
    std::string tok;
    iss >> tok;
    if (!iss || tok.empty()) {
      // reset();
      return false;
    }
    if (tok == "?") {
      reset();
      return true;
    } else {
      int f = tok.find("-");
      if (f == (int) std::string::npos) {
        reset();
        return false;
      }
      std::string el_str = tok.substr(0, f);
      if (! chemical_symbol::symbol_is_tabulated(el_str)) {
        reset();
        return false;
      }
      int z = chemical_symbol::z_from_symbol(el_str);
      std::string a_str = tok.substr(f+1);
      int a = 0;
      if (! token_to_int(a_str, 1, max_number_of_protons(), a)) {
        reset();
        return false;
      }
      this->set(z, a);
      return true;
    }
  }

  std::string isotope::id::get_name() const
  {
    std::string name;
    to_string(name);
    return name;
  }

  bool isotope::id::operator<(const isotope::id & id_) const
  {
    if (this->_z_ < id_._z_) {
      return true;
    } else if (this->_z_ == id_._z_) {
      if (this->_a_ < id_._a_) {
        return true;
      }
    }
    return false;
  }

  bool isotope::id::operator==(const isotope::id & id_) const
  {
    return (this->_z_ == id_._z_) && (this->_a_ == id_._a_);
  }

  void isotope::id::tree_dump(std::ostream & out_,
                              const std::string & title_,
                              const std::string & indent_,
                              bool inherit_) const
  {
    if (! title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }
    out_ << indent_ << i_tree_dumpable::tag
         << "Z          : " << get_z() << std::endl;
    out_ << indent_ << i_tree_dumpable::tag
         << "A          : " << get_a() << std::endl;
    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Name       : '" << to_string() << "'" << std::endl;
    return;
  }

  isotope::record_type::record_type()
  {
    symbol  = "";
    mx      = std::numeric_limits<double>::quiet_NaN();
    mx_err  = std::numeric_limits<double>::quiet_NaN();
    bea     = std::numeric_limits<double>::quiet_NaN();
    bea_err = std::numeric_limits<double>::quiet_NaN();
    am      = std::numeric_limits<double>::quiet_NaN();
    am_err  = std::numeric_limits<double>::quiet_NaN();
    return;
  }

  void isotope::record_type::tree_dump(std::ostream & out_,
                                       const std::string & title_,
                                       const std::string & indent_,
                                       bool inherit_) const
  {
    if (! title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }
    out_.precision(15);
    out_ << indent_ << i_tree_dumpable::tag
         << "symbol  = '" << symbol << "'" << std::endl;
    out_ << indent_ << i_tree_dumpable::tag
         << "mx      = " << mx / CLHEP::keV     << " [keV]" << std::endl;
    out_ << indent_ << i_tree_dumpable::tag
         << "mx_err  = " << mx_err / CLHEP::keV << " [keV]" << std::endl;
    out_ << indent_ << i_tree_dumpable::tag
         << "bea     = " << bea / CLHEP::keV     << " [keV/A]" << std::endl;
    out_ << indent_ << i_tree_dumpable::tag
         << "bea_err = " << bea_err / CLHEP::keV << " [keV/A]" << std::endl;
    out_ << indent_ << i_tree_dumpable::tag
         << "am      = " << am     << " [u]" << std::endl;
    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "am_err  = " << am_err << " [u]" << std::endl;
    return;
  }

  // static
  void isotope::load_ame2003_table(isotope::isotope_dict_type & db_)
  {
    bool devel = false;
    // devel = true;
    int a_max = 0;
    // a_max = 250;
    // Open an ifstream from file 'mass.mas03':
    std::string tape_name = materials::get_resource("data/mass.mas03", true);
    std::ifstream ifstr_tape;
    ifstr_tape.open(tape_name.c_str());
    DT_THROW_IF(! ifstr_tape.is_open(), std::runtime_error, "Cannot open '" << tape_name << "' !");
    std::string ame03_line;
    // Skip header:
    for (int i = 0 ; i < 39 ; i++) {
      std::getline(ifstr_tape, ame03_line);
    }
    // Load records:
    while (std::getline(ifstr_tape, ame03_line) ) {
      if (devel) {
        std::cerr << "DEVEL: materials::isotope::load_ame2003_table: "
                  << "line =\n'" <<  ame03_line << "'"
                  << std::endl;
      }
      // Parse the line record:
      std::string z_str       = ame03_line.substr(9, 5);
      std::string a_str       = ame03_line.substr(14, 5);
      std::string el_str      = ame03_line.substr(20, 3);
      std::string mx_str      = ame03_line.substr(29, 13);
      std::string mx_err_str  = ame03_line.substr(41, 11);
      std::string bea_str     = ame03_line.substr(52, 11);
      std::string bea_err_str = ame03_line.substr(63, 9);
      std::string am_str      = ame03_line.substr(96, 3);
      std::string am_def_str  = ame03_line.substr(100, 12);
      std::string am_err_str  = ame03_line.substr(112, 11);
      if (devel) {
        std::cerr << "DEVEL: materials::isotope::load_ame2003_table: " << std::endl
                  << "  z_str=" << z_str << " a_str=" << a_str << std::endl
                  << "  mx_str=" << mx_str << " am_err_str=" << mx_err_str << std::endl
                  << "  bea_str=" << bea_str << " bea_err_str=" << bea_err_str << std::endl
                  << "  am_str=" << am_str << am_def_str << " am_err_str=" << am_err_str
                  << std::endl;
      }
      isotope::id iid;
      int z;
      DT_THROW_IF (! token_to_int(z_str, 0, 118, z), std::logic_error,
                   "Invalid Z format !");
      int a;
      DT_THROW_IF (! token_to_int(a_str, 1, 293, a), std::logic_error,
                   "Invalid A format !");
      if (devel) {
        std::cerr << "DEVEL: materials::isotope::load_ame2003_table: "
                  << "Z=" << z << " A=" << a
                  << std::endl;
      }
      if (a_max > 0 && a > a_max) {
        break;
      }
      iid.set(z, a);

      isotope::record_type r;
      DT_THROW_IF (! token_to_string(el_str, r.symbol), std::logic_error,
                   "Invalid format for element symbol !");

      DT_THROW_IF (! token_to_double(mx_str, r.mx), std::logic_error,
                   "Invalid format for mass excess !");
      r.mx *= CLHEP::keV;
      DT_THROW_IF (! token_to_double(mx_err_str, r.mx_err), std::logic_error,
                   "Invalid format for mass excess error !");
      r.mx_err *= CLHEP::keV;
      DT_THROW_IF (! token_to_double(bea_str, r.bea), std::logic_error,
                   "Invalid format for binding energy per nucleon !");
      r.bea *= CLHEP::keV;
      DT_THROW_IF (! token_to_double(bea_err_str, r.bea_err), std::logic_error,
                   "Invalid format for binding energy per nucleon error !");
      r.bea_err *= CLHEP::keV;
      std::ostringstream am_oss;
      am_oss << am_str << am_def_str;
      double am;
      DT_THROW_IF (! token_to_double(am_oss.str(), am), std::logic_error,
                   "Invalid format for atomic mass !");
      r.am = am * 1.e-6;
      double am_err;
      DT_THROW_IF (! token_to_double(am_err_str, am_err), std::logic_error,
                   "Invalid format for atomic mass error !");
      r.am_err = (am_err * 1e-6);
      if (devel) {
        std::cerr << "DEVEL: materials::isotope::load_ame2003_table: "
                  << "iid = '" << iid.to_string() << "' "
                  << " mx=" << r.mx / CLHEP::keV << "+/-" << r.mx_err / CLHEP::keV << " [keV]"
                  << " am=" << r.am << "+/-" << r.am_err << " [u]"
                  << std::endl;
      }
      if (iid.is_valid()) {
        db_[iid] = r;
        /*
          {
          record_type dummy;
          if (devel) {
            std::cerr << "DEVEL: materials::isotope::load_ame2003_table: "
                      << "TEST"
                      << std::endl;
          }
          db_[iid] = dummy;
        */
      }
      /*
        std::map<id, record_type>::iterator found = db_.find(iid);
        if (found != db_.end()) {
        record_type & rec = found->second;
        rec = r;
        if (devel) {
        std::cerr << "Registered isotope ID = '" << iid.to_string() << "'" << std::endl;
        rec.print(std::cerr);
        }
        }
      */
    }
    if (devel) {
      std::cerr << "Registered isotopes: " << std::endl;
      print_table_of_isotopes(db_, std::cerr, "Registered isotopes: ", "DEVEL: ");
      std::cerr << std::endl;
    }
    return;
  }

  // static
  void isotope::print_table_of_isotopes(const isotope_dict_type & toi_,
                                        std::ostream & out_,
                                        const std::string & title_,
                                        const std::string & indent_,
                                        bool inherit_)
  {
    if (! title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }
    for (isotope::isotope_dict_type::const_iterator i = toi_.begin();
         i != toi_.end();
         i++) {
      isotope::isotope_dict_type::const_iterator j = i;
      j++;
      const id & iid = i->first;
      const record_type & irec = i->second;
      std::ostringstream indent_ss;
      indent_ss << indent_;
      out_ << indent_;
      if (j != toi_.end()) {
        out_ << i_tree_dumpable::tag;
        indent_ss << i_tree_dumpable::skip_tag;
      /*
      } else {
        out_ << i_tree_dumpable::last_tag;
        indent_ss << i_tree_dumpable::last_skip_tag;
       */
      }
      out_ << "Isotope '" << iid.to_string() << "' : " << std::endl;
      irec.tree_dump(out_, "",  indent_ss.str());
    }
    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Total number of registered isotopes : " << toi_.size() << std::endl;

    return;
  }

  const isotope::isotope_dict_type & isotope::table_of_isotopes()
  {
    bool devel = false;
    static boost::scoped_ptr<isotope_dict_type> _TOI;
    if (! _TOI) {
      if (devel) {
        std::cerr << "DEVEL: materials::isotope::table_of_isotopes: "
                  << "Instantiating the table of isotopes..."
                  << std::endl;
      }
      _TOI.reset(new isotope_dict_type);
      // For now we use the Ame2003 table.
      if (devel) {
        std::cerr << "DEVEL: materials::isotope::table_of_isotopes: "
                  << "Loading the AME 2033 table..."
                  << std::endl;
      }
      load_ame2003_table(*_TOI);
      if (devel) {
        std::cerr << "DEVEL: materials::isotope::table_of_isotopes: "
                  << "Loaded : " << _TOI->size() << " records."
                  << std::endl;
      }
    }
    return *_TOI;
  }

  bool isotope::id_is_tabulated(const id & id_)
  {
    return table_of_isotopes().find(id_) != table_of_isotopes().end();
  }

  const isotope::record_type & isotope::table_record_from_id(const id & id_)
  {
    isotope_dict_type::const_iterator found = table_of_isotopes().find(id_);
    DT_THROW_IF(found == table_of_isotopes().end(), std::logic_error,
                "Isotope (Z=" << id_.get_z() << ",A="<< id_.get_a() << ") is not tabulated !");
    return found->second;
  }

  // static
  std::string isotope::isomeric_to_label(isomeric_level_type level_)
  {
    switch (level_) {
    case GROUND_STATE : return "";
    case ISOMERIC_M   : return "m";
    case ISOMERIC_M1  : return "m1";
    case ISOMERIC_M2  : return "m2";
    case ISOMERIC_M3  : return "m3";
    case ISOMERIC_F   : return "f";
    case ISOMERIC_F1  : return "f1";
    case ISOMERIC_F2  : return "f2";
    case ISOMERIC_F3  : return "f3";
    default: return "?";
    }
  }

  // static
  isotope::isomeric_level_type isotope::label_to_isomeric(const std::string & label_)
  {
    if (label_ == "") {
      return GROUND_STATE;
    } else if (label_ == "m") {
      return ISOMERIC_M;
    } else if (label_ == "m1") {
      return ISOMERIC_M1;
    } else if (label_ == "m2") {
      return ISOMERIC_M2;
    } else if (label_ == "m3") {
      return ISOMERIC_M3;
    } else if (label_ == "f") {
      return ISOMERIC_F;
    } else if (label_ == "m1") {
      return ISOMERIC_F1;
    } else if (label_ == "m2") {
      return ISOMERIC_F2;
    } else if (label_ == "m3") {
      return ISOMERIC_F3;
    }
    return ISOMERIC_INVALID;
  }

  datatools::properties & isotope::grab_properties()
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Locked isotope !");
    return _properties_;
  }

  const datatools::properties & isotope::get_properties() const
  {
    return _properties_;
  }

  void isotope::reset()
  {
    _locked_ = false;
    _set_defaults();
    return;
  }

  void isotope::set_default_data()
  {
    datatools::invalidate(_mass_excess_);
    datatools::invalidate(_err_mass_excess_);
    datatools::invalidate(_atomic_mass_);
    datatools::invalidate(_err_atomic_mass_);
    datatools::invalidate(_half_life_);
    datatools::invalidate(_err_half_life_);
    return;
  }

  void isotope::_set_defaults()
  {
    _z_ = chemical_symbol::Z_UNDEFINED;
    _a_ = 0;
    _isomeric_ = ISOMERIC_INVALID;
    _name_.clear();
    set_default_data();
    return;
  }

  bool isotope::is_valid() const
  {
    return _z_ >= 0
      && _z_ <= max_number_of_protons()
      && _a_ >= 1
      && _a_ <= max_number_of_nucleons()
      && _isomeric_ != ISOMERIC_INVALID;
  }

  isotope::isotope()
  {
    _locked_ = false;
    _set_defaults();
    return;
  }

  isotope::isotope(const std::string & name_)
  {
    _locked_ = false;
    _set_defaults();
    set_name(name_);
    return;
  }

  isotope::isotope(const char * name_)
  {
    _locked_ = false;
    _set_defaults();
    set_name(std::string(name_));
    return;
  }

  isotope::isotope(int z_,
                   int a_ ,
                   isomeric_level_type i_,
                   unsigned int build_flags_)
  {
    _locked_ = false;
    _set_defaults();
    set_zai(z_, a_, i_);
    set_name(get_zai_name());
    build(build_flags_);
    return;
  }

  isotope::isotope(const std::string & ch_symbol_,
                   int a_ ,
                   isomeric_level_type i_,
                   unsigned int build_flags_)
  {
    _locked_ = false;
    _set_defaults();
    DT_THROW_IF (! chemical_symbol::symbol_is_tabulated(ch_symbol_),
                 std::logic_error,
                 "Chemical symbol '" << ch_symbol_ << "' is not known!");
    int z = chemical_symbol::z_from_symbol(ch_symbol_);
    set_zai(z, a_, i_);
    set_name(get_zai_name());
    build(build_flags_);
    return;
  }

  isotope::isotope(const char * ch_symbol_,
                   int a_ ,
                   isomeric_level_type i_,
                   unsigned int build_flags_)
  {
    _locked_ = false;
    _set_defaults();
    DT_THROW_IF (! chemical_symbol::symbol_is_tabulated(ch_symbol_),
                 std::logic_error,
                 "Chemical symbol '" << ch_symbol_ << "' is not known!");
    int z = chemical_symbol::z_from_symbol(ch_symbol_);
    set_zai(z, a_, i_);
    set_name(get_zai_name());
    build(build_flags_);
    return;
  }

  isotope::isotope(const std::string & name_,
                   int z_,
                   int a_ ,
                   isomeric_level_type i_,
                   unsigned int build_flags_)
  {
    _locked_ = false;
    _set_defaults();
    set_name(name_);
    set_zai(z_, a_, i_);
    build(build_flags_);
    return;
  }

  isotope::isotope(const char * name_,
                   int z_,
                   int a_ ,
                   isomeric_level_type i_,
                   unsigned int build_flags_)
  {
    _locked_ = false;
    _set_defaults();
    set_name(std::string(name_));
    set_zai(z_, a_, i_);
    build(build_flags_);
    return;
  }

  isotope::isotope(const std::string & name_,
                   const std::string & ch_symbol_,
                   int a_,
                   isomeric_level_type i_,
                   unsigned int build_flags_)
  {
    _locked_ = false;
    _set_defaults();
    set_name(name_);
    DT_THROW_IF (! chemical_symbol::symbol_is_tabulated(ch_symbol_),
                 std::logic_error,
                 "Chemical symbol '" << ch_symbol_ << "' is not known!");
    int z = chemical_symbol::z_from_symbol(ch_symbol_);
    set_zai(z, a_, i_);
    build(build_flags_);
    return;
  }

  isotope::isotope(const char * name_,
                   const char * ch_symbol_,
                   int a_,
                   isomeric_level_type i_,
                   unsigned int build_flags_)
  {
    _locked_ = false;
    _set_defaults();
    set_name(std::string(name_));
    DT_THROW_IF (! chemical_symbol::symbol_is_tabulated(std::string(ch_symbol_)),
                 std::logic_error,
                 "Chemical symbol '" << ch_symbol_ << "' is not known!");
    int z = chemical_symbol::z_from_symbol(ch_symbol_);

    set_zai(z, a_, i_);
    build(build_flags_);
    return;
  }

  isotope::~isotope()
  {
    if (_locked_) {
      reset();
    }
    return;
  }

  const std::string & isotope::get_name() const
  {
    return _name_;
  }

  int isotope::get_z() const
  {
    return _z_;
  }

  int isotope::get_a() const
  {
    return _a_;
  }

  int isotope::get_n() const
  {
    DT_THROW_IF (! is_valid(), std::logic_error,
                 "Isotope is not valid ! Cannot determine the number of neutrons");
    return _a_ - _z_;
  }

  isotope::isomeric_level_type isotope::get_isomeric() const
  {
    return _isomeric_;
  }

  bool isotope::is_ground_state() const
  {
    return _isomeric_ == GROUND_STATE;
  }

  isotope::id isotope::get_id() const
  {
    id iid(_z_, _a_);
    return iid;
  }

  double isotope::get_mass_excess() const
  {
    return _mass_excess_;
  }

  double isotope::get_err_mass_excess() const
  {
    return _err_mass_excess_;
  }

  double isotope::get_atomic_mass() const
  {
    return _atomic_mass_;
  }

  double isotope::get_err_atomic_mass() const
  {
    return _err_atomic_mass_;
  }

  double isotope::get_half_life() const
  {
    return _half_life_;
  }

  double isotope::get_err_half_life() const
  {
    return _err_half_life_;
  }

  void isotope::set_name(const std::string & name_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Locked isotope !");
    _name_ = name_;
    return;
  }

  void isotope::set_zai(int z_, int a_, isomeric_level_type i_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Locked isotope !");
    {
      id iid(z_, a_);
      DT_THROW_IF(! id_is_tabulated(iid), std::logic_error,
                  "Isotope (Z=" << z_ << ",A="
                  << a_ << ") is not tabulated !");
    }
    set_z(z_);
    set_a(a_);
    set_isomeric(i_);
    set_default_data();
    return;
  }

  bool isotope::find_mass_data()
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Locked isotope !");
    id iid(_z_, _a_);
    if (id_is_tabulated(iid)) {
      const record_type & record = table_record_from_id(iid);
      set_mass_excess(record.mx, record.mx_err);
      set_atomic_mass(record.am, record.am_err);
    }
    return true;
  }

  bool isotope::find_decay_data()
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Locked isotope !");
    DT_LOG_WARNING(datatools::logger::PRIO_WARNING, "Unsupported method !");
    return false;
  }

  /*
    {
    is_za_found = true;
    std::string m_str = ame03_line.substr(96, 4);
    std::string m2_str = ame03_line.substr(100, 12);
    std::string m3_str = ame03_line.substr(113, 11);
    //cerr << "DEVEL: find_mass:   m_str=" << m_str << endl;
    //cerr << "DEVEL: find_mass:   m2_str=" << m2_str << endl;
    //cerr << "DEVEL: find_mass:   m3_str=" << m3_str << endl;
    double mass = ame3line_to_double (m_str);
    double mass_default = ame3line_to_double (m2_str) * 1.E-6;
    double error_mass = ame3line_to_double (m3_str) * 1.E-6;
    //cerr << "DEVEL: find_mass:   mass=" << mass << " amu" << endl;
    //cerr << "DEVEL: find_mass:   mass_default=" << mass_default << " amu" << endl;
    //cerr << "DEVEL: find_mass:   error_mass=" << error_mass << " amu" << endl;
    _set_mass_ (mass + mass_default, error_mass);
    }
  */

  const std::string & isotope::get_chemical_symbol() const
  {
    return chemical_symbol::symbol_from_z(_z_);
  }

  std::string isotope::get_zai_name()  const
  {
    std::ostringstream oss;
    if (! is_valid()) {
      oss << "?";
    } else {
      id iid = get_id();
      oss << iid.to_string();
      oss << isomeric_to_label(_isomeric_);
    }
    return oss.str();
  }

  void isotope::set_z(int z_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Locked isotope !");
    DT_THROW_IF(z_ < 1 || z_ > max_number_of_protons(), std::range_error, "Invalid Z value (" << z_ << ") !");
    _z_ = z_;
    return;
  }

  void isotope::set_a(int a_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Locked isotope !");
    DT_THROW_IF(a_ < 1 || a_ > max_number_of_nucleons(), std::range_error, "Invalid A value (" << a_ << ") !");
    _a_ = a_;
    return;
  }

  void isotope::set_isomeric(isomeric_level_type i_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Locked isotope !");
    _isomeric_ = i_;
    return;
  }

  void isotope::set_mass_excess(double mass_excess_, double err_mass_excess_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Locked isotope !");
    DT_THROW_IF (err_mass_excess_ < 0.,
                 std::logic_error,
                 "Invalid mass excess error value : '"
                 << err_mass_excess_ << " '!");
    _mass_excess_ = mass_excess_;
    _err_mass_excess_ = err_mass_excess_;
    return;
  }

  void isotope::set_atomic_mass(double mass_, double err_mass_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Locked isotope !");
    DT_THROW_IF ((mass_ < 0.) || (err_mass_ < 0.),
                 std::logic_error,
                 "Invalid atomic mass (+_ error) values : '" << mass_ << "' '" << err_mass_ << " '!");
    _atomic_mass_ = mass_;
    _err_atomic_mass_ = err_mass_;
    return;
  }

  void isotope::set_half_life(double half_life_, double err_half_life_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Locked isotope !");
    DT_THROW_IF(  half_life_ <= 0. || err_half_life_ < 0. ,
                  std::logic_error,
                  "Invalid half-life value : '" << half_life_  << " +- '" << err_half_life_<< "' !");
    _half_life_ = half_life_;
    _err_half_life_ = err_half_life_;
    return;
  }

  bool isotope::has_mass_excess_data() const
  {
    return datatools::is_valid(_mass_excess_);
  }

  bool isotope::has_atomic_mass_data() const
  {
    return datatools::is_valid(_atomic_mass_);
  }

  bool isotope::has_decay_data() const
  {
    return datatools::is_valid(_half_life_);
  }

  bool isotope::is_stable() const
  {
    return _half_life_ == std::numeric_limits<double>::infinity();
  }

  bool isotope::is_locked() const
  {
    return _locked_;
  }

  void isotope::lock()
  {
    _locked_ = true;
    return;
  }

  void isotope::unlock()
  {
    _locked_ = false;
    return;
  }

  void isotope::build(unsigned int build_flags_)
  {
    DT_THROW_IF(is_locked(), std::logic_error, "Locked isotope !");
    if (build_flags_ & BF_MASS_DATA) {
      find_mass_data();
    }
    if (build_flags_ & BF_DECAY_DATA) {
      find_decay_data();
    }
    if (build_flags_ & BF_LOCK) {
      lock();
    }
    return;
  }

  void isotope::tree_dump(std::ostream & out_,
                          const std::string & title_,
                          const std::string & indent_,
                          bool inherit_) const
  {

    std::string indent;
    if (! indent_.empty()) indent = indent_;
    if (! title_.empty()) {
      out_ << indent << title_ << std::endl;
    }
    out_ << indent << i_tree_dumpable::tag
         << "Name       : '" << get_name() << "'" << std::endl;
    out_ << indent << i_tree_dumpable::tag
         << "Z          : " << get_z() << std::endl;
    out_ << indent << i_tree_dumpable::tag
         << "A          : " << get_a() << std::endl;
    out_ << indent << i_tree_dumpable::tag
         << "Isomeric state : '" << isomeric_to_label(get_isomeric()) << "'" << std::endl;
    out_ << indent << i_tree_dumpable::tag
         << "ZAI name   : '" << get_zai_name() << "'" << std::endl;

    if (has_mass_excess_data()) {
      out_ << indent << i_tree_dumpable::tag
           << "Mass excess : " <<  get_mass_excess() / CLHEP::keV << " +- "
           << get_err_mass_excess() / CLHEP::keV << " keV" << std::endl;
    } else {
      out_ << indent << i_tree_dumpable::tag
           << "Mass excess : " <<  "<no data>" << std::endl;
    }

    if (has_atomic_mass_data()) {
      out_ << indent << i_tree_dumpable::tag
           << "Atomic mass : " <<  get_atomic_mass() << " +- "
           << get_err_atomic_mass() << " [g/mol]" << std::endl;
    }else {
      out_ << indent << i_tree_dumpable::tag
           << "Atomic mass : " <<  "<no data>" << std::endl;
    }

    if (has_decay_data()) {
      if (is_stable()) {
        out_ << indent << i_tree_dumpable::tag << "Stable     : true"   << std::endl;
      } else {
        out_ << indent << i_tree_dumpable::tag << "Unstable   : ";
        out_ << "T1/2 = "
             << get_half_life() / CLHEP::second << " +- "
             << get_err_half_life() / CLHEP::second << " [s]" << std::endl;
      }
    } else {
      out_ << indent << i_tree_dumpable::tag
           << "Decay : " <<  "<no data>"
           << std::endl;
    }

    out_ << indent << i_tree_dumpable::tag << "Properties : ";
    if (_properties_.size() != 0) {
      out_ << std::endl;
      std::ostringstream indent_oss;
      indent_oss << indent;
      indent_oss << i_tree_dumpable::skip_tag;
      _properties_.tree_dump(out_, "", indent_oss.str());
    } else {
      out_ << "<empty>" << std::endl;
    }

    out_ << indent << i_tree_dumpable::inherit_tag(inherit_)
         << "Locked     : " << (is_locked()? "Yes": "No") << std::endl;

    return;
  }

} // end of namespace materials


bool token_to_double(const std::string & token_, double & val_)
{
  datatools::invalidate(val_);
  std::istringstream iss(token_);
  iss >> val_;
  if (!iss) {
    datatools::invalidate(val_);
    return false;
  }
  return true;
}

bool token_to_int(const std::string & token_, int min_, int max_, int & val_)
{
  int val = max_ + 1;
  std::istringstream iss(token_);
  iss >> std::ws >> val;
  if (!iss) {
    return false;
  }
  if (val < min_) {
    val_ = min_ - 1;
    return false;
  }
  if (val > max_) {
    val_ = max_ + 1;
    return false;
  }
  val_ = val;
  return true;
}

bool token_to_string(const std::string & token_, std::string & val_)
{
  std::istringstream iss(token_);
  iss >> val_;
  if (!iss) {
    val_.clear();
    return false;
  }
  return true;
}
