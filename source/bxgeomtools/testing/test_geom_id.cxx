// -*- mode: c++ ; -*-
// test_geom_id.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/geom_id.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'geom_id'!" << endl;

      bool debug = false;
      bool interactive = false;

      int iarg = 1;
      while (iarg < argc_)
        {
          string arg = argv_[iarg];

          if (arg == "-d" || arg == "--debug") debug = true;
          if (arg == "-i" || arg == "--interactive") interactive = true;

          iarg++;
      }

      const uint32_t TYPE_TC_SIDE = 3;
      const uint32_t TYPE_GG_CELL = 0;
      const uint32_t TYPE_SCIN_BLOCK = 1;

      geomtools::geom_id id0;
      geomtools::geom_id id1 (TYPE_GG_CELL, 2, 3, 1, 4);
      geomtools::geom_id id2 (TYPE_SCIN_BLOCK, 14, 7, 6, 3);
      geomtools::geom_id id3 (TYPE_TC_SIDE, 1);

      cout << "ID0 : " << id0 << ' ' << (id0.is_valid () ? "[Valid]": "[Invalid]") << endl;
      cout << "ID1 : " << id1 << ' ' << (id1.is_valid () ? "[Valid]": "[Invalid]") << endl;
      cout << "ID2 : " << id2 << ' ' << (id2.is_valid () ? "[Valid]": "[Invalid]") << endl;
      cout << "ID3 : " << id3 << ' ' << (id3.is_valid () ? "[Valid]": "[Invalid]") << endl;

      if (id1 < id0)
        {
          cout << "ID1 < ID0" << endl;
        }

      id0.set_type (TYPE_GG_CELL);
      id0.set (4, 666);
      cout << "ID0 : " << id0 << ' ' << (id0.is_valid () ? "[Valid]": "[Invalid]") << endl;
      id0.set (0, 0);
      cout << "ID0 : " << id0 << ' ' << (id0.is_valid () ? "[Valid]": "[Invalid]") << endl;

      id0.set (1, 0);
      id0.set (2, 0);
      id0.set (3, 0);
      cout << "ID0 : " << id0 << ' ' << (id0.is_valid () ? "[Valid]": "[Invalid]") << endl;
      id0.invalidate ();
      cout << "ID0 : " << id0 << ' ' << (id0.is_valid () ? "[Valid]": "[Invalid]") << endl;
      id2.extract_to (id3);
      cout << "ID3 : " << id3 << ' ' << (id3.is_valid () ? "[Valid]": "[Invalid]") << endl;
      id2.extract_to (id3);

      geomtools::geom_id id4 (TYPE_TC_SIDE, geomtools::geom_id::INVALID_ADDRESS);
      id4.extract_from (id2);
      cout << "ID4 : " << id4 << ' ' << (id4.is_valid () ? "[Valid]": "[Invalid]") << endl;

      geomtools::geom_id id5;
      id5.set_type (TYPE_GG_CELL);
      id5.set_depth (6);
      cout << "ID5 : " << id5 << ' ' << (id5.is_valid () ? "[Valid]": "[Invalid]") << endl;

      if (interactive) {
        id5.reset ();
        cout << "ID5 : " << id5 << ' ' << (id5.is_valid () ? "[Valid]": "[Invalid]") << endl;
        cin >> id5;
        if (! cin)
          {
            cerr << "Invalid format !" << endl;
          }
        else
          {
            cout << "ID5 : " << id5 << ' '
                 << (id5.is_valid () ? "[Valid]": "[Invalid]") << endl;
          }
      }

     geomtools::geom_id id6;
     geomtools::geom_id::make (id6, 234, 4);
     cout << "ID6 : " << id6 << ' '
          << (id6.is_valid () ? "[Valid]": "[Invalid]") << endl;

     geomtools::geom_id id7 (TYPE_SCIN_BLOCK, 14, 7, 6, geomtools::geom_id::ANY_ADDRESS);
     cout << "ID7 : " << id7 << ' ' << (id7.is_valid () ? "[Valid]": "[Invalid]") << endl;
     cout << "ID7 : " << id7 << ' ' << (id7.is_complete () ? "[complete]": "[incomplete]") << endl;
     id7.set_any (1);
     cout << "ID7 : " << id7 << ' ' << (id7.is_complete () ? "[complete]": "[incomplete]") << endl;
     id7.set_invalid (2);
     cout << "ID7 : " << id7 << ' ' << (id7.is_complete () ? "[complete]": "[incomplete]") << endl;

     geomtools::geom_id id8 (TYPE_SCIN_BLOCK, 14, 7, 6, 2);
     geomtools::geom_id id9 (TYPE_SCIN_BLOCK, 14, 7, 6, geomtools::geom_id::ANY_ADDRESS);
     geomtools::geom_id id10 (TYPE_SCIN_BLOCK, 14, 7, 6, 3);
     if (geomtools::geom_id::match (id8, id9))
       {
         cout << id8 << " and " << id9 << " match !" << endl;
       }
     else
       {
         cout << id8 << " and " << id9 << " do not match !" << endl;
       }
     if (geomtools::geom_id::match (id8, id9, true))
       {
         cout << id8 << " and " << id9 << " match !" << endl;
       }
     else
       {
         cout << id8 << " and " << id9 << " do not match exactly !" << endl;
       }
     if (geomtools::geom_id::match (id8, id10))
       {
         cout << id8 << " and " << id10 << " match !" << endl;
       }
     else
       {
         cout << id8 << " and " << id10 << " do not match !" << endl;
       }

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

// end of test_geom_id.cxx
