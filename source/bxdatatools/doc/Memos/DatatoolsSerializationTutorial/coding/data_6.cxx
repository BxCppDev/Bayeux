#include <datatools/serialization/i_serializable.h>
#include <boost/serialization/nvp.hpp>

/*** sdata1 interface ***/
class sdata1 : DATATOOLS_SERIALIZABLE_CLASS  // Shortcut macro
{
private:
  bool _flag_;
public:
  sdata1 () ;
  DATATOOLS_SERIALIZATION_DECLARATION(); // Shortcut macro
};

// Declaration macro of the class exporting code (GUID) :
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(sdata1, "sdata1")

/*** sdata2 interface ***/
class sdata2 : DATATOOLS_SERIALIZABLE_CLASS  // Shortcut macro
{
private:
  double _value_;
public:
  sdata2 () ;
  DATATOOLS_SERIALIZATION_DECLARATION(); // Shortcut macro
};

// Declaration macro of the class exporting code (GUID) :
BOOST_CLASS_EXPORT_KEY2(sdata2, "sdata2")

/*** sdata1 implementation ***/
sdata1::sdata1 () : _flag_ (true) {}

// Shortcut macro :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION (sdata1, "sdata1")

/*** sdata2 implementation ***/
sdata2::sdata2 () : _value_ (0.0) {}

// Shortcut macro :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION (sdata2, "sdata2")

/*** sdata1 serialization code definition ***/
template<class Archive>
void sdata1::serialize (Archive & ar_, const unsigned int version_) 
{
  ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  ar_ & boost::serialization::make_nvp ("flag", _flag_); 
  return;
}

/*** sdata2 serialization code definition ***/
template<class Archive>
void sdata2::serialize (Archive & ar_, const unsigned int version_) 
{
  ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  ar_ & boost::serialization::make_nvp ("value", _value_); 
  return;
}

/** Implementation of the classes' exporting code and automatic 
 *  instantiation of template code for all Boost archives 
 *  used by the datatools library. This code segment must be 
 *  invoked from the main program or pre-compiled within a DLL.
 */
#include <datatools/serialization/archives_instantiation.h>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(sdata1)
BOOST_CLASS_EXPORT_IMPLEMENT(sdata1) 

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(sdata2)
BOOST_CLASS_EXPORT_IMPLEMENT(sdata2) 

/*** main program ***/
#include <cstdlib>
#include <datatools/serialization/io_factory.h>
#include <boost/serialization/vector.hpp>
int main (void) 
{
  srand48 (314159); // Initialize the drand48 PRNG.
  namespace ds = datatools::serialization; // Shortcut namespace.
  typedef std::vector<ds::i_serializable *> data_array_type; // Useful typedef

  // Serialize some mixed 'sdata1' and 'sdata2' instances : 
  {
    data_array_type data;
    // Populate the array of data :
    std::cout << "Data to be stored : " << std::endl;
    for (int i = 0; i < 10; ++i)
      {
	ds::i_serializable * a_data_ptr;
	// Randomize (50%/50%) the type of data to be stored in the array :
	if (drand48 () < 0.5) a_data_ptr = new sdata1;
	else a_data_ptr = new sdata2;
	data.push_back (a_data_ptr); 
	std::cout << "Serial tag is '" << data.back ()->get_serial_tag () 
		  << "'" << std::endl;
      }
    std::cout << std::endl;
    ds::data_writer the_writer ("data_6.txt", ds::using_multi_archives);

    // Serialize the array of data pointers :
    the_writer.store ("array", data);

    for (int i = 0; i < data.size (); ++i) delete data[i];
  }

  // Deserialize some mixed 'sdata1' and 'sdata2' instances : 
  {
    data_array_type data;
    ds::data_reader the_reader ("data_6.txt", ds::using_multi_archives);

    // Deserialize the array of data pointers :
    the_reader.load ("array", data);  

    std::cout << "Loaded data : " << std::endl;
    for (int i = 0; i < data.size (); ++i)
      {
	std::cout << "Serial tag is '" << data[i]->get_serial_tag () 
		  << "'" << std::endl;
      }
    for (int i = 0; i < data.size (); ++i) delete data[i];
  }
  return 0;
}
