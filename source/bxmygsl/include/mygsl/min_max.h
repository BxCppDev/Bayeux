//// \file mygsl/min_max.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-10-02
 * Last modified: 2014-11-18
 *
 * Description:
 *
 *   An algorithm that computes the min and max values from
 *   a list of numbers.
 *
 */

#ifndef MYGSL_MIN_MAX_H
#define MYGSL_MIN_MAX_H 1


// Third Party:
// - Bayeux/datatools
#include <datatools/i_tree_dump.h>

namespace mygsl {

  /// \brief Record the min and max values from a set of values
  class min_max : public datatools::i_tree_dumpable
  {
  public:

    /// Check validity
    bool is_valid() const;

    /// Return the minimum value
    double get_min() const;

    /// Return the maximum value
    double get_max() const;

    /// Return the median value
    double get_median() const;

    /// Return the width value
    double get_width() const;

    /// Return the half width value
    double get_half_width() const;

    /// Return the index of the minimum value
    int get_min_index() const;

    /// Return the index of the maximum value
    int get_max_index() const;

    /// Return the value counter
    int get_counter() const;

    /// Default constructor
    min_max();

    /// Reset
    void reset();

    /// Add a value
    void add(double value_);

    /// Smart printing
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

  private:

    int    _counter_;   //!< Value counter
    double _min_;       //!< Minimum value
    int    _min_index_; //!< Index of the minimum value
    double _max_;       //!< Maximum value
    int    _max_index_; //!< Index of the maximum value

  };

} // end of namespace mygsl

#endif // MYGSL_MIN_MAX_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */
