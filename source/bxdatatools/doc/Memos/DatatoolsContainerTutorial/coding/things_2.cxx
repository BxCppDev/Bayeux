#include <iostream>
#include <boost/cstdint.hpp>
#include <datatools/properties.h>
#include <datatools/things.h>
#include <datatools/i_serializable.h>
/*** Interface of the class (using macros) ***/
class storable_type : DATATOOLS_SERIALIZABLE_CLASS
{
public:
  storable_type ();
  void set_value (int32_t);
  int32_t get_value () const;
private:
  int32_t _value_;
  /* special macro from the 'datatools::i_serializable' interface */
  DATATOOLS_SERIALIZATION_DECLARATION();
};
// interface for pointer serialization :
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(storable_type, "storable_type")
/*** Implementation of the class ***/
// serial serialization tag :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(storable_type, "storable_type")
storable_type::storable_type () : _value_ (0)
{
}
void storable_type::set_value (int32_t value_)
{
  _value_ = value_;
  return;
}
int32_t storable_type::get_value () const
{
  return _value_;
}
/*** Serialization implementation ***/
#include <boost/serialization/nvp.hpp>
// template Boost serialization method :
template<class Archive>
void storable_type::serialize (Archive & ar_, const unsigned int version_)
{
  ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  ar_ & boost::serialization::make_nvp ("value", _value_);
  return;
}
// automated pre-instantiation of serialization template code :
#include <datatools/archives_instantiation.h>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(storable_type)
// export class for pointer serialization :
BOOST_CLASS_EXPORT_IMPLEMENT(storable_type)

/*** main program ***/
int main (void) {
  datatools::things bag;
  bag.set_description ("A test things container");

  bag.add<datatools::properties> ("foo").store_flag ("test");
  bag.add<storable_type> ("dummy").set_value (12345);

  bag.tree_dump (std::cout, "The bag: ");
  std::cout << std::endl;

  bag.get<datatools::properties> ("foo").tree_dump (std::cout,
                                                    "foo : ");
  const storable_type & dummy_ref = bag.get<storable_type> ("dummy");
  std::cout << "dummy's value = " << dummy_ref.get_value () << std::endl;
  std::cout << std::endl;

  bag.remove ("foo");
  bag.tree_dump (std::cout, "The bag: ");
  return 0;
}
