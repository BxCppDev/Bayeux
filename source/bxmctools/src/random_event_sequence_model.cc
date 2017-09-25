// random_event_sequence_model.cc

// Ourselves:
#include <mctools/random_event_sequence_model.h>

// Standard library:
#include <iostream>
#include <iomanip>

// Third party:
// Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/exception.h>

namespace mctools {

  /// \brief Source info
  struct random_event_sequence_model::source_info
  {
    std::string _decay_label_;
    std::string _vertex_label_;
    double      _activity_;
    double      _prob_;
  };

  random_event_sequence_model::random_event_sequence_model()
  {
    datatools::invalidate(_tstart_);
    datatools::invalidate(_tstop_);
    datatools::invalidate(_current_time_);
    datatools::invalidate(_effective_lifetime_);
    datatools::invalidate(_effective_activity_);
    return;
  }

  random_event_sequence_model::~random_event_sequence_model()
  {
    return;
  }

  void random_event_sequence_model::set_target(std::list<events::base_event> & t_)
  {
    _target_ = &t_;
    _target_->clear();
    return;
  }

  bool random_event_sequence_model::is_time_window_mode() const
  {
    if (!datatools::is_valid(_tstart_)) return false;
    if (!datatools::is_valid(_tstop_)) return false;
    return true;
  }

  bool random_event_sequence_model::is_number_of_events_mode() const
  {
    if (!datatools::is_valid(_tstart_)) return false;
    if (datatools::is_valid(_tstop_)) return false;
    if (_number_of_events_ <= 0) return false;
    return true;
  }

  bool random_event_sequence_model::is_valid() const
  {
    if (!datatools::is_valid(_tstart_)) return false;
    if (!datatools::is_valid(_tstop_)) return false;
    if(_sources_.size() == 0) return false;
    return true;
  }

  bool random_event_sequence_model::is_initialized() const
  {
    return _initialized_;
  }

  void random_event_sequence_model::initialize()
  {
    _current_number_of_events_ = 0;
    _current_time_ = _tstart_;
    _compute_();
    _initialized_ = true;
    return;
  }

  void random_event_sequence_model::reset()
  {
    _initialized_ = false;
    _sources_.clear();
    datatools::invalidate(_tstart_);
    datatools::invalidate(_tstop_);
    _number_of_events_ = 0;
    datatools::invalidate(_current_time_);
    datatools::invalidate(_effective_lifetime_);
    _current_number_of_events_ = 0;
    return;
  }

  void random_event_sequence_model::set_random(mygsl::rng & rng_)
  {
    _random_ = &rng_;
    return;
  }

  std::size_t random_event_sequence_model::get_number_of_sources() const
  {
    return _sources_.size();
  }

  int random_event_sequence_model::add_source(const std::string & decay_label_,
                                              const std::string & vertex_label_,
                                              const double activity_)
  {
    int idx = -1;
    source_info sinfo;
    sinfo._decay_label_ = decay_label_;
    sinfo._vertex_label_ = vertex_label_;
    sinfo._activity_ = activity_;
    sinfo._prob_ = datatools::invalid_real();
    idx = (int) _sources_.size();
    _sources_.push_back(sinfo);
    return idx;
  }

  void random_event_sequence_model::get_source_info(const unsigned int index_,
                                                    std::string & decay_label_,
                                                    std::string & vertex_label_,
                                                    double & activity_) const
  {
    // DT_THROW_IF(is_initialized(),
    //             std::logic_error,
    //             "Event sources are not yet defined!");
    DT_THROW_IF(index_ >= _sources_.size(),
                std::range_error,
                "Invalid source index [" << index_ << "]!");
    const source_info & sinfo = _sources_[index_];
    decay_label_ = sinfo._decay_label_;
    vertex_label_ = sinfo._vertex_label_;
    activity_ = sinfo._activity_;
    return;
  }

  void random_event_sequence_model::set_events_from(const std::size_t number_of_events_,
                                                    const double tstart_)
  {
    DT_THROW_IF(!datatools::is_valid(tstart_),
                std::logic_error,
                "Invalid start time!");
    _tstart_ = tstart_;
    datatools::invalidate(_tstop_);
    _number_of_events_ = number_of_events_;
    return;
  }

