/// \file mygsl/best_value.h

#ifndef MYGSL_BEST_VALUE_H
#define MYGSL_BEST_VALUE_H 1

// Standard library:
#include <iostream>

// Third party:
// - Bayeux/datatools:
#include <datatools/i_serializable.h>

/// \brief Top-level namespace of the Bayeux/mygsl module library
namespace mygsl {

  /// \brief A data structure representing a numeric value and its associated error
  class best_value : public datatools::i_serializable
    {
    public:

      operator double () const;
      void set_value (double value_);
      void set_error_low (double error_low_);
      void set_error_high (double error_lhigh_);
      void set_confidence_level (double confidence_level_);

      bool has_errors () const;
      bool is_symmetric_error () const;
      bool has_value () const;
      bool has_confidence_level () const;

      void reset ();
      void unset_errors ();
      void unset_value ();
      void unset_confidence_level ();

      double get_value () const;
      double get_min_value () const;
      double get_max_value () const;
      double get_error_low () const;
      double get_error_high () const;
      double get_confidence_level () const;

      best_value ();

      best_value (double value_);

      best_value (double value_,
                  double error_);

      best_value (double value_,
                  double error_,
                  double CL_);

      best_value (double value_,
                  double error_low_,
                  double error_high_,
                  double CL_);

      ~best_value () override;

    private:

      double _value_;
      double _error_low_;
      double _error_high_;
      double _confidence_level_;

      DATATOOLS_SERIALIZATION_DECLARATION()

    };

  std::ostream & operator<< (std::ostream & out_, const best_value & bv_);

} // namespace mygsl

#endif // MYGSL_BEST_VALUE_H

// Local Variables:
// mode: c++
// coding: utf-8
// End:
