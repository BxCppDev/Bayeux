#include <vector>
#include <list>
#include <fstream>

// >>> Added 
#include <boost/foreach.hpp>              // Useful macro
#include <boost/cstdint.hpp>              // Support portable integral types
#include <boost/serialization/access.hpp> // See the friend boost::serialization::access below
#include <boost/serialization/nvp.hpp>    // To support XML I/O archives
#include <boost/serialization/vector.hpp> // Support STL vector serialization
#include <boost/serialization/list.hpp>   // Support STL list serialization
#include <boost/archive/text_oarchive.hpp>  // Support text output archive
#include <boost/archive/xml_oarchive.hpp>   // Support XML output archive
#include <boost/archive/text_iarchive.hpp>  // Support text input archive
#include <boost/archive/xml_iarchive.hpp>   // Support XML input archive
// <<<

using namespace std;

class MyObj
{
  int32_t val; // Ensure portability (int's size is not garanteed across != platforms)

public:

  MyObj(){ val = 0; } // Must have a defualt constructor for the internal deserialization factory

  MyObj(int v){ val = v; }

  ~MyObj(){}
 
  int32_t get_val () const
  {
    return val;
  }

private:
  // let the Boost/Serialization enter the private scope of the class:
  friend class boost::serialization::access;
  // provide the declaration of the main template serialization method:
  template<class Archive>
  void serialize (Archive & ar, const unsigned int file_version);

};

// this is the implementation of the main template serialization method:
template<class Archive>
void MyObj::serialize (Archive & ar, const unsigned int file_version)
{
  ar & boost::serialization::make_nvp ("val", val);
  return;
}

// Useful typedefs:
typedef MyObj object_type;
typedef object_type * ptr_type;
typedef list<ptr_type> list_ptr_type;
typedef vector<list_ptr_type> vector_list_ptr_type;

// A function to print the 'layers' container: 
void print (const vector_list_ptr_type & the_layers)
{
  cout << "Layers are: " << endl << "  ";
  BOOST_FOREACH(list_ptr_type a_list, the_layers)
    {
      BOOST_FOREACH(ptr_type a_ptr, a_list)
	{
	  if (a_ptr != 0) 
	    {
	      cout << a_ptr->get_val () << ' '; 
	    }
	}
      cout << endl << "  ";
    }
  cout << endl;
  return;
}

// A function to deallocate the 'layers' container: 
void free (vector_list_ptr_type & the_layers, bool verbose = false)
{
  cout << "Deallocating... " << endl;
  BOOST_FOREACH(list_ptr_type & a_list, the_layers)
    {
      BOOST_FOREACH(ptr_type a_ptr, a_list)
	{
	  if (a_ptr != 0) 
	    {
	      if (verbose)
		{
		  cout << "  Deleting object with val==" << a_ptr->get_val () << "..." << endl;
		}
	      delete a_ptr;	  
	      a_ptr = 0;
	    }
	}
      a_list.clear ();
    }
  the_layers.clear ();
  cout << "Done." << endl;
  return;
}


int main (int argc, char *argv[])
{
  {
    vector_list_ptr_type layers;
    
    // Populate the container :
    for(int i=0; i<5; i++)
      {
	list_ptr_type tmp;
	
	for(int j=0; j<10; j++)
	  tmp.push_back( new object_type(j+i*100) );
	
	layers.push_back(tmp);
      }

    // Print it !
    print (layers);    
    
    // Serialization (text) : 
    {
      ofstream toafile ("ar.txt");
      boost::archive::text_oarchive toa (toafile);
      toa & layers;
    }
    
    // Serialization (XML) : 
    {
      ofstream xoafile ("ar.xml");
      boost::archive::xml_oarchive xoa (xoafile);
      xoa & boost::serialization::make_nvp ("layers", layers);
    }
    
    free (layers, true);    
  }
  
  {
    vector_list_ptr_type layers;
    // De-serialization from (text):
    {
      ifstream tiafile ("ar.txt");
      boost::archive::text_iarchive tia (tiafile);
      tia & layers;
    }
    cout << "Layers loaded from the text archive : " << endl;
    print (layers);    
    free (layers);    
  }
  
  {
    vector_list_ptr_type layers;
    // De-serialization from (xml):
    {
      ifstream xiafile ("ar.xml");
      boost::archive::xml_iarchive xia (xiafile);
      xia & boost::serialization::make_nvp ("layers", layers);
    }
    cout << "Layers loaded from the XML archive : " << endl;
    print (layers);    
    free (layers);    
  }

  return 0;
}
