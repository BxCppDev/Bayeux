// test_lahague.cxx
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//                       and Arnaud Chapon <achapon@cerap.fr>
//
// This file is part of LaHague.
//
// LaHague is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LaHague is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with LaHague. If not, see <http://www.gnu.org/licenses/>.
//

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// This project:
#include <lahague/lahague_config.h>
#include <lahague/version.h>

int main (int /* argc_ */, char ** /* argv_*/)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for the 'lahague' program library." << endl;

      clog << "lahague version : " << LAHAGUE_LIB_VERSION << endl;

      clog << "The end." << endl;
    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error !" << endl;
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}