  void random_event_sequence_model::set_time_window(const double tstart_,
                                                    const double tstop_)
  {
    DT_THROW_IF(!datatools::is_valid(tstart_),
                std::logic_error,
                "Invalid start time!");
    DT_THROW_IF(!datatools::is_valid(tstop_),
                std::logic_error,
                "Invalid stop time!");
    DT_THROW_IF(tstart_ >= tstop_,
                std::logic_error,
                "Invalid time range!");
    _tstart_ = tstart_;
    _tstop_ = tstop_;
    _number_of_events_ = 0;
    return;
  }

  bool random_event_sequence_model::is_relative_time() const
  {
    return _relative_time_;
  }

  void random_event_sequence_model::set_relative_time(const bool rt_)
  {
    _relative_time_ = rt_;
    return;
  }

  void random_event_sequence_model::_compute_()
  {
    double s = 0.0;
    for (std::size_t i = 0; i < _sources_.size(); i++) {
      const source_info & sinfo = _sources_[i];
      s += sinfo._activity_;
    }
    double p = 0.0;
    for (std::size_t i = 0; i < _sources_.size(); i++) {
      source_info & sinfo = _sources_[i];
      sinfo._prob_ = p + sinfo._activity_ / s;
      p = sinfo._prob_;
      std::clog << "[debug] p = " << p << std::endl;
    }
    _effective_activity_ = s;
    _effective_lifetime_ = 1. / _effective_activity_;
    return;
  }

  bool random_event_sequence_model::_shoot_one_more_()
  {
    bool success(true);
    double prob = _random_->uniform();
    int isource = -1;
    for (std::size_t i = 0; i < _sources_.size(); i++) {
      const source_info & sinfo = _sources_[i];
      if (prob < sinfo._prob_) {
        isource = i;
        break;
      }
    }
    if (isource == -1) {
      success = false;
    } else {
      double decay_time = _random_->exponential(_effective_lifetime_);
      _current_time_ += decay_time;
      if (success) {
        if (is_time_window_mode()) {
          if (_current_time_ > _tstop_) {
            success = false;
          }
        } else {
          _current_number_of_events_++;
          if (is_number_of_events_mode() && (_current_number_of_events_ > _number_of_events_)) {
            success = false;
          }
        }
        if (success) {
          events::base_event ev;
          ev.set_category(isource);
          if (_relative_time_) {
            ev.set_time(decay_time);
          } else {
            ev.set_time(_current_time_);
          }
          _target_->push_back(ev);
        }
      }
    }
    return success;
  }

  std::size_t random_event_sequence_model::generate_sequence(const std::size_t number_)
  {
    std::size_t nev = 0;
    while (true) {
      if (_shoot_one_more_()) {
        nev++;
        //events::base_event & ev =
        //  _target_->back();
      } else {
        break;
      }
      if (number_ > 0) {
        if (nev == number_) {
          break;
        }
      }
    }
    return nev;
  }

  void random_event_sequence_model::tree_dump(std::ostream & out_,
                                              const std::string & title_,
                                              const std::string & indent_,
                                              bool inherit_) const
  {
    if (!title_.empty()) out_ << indent_ << title_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Sources : "
         << _sources_.size() << std::endl;
    for (std::size_t i = 0; i < _sources_.size(); i++) {
      const source_info & sinfo = _sources_[i];
      out_ << indent_ << datatools::i_tree_dumpable::skip_tag;
      if (i + 1 == _sources_.size()) {
        out_ << datatools::i_tree_dumpable::last_tag;
      } else {
        out_ << datatools::i_tree_dumpable::tag;
      }
      out_ << '[' << i  << "] "
           << sinfo._decay_label_ << "@" << sinfo._vertex_label_ << " : "
           << sinfo._activity_ / CLHEP::becquerel << " Bq"
           << std::endl;
    }

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Start time : "
         << std::setprecision(15) << _tstart_ / CLHEP::second  << " s" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Stop time : "
         << std::setprecision(15) << _tstop_ / CLHEP::second  << " s" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Number of events : "
         << _number_of_events_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Relative time : "
         << std::boolalpha << _relative_time_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Effective lifetime : "
         << _effective_lifetime_ / CLHEP::second << " s" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Initialized  : " << std::boolalpha << is_initialized() << std::endl;

    return;
  }

} // end of namespace mctools
