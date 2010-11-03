// -*- mode: c++; -*-
#ifndef brio_TArrayCMod
#define brio_TArrayCMod


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TArrayCMod                                                           //
//                                                                      //
// Array of chars or bytes (8 bits per element).                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TArray
#include "TArray.h"
#endif

class TArrayCMod : public TArray 
{
public:
  //[fN] Array of fN chars
  Char_t    *fArray;       //
  Int_t      fCapacity;    //! Do not store the capacity

public:
   TArrayCMod();
   virtual ~TArrayCMod();
   void Reserve (Int_t);
   //const Char_t *GetArray() const { return fArray; }
   //Char_t       *GetArray() { return fArray; }
   void          Clear();
   void          Set(Int_t n);

   // Mandatory by TArray inheritance:
   Double_t      GetAt(Int_t i) const { return 0.0; }
   void SetAt(Double_t x, Int_t i) { return; }


   // Workaround
   //void Streamer(TBuffer &b); // let ClassDef build the Streamer
   void __Streamer(TBuffer &b); 

   ClassDef(TArrayCMod,1)  //Array of chars
};

/*
#if defined R__TEMPLATE_OVERLOAD_BUG
template <>
#endif
inline TBuffer &operator>>(TBuffer &buf, TArrayCMod *&obj)
{
   // Read TArrayCMod object from buffer.
   obj = (TArrayCMod *) TArray::ReadArray(buf, TArrayCMod::Class());
   return buf;
}

#if defined R__TEMPLATE_OVERLOAD_BUG
template <>
#endif
inline TBuffer &operator<<(TBuffer &buf, const TArrayCMod *obj)
{
   // Write a TArrayCMod object into buffer
   return buf << (TArray*)obj;
}
*/

/*
inline Char_t TArrayCMod::At(Int_t i) const
{
   if (!BoundsOk("TArrayCMod::At", i)) return 0;
   return fArray[i];
}

inline Char_t &TArrayCMod::operator[](Int_t i)
{
   if (!BoundsOk("TArrayCMod::operator[]", i))
      i = 0;
   return fArray[i];
}

inline Char_t TArrayCMod::operator[](Int_t i) const
{
   if (!BoundsOk("TArrayCMod::operator[]", i)) return 0;
   return fArray[i];
}
*/

#endif // brio_TArrayCMod

// end of TArrayCMod.h

