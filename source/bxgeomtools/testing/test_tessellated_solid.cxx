// -*- mode: c++; -*- 
// test_tessellated_solid.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/gnuplot_draw.h>
#include <geomtools/tessellation.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Hello, this is a sample program " 
	   << "for class 'tessellated_solid'!" << endl; 
  
      bool debug = false;

      int iarg = 1;
      while (iarg < argc_)
	{
    
	  string arg = argv_[iarg];

	  if (arg == "-d" || arg == "--debug") debug = true;

	  iarg++;
	}
    
      geomtools::tessellated_solid TS1;

      int ivtx = 0;
      int P = TS1.add_vertex (ivtx++, 0, 0, 0);
      int Q = TS1.add_vertex (ivtx++, 1, 0, 0);
      int R = TS1.add_vertex (ivtx++, 1, 1, 0);
      int S = TS1.add_vertex (ivtx++, 0, 1, 0);
      int T = TS1.add_vertex (ivtx++, 0.5, 0.5, 1);
      //TS1.add_vertex (ivtx++, 0.5, 0.5,-1);

      int ifct = 0;
      TS1.add_facet4 (ifct++, P, S, R, Q);
      TS1.add_facet3 (ifct++, P, Q, T);
      TS1.add_facet3 (ifct++, Q, R, T);
      TS1.add_facet3 (ifct++, R, S, T);
      TS1.add_facet3 (ifct++, S, P, T);

      TS1.lock ();
      TS1.dump (clog);
   
      geomtools::tessellated_solid TS2;
      double z0 = 2;
      int A1 = TS2.add_vertex (ivtx++, 0, 0, z0);
      int B1 = TS2.add_vertex (ivtx++, 2, 0, z0);
      int C1 = TS2.add_vertex (ivtx++, 2, 1, z0);
      int D1 = TS2.add_vertex (ivtx++, 0, 1, z0);
      int A2 = TS2.add_vertex (ivtx++, 0, 0, z0 + 0.5);
      int B2 = TS2.add_vertex (ivtx++, 2, 0, z0 + 0.5);
      int C2 = TS2.add_vertex (ivtx++, 2, 1, z0 + 0.5);
      int D2 = TS2.add_vertex (ivtx++, 0, 1, z0 + 0.5);

      TS2.add_facet4 (ifct++, A2, D2, C2, B2);
      TS2.add_facet4 (ifct++, A1, D1, C1, B1);
      TS2.add_facet4 (ifct++, A1, B1, B2, A2);
      TS2.add_facet4 (ifct++, B1, C1, C2, B2);
      TS2.add_facet4 (ifct++, C1, D1, D2, C2);
      TS2.add_facet4 (ifct++, D1, A1, A2, D2);

      TS2.lock ();
      TS2.dump (clog);
      
      geomtools::vector_3d tessel_pos;
      geomtools::rotation_3d tessel_rot;
      geomtools::create_rotation (tessel_rot, 0.0, 0.0, 0.0);
      geomtools::gnuplot_draw::draw_tessellated (cout, tessel_pos, tessel_rot, TS1);
      cout << endl;
      geomtools::gnuplot_draw::draw_tessellated (cout, tessel_pos, tessel_rot, TS2);
      cout << endl << endl;
 
    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl; 
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_tessellated_solid.cxx
