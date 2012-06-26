// -*- mode: c++; -*- 
// genbb_commons.cc
 
#include <genbb_help/genbb_commons.h>

void ENRANGE_t::reset ()
{
  ebb1 = 0.0F;
  ebb2 = 4.3F; // MeV
  toallevents = 1.0F;
  levelE = 0;
  return;
}
  
ENRANGE_t::ENRANGE_t ()
{
  reset ();
  return;
}

GENEVENT_t::GENEVENT_t ()
{
  reset ();
  return;
}

void GENEVENT_t::reset ()
{
  // reset Fortran common block:
  this->npfull = 0;
  this->tevst = 0.0F;
  for (int i = 0; i < (int) GENBB_MAX_NB_PARTICLES; i++)
    {
      this->npgeant[i] = 0;
      for (int j = 0; j < 3; j++)
        {
          this->pmoment[i][j] = 0.0F;
        }
      this->ptime[i] = 0.0F;
    }
  return;
}

GENBBPAR_t::GENBBPAR_t ()
{
  reset ();
  return;
}

void GENBBPAR_t::reset ()
{
  nevents = 1;
  ievstart = 1;
  irndmst = 1;
  iwrfile = 0;
  for (int i = 0; i < 256; i++)
    {
      chfile[i] = 0;
    }
  return;
}

// end of genbb_commons.cc
