/* test_serialization_4.cxx */

#include <iostream> 
#include <string>
#include <stdexcept>

#include <boost/filesystem.hpp>

#include <datatools/serialization_macros.h>
#include <datatools/archives_instantiation.h>
#include <datatools/io_factory.h>
#include <datatools/i_serializable.h>
#include <datatools/safe_serial.h>

#include <boost/serialization/export.hpp>

using namespace std;

// A serializable class:
class data : public datatools::i_serializable     
{
public:
  static const string SERIAL_TAG;
  virtual const string & get_serial_tag () const;

public:
  int32_t value; // An integral value (32 bits)

private:
  friend class boost::serialization::access; 
  BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

};

template<class Archive>
void data::serialize (Archive & ar_, 
                      const unsigned int version_)
{
  ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  ar_ & boost::serialization::make_nvp ("value", value);
  return;
}

const string data::SERIAL_TAG = "data";

const string & data::get_serial_tag () const
{
  return data::SERIAL_TAG;
}

//BOOST_CLASS_EXPORT_KEY2 (data, "data")
//BOOST_CLASS_EXPORT_IMPLEMENT (data)
//DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(data)

int main (void) 
{
  string filename = "test_serialization_4.xml";

  {
    // Save 1000 data objects:

    /* Create a serializer to file 'filename':
     * the 'datatools::serialization::using_multi_archives' flag 
     * is mandatory here to avoid memory corruption. 
     */
    datatools::data_writer writer (filename, datatools::using_multi_archives);
    for (int i = 0; i < 1000; i++) 
      {
        data the_data;
        the_data.value = i;
        writer.store (the_data);
      }
  }    

  {
    // Reload the objects: 

    /* Create a deserializer from file 'filename'::
     * the 'datatools::serialization::using_multi_archives' flag 
     * is mandatory here to avoid memory corruption. 
     */
    datatools::data_reader reader (filename,datatools::using_multi_archives);    
    while (reader.has_record_tag ()) 
      {
        if (reader.record_tag_is (data::SERIAL_TAG)) 
          {
            data the_data;
            reader.load (the_data);
          }
        else 
          {
            string bad_tag = reader.get_record_tag ();
            clog << "ERROR: Unknown serialization tag '" 
                 << bad_tag << "'! Cannot de-serialized more !" << endl; 
            break;
          }
      } 
  }

  return 0;
}

/* end of test_serialization_4.cxx */
