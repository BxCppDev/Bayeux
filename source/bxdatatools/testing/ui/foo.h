//! \file  ui/foo.h
//! \brief A dummy class for UI testing
//
// Copyright (c) 2015-2016 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of datatools.
//
// datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools. If not, see <http://www.gnu.org/licenses/>.

#ifndef DATATOOLS_TESTING_UI_FOO_H
#define DATATOOLS_TESTING_UI_FOO_H

#include <vector>
#include <datatools/utils.h>
#include <datatools/clhep_units.h>
#include <datatools/exception.h>
#include <datatools/logger.h>
#ifndef Q_MOC_RUN
#include <datatools/reflection_interface.h>
#endif // Q_MOC_RUN

//! \brief A dummy object
class foo
{
public:

  //! \brief Nested enumeration type
  enum dummy_type {
    none  = 0,
    one   = 1,
    two   = 2,
    three = 3
  };

private:
  std::string _name_; ///< String
  bool   _flag_  = false; ///< Boolean
  int    _value_ = 0;    ///< Integer
  double _x_     = 3.14; ///< Dimensionless real
  double _mass_  = -1.0; ///< Real with explicit "mass" dimension
  double _time_  = -1.0; ///< Real with implicit "time" dimension (ms)
  std::vector<int> _counts_; ///< Vector of maximum 3 elements
  dummy_type _b_; ///< Enumeration
  datatools::logger::priority _logging_; ///< Logging priority

public:

  foo()
  {
    _flag_ = false;
    _value_ = 0;
    _x_    = 3.14;
    _mass_ = -1.0;
    _time_ = -1.0;
    _b_ = none;
    _logging_ = datatools::logger::PRIO_FATAL;
    return;
  }

  void reset()
  {
    _name_.clear();
    _flag_ = false;
    _value_ = 0;
    _x_    = 3.14;
    _mass_ = -1.0;
    _time_ = -1.0;
    _counts_.clear();
    _b_ = none;
    _logging_ = datatools::logger::PRIO_FATAL;
   return;
  }

  const std::string & get_name() const
  {
    return _name_;
  }

  void set_name(const std::string & name_)
  {
    _name_ = name_;
    return;
  }

  datatools::logger::priority get_logging() const
  {
    return _logging_;
  }

  void set_logging(datatools::logger::priority logging_)
  {
    _logging_ = logging_;
    return;
  }

  bool is_flag() const
  {
    return _flag_;
  }

  void set_flag(bool flag_)
  {
    _flag_ = flag_;
    return;
  }

  int get_value() const
  {
    return _value_;
  }

  void set_value(int value_)
  {
    _value_ = value_;
    return;
  }

  double get_x() const
  {
    return _x_;
  }

  void set_x(double x_)
  {
    _x_ = x_;
    return;
  }

  dummy_type get_b() const
  {
    return _b_;
  }

  void set_b(dummy_type b_)
  {
    _b_ = b_;
    return;
  }

  double get_mass() const
  {
    if (_mass_ <= 0.0) return datatools::invalid_real();
    return _mass_;
  }

  void set_mass(double mass_)
  {
    DT_THROW_IF(mass_ <= 0.0, std::domain_error, "Invalid negative mass!");
    _mass_ = mass_;
    return;
  }

  double get_time() const
  {
    if (_time_ <= 0.0) return datatools::invalid_real();
    return _time_;
  }

  void set_time(double time_)
  {
    DT_THROW_IF(time_ <= 0.0, std::domain_error, "Invalid negative time!");
    _time_ = time_;
    return;
  }

  void zero()
  {
    set_value(0);
    return;
  }

  void test()
  {
    return;
  }

  void incr()
  {
    _value_++;
    return;
  }

  void decr()
  {
    _value_--;
    return;
  }

  void set_sum(const std::vector<int> & values_)
  {
    std::cerr << "DEVEL: foo::set_sum: Entering...\n";
    int sum = 0;
    for (int i = 0; i < (int) values_.size(); i++) {
      sum += values_[i];
    }
    set_value(sum);
    std::cerr << "DEVEL: foo::set_sum: sum=" << sum << "\n";
    std::cerr << "DEVEL: foo::set_sum: Exiting.\n";
    return;
  }

  int get_count(int index_) const
  {
    DT_THROW_IF(index_ < 0 || index_ >= _counts_.size(),
                std::range_error, "Invalid count index [" << index_ << "] !");
    return _counts_[index_];
  }

  void add_count(int value_)
  {
    if (_counts_.size() < 3) {
      _counts_.push_back(value_);
    } else {
      DT_THROW(std::range_error, "Full counts!");
    }
  }

  void print() const
  {
    std::cout << *this << std::endl;
  }

  friend std::ostream & operator<<(std::ostream & out_, const foo & f_)
  {
    out_ << "foo={"
         << "name='" << f_._name_ << "'"
         << ",flag=" << f_._flag_
         << ",value=" << f_._value_;
    out_ << ",x=";
    if (!datatools::is_valid(f_._x_)) {
      out_ << "<invalid>";
    } else {
      out_ << f_._x_;
    }
    out_ << ",mass=";
    if (!datatools::is_valid(f_._mass_) || f_._mass_ <= 0.0) {
      out_ << "<invalid>";
    } else {
      out_ << f_._mass_ / CLHEP::mg << " mg";
    }
    out_ << ",time=";
    if (!datatools::is_valid(f_._time_) || f_._time_ <= 0.0) {
      out_ << "<invalid>";
    } else {
      out_<< f_._time_ << " ms";
    }
    out_ << ",counts=";
    if (f_._counts_.size()) {
      out_ << "(";
      for (std::size_t i = 0; i < 3; i++) {
        if (i > 0) {
          out_ << ",";
        }
        if (i >= f_._counts_.size()) {
          out_ << "<unset>";
        } else {
          out_ << f_._counts_[i];
        }
      }
      out_ << ")";
    } else {
      out_ << "<empty>";
    }
    out_ << ",b='";
    switch(f_._b_) {
    case none: out_ << "none"; break;
    case one: out_ << "one"; break;
    case two: out_ << "two"; break;
    case three: out_ << "three"; break;
    }
    out_ << "'";
    out_ << ",logging='";
    out_ << datatools::logger::get_priority_label(f_._logging_) << "'";
    out_ << '}';
    return out_;
  }

};

#ifndef Q_MOC_RUN
// Activate reflection layer for the 'foo' class:
DR_CLASS_INIT(::foo)
DR_TYPE_INIT(::foo::dummy_type)
#endif // Q_MOC_RUN

#endif // DATATOOLS_TESTING_UI_FOO_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
