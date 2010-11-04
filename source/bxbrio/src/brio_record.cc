// -*- mode: c++ ; -*- 
/* brio_record.cc
 */

#include <brio/detail/brio_record.h>

ClassImp(brio_record)
 
void brio_record::reset ()
{
  fDataBuffer.Clear ();
  /*
  if (fDataBuffer.fArray != 0)
    {
      delete [] fDataBuffer.fArray;
      fDataBuffer.fArray = 0;
    }
  fDataBuffer.fN = 0;
  */
  return;
}
  
// ctor:
brio_record::brio_record ()
{
  fSerialTag = "";
  fVersionTag = 0;
  return;
}
  
// dtor:
brio_record::~brio_record ()
{
  reset ();
  return;
}
  
/*
  void brio_record::Streamer (TBuffer & buffer_)
  {
  // Stream a brio_record object
  if (buffer_.IsReading ()) 
  {
  Version_t v = buffer_.ReadVersion ();
  TObject::Streamer (buffer_);
  buffer_ >> fSerialTag;
  buffer_ >> fVersionTag;
  fDataBuffer.Streamer (buffer_);
  } 
  else 
  {
  buffer_.WriteVersion (brio_record::IsA ());
  TObject::Streamer(buffer_);
  buffer_ << fSerialTag;
  buffer_ << fVersionTag;
  fDataBuffer.Streamer (buffer_);
  }
  return;
  }  
*/

// end of brio_record.cc
