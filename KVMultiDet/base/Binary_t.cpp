//$Id: Binary_t.cpp,v 1.3 2007/11/21 11:23:34 franklan Exp $

////////////////////////////////////////////////////////////////////////////
//BinaryX_t with X=8, 16, 32 or 64
//
//Classes representing binary numbers with 8, 16, 32 or 64 bits.
//
//The following are valid initialisations:
//
//      BinaryX_t a,b,c; Hexa_t d;
//      a = 26;
//      b = "110001";
//      c = d;
//      BinaryX_t e("1111");
//
//Use Value() to obtain the decimal value of the number.
//Use String() to obtain a string containing the binary representation.
//Default behaviour is to use the smallest number of bits necessary (single '0' for zero).
//
//If you use SetNBits to set the number of bits, any value, even 0, will always be represented using that number of bits.
//Use SetNBits(0) to reset to default behaviour.
//You can also impose the number of bits by calling String(nbits).
//
//NOTA BENE: the number of bits affects only the representation of the number as
//a string of '1's and '0's.
//It does not in any way affect either the current value of the number or its maximum possible value (given by Max()).
//
//BIT NUMBERING CONVENTION
//It is the same as the ROOT macro BIT(i) i.e.
//
//      Number = 01101110
//      Bit    = 76543210
//
//SetBit(UChar_t nbit, T val)
//---------------------------
//Set bit 'nbit' to 1 (the least significant bit is numbered 0).
//If "val" is given, bit 'nbit' is set to the same value as the equivalent bit in 'val'
//
//ResetBit(UChar_t nbit)
//----------------------
//Set bit 'nbit' to 0 (the least significant bit is numbered 0).
//
//TestBit(UChar_t nbit)
//---------------------
//Returns kTRUE if bit 'nbit' is equal to '1' (the least significant bit is numbered 0).
//
//Hexa()
//------
//Returns number in its hexadecimal form (see Hexa_t)
//
//WriteSubvalue(const T val, UChar_t msb, UChar_t nbits)
//------------------------------------------------------
//Write the value 'val' in the 'nbits' bits from bit 'msb' (Most Significant Bit) to bit (msb-nbits+1).
//
//i.e. suppose b is an 8-bit binary number and we want to write the decimal value 10 in the 4 bits from bit 2 to bit 5
//(remember that the bits are numbered from right to left, LSB=bit 0).
//If b="00000000" and we do b.WriteSubvalue(10, 5, 4), we will have b="00101000"
//
//Error if the following conditions are not respected:
//1 <= nbits <= fNBits   0 <= msb <= (fNBits-1)  (msb-nbits+1)>=0
//
//Subvalue(UChar_t firstbit, UChar_t nbits)
//-----------------------------------------
//Returns the decimal value of a part of this binary number.
//('firstbit' is equivalent to 'msb' argument in WriteSubvalue).
//The bits (firstbit, firstbit-1, firstbit-2, ... , firstbit-nbits+1) [see BIT NUMBERING CONVENTION]
//are used.
//If firstbit-nbits+1 < 0 (i.e. there are not enough bits in the number)
//we use all the bits from firstbit down to bit 0.
//
//Max()
//-----
//Returns maximum value that can be stored in number, i.e. when all bits = 1
//
//Print()
//-------
//Print following info: string representation of binary number, number of bits, optional number of bits used for representation
//(default value 0), decimal value of number.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Binary_t.h"

templateClassImp(Binary_t)
