/// \file mygsl/interval.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-10-02
 * Last modified: 2013-04-11
 *
 * License:
 *
 *
 * Description:
 *
 *   A simple interval [min:max] for double precision values
 *
 *   Supported bounds schemes are :
 * [0:1]
 * (0:1]
 * [0:1)
 * ( :1]
 * [0: )
 * [0: )
 * ( : )
 * [!]
 *
 * History:
 *
 *
 *
 */

#ifndef MYGSL_INTERVAL_H
#define MYGSL_INTERVAL_H 1

// Standard library:
#include <iostream>

// Third party:
// - Bayeux/datatools :
#include <datatools/i_serializable.h>

namespace mygsl {

  /// \brief An interval with real bounds
  class interval : public datatools::i_serializable
    {
    public:

      static const double NO_MIN_VALUE;
      static const double NO_MAX_VALUE;
      static const double NO_VALUE;
      static const double DEFAULT_EPS;
      static const double AUTO_EPS;
      static const double auto_eps;

      static const char IO_SEP;
      static const char IO_EMPTY;
      static const char IO_OPEN_INCL;
      static const char IO_OPEN_EXCL;
      static const char IO_CLOSE_INCL;
      static const char IO_CLOSE_EXCL;

      static const bool included;
      static const bool excluded;

    public:

      /// Default constructor
      interval ();

      /// Constructor
      interval (double min_, double max_, double eps_ = AUTO_EPS);

      /// Constructor
      interval (double min_, bool min_include_,
                double max_, bool max_include_,
                double eps_ = AUTO_EPS);

      /// Destructor
      ~interval () override;

      void remove_min ();

      void remove_max ();

      void reset ();

      void set_min_included (bool inc_);

      void set_max_included (bool inc_);

      bool is_min_included () const;

      bool is_max_included () const;

      bool is_valid () const;

      bool has_min () const;

      bool has_max () const;

      double get_min () const;

      double get_max () const;

      double get_eps () const;

      bool union_with (const interval &);

      bool intercept_with (const interval &);

      bool is_in (double x_) const;

      bool is_in_safe (double x_) const;

      bool is_in_min_unsafe (double x_) const;

      bool is_in_max_unsafe (double x_) const;

      interval get_safe_interval (double eps_ = AUTO_EPS) const;

      void set_eps (double eps_);

      void set (double min_, double max_, double eps_ = AUTO_EPS);

      bool is_no_limit () const;

      bool is_empty () const;

      void open ();

      void close ();

      bool intersection_with (const interval & i_) const;

      static interval make_empty ();

      static interval make_no_limit (double eps_ = AUTO_EPS);

      static interval make_neighbourhood (double value_, double delta_, bool included_ = excluded);

      static interval make_min (double min_, bool min_include_ = excluded,
                                double eps_ = AUTO_EPS);

      static interval make_max (double max_, bool max_include_ = excluded,
                                double eps_ = AUTO_EPS);

      static interval make_min_max_included (double min_,
                                             double max_,
                                             double eps_ = AUTO_EPS);

      static interval make_min_max_excluded (double min_,
                                             double max_,
                                             double eps_ = AUTO_EPS);

      static interval make_min_max (double min_, bool min_include_,
                                    double max_, bool max_include_,
                                    double eps_ = AUTO_EPS);

      friend std::ostream & operator<< (std::ostream & out_, const interval & i_);

      friend std::istream & operator>> (std::istream & in_, interval & i_);

    private:

      bool   _min_included_; //!< Flag to include the min bound
      bool   _max_included_; //!< Flag to include the max bound
      double _min_; //!< Min bound
      double _max_; //!< Max bound
      double _eps_; //!< Epsilon

      DATATOOLS_SERIALIZATION_DECLARATION()

    };

} // end of namespace mygsl

#endif // MYGSL_INTERVAL_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */
