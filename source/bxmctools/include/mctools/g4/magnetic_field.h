// -*- mode: c++ ; -*-
/* magnetic_field.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-06-19
 * Last modified: 2013-03-09
 *
 * License:
 *
 * Description:
 *
 *   G4 interface for magnetic field
 *
 * History:
 *
 */

#ifndef MCTOOLS_G4_MAGNETIC_FIELD_H_
#define MCTOOLS_G4_MAGNETIC_FIELD_H_ 1

#include <map>

#include <boost/cstdint.hpp>

#include <geomtools/smart_id_locator.h>

// G4 stuff:
#include <G4MagneticField.hh>

#include <mctools/g4/loggable_support.h>

namespace datatools {
  class properties;
}

namespace geomtools {
  class manager;
}

namespace emfield {
  class base_electromagnetic_field;
  class electromagnetic_field_manager;
}

namespace mctools {

  namespace g4 {

    class detector_construction;

    class magnetic_field : public G4MagneticField,
                           public loggable_support
    {
    public:

      typedef std::map<uint32_t, double> field_map_type;

      bool is_initialized () const;

      bool is_active () const;

      bool has_name() const;

      void set_name(const std::string & name_);

      const std::string & get_name() const;

      void set_mag_field_check_pos_time(bool);

      bool is_mag_field_check_pos_time() const;

      bool has_mag_field() const;

      void set_mag_field (const emfield::base_electromagnetic_field &);

      const emfield::base_electromagnetic_field & get_mag_field() const;

      magnetic_field ();

      virtual ~magnetic_field ();

      void initialize (const datatools::properties & config_);

      void initialize ();

      void dump (std::ostream & out_ = std::clog) const;

      // G4 interface:
      void GetFieldValue (const double position_[3],
                          double * b_field_) const;

    private:

      bool                                           _initialized_;
      std::string                                    _name_;
      const emfield::base_electromagnetic_field    * _mag_field_;
      bool                                           _mag_field_check_pos_time_;
      geomtools::vector_3d                           _standalone_constant_mag_field_;

      friend class detector_construction;

    };

  } // end of namespace g4

} // end of namespace mctools

#endif // MCTOOLS_G4_MAGNETIC_FIELD_H_

// end of manager.h
