// -*- mode: c++; -*-
/** my_data.h
 *
 * Two serializable test classes
 *
 * Use recommandations from:
 *   http://www.boost.org/doc/libs/1_46_1/libs/serialization/doc/index.html
 *
 */

#ifndef DATATOOLS_TEST_MY_DATA_H
#define DATATOOLS_TEST_MY_DATA_H

// Standard Library:
#include <iostream>
#include <string>
#include <vector>

// Third Party:
// - Boost:
#include <boost/cstdint.hpp>
#include <boost/serialization/access.hpp>

// This project:
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>

namespace datatools {

  namespace test {

    class data_t :
      public datatools::i_serializable,
      public datatools::i_tree_dumpable
    {
    public:

      static const std::string SERIAL_TAG;

      virtual const std::string & get_serial_tag() const;

    private:

      int8_t   _flags_;
      int32_t  _ival_;
      double   _dval_;
      std::vector<double> _dvals_;

    public:

      void init();

      virtual ~data_t();

      data_t();

      data_t(char f_, int i_, double d_, size_t n_);

      const int8_t  & flags() const { return _flags_; }

      const int32_t & ival()  const { return _ival_; }

      const double  & dval()  const { return _dval_; }

      const std::vector<double> & dvals() const { return _dvals_; }

    private:

      friend class boost::serialization::access;
      BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

      public:

      virtual void tree_dump(std::ostream & out_         = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const;

    };

    /****************************************************************************/

    class more_data_t : public data_t
    {
    public:

      static const std::string SERIAL_TAG;

      virtual const std::string & get_serial_tag() const;

    private:

      std::string _name_;

    public:

      more_data_t();

      more_data_t(char c_, int i_, double d_, size_t n_,
                  const std::string & name_);

      virtual ~more_data_t();

      const std::string & name() const { return _name_; }

    private:
      friend class boost::serialization::access;
      BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

      public:

      virtual void tree_dump(std::ostream & out_         = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const;

    };

    void randomize_data(data_t & data_);

    void randomize_more_data(more_data_t & data_);

  } // namespace test

} // namespace datatools

#endif // DATATOOLS_TEST_MY_DATA_H
