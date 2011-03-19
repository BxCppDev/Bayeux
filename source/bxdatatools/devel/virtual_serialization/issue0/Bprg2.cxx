//

#include <iostream>
#include <fstream>
#include <exception>

#ifndef INCLUDE_ARCHIVES_BASE
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#endif

#include <Ac1.hpp>
#include <Ac2.hpp>
#include <Bc3.hpp>

int main (void)
{
  try
    {
      const size_t NOBJSMAX = 6;
      int count;
      {
	A::base * objects[NOBJSMAX];
	for (int i = 0; i < NOBJSMAX; i++)
	  {
	    objects[i] = 0;
	  }
	int index = 0;
	objects[index++] = dynamic_cast <A::base *> (new A::c1 (11));
	objects[index++] = dynamic_cast <A::base *> (new A::c2 ("22"));
	objects[index++] = dynamic_cast <A::base *> (new A::c1 (33));
	objects[index++] = dynamic_cast <A::base *> (new B::c3 (44));
	objects[index++] = dynamic_cast <A::base *> (new B::c3 (55));
	objects[index++] = dynamic_cast <A::base *> (new A::c2 ("66"));
	for (int i = 0; i < index; i++)
	  {
	    if (objects[i] != 0) objects[i]->print ();
	  }
	{
	  std::clog << "Storing..." << std::endl;
	  std::ofstream tof ("Bprg2.data");
	  boost::archive::binary_oarchive oa (tof);
	  oa & objects;
	}
	for (int i = 0; i < NOBJSMAX; i++)
	  {
	    if (objects[i] != 0) delete objects[i];
	    objects[i] = 0;
	  }
	count = index;
      }

      {
	A::base * objects[NOBJSMAX];
	{
	  std::clog << "Loading..." << std::endl;
	  std::ifstream tif ("Bprg2.data");
	  boost::archive::binary_iarchive ia (tif);
	  ia & objects;
	}
	for (int i = 0; i < count; i++)
	  {
	    if (objects[i] != 0) objects[i]->print ();
	  }
	for (int i = 0; i < NOBJSMAX; i++)
	  {
	    if (objects[i] != 0) delete objects[i];
	    objects[i] = 0;
	  }
      }

    }
  catch (std::exception &x)
    {
      std::cerr << "Bprg2: error: " << x.what () << std::endl;
      return 1;
    }
  return 0;
}
