/// \file mctools/random_event_sequence_model.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2017-09-12
 * Last modified: 2017-09-12
 *
 * License: GPL3
 *
 * Description:
 *
 *   A model to build sequences of independant decay events
 *   random distributed in time.
 *
 */

#ifndef MCTOOLS_RANDOM_EVENT_SEQUENCE_MODEL_H
#define MCTOOLS_RANDOM_EVENT_SEQUENCE_MODEL_H

// Standard library:
#include <iostream>
#include <string>
#include <list>

// Bayeux/datatools:
#include <datatools/i_tree_dump.h>

// Bayeux/mygsl:
#include <mygsl/rng.h>

// Bayeux/events:
#include <events/base_event.h>

namespace mctools {

  /// \brief
  class random_event_sequence_model
    : public datatools::i_tree_dumpable
  {

  public:

    /// \brief Source info
    struct source_info;
    // {
    //   std::string _decay_label_;
    //   std::string _vertex_label_;
    //   double      _activity_;
    //   double      _prob_;
    // };

    typedef std::vector<source_info> rcm_col_type;

    random_event_sequence_model();

    ~random_event_sequence_model();

    void set_random(mygsl::rng & rng_);

    /// Check if event's time is generated relatively to the previous event
    bool is_relative_time() const;

    /// Force event's time to be generated relatively to the previous event
    void set_relative_time(const bool rt_);

    int add_source(const std::string & decay_label_,
                   const std::string & vertex_label_,
                   const double activity_);

    void set_time_window(const double tstart_, const double tstop_);

    void set_events_from(const std::size_t number_of_events_,
                         const double tstart_);

    std::size_t get_number_of_sources() const;

    void get_source_info(const unsigned int index_,
                         std::string & decay_label_,
                         std::string & vertex_label_,
                         double & activity_) const;

    bool is_time_window_mode() const;

    bool is_number_of_events_mode() const;

    void set_target(std::list<events::base_event> &);

    bool is_valid() const;

    bool is_initialized() const;

    void reset();

    void initialize();

    /// Smart print
    virtual void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const;

    std::size_t generate_sequence(const std::size_t number_ = 0);

  private:

    void _compute_();

    bool _shoot_one_more_();

  private:

    bool _initialized_ = false;
    mygsl::rng * _random_ = nullptr;
    bool         _relative_time_ = false;
    rcm_col_type _sources_;
    double       _tstart_;
    double       _tstop_;
    std::size_t  _number_of_events_ = 0;

    double       _effective_activity_;
    double       _effective_lifetime_;
    std::size_t  _current_number_of_events_ = 0;
    double       _current_time_;

    std::list<events::base_event> * _target_;

  };

} // end of namespace mctools

#endif // MCTOOLS_RANDOM_EVENT_SEQUENCE_MODEL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
