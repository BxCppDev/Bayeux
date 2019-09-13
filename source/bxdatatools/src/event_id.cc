// event_id.cc

// Ourselves:
#include <datatools/event_id.h>

// Standard Library:
#include <sstream>
#include <stdexcept>

// This project:
#include <datatools/exception.h>

// Special backward compatibility support for serialization :
DATATOOLS_SERIALIZATION_EXT_SERIAL_TAG_IMPLEMENTATION(datatools::event_id,"datatools::event_id")
DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_IMPLEMENTATION(datatools::event_id,"datatools::event::event_id")

namespace datatools {

  DATATOOLS_SERIALIZATION_IMPLEMENTATION_ADVANCED(event_id, "datatools::event_id")

  // static
  const int  event_id::INVALID_RUN_NUMBER;
  const int  event_id::ANY_RUN_NUMBER;
  const int  event_id::INVALID_EVENT_NUMBER;
  const int  event_id::ANY_EVENT_NUMBER;
  const char event_id::IO_FORMAT_SEP;
  const char event_id::IO_FORMAT_ANY;
  const char event_id::IO_FORMAT_INVALID;

  event_id::event_id(int event_number_)
    : event_id()
  {
    this->set_event_number(event_number_);
    return;
  }

  event_id::event_id(int run_number_, int event_number_)
  {
    this->set(run_number_, event_number_);
    return;
  }

  void event_id::reset()
  {
    this->set_run_number(INVALID_RUN_NUMBER);
    this->set_event_number(INVALID_EVENT_NUMBER);
    return;
  }

  void event_id::clear()
  {
    reset ();
    return;
  }

  int event_id::get_run_number() const
  {
    return _run_number_;
  }

  int event_id::get_event_number() const
  {
    return _event_number_;
  }

  void event_id::set_run_number(int run_number_) {
    if (run_number_ == ANY_RUN_NUMBER || run_number_ > INVALID_RUN_NUMBER) {
      _run_number_ = run_number_;
    } else {
      _run_number_ = INVALID_RUN_NUMBER;
    }
    return;
  }

  void event_id::set_event_number(int event_number_) {
    if (event_number_ == ANY_EVENT_NUMBER || event_number_ > INVALID_EVENT_NUMBER) {
      _event_number_ = event_number_;
    } else {
      _event_number_ = INVALID_EVENT_NUMBER;
    }
    return;
  }

  void event_id::set(int run_number_, int event_number_)
  {
    this->set_run_number(run_number_);
    this->set_event_number(event_number_);
    return;
  }

  bool event_id::has(int run_number_, int event_number_) const
  {
    if (! is_valid ()) return false;
    if (run_number_ != ANY_RUN_NUMBER) {
      if (_run_number_ != run_number_) return false;
    }
    if (event_number_ != ANY_EVENT_NUMBER) {
      if (_event_number_ != event_number_) return false;
    }
    return true;
  }

  bool event_id::match(int run_number_, int event_number_) const
  {
    return has(run_number_, event_number_);
  }

  bool event_id::is_valid() const
  {
    return (_run_number_ != INVALID_RUN_NUMBER) && (_event_number_ != INVALID_EVENT_NUMBER);
  }

  bool event_id::is_complete() const
  {
    if (_run_number_ <= INVALID_RUN_NUMBER) return false;
    if (_event_number_ <= INVALID_EVENT_NUMBER) return false;
    return true;
  }

  bool event_id::operator==(const event_id & id_) const
  {
    if (!is_valid()) return false;
    if (!id_.is_valid()) return false;
    if (_run_number_ != id_._run_number_) return false;
    if (_event_number_ != id_._event_number_) return false;
    return true;
  }


  bool event_id::operator<(const event_id & id_) const
  {
    if (!is_valid()) return false;
    if (!id_.is_valid()) return false;
    if (_run_number_ < id_._run_number_) return true;
    if (_run_number_ == id_._run_number_) {
      if (_event_number_ < id_._event_number_) return true;
    }
    return false;
  }

