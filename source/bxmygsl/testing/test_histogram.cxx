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

#include <boost/filesystem.hpp>
#include <datatools/serialization/io_factory.h>

// Some pre-processor guard about Boost I/O usage and linkage :
#include <datatools/serialization/bio_guard.h>
#include <mygsl/serialization/bio_guard.h>

#include <mygsl/histogram.h>

int main(int argc_ , char ** argv_)
{
  try 
    {
      long   seed48  = 12345;
      size_t nbins   = 100;
      size_t nshoots = 10000;

      srand48(seed48);

      std::clog << "NOTICE: h" << std::endl;
  
      {
        mygsl::histogram h0(10, 0., 10.);
        h0.fill (0.5, -5.0);
        h0.fill (1.5, -2.0);
        //h0.fill (2.5, 0.0);
        h0.fill (3.5, 10.0);
        h0.fill (4.5, 13.0);
        h0.fill (5.5, 15.0);
        h0.fill (6.5, 10.0);
        h0.fill (7.5, 5.0);
        h0.fill (8.5, 3.0);
        h0.fill (9.5, 1.0);
        h0.dump(std::clog);
        h0.print_ascii(std::clog);
        h0.tree_dump(std::clog, "Histogram 'h0' : ");
        std::clog << std::endl;
      }  

      mygsl::histogram h(nbins, 0., 10.);
      {
        std::clog << "NOTICE: h fill" << std::endl;
      
        for (int i= 0; i < nshoots; i++) 
          {
            double x = -std::log(drand48());
            h.fill(x);
          }
        h.dump(std::clog);

        std::clog << "NOTICE: h print" << std::endl;
      
        std::string sname = "test_histogram.data";
        std::ofstream ofhist(sname.c_str());
        h.print(ofhist);
        ofhist.close();
      }

      {
        std::string sname = "test_histogram.store";

        std::clog << "NOTICE: h store" << std::endl;
        h.print_ascii(std::clog);
        std::ofstream ofstore(sname.c_str());
        h.to_stream(ofstore);
        ofstore.close();
        
        std::clog << "NOTICE: h load" << std::endl;
        std::ifstream ifstate(sname.c_str());
        h.from_stream(ifstate);
        ifstate.close();
        h.print_ascii(std::clog);
      
        std::clog << "NOTICE: h fill" << std::endl;
        for (int i = 0; i < nshoots; i++) 
          {
            double x = -std::log(drand48());
            h.fill(x);
          }
      }
      h.tree_dump(std::clog, "Histogram 'h' : ");
      std::clog << std::endl;
      
      {
        std::clog << "NOTICE: h scale & shift" << std::endl;
      
        h.scale(1.4);
        h.shift(100.0);
        h.dump(std::clog);
      
        std::clog << "NOTICE: h print" << std::endl;
      
        std::string sname = "test_histogram2.data";
        std::ofstream ofhist(sname.c_str());
        h.print(ofhist);
        ofhist.close();
        h.print_ascii(std::clog);
        h.tree_dump(std::clog, "Histogram 'h' : ");
        std::clog << std::endl;
      }
   
      size_t nbins2 = 25;
      mygsl::histogram h2(h);
      { 
        std::clog << "NOTICE: h2" << std::endl;
      
        h2.dump(std::clog);
      
        std::clog << "NOTICE: h2 rebin" << std::endl;
      
        h2.rebin(nbins2);
        std::string sname = "test_histogram3.data";
        std::ofstream ofhist(sname.c_str());
        h2.print(ofhist);
        ofhist.close();
        h2.dump(std::clog);
        h2.print_ascii(std::clog);
      }
      h2.tree_dump(std::clog, "Histogram 'h2' : ");
      std::clog << std::endl;

      mygsl::histogram h3(h2);
      {
        std::clog << "NOTICE: h3 rebin" << std::endl;
        if (h3.can_rebin (5))
          {
            h3.rebin (5);
            std::clog << "NOTICE: Rebin h3." << std::endl;
          }
        else
          {
            std::clog << "NOTICE: Cannot rebin h3." << std::endl;
          }
        h3.scale(-0.1);
        std::string sname = "test_histogram4.data";
        std::ofstream ofhist(sname.c_str());
        h3.print(ofhist);
        ofhist.close();
        h3.dump(std::clog);
        h3.print_ascii(std::clog);
      }
      h3.tree_dump(std::clog, "Histogram 'h3' : ");
      std::clog << std::endl;

      mygsl::histogram hh1, hh2, hh3;
      {
        std::clog << "NOTICE: hh1" << std::endl;
      
        hh1 = h;

        std::clog << "NOTICE: hh1.bins=" << hh1.bins() 
                  << std::endl;
        std::clog << "NOTICE: hh2" << std::endl;
      
        hh2 = h * 1.5;

        std::clog << "NOTICE: hh2.bins=" << hh2.bins() 
                  << std::endl;
    
        std::clog << "NOTICE: hh3" << std::endl;
      
        hh3 = hh2 - hh1;

        std::clog << "NOTICE: hh3.bins=" << hh3.bins() 
                  << std::endl;
        std::clog << "NOTICE: hh3 done" << std::endl;
      
        std::string sname = "test_histogram5.data";
        std::ofstream ofhist(sname.c_str());
        hh3.print(ofhist);
        hh3.dump(std::clog);
        ofhist.close();
      }
      hh3.tree_dump(std::clog, "Histogram 'hh3' : ");
      std::clog << std::endl;

      {
        std::clog << "NOTICE: hh4 from hh3 pdf" << std::endl;

        mygsl::histogram::pdf pdf1(hh3);
        mygsl::histogram hh4(10, 0., 10.);
      
        for (int i = 0; i < 2 * nshoots; i++) 
          {
            double r = drand48();
            hh4.fill(pdf1.sample(r));
          }
        
        std::string sname = "test_histogram6.data";
        std::ofstream ofhist(sname.c_str());
        hh4.print(ofhist);
        ofhist.close();
        hh4.dump(std::clog);
        hh4.print_ascii(std::clog);
        hh4.tree_dump(std::clog, "Histogram 'hh4' : ");
        std::clog << std::endl;
    
        {
          std::clog << "INFO: " 
                    << "Test serialization..." 
                    << std::endl;
          std::string filename = "test_histogram.xml";
          {
            std::clog << "INFO: " 
                      << "Test serialization: writer..." 
                      << std::endl;
            datatools::serialization::data_writer writer (filename);
            writer.store (hh4);     
          }
          
          {
            std::clog << "INFO: " 
                      << "Test serialization: reader..." 
                      << std::endl;
            mygsl::histogram hh4bis;
            datatools::serialization::data_reader reader (filename);
            if (reader.has_record_tag ())
              { 
                if (reader.record_tag_is (mygsl::histogram::SERIAL_TAG)) 
                  {
                    reader.load (hh4bis);
                    hh4bis.dump (std::clog);
                    hh4bis.tree_dump(std::clog, "Histogram 'hh4bis' : ");
                    std::clog << std::endl;
                   }
                else
                  {
                    std::clog << "ERROR: " 
                              << "Cannot load an histogram 1D object !" 
                              << std::endl;
                  }
              }
            else
              {
                std::clog << "ERROR: " 
                          << "No object in the Boost archive !" 
                          << std::endl;
              }     
          }
        }
        
      }
     try
        {
          mygsl::histogram h;
          h *= 2.0;
        }
      catch(std::exception & pb) 
        {
          std::cerr << "WARNING: As expected, there is an error : " << pb.what() << std::endl;
          
        }
 
      try
        {
          mygsl::histogram ha, hb, hc;
          hc = ha * hb;
        }
      catch(std::exception & pb) 
        {
          std::cerr << "WARNING: As expected, there is an error : " << pb.what() << std::endl;      
        }
      
      std::clog << "NOTICE: The end." << std::endl;
       

    }
  catch(std::exception & x) 
    {
      std::cerr << "ERROR: " << x.what() << std::endl;
      return (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

// end of test_histogram.cxx
