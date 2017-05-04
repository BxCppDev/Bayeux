// address_set.cc

// Ourselves:
#include <geomtools/address_set.h>

// Standard library:
#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <string>
#include <algorithm>

namespace geomtools {

  bool address_set::is_valid() const
  {
    return _mode_ != MODE_INVALID;
  }

  void address_set::invalidate()
  {
    reset();
    _mode_ = MODE_INVALID;
    return;
  }

  void address_set::_reset_list_()
  {
    _addresses_.clear();
    return;
  }

  void address_set::_reset_range_()
  {
    _range_min_ = 1;
    _range_max_ = 0;
    return;
  }

  bool address_set::is_reverse() const
  {
    return _reverse_;
  }

  void address_set::set_reverse(bool a_reverse)
  {
    _reverse_ = a_reverse;
    return;
  }

  bool address_set::is_mode_none() const
  {
    return _mode_ == MODE_NONE;
  }

  bool address_set::is_mode_all() const
  {
    return _mode_ == MODE_ALL;
  }

  bool address_set::is_mode_range() const
  {
    return _mode_ == MODE_RANGE;
  }

  bool address_set::is_mode_list() const
  {
    return _mode_ == MODE_LIST;
  }

  void address_set::set_mode_none()
  {
    _reset_range_();
    _reset_list_();
    _mode_ = MODE_NONE;
    return;
  }

  void address_set::set_mode_all()
  {
    _reset_range_();
    _reset_list_();
    _mode_ = MODE_ALL;
    return;
  }

  void address_set::set_mode_range()
  {
    _reset_list_();
    _reset_range_();
    _mode_ = MODE_RANGE;
    return;
  }

  void address_set::set_range(uint32_t a_min, uint32_t a_max)
  {
    if (! is_mode_range()) {
      set_mode_range();
    }
    if (a_min >  a_max) {
      _reset_range_();
    } else {
      _range_min_ = a_min;
      _range_max_ = a_max;
    }
    return;
  }

  void address_set::set_mode_range(uint32_t a_min, uint32_t a_max)
  {
    _mode_ = MODE_RANGE;
    set_range(a_min, a_max);
    return;
  }

  void address_set::set_mode_list()
  {
    _reset_list_();
    _reset_range_();
    _mode_ = MODE_LIST;
    return;
  }

  void address_set::add_to_list(uint32_t a_value)
  {
    if (! is_mode_list()) {
      set_mode_list();
    }
    _addresses_.insert(a_value);
    return;
  }

  bool address_set::match(uint32_t a_value) const
  {
    bool matched_value = false;

    if (is_mode_all()) {
      matched_value = true;
    } else if (is_mode_range()) {
      if (a_value < _range_min_) matched_value = false;
      else if (a_value > _range_max_) matched_value = false;
      else matched_value = true;
    } else if (is_mode_list()) {
      matched_value = _addresses_.find(a_value) != _addresses_.end();
    } else {
      matched_value = false;
    }
    if (is_reverse()) {
      matched_value = ! matched_value;
    }
    return matched_value;
  }

  void address_set::reset()
  {
    _reverse_ = false;
    _mode_ = MODE_DEFAULT;
    _reset_range_();
    _reset_list_();
    return;
  }

  address_set::address_set()
  {
    reset();
    return;
  }

  std::ostream & operator<<(std::ostream & a_out, const address_set & a_addset)
  {
    if (! a_addset.is_valid()) {
      a_out << "?";
      return a_out;
    }
    if (a_addset.is_reverse()) {
      a_out << '!';
    }
    if (a_addset.is_mode_none()) {
      a_out << "{!}";
    } else if (a_addset.is_mode_all()) {
        a_out << "{*}";
    } else if (a_addset.is_mode_range()) {
      a_out << '[';
      a_out << a_addset._range_min_;
      a_out << ';';
      a_out << a_addset._range_max_;
      a_out << ']';
    } else {
      a_out << '{';
      for(std::set<uint32_t>::const_iterator i = a_addset._addresses_.begin();
          i != a_addset._addresses_.end();
          i++) {
        std::set<uint32_t>::const_iterator j = i;
        j++;
        a_out << *i;
        if (j != a_addset._addresses_.end()) {
          a_out << ';';
        }
      }
      a_out << '}';
    }

    return a_out;
  }

  std::istream & operator>>(std::istream & a_in, address_set & a_addset)
  {
    a_addset.invalidate();
    bool reverse = false;
    char c = a_in.peek();
    if (c == '!')
      {
        a_in.get();
        reverse = true;
        c = a_in.peek();
      }
    if (c == '{')
      {
        a_in.get();
        c = a_in.peek();
        if (c == '!')
          {
            a_in.get();
            c = a_in.peek();
            if (c != '}')
              {
                a_addset.invalidate();
                a_in.setstate(std::ios::failbit);
                return a_in;
              }
            a_in.get();
            a_addset.set_mode_none();
          }
        else if (c == '*')
          {
            a_in.get();
            c = a_in.peek();
            if (c != '}')
              {
                a_addset.invalidate();
                a_in.setstate(std::ios::failbit);
                return a_in;
              }
            a_in.get();
            a_addset.set_mode_all();
          }
        else
          {
            // mode list:
            while(true)
              {
                uint32_t v;
                a_in >> v >> std::ws;
                if (! a_in)
                  {
                    return a_in;
                  }
                a_addset.add_to_list(v);
                c = a_in.peek();
                if (c == '}')
                  {
                    a_in.get();
                    break;
                  }
                if (c != ';')
                  {
                    a_addset.invalidate();
                    a_in.setstate(std::ios::failbit);
                    return a_in;
                  }
                a_in.get();
              }
          }
      }
    else if (c == '[')
      {
        // mode range:
        a_in.get();
        uint32_t min, max;
        a_in >> min;
        if (! a_in)
          {
            a_addset.invalidate();
            a_in.setstate(std::ios::failbit);
            return a_in;
          }
        c = a_in.peek();
        if (c != ';')
          {
            a_addset.invalidate();
            a_in.setstate(std::ios::failbit);
            return a_in;
          }
        a_in.get();
        a_in >> max;
        if (! a_in)
          {
            a_addset.invalidate();
            a_in.setstate(std::ios::failbit);
            return a_in;
          }
        c = a_in.peek();
        if (c != ']')
          {
            a_addset.invalidate();
            a_in.setstate(std::ios::failbit);
            return a_in;
          }
        a_in.get();
        a_addset.set_range(min, max);
      }
    else
      {
        a_addset.invalidate();
        a_in.setstate(std::ios::failbit);
        return a_in;
      }
    a_addset.set_reverse(reverse);
    return a_in;
  }

} // end of namespace geomtools