  bool event_id::operator>(const event_id & id_) const
  {
    if (!is_valid()) return false;
    if (!id_.is_valid()) return false;
    if (_run_number_ > id_.get_run_number()) return true;
    if (_run_number_ == id_._run_number_) {
      if (_event_number_ > id_._event_number_) return true;
    }
    return false;
  }

  std::string event_id::to_string() const
  {
    std::ostringstream out;
    out << *this;
    return out.str();
  }

  void event_id::from_string(const std::string & word_)
  {
    event_id id;
    std::istringstream in(word_);
    in >> id;
    DT_THROW_IF (!in,std::runtime_error,"Format error in '" << word_ << "' !");
    *this = id;
    return;
  }

  std::ostream & operator<<(std::ostream & out_, const event_id & id_)
  {
    if (id_._run_number_ == event_id::ANY_RUN_NUMBER) {
      out_ << event_id::IO_FORMAT_ANY;
    } else if (id_._run_number_ == event_id::INVALID_RUN_NUMBER) {
      out_ << event_id::IO_FORMAT_INVALID;
    } else {
      out_ << id_._run_number_;
    }
    out_ << event_id::IO_FORMAT_SEP;
    if (id_._event_number_ == event_id::ANY_EVENT_NUMBER) {
      out_ << event_id::IO_FORMAT_ANY;
    } else if (id_._event_number_ == event_id::INVALID_EVENT_NUMBER) {
      out_ << event_id::IO_FORMAT_INVALID;
    } else {
      out_ << id_._event_number_;
    }
    return out_;
  }

  std::istream & operator>>(std::istream & in_, event_id & id_)
  {
    id_.reset();
    int r, e;
    char c = 0;

    c = in_.get();
    if (!in_) {
      return in_;
    }
    if (c == event_id::IO_FORMAT_ANY) {
      r = event_id::ANY_RUN_NUMBER;
    } else if (c == event_id::IO_FORMAT_INVALID) {
      r = event_id::INVALID_RUN_NUMBER;
    } else {
      in_.putback(c);
      in_ >> r;
      if (!in_) {
        in_.setstate(std::ios_base::failbit);
        return in_;
      }
      if (r < 0) {
        in_.setstate(std::ios_base::failbit);
        return in_;
      }
    }

    c = in_.get();
    if (!in_) {
      return in_;
    }
    if (c != event_id::IO_FORMAT_SEP) {
      in_.setstate(std::ios_base::failbit);
      return in_;
    }

    c = in_.get();
    if (!in_) {
      return in_;
    }
    if (c == event_id::IO_FORMAT_ANY) {
      e = event_id::ANY_EVENT_NUMBER;
    } else if (c == event_id::IO_FORMAT_INVALID) {
      e = event_id::INVALID_EVENT_NUMBER;
    } else {
      in_.putback(c);
      in_ >> e;
      if (!in_) {
        in_.setstate(std::ios_base::failbit);
        return in_;
      }
      if (e < 0) {
        in_.setstate(std::ios_base::failbit);
        return in_;
      }
    }
    if (in_) {
      id_.set(r, e);
    }
    return in_;
  }

  void event_id::tree_dump(std::ostream & out_,
                           const std::string & title_,
                           const std::string & indent_,
                           bool inherit_) const
  {
    if (!title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ << i_tree_dumpable::tag
          << "Run number   : " << _run_number_ << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
          << "Event number : " << _event_number_ << std::endl;

    return;
  }

  void event_id::smart_print(std::ostream & out_,
                             const std::string & title_,
                             const std::string & indent_) const
  {
    tree_dump(out_, title_, indent_, false);
    return;
  }

  std::string event_id::to_smart_string(const std::string & title_,
                                        const std::string & indent_) const
  {
    std::ostringstream oss;
    this->smart_print(oss, title_, indent_);
    return oss.str();
  }

  void event_id::dump() const {
    this->tree_dump(std::clog);
    return;
  }

} // end of namespace datatools
