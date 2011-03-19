//

#include <iostream>
#include <fstream>
#include <exception>

#ifndef INCLUDE_ARCHIVES_BASE
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#endif

#include <Ac1.hpp>
#include <Ac2.hpp>
#include <Bc3.hpp>

int main (void)
{
  try
    {
      const size_t NOBJS = 6;
      {
	A::base * objects[NOBJS];
	for (int i = 0; i < NOBJS; i++)
	  {
	    objects[i] = 0;
	  }
	objects[0] = dynamic_cast <A::base *> (new A::c1 (11));
	objects[1] = dynamic_cast <A::base *> (new A::c2 ("22"));
	objects[2] = dynamic_cast <A::base *> (new A::c1 (33));
	//objects[3] = dynamic_cast <A::base *> (new B::c3 (44));
	//objects[4] = dynamic_cast <A::base *> (new B::c3 (55));
	objects[5] = dynamic_cast <A::base *> (new A::c2 ("66"));
	for (int i = 0; i < NOBJS; i++)
	  {
	    if (objects[i] != 0) objects[i]->print ();
	  }
	{
	  std::clog << "Storing..." << std::endl;
	  std::ofstream tof ("Bprg1.txt");
	  boost::archive::text_oarchive toa (tof);
	  toa & objects;
	}
	for (int i = 0; i < NOBJS; i++)
	  {
	    if (objects[i] != 0) delete objects[i];
	    objects[i] = 0;
	  }
      }

      {
	A::base * objects[NOBJS];
	for (int i = 0; i < NOBJS; i++)
	  {
	    objects[i] = 0;
	  }
	{
	  std::clog << "Loading..." << std::endl;
	  std::ifstream tif ("Bprg1.txt");
	  boost::archive::text_iarchive tia (tif);
	  tia & objects;
	}
	for (int i = 0; i < NOBJS; i++)
	  {
	    objects[i]->print ();
	  }
	for (int i = 0; i < NOBJS; i++)
	  {
	    if (objects[i] != 0) delete objects[i];
	    objects[i] = 0;
	  }
      }

    }
  catch (std::exception & x)
    {
      std::cerr << "Bprg1: error: " << x.what () << std::endl;
      return 1;
    }
  return 0;
}
