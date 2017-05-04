// -*- mode: c++; -*-

// Ourselves:
#include <datatools_test_my_data.h>

// Standard Library:
#include <sstream>

namespace datatools {

  namespace test {

    const std::string data_t::SERIAL_TAG = "datatools::test::data_t";

    const std::string & data_t::get_serial_tag() const
    {
      return data_t::SERIAL_TAG;
    }

    void data_t::init()
    {
      _flags_ = 'a';
      _ival_ = 666;
      _dval_ = 3.14;
      return;
    }

    data_t::~data_t()
    {
      _dvals_.clear();
      return;
    }

    data_t::data_t() : datatools::i_serializable()
    {
      init();
      return;
    }

    data_t::data_t(char a_f, int a_i, double a_d, size_t a_n)
    {
      init();
      _flags_ = a_f;
      _ival_ = a_i;
      _dval_ = a_d;
      double v = 10.;
      for(int i = 0; i <(int) a_n; i++)
        {
          _dvals_.push_back(v);
          v *= 3.;
        }
      return;
    }

    void data_t::tree_dump(std::ostream & a_out,
                           const std::string & a_title,
                           const std::string & a_indent,
                           bool a_inherit) const
    {
      std::string indent;
      if(! a_indent.empty()) indent = a_indent;
      if(! a_title.empty()) {
        a_out << indent << a_title << std::endl;
      }
      a_out << indent << i_tree_dumpable::tag << "Address : [" << this << ']' << std::endl;
      a_out << indent << i_tree_dumpable::tag << "flags : '" << _flags_ << "'" << std::endl;
      a_out << indent << i_tree_dumpable::tag << "ival  : "  << _ival_ << std::endl;
      a_out << indent << i_tree_dumpable::tag << "dval  : "
            << _dval_ << std::endl;
      a_out << indent << i_tree_dumpable::inherit_tag(a_inherit)
            << "dvals[" << _dvals_.size() << "] : ";
      for(int i = 0; i <(int) _dvals_.size(); i++)
        {
          a_out << _dvals_[i] << ' ';
        }
      a_out << std::endl;
      return;
    }

    /* ************************************************************************** */

    const std::string more_data_t::SERIAL_TAG = "datatools::test::more_data_t";

    more_data_t::more_data_t() : data_t('z', -1, 3.14159, 3)
    {
      _name_ = "<anonymous>";
      return;
    }

    more_data_t::more_data_t(char a_c, int a_i, double a_d, size_t a_n,
                             const std::string & a_name)
      : data_t(a_c, a_i, a_d, a_n)
    {
      _name_ = a_name;
      return;
    }

    more_data_t::~more_data_t()
    {
      return;
    }

    const std::string & more_data_t::get_serial_tag() const
    {
      return more_data_t::SERIAL_TAG;
    }

    void more_data_t::tree_dump(std::ostream & a_out,
                                const std::string & a_title,
                                const std::string & a_indent,
                                bool a_inherit) const
    {
      std::string indent;
      if(! a_indent.empty()) indent = a_indent;
      data_t::tree_dump(a_out, a_title, a_indent, true);
      a_out << indent << i_tree_dumpable::inherit_tag(a_inherit)
            << "name = "  << _name_ << std::endl;
      return;
    }

    void randomize_data(data_t & a_data)
    {
      const data_t d((char)(32 + drand48() * 90),
                     (int)(drand48() * 1000),
                     (double) drand48(),
                     1 +(size_t)(drand48() * 5));
      a_data = d;
      return;
    }

    void randomize_more_data(more_data_t & a_data)
    {
      std::ostringstream name_oss;
      name_oss << "a_data" <<(int)(drand48() * 1000);
      const more_data_t md((char)(32 + drand48() * 90),
                           (int)(drand48() * 1000),
                           (double) drand48(),
                           1 +(size_t)(drand48() * 5),
                           name_oss.str());
      a_data = md;
      return;
    }

  } // end of namespace test

} // end of namespace datatools
