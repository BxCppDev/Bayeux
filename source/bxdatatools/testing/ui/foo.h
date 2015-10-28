//! \file  ui/foo.h
//! \brief A dummy class for UI testing
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
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

//! \brief A dummy object
class foo
{
public:
  foo()
  {
    _value_ = 0;
    return;
  }
  int get() const
  {
    return _value_;
  }
  void set(int value_)
  {
    _value_ = value_;
    return;
  }
  void zero()
  {
    set(0);
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
    int sum = 0;
    for (int i = 0; i < (int) values_.size(); i++) {
      sum += values_[i];
    }
    set(sum);
    return;
  }
  friend std::ostream & operator<<(std::ostream & out_, const foo & f_)
  {
    out_ << "foo={" << f_._value_ << '}';
    return out_;
  }
private:
  int _value_;
};

#endif // DATATOOLS_TESTING_UI_FOO_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
