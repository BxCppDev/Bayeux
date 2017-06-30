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

TArrayCMod::TArrayCMod() {
  fN = 0;
  fArray = 0;
  fCapacity = 0;
}

TArrayCMod::~TArrayCMod() {
  if (fArray != 0) delete [] fArray;
  fArray = 0;
}

void TArrayCMod::Clear() {
  fN = 0;
}

void TArrayCMod::Reserve(Int_t n) {
  if (n > fCapacity) {
    Char_t *temp = fArray;
    fArray = new Char_t[n];
    memcpy(fArray, temp, fN * sizeof(Char_t));
    memset(&fArray[fN], 0, (n - fN) * sizeof(Char_t));
    fCapacity = n;
  }
}

void TArrayCMod::Set(Int_t n) {
  // Set size of this array to n chars.
  if (n < 0) return;
  Reserve(n);
  fN = n;
  memset(fArray, 0, fN * sizeof (Char_t));
}

void TArrayCMod::Streamer(TBuffer &b) {
  // Stream a TArrayCMod object.
  if (b.IsReading()) {
    Int_t n;
    b >> n;
    Set(n);
    b.ReadFastArray(fArray,n);
  } else {
    b << fN;
    b.WriteFastArray(fArray, fN);
  }
}
