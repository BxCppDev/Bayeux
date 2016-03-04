/// \file dummy_vg.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-11-24
 * Last modified: 2015-11-24
 *
 * Description:
 *   A dummy vertex/time generator
 */

#ifndef GENVTX_TESTING_DUMMY_VG_H
#define GENVTX_TESTING_DUMMY_VG_H 1

// Standard library:
#include <iostream>

// Third party:
#include <datatools/exception.h>
#include <datatools/clhep_units.h>
#include <mygsl/rng.h>

// This project:
#include <genvtx/i_vertex_generator.h>

namespace datatools {
  class properties;
}

namespace genvtx {

  namespace testing {

    //! \brief A dummy vertex/time generator
    class dummy_vg : public genvtx::i_vertex_generator
    {
    public:

      /// Constructor
      dummy_vg()
      {
        _set_time_generator(true); // This indicates the generator also generates decay time
        _length_ = datatools::invalid_real();
        _tau_ = datatools::invalid_real();
        return;
      }

      /// Destructor
      virtual ~dummy_vg()
      {
        if (is_initialized()) {
          reset();
        }
        return;
      }

      // Set the length of the random decay Z-segment
      void set_length(double l_)
      {
        DT_THROW_IF(l_ <= 0.0, std::logic_error, "Invalid length!");
        _length_ = l_;
        return;
      }

      // Set the tau/mean life time of the random decay
      void set_tau(double t_)
      {
        DT_THROW_IF(t_ <= 0.0, std::logic_error, "Invalid tau!");
        _tau_ = t_;
        return;
      }

      /// Initialization
      virtual void initialize(const ::datatools::properties & config_,
                              ::datatools::service_manager &,
                              ::genvtx::vg_dict_type &)
      {
        double default_length_unit = CLHEP::mm;
        double default_time_unit = CLHEP::second;

        if (!datatools::is_valid(_length_)) {
          if (config_.has_key("length")) {
            double length = config_.fetch_real("length");
            if (!config_.has_explicit_unit("length")) {
              length *= default_length_unit;
            }
            set_length(length);
          }
        }

        if (!datatools::is_valid(_tau_)) {
          if (config_.has_key("tau")) {
            double tau = config_.fetch_real("tau");
            if (!config_.has_explicit_unit("tau")) {
              tau *= default_time_unit;
            }
            set_tau(tau);
          }
        }

        DT_THROW_IF(!datatools::is_valid(_length_), std::logic_error, "Missing length!");
        DT_THROW_IF(!datatools::is_valid(_tau_), std::logic_error, "Missing tau!");
        return;
      }

      /// Reset
      virtual void reset()
      {
        _length_ = datatools::invalid_real();
        _tau_ = datatools::invalid_real();
        return;
      }

      /// Check initialization status
      virtual bool is_initialized() const
      {
        return datatools::is_valid(_length_) && datatools::is_valid(_tau_) ;
      }

      /// Smart print
      void tree_dump(std::ostream & out_ = std::clog,
                     const std::string & title_ = "",
                     const std::string & indent_ = "",
                     bool inherit_ = false) const
      {
        i_vertex_generator::tree_dump (out_, title_, indent_, true);
        out_ << indent_ << datatools::i_tree_dumpable::tag;
        out_ << "Length : " << _length_ / CLHEP::cm << " cm" << std::endl;
        out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_);
        out_ << "Tau    : " << _tau_ / CLHEP::second << " s" << std::endl;
        return;
      }

    protected :

      /// Randomize vertex/time
      virtual void _shoot_vertex_and_time(::mygsl::rng & random_, ::geomtools::vector_3d & vertex_, double & time_)
      {
        double x(0.0);
        double y(0.0);
        double z = random_.flat(0.0 * CLHEP::cm, _length_);
        vertex_.set(x, y, z);
        double tau = _tau_; //1.0 * CLHEP::second;
        time_ = random_.exponential(tau);
        return;
      }

    private:

      double _length_; //!< Length
      double _tau_;    //!< Tau

      /// Registration macro
      /// @arg dummy_vg the class to be registered
      GENVTX_VG_REGISTRATION_INTERFACE(dummy_vg)

    };

    GENVTX_VG_REGISTRATION_IMPLEMENT(dummy_vg, "genvtx::testing::dummy_vg")

  } // end of namespace testing

} // end of namespace genvtx

#endif // GENVTX_TESTING_DUMMY_VG_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
