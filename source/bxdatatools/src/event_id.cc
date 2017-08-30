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

  DATATOOLS_SERIALIZATION_IMPLEMENTATION_ADVANCED(event_id,"datatools::event_id")

  // static
  const int  event_id::INVALID_RUN_NUMBER;
  const int  event_id::ANY_RUN_NUMBER;
  const int  event_id::INVALID_EVENT_NUMBER;
  const int  event_id::ANY_EVENT_NUMBER;
  const char event_id::IO_FORMAT_SEP;
  const char event_id::IO_FORMAT_ANY;
  const char event_id::IO_FORMAT_INVALID;

  event_id::event_id()
    : run_number_(INVALID_RUN_NUMBER),
      event_number_(INVALID_EVENT_NUMBER) {}


  event_id::event_id(int a_event_number)
    : run_number_(INVALID_RUN_NUMBER),
      event_number_(INVALID_EVENT_NUMBER) {
    this->set_event_number(a_event_number);
  }


  event_id::event_id(int a_run_number, int a_event_number) {
    this->set(a_run_number, a_event_number);
  }


  event_id::~event_id() {}


  void event_id::reset() {
    this->set_run_number(INVALID_RUN_NUMBER);
    this->set_event_number(INVALID_EVENT_NUMBER);
  }


  void event_id::clear() {
    reset ();
  }


  int event_id::get_run_number() const {
    return run_number_;
  }


  int event_id::get_event_number() const {
    return event_number_;
  }


  void event_id::set_run_number(int a_run_number) {
    if (a_run_number == ANY_RUN_NUMBER || a_run_number > INVALID_RUN_NUMBER) {
      run_number_ = a_run_number;
    } else {
      run_number_ = INVALID_RUN_NUMBER;
    }
  }


  void event_id::set_event_number(int a_event_number) {
    if (a_event_number == ANY_EVENT_NUMBER || a_event_number > INVALID_EVENT_NUMBER) {
      event_number_ = a_event_number;
    } else {
      event_number_ = INVALID_EVENT_NUMBER;
    }
  }


  void event_id::set(int a_run_number, int a_event_number) {
    this->set_run_number(a_run_number);
    this->set_event_number(a_event_number);
  }


  bool event_id::has(int a_run_number, int a_event_number) const {
    if (! is_valid ()) return false;
    if (a_run_number != ANY_RUN_NUMBER) {
      if (run_number_ != a_run_number) return false;
    }
    if (a_event_number != ANY_EVENT_NUMBER) {
      if (event_number_ != a_event_number) return false;
    }
    return true;
  }

  bool event_id::match(int a_run_number, int a_event_number) const {
    return has(a_run_number, a_event_number);
  }

  bool event_id::is_valid() const {
    return (run_number_ != INVALID_RUN_NUMBER) && (event_number_ != INVALID_EVENT_NUMBER);
  }

  bool event_id::is_complete() const
  {
    if (run_number_ <= INVALID_RUN_NUMBER) return false;
    if (event_number_ <= INVALID_EVENT_NUMBER) return false;
    return true;
  }

  bool event_id::operator==(const event_id& a_id) const {
    if (!is_valid()) return false;
    if (!a_id.is_valid()) return false;
    if (run_number_ != a_id.run_number_) return false;
    if (event_number_ != a_id.event_number_) return false;
    return true;
  }


  bool event_id::operator<(const event_id& a_id) const {
    if (!is_valid()) return false;
    if (!a_id.is_valid()) return false;
    if (run_number_ < a_id.run_number_) return true;
    if (run_number_ == a_id.run_number_) {
      if (event_number_ < a_id.event_number_) return true;
    }
    return false;
  }


  bool event_id::operator>(const event_id & a_id) const {
    if (!is_valid()) return false;
    if (!a_id.is_valid()) return false;
    if (run_number_ > a_id.get_run_number()) return true;
    if (run_number_ == a_id.run_number_) {
      if (event_number_ > a_id.event_number_) return true;
    }
    return false;
  }


  std::string event_id::to_string() const {
    std::ostringstream out;
    out << *this;
    return out.str();
  }


  void event_id::from_string(const std::string & a_word) {
    event_id id;
    std::istringstream in(a_word);
    in >> id;
    DT_THROW_IF (!in,std::runtime_error,"Format error in '" << a_word << "' !");
    *this = id;
  }


  std::ostream& operator<<(std::ostream & a_out, const event_id & a_id) {
    if (a_id.run_number_ == event_id::ANY_RUN_NUMBER) {
      a_out << event_id::IO_FORMAT_ANY;
    } else if (a_id.run_number_ == event_id::INVALID_RUN_NUMBER) {
      a_out << event_id::IO_FORMAT_INVALID;
    } else {
      a_out << a_id.run_number_;
    }
    a_out << event_id::IO_FORMAT_SEP;
    if (a_id.event_number_ == event_id::ANY_EVENT_NUMBER) {
      a_out << event_id::IO_FORMAT_ANY;
    } else if (a_id.event_number_ == event_id::INVALID_EVENT_NUMBER) {
      a_out << event_id::IO_FORMAT_INVALID;
    } else {
      a_out << a_id.event_number_;
    }
    return a_out;
  }


  std::istream& operator>>(std::istream& a_in, event_id & a_id) {
    a_id.reset();
    int r, e;
    char c = 0;

    c = a_in.get();
    if (!a_in) {
      return a_in;
    }
    if (c == event_id::IO_FORMAT_ANY) {
      r = event_id::ANY_RUN_NUMBER;
    } else if (c == event_id::IO_FORMAT_INVALID) {
      r = event_id::INVALID_RUN_NUMBER;
    } else {
      a_in.putback(c);
      a_in >> r;
      if (!a_in) {
        a_in.setstate(std::ios_base::failbit);
        return a_in;
      }
      if (r < 0) {
        a_in.setstate(std::ios_base::failbit);
        return a_in;
      }
    }

    c = a_in.get();
    if (!a_in) {
      return a_in;
    }
    if (c != event_id::IO_FORMAT_SEP) {
      a_in.setstate(std::ios_base::failbit);
      return a_in;
    }

    c = a_in.get();
    if (!a_in) {
      return a_in;
    }
    if (c == event_id::IO_FORMAT_ANY) {
      e = event_id::ANY_EVENT_NUMBER;
    } else if (c == event_id::IO_FORMAT_INVALID) {
      e = event_id::INVALID_EVENT_NUMBER;
    } else {
      a_in.putback(c);
      a_in >> e;
      if (!a_in) {
        a_in.setstate(std::ios_base::failbit);
        return a_in;
      }
      if (e < 0) {
        a_in.setstate(std::ios_base::failbit);
        return a_in;
      }
    }
    if (a_in) {
      a_id.set(r, e);
    }
    return a_in;
  }


  void event_id::tree_dump(std::ostream& a_out,
                           const std::string& a_title,
                           const std::string& a_indent,
                           bool a_inherit) const {
    std::string indent;

    if (!a_indent.empty()) indent = a_indent;

    if (!a_title.empty()) {
      a_out << indent << a_title << std::endl;
    }
    a_out << indent << i_tree_dumpable::tag
          << "Run number   : " << run_number_ << std::endl;
    a_out << indent << i_tree_dumpable::inherit_tag(a_inherit)
          << "Event number : " << event_number_ << std::endl;
    return;
  }


  void event_id::smart_print(std::ostream& out,
                             const std::string& title,
                             const std::string& indent) const
  {
    tree_dump(out, title, indent, false);
    return;
  }

  std::string event_id::to_smart_string(const std::string& title,
                                        const std::string& indent) const
  {
    std::ostringstream oss;
    this->smart_print(oss,title,indent);
    return oss.str();
  }

  void event_id::dump() const {
    this->tree_dump(std::clog);
    return;
  }

} // end of namespace datatools
