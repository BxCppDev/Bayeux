//

#include <iostream>
#include <fstream>
#include <exception>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <Ac1.hpp>
#include <Ac2.hpp>
#include <Aserialization.ipp>

int main (void)
{
  try
    {
      bool io = true;
      const size_t NOBJSMAX = 6;
      int count;
      {
	A::base * objects[NOBJSMAX];
	for (int i = 0; i < NOBJSMAX; i++)
	  {
	    objects[i] = 0;
	  }
	int index = 0;
	objects[index++] = dynamic_cast <A::base *> (new A::c1 (111));
	objects[index++] = dynamic_cast <A::base *> (new A::c2 ("222"));
	objects[index++] = dynamic_cast <A::base *> (new A::c1 (333));
	// objects[index++] = dynamic_cast <A::base *> (new A::c1 (444));
	// objects[index++] = dynamic_cast <A::base *> (new A::c1 (555));
	// objects[index++] = dynamic_cast <A::base *> (new A::c2 ("666"));
	for (int i = 0; i < index; i++)
	  {
	    objects[i]->print ();
	  }
	if (io)
	  {
	    std::clog << "Storing..." << std::endl;
	    std::ofstream tof ("Aprg1.txt");
	    boost::archive::text_oarchive toa (tof);
	    toa & objects;
	  }
	for (int i = 0; i < index; i++)
	  {
	    if (objects[i] != 0) delete objects[i];
	    objects[i] = 0;
	  }
	count = index;
      }

      {
	A::base * objects[NOBJSMAX];
	for (int i = 0; i < NOBJSMAX; i++)
	  {
	    objects[i] = 0;
	  }
	if (io)
	  {
	    std::clog << "Loading..." << std::endl;
	    std::ifstream tif ("Aprg1.txt");
	    boost::archive::text_iarchive tia (tif);
	    tia & objects;
	  }
	for (int i = 0; i < count; i++)
	  {
	    if (objects[i] != 0) objects[i]->print ();
	  }
	for (int i = 0; i < count; i++)
	  {
	    if (objects[i] != 0) delete objects[i];
	    objects[i] = 0;
	  }
      }

    }
  catch (std::exception &x)
    {
      std::cerr << "Aprg1: error: " << x.what () << std::endl;
      return 1;
    }
  return 0;
}
