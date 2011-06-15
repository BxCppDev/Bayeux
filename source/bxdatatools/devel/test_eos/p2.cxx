// p2.cxx
// author: Francois Mauger <maugeer@lpccaen.in2p3.fr>
// date: 2011-06-15
// what: a sample program that check the (de)serialization of finite
//       and nonfinite floating number (float and double) using 
//       new portable binary archives
// tested plateforms: Linux (32bits/i686 and 64bits/x86_64)

#include <iostream>
#include <fstream>
#include <vector>
#include <limits>

#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>

#include <boost/archive/portable_binary_oarchive.hpp>
#include <boost/archive/portable_binary_iarchive.hpp>

using namespace std;
int main (void)
{
  clog << "Test program for serialization of non finite floating \n"
       << "numbers using eos::portable_binary_[o/i]archive(s) :" << endl;

  {
    // single precision float : 
    vector<float> vf1;
    vf1.assign (3, 0.0F);
    vf1[0] = -1.0F;
    vf1[1] =  0.0F;
    vf1[2] = +1.0F;
    cout << "vf1:" << endl;
    BOOST_FOREACH (const float & f, vf1)
      {
	cout << "  float value=" << f << endl;
      }

    vector<float> vf2;
    vf2.assign (3, 0.0F);
    vf2[0] = -numeric_limits<float>::infinity ();
    vf2[1] = +numeric_limits<float>::infinity ();
    vf2[2] =  numeric_limits<float>::quiet_NaN ();
    cout << "vf2:" << endl;
    BOOST_FOREACH (const float & f, vf2)
      {
	cout << "  float value=" << f << endl;
      }

    // double precision float : 
    vector<double> vd1;
    vd1.assign (3, 0.0);
    vd1[0] = -1.0;
    vd1[1] =  0.0;
    vd1[2] = +1.0;
    cout << "vd1:" << endl;
    BOOST_FOREACH (const double & f, vd1)
      {
	cout << "  double value=" << f << endl;
      }

    vector<double> vd2;
    vd2.assign (3, 0.0);
    vd2[0] = -numeric_limits<double>::infinity ();
    vd2[1] = +numeric_limits<double>::infinity ();
    vd2[2] =  numeric_limits<double>::quiet_NaN ();
    cout << "vd2:" << endl;
    BOOST_FOREACH (const double & f, vd2)
      {
	cout << "  double value=" << f << endl;
      }

    {
      clog << "Storing..." << endl;
      ofstream fout ("p2.data", ios_base::binary);
      boost::archive::portable_binary_oarchive poa (fout);

      uint32_t szf1 = vf1.size ();
      poa & szf1;
      for (int i = 0; i < szf1; ++i)
	{
	  poa & vf1[i];
	}
      uint32_t szf2 = vf2.size ();
      poa & szf2;
      for (int i = 0; i < szf2; ++i)
	{
	  poa & vf2[i];
	}

      uint32_t szd1 = vd1.size ();
      poa & szd1;
      for (int i = 0; i < szd1; ++i)
	{
	  poa & vd1[i];
	}
      uint32_t szd2 = vd2.size ();
      poa & szd2;
      for (int i = 0; i < szd2; ++i)
	{
	  poa & vd2[i];
	}
      clog << "Done." << endl;
    }
  }

  /* the output 'p2.data' file is :

    shell> od -t u1 p2.data 
    0000000 127   1   7   1   3   4   0   0 128 191   0   4   0   0 128  63
    0000020   1   3   4   0   0 128 255   4   0   0 128 127   4 255 255 255
    0000040 127   1   3   8   0   0   0   0   0   0 240 191   0   8   0   0
    0000060   0   0   0   0 240  63   1   3   8   0   0   0   0   0   0 240
    0000100 255   8   0   0   0   0   0   0 240 127   8 255 255 255 255 255
    0000120 255 255 127
    0000123

  */

  {
    vector<float> vf1;
    vector<float> vf2;
    vector<double> vd1;
    vector<double> vd2;
    {
      clog << "Loading..." << endl;
      ifstream fin ("p2.data", ios_base::binary);
      boost::archive::portable_binary_iarchive pia (fin);

      uint32_t szf1;
      pia & szf1;
      vf1.assign (szf1, 0.0F);      
      for (int i = 0; i < szf1; ++i)
	{
	  pia & vf1[i];
	}

      uint32_t szf2;
      pia & szf2;
      vf2.assign (szf2, 0.0F);      
      for (int i = 0; i < szf2; ++i)
	{
	  pia & vf2[i];
	}

      uint32_t szd1;
      pia & szd1;
      vd1.assign (szd1, 0.0);      
      for (int i = 0; i < szd1; ++i)
	{
	  pia & vd1[i];
	}

      uint32_t szd2;
      pia & szd2;
      vd2.assign (szd2, 0.0);      
      for (int i = 0; i < szd2; ++i)
	{
	  pia & vd2[i];
	}
      clog << "Done." << endl;
    }
    cout << "vf1:" << endl;
    BOOST_FOREACH (const float & f, vf1)
      {
	cout << "  float value=" << f << endl;
      }
    cout << "vf2:" << endl;
    BOOST_FOREACH (const float & f, vf2)
      {
	cout << "  float value=" << f << endl;
      }
    cout << "vd1:" << endl;
    BOOST_FOREACH (const double & f, vd1)
      {
	cout << "  double value=" << f << endl;
      }
    cout << "vd2:" << endl;
    BOOST_FOREACH (const double & f, vd2)
      {
	cout << "  double value=" << f << endl;
      }
  }
  return 0;
}

// end of p2.cxx
