
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TArrayCMod                                                           //
//                                                                      //
// Array of chars or bytes (8 bits per element)                         //
// with capacity support                                                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <brio/detail/TArrayCMod.h>

#include "TBuffer.h"

using namespace std;

ClassImp(TArrayCMod)

//______________________________________________________________________________
TArrayCMod::TArrayCMod()
{
  //cerr << "DEVEL: " << "TArrayCMod::TArrayCMod: " << "Entering..." << endl;
  // Default TArrayCMod ctor.
  fN = 0;
  fArray = 0;
  fCapacity = 0;
  //cerr << "DEVEL: " << "TArrayCMod::TArrayCMod: " << "Exiting." << endl;
}

//______________________________________________________________________________
TArrayCMod::~TArrayCMod()
{
  //cerr << "DEVEL: " << "TArrayCMod::~TArrayCMod: " << "Entering..." << endl;
   // Delete TArrayCMod object.
  if (fArray != 0) 
    {
      delete [] fArray;
    }
   fArray = 0;
   //cerr << "DEVEL: " << "TArrayCMod::~TArrayCMod: " << "Exiting." << endl;
}

//______________________________________________________________________________
void TArrayCMod::Clear()
{
  //memset(fArray, 0, fN * sizeof(Char_t));
  fN = 0;
}

//______________________________________________________________________________
void TArrayCMod::Reserve(Int_t n)
{
  if (n > fCapacity)
    {
      Char_t * temp = fArray;
      fArray = new Char_t[n];
      memcpy (fArray, temp, fN * sizeof (Char_t));
      memset(&fArray[fN], 0, (n - fN) * sizeof (Char_t));
      fCapacity = n;
    }
}

//______________________________________________________________________________
void TArrayCMod::Set(Int_t n)
{
   // Set size of this array to n chars.
 
   if (n < 0) return;
   Reserve (n);
   fN = n;
   memset (fArray, 0, fN * sizeof (Char_t));
   /*
   if (n != fN) {
      Char_t *temp = fArray;
      if (n != 0) {
         fArray = new Char_t[n];
         if (n < fN) memcpy(fArray,temp, n*sizeof(Char_t));
         else {
            memcpy(fArray,temp,fN*sizeof(Char_t));
            memset(&fArray[fN],0,(n-fN)*sizeof(Char_t));
         }
      } else {
         fArray = 0;
      }
      if (fN) delete [] temp;
      fN = n;
   }
   */
}

//_______________________________________________________________________
void TArrayCMod::Streamer(TBuffer &b)
{
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

// end of TArrayCMod.cc
