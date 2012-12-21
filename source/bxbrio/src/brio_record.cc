// -*- mode: c++ ; -*- 
/* brio_record.cc
 */

#include <brio/detail/brio_record.h>

ClassImp(brio_record)
 
void brio_record::reset ()
{
  fDataBuffer.Clear ();
  return;
}
  
brio_record::brio_record ()
{
  fSerialTag = "";
  fVersionTag = 0;
  return;
}
  
brio_record::~brio_record ()
{
  reset ();
  return;
}

// end of brio_record.cc
