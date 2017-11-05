// ////////////////////////////////////////////////////////////////////////
//                                                                       //
// TArrayCMod                                                            //
//                                                                       //
// Array of chars or bytes (8 bits per element)                          //
// with capacity support                                                 //
//                                                                       //
// ////////////////////////////////////////////////////////////////////////

// Ourselves:
#include <brio/detail/TArrayCMod.h>

// Third Party:
// - ROOT:
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++11-long-long"
#endif
#include "TBuffer.h"
#ifdef __clang__
#pragma clang diagnostic pop
#endif

// According to this post:
//   http://root.cern.ch/phpBB3/viewtopic.php?f=3&t=11917&p=51767#p51767
// ClassImp is only needed for THtml docs, so comment it out.
// ClassImp(TArrayCMod)

TArrayCMod::TArrayCMod()
{
  fN = 0;
  fArray = 0;
  fCapacity = 0;
  return;
}

TArrayCMod::~TArrayCMod()
{
  if (fArray != 0) delete [] fArray;
  fArray = 0;
  return;
}

void TArrayCMod::Clear()
{
  fN = 0;
  return;
}

void TArrayCMod::Reserve(Int_t n_)
{
  if (n_ > fCapacity) {
    Char_t *temp = fArray;
    fArray = new Char_t[n_];
    memcpy(fArray, temp, fN * sizeof(Char_t));
    memset(&fArray[fN], 0, (n_ - fN) * sizeof(Char_t));
    fCapacity = n_;
  }
  return;
}

void TArrayCMod::Set(Int_t n_)
{
  // Set size of this array to n chars.
  if (n_ < 0) return;
  Reserve(n_);
  fN = n_;
  memset(fArray, 0, fN * sizeof (Char_t));
  return;
}

void TArrayCMod::Streamer(TBuffer & buffer_)
{
  // Stream a TArrayCMod object.
  if (buffer_.IsReading()) {
    Int_t n;
    buffer_ >> n;
    Set(n);
    buffer_.ReadFastArray(fArray,n);
  } else {
    buffer_ << fN;
    buffer_.WriteFastArray(fArray, fN);
  }
  return;
}
