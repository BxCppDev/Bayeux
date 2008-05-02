// test_histogram.cxx
/*
 * Histogram 1D sample program
 *
 *
 */

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>

#include <mygsl/histogram.h>

int main(int argc_ , char ** argv_)
{
  try 
    {
      mygsl::histogram::g_debug = true;

      long   seed    = 12345;
      size_t bins    = 100;
      size_t nshoots = 10000;

      srand48(seed);

      std::cerr << "NOTICE: h" << std::endl;
    
      mygsl::histogram h(bins,0,10.);

      {
	std::cerr << "NOTICE: h fill" << std::endl;
      
	for (int i= 0; i < nshoots; i++) 
	  {
	    double x = -std::log(drand48());
	    h.fill(x);
	  }
	h.dump(std::cerr);

	std::cerr << "NOTICE: h print" << std::endl;
      
	std::string sname = "test_histogram.data";
	std::ofstream ofhist(sname.c_str());
	h.print(ofhist);
	ofhist.close();
      }

      {
	std::cerr << "NOTICE: h store" << std::endl;
      
	std::string sname = "test_histogram.store";
	std::ofstream ofstore(sname.c_str());
	h.to_stream(ofstore);
	ofstore.close();
      
	std::cerr << "NOTICE: h load" << std::endl;
      
	std::ifstream ifstate(sname.c_str());
	h.from_stream(ifstate);
	ifstate.close();
      
	std::cerr << "NOTICE: h fill" << std::endl;
      
	for (int i = 0; i < nshoots; i++) 
	  {
	    double x = -std::log(drand48());
	    h.fill(x);
	  }
      }

      {
	std::cerr << "NOTICE: h scale & shift" << std::endl;
      
	h.scale(1.4);
	h.shift(100.0);
	h.dump(std::cerr);
      
	std::cerr << "NOTICE: h print" << std::endl;
      
	std::string sname = "test_histogram2.data";
	std::ofstream ofhist(sname.c_str());
	h.print(ofhist);
	ofhist.close();
      }
   
      size_t bins2 = 25;
      mygsl::histogram h2(h);
      { 
	std::cerr << "NOTICE: h2" << std::endl;
      
	h2.dump(std::cerr);
      
	std::cerr << "NOTICE: h2 rebin" << std::endl;
      
	h2.rebin(bins2);
	std::string sname = "test_histogram3.data";
	std::ofstream ofhist(sname.c_str());
	h2.print(ofhist);
	ofhist.close();
	h2.dump(std::cerr);
      }

      mygsl::histogram h3(h2,5);
      {
	std::cerr << "NOTICE: h3 rebin" << std::endl;

	h3.scale(-0.1);
	std::string sname = "test_histogram4.data";
	std::ofstream ofhist(sname.c_str());
	h3.print(ofhist);
	ofhist.close();
	h3.dump(std::cerr);
      }

      mygsl::histogram hh1, hh2, hh3;
      {
	std::cerr << "NOTICE: hh1" << std::endl;
      
	hh1 = h;

	std::cerr << "NOTICE: hh1.bins=" << hh1.bins() 
		  << std::endl;
	std::cerr << "NOTICE: hh2" << std::endl;
      
	hh2 = h * 1.5;

	std::cerr << "NOTICE: hh2.bins=" << hh2.bins() 
		  << std::endl;
    
	std::cerr << "NOTICE: hh3" << std::endl;
      
	hh3 = hh2 - hh1;

	std::cerr << "NOTICE: hh3.bins=" << hh3.bins() 
		  << std::endl;
	std::cerr << "NOTICE: hh3 done" << std::endl;
      
	std::string sname = "test_histogram5.data";
	std::ofstream ofhist(sname.c_str());
	hh3.print(ofhist);
	hh3.dump(std::cerr);
	ofhist.close();
      }

      {
	std::cerr << "NOTICE: hh4 from hh3 pdf" << std::endl;

	mygsl::histogram::pdf pdf1(hh3);
	mygsl::histogram hh4(100, 0., 10.);
      
	for (int i = 0; i < 2 * nshoots; i++) 
	  {
	    double r = drand48();
	    hh4.fill(pdf1.sample(r));
	  }
	
	std::string sname = "test_histogram6.data";
	std::ofstream ofhist(sname.c_str());
	hh4.print(ofhist);
	ofhist.close();
	hh4.dump(std::cerr);
      
      }

    }
  catch(std::exception & x) 
    {
      std::cerr << "ERROR: " << x.what() << std::endl;
      return (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

// end of test_histogram.cxx
