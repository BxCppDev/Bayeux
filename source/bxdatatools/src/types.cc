// types.cc

// Ourselves
#include <datatools/types.h>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>

// Clang doesn't like bimap's use of BOOST_PP...
#if defined (__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wredeclared-class-member"
#endif

#include <boost/bimap.hpp>

#if defined (__clang__)
#pragma clang diagnostic pop
#endif


namespace datatools {

  const boost::bimap< int, std::string > & get_type_to_labels_map()
  {
    typedef boost::bimap< int, std::string > bm_type;
    static boost::scoped_ptr<bm_type> _bmap;
    if (_bmap.get() == 0) {
      _bmap.reset(new bm_type);
      _bmap.get()->insert( bm_type::value_type(TYPE_NONE,    "?" ) );
      _bmap.get()->insert( bm_type::value_type(TYPE_BOOLEAN, "boolean" ) );
      _bmap.get()->insert( bm_type::value_type(TYPE_INTEGER, "integer" ) );
      _bmap.get()->insert( bm_type::value_type(TYPE_REAL,    "real" ) );
      _bmap.get()->insert( bm_type::value_type(TYPE_STRING,  "string" ) );
    }
    return *_bmap.get();
  }

  const std::string get_label_from_type(int type_)
  {
    const boost::bimap< int, std::string >::left_const_iterator found
      = get_type_to_labels_map().left.find(type_);
    if (found == get_type_to_labels_map().left.end()) {
      return "?";
    }
    return found->second;
  }

  int get_type_from_label(const std::string & label_)
  {
    const boost::bimap< int, std::string >::right_const_iterator found
      = get_type_to_labels_map().right.find(label_);
    if (found == get_type_to_labels_map().right.end()) {
      return TYPE_NONE;
    }
    return found->second;
  }

} // namespace datatools
