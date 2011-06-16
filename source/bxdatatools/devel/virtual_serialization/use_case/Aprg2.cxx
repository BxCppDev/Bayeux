//

#include <iostream>
#include <fstream>
#include <exception>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <Ac1.hpp>
#include <Ac2.hpp>
#include <Ad1.hpp>

int main (void)
{
  try
    {
      bool io = true;
      const size_t NOBJS = 6;
      {
	A::base * objects[NOBJS];
	objects[0] = dynamic_cast <A::base *> (new A::c1 (111));
	objects[1] = dynamic_cast <A::base *> (new A::c2 ("222"));
	objects[2] = dynamic_cast <A::base *> (new A::c1 (333));
	objects[3] = dynamic_cast <A::base *> (new A::c1 (444));
	objects[4] = dynamic_cast <A::base *> (new A::d1 (555, 1000));
	objects[5] = dynamic_cast <A::base *> (new A::c2 ("666"));
	for (int i = 0; i < NOBJS; i++)
	  {
	    objects[i]->print ();
	  }
	if (io)
	  {
	    std::clog << "Storing..." << std::endl;
	    std::ofstream tof ("Aprg2.txt");
	    boost::archive::text_oarchive toa (tof);
	    toa & objects;
	  }
	
	for (int i = 0; i < NOBJS; i++)
	  {
	    delete objects[i];
	    objects[i] = 0;
	  }
      }

      {
	A::base * objects[NOBJS];
	for (int i = 0; i < NOBJS; i++)
	  {
	    objects[i] = 0;
	  }
	if (io)
	  {
	    std::clog << "Loading..." << std::endl;
	    std::ifstream tif ("Aprg2.txt");
	    boost::archive::text_iarchive tia (tif);
	    tia & objects;
	  }
	for (int i = 0; i < NOBJS; i++)
	  {
	    if (objects[i] != 0) objects[i]->print ();
	  }
	for (int i = 0; i < NOBJS; i++)
	  {
	    delete objects[i];
	    objects[i] = 0;
	  }
      }

    }
  catch (std::exception &x)
    {
      std::cerr << "error: " << x.what () << std::endl;
      return 1;
    }
  return 0;
}
