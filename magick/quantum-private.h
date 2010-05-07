/*
  Copyright 1999-2008 ImageMagick Studio LLC, a non-profit organization
  dedicated to making software imaging solutions freely available.

  You may not use this file except in compliance with the License.
  obtain a copy of the License at

    http://www.imagemagick.org/script/license.php

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  MagickCore quantum inline methods.
*/
#ifndef _MAGICKCORE_QUANTUM_PRIVATE_H
#define _MAGICKCORE_QUANTUM_PRIVATE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#include "magick/cache.h"

typedef struct _QuantumState
{
  EndianType
    endian;

  double
    minimum,
    scale,
    inverse_scale;

  unsigned long
    pixel,
    bits;

  const unsigned long
    *mask;
} QuantumState;

struct _QuantumInfo
{
  unsigned long
    depth,
    quantum;

  QuantumFormatType
    format;

  double
    minimum,
    maximum,
    scale;

  size_t
    pad;

  MagickBooleanType
    min_is_white,
    pack;

  QuantumAlphaType
    alpha_type;

  unsigned long
    number_threads;

  unsigned char
    **pixels;

  size_t
    extent;

  SemaphoreInfo
    *semaphore;

  unsigned long
    signature;
};

static inline MagickSizeType GetQuantumRange(const unsigned long depth)
{
  return((MagickSizeType) ((MagickULLConstant(1) << (depth-1))+
    ((MagickULLConstant(1) << (depth-1))-1)));
}

static inline float HalfToSinglePrecision(const unsigned short half)
{
#define ExponentBias  (127-15)
#define ExponentMask  0x7c00
#define ExponentShift  23
#define SignBitShift  31
#define SignificandShift  13
#define SignificandMask  0x00000400

  typedef union _SinglePrecision
  {
    unsigned int
      fixed_point;

    float
      single_precision;
  } SinglePrecision;

  register unsigned int
    exponent,
    significand,
    sign_bit;

  SinglePrecision
    map;

  unsigned int
    value;

  /*
    The IEEE 754 standard specifies half precision as having:

      Sign bit: 1 bit
      Exponent width: 5 bits
      Significand precision: 11 (10 explicitly stored)
  */
  sign_bit=(unsigned int) ((half >> 15) & 0x00000001);
  exponent=(unsigned int) ((half >> 10) & 0x0000001f);
  significand=(unsigned int) (half & 0x000003ff);
  if (exponent == 0)
    {
    	if (significand == 0)
        value=sign_bit << SignBitShift;
	    else
	      {
          while ((significand & SignificandMask) == 0)
          {
            significand<<=1;
            exponent--;
          }
	        exponent++;
          significand&=(~SignificandMask);
          exponent+=ExponentBias;
          value=(sign_bit << SignBitShift) | (exponent << ExponentShift) |
            (significand << SignificandShift);
       	}
    }
  else
    if (exponent == SignBitShift)
      {
        value=(sign_bit << SignBitShift) | 0x7f800000;
        if (significand != 0)
          value|=(significand << SignificandShift);
      }
    else
      {
        exponent+=ExponentBias;
        significand<<=SignificandShift;
        value=(sign_bit << SignBitShift) | (exponent << ExponentShift) |
          significand;
      }
  map.fixed_point=value;
  return(map.single_precision);
}

static inline void InitializeQuantumState(const QuantumInfo *quantum_info,
  const EndianType endian,QuantumState *quantum_state)
{
  static const unsigned long mask[32] =
  {
    0x00000000UL, 0x00000001UL, 0x00000003UL, 0x00000007UL, 0x0000000fUL,
    0x0000001fUL, 0x0000003fUL, 0x0000007fUL, 0x000000ffUL, 0x000001ffUL,
    0x000003ffUL, 0x000007ffUL, 0x00000fffUL, 0x00001fffUL, 0x00003fffUL,
    0x00007fffUL, 0x0000ffffUL, 0x0001ffffUL, 0x0003ffffUL, 0x0007ffffUL,
    0x000fffffUL, 0x001fffffUL, 0x003fffffUL, 0x007fffffUL, 0x00ffffffUL,
    0x01ffffffUL, 0x03ffffffUL, 0x07ffffffUL, 0x0fffffffUL, 0x1fffffffUL,
    0x3fffffffUL, 0x7fffffffUL
  };

  quantum_state->endian=endian;
  quantum_state->minimum=quantum_info->minimum;
  quantum_state->scale=quantum_info->scale;
  quantum_state->inverse_scale=1.0;
  if (quantum_state->scale != 0.0)
    quantum_state->inverse_scale/=quantum_state->scale;
  quantum_state->pixel=0UL;
  quantum_state->bits=0UL;
  quantum_state->mask=mask;
}

static inline unsigned char *PopCharPixel(const unsigned char pixel,
  unsigned char *pixels)
{
  *pixels++=pixel;
  return(pixels);
}

static inline unsigned char *PopLongPixel(const EndianType endian,
  const unsigned long pixel,unsigned char *pixels)
{
  register unsigned int
    quantum;

  quantum=(unsigned int) pixel;
  if (endian != LSBEndian)
    {
      *pixels++=(unsigned char) (quantum >> 24);
      *pixels++=(unsigned char) (quantum >> 16);
      *pixels++=(unsigned char) (quantum >> 8);
      *pixels++=(unsigned char) (quantum);
      return(pixels);
    }
  *pixels++=(unsigned char) (quantum);
  *pixels++=(unsigned char) (quantum >> 8);
  *pixels++=(unsigned char) (quantum >> 16);
  *pixels++=(unsigned char) (quantum >> 24);
  return(pixels);
}

static inline unsigned char *PopShortPixel(const EndianType endian,
  const unsigned short pixel,unsigned char *pixels)
{
  register unsigned int
    quantum;

  quantum=pixel;
  if (endian != LSBEndian)
    {
      *pixels++=(unsigned char) (quantum >> 8);
      *pixels++=(unsigned char) (quantum);
      return(pixels);
    }
  *pixels++=(unsigned char) (quantum);
  *pixels++=(unsigned char) (quantum >> 8);
  return(pixels);
}

static inline const unsigned char *PushCharPixel(const unsigned char *pixels,
  unsigned char *pixel)
{
  *pixel=(*pixels++);
  return(pixels);
}

static inline const unsigned char *PushLongPixel(const EndianType endian,
  const unsigned char *pixels,unsigned long *pixel)
{
  register unsigned int
    quantum;

  if (endian != LSBEndian)
    {
      quantum=(unsigned int) (*pixels++ << 24);
      quantum|=(unsigned int) (*pixels++ << 16);
      quantum|=(unsigned int) (*pixels++ << 8);
      quantum|=(unsigned int) (*pixels++);
    }
  else
    {
      quantum=(unsigned int) (*pixels++);
      quantum|=(unsigned int) (*pixels++ << 8);
      quantum|=(unsigned int) (*pixels++ << 16);
      quantum|=(unsigned int) (*pixels++ << 24);
    }
  *pixel=(unsigned long) (quantum & 0xffffffff);
  return(pixels);
}

static inline const unsigned char *PushShortPixel(const EndianType endian,
  const unsigned char *pixels,unsigned short *pixel)
{
  register unsigned int
    quantum;

  if (endian != LSBEndian)
    {
      quantum=(unsigned int) (*pixels++ << 8);
      quantum|=(unsigned int) *pixels++;
    }
  else
    {
      quantum=(unsigned int) *pixels++;
      quantum|=(unsigned int) (*pixels++ << 8);
    }
  *pixel=(unsigned short) (quantum & 0xffff);
  return(pixels);
}

static inline Quantum ScaleAnyToQuantum(const QuantumAny quantum,
  const QuantumAny range)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((Quantum) (((MagickRealType) QuantumRange*quantum)/range+0.5));
#else
  return((Quantum) (((MagickRealType) QuantumRange*quantum)/range+0.0));
#endif
}

static inline QuantumAny ScaleQuantumToAny(const Quantum quantum,
  const QuantumAny range)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((QuantumAny) (((MagickRealType) range*quantum)/QuantumRange+0.5));
#else
  return((QuantumAny) (((MagickRealType) range*quantum)/QuantumRange+0.5));
#endif
}

#if (MAGICKCORE_QUANTUM_DEPTH == 8)
static inline Quantum ScaleCharToQuantum(const unsigned char value)
{
  return((Quantum) value);
}

static inline Quantum ScaleLongToQuantum(const unsigned long value)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((Quantum) ((value+8421504UL)/16843009UL));
#else
  return((Quantum) (value/16843009.0));
#endif
}

static inline Quantum ScaleMapToQuantum(const MagickRealType value)
{
  if (value <= 0.0)
    return((Quantum) 0);
#if defined(MAGICKCORE_HDRI_SUPPORT)
  if (value >= MaxMap)
    return((Quantum) QuantumRange);
  return((Quantum) value);
#else
  if ((value+0.5) >= MaxMap)
    return((Quantum) QuantumRange);
  return((Quantum) (value+0.5));
#endif
}

static inline unsigned long ScaleQuantumToLong(const Quantum quantum)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((unsigned long) (16843009UL*quantum));
#else
  if (quantum <= 0.0)
    return(0UL);
  if ((16843009.0*quantum) >= 4294967295.0)
    return(4294967295UL);
  return((unsigned long) (16843009.0*quantum+0.5));
#endif
}

static inline unsigned long ScaleQuantumToMap(const Quantum quantum)
{
  if (quantum < 0.0)
    return(0UL);
  if (quantum >= (Quantum) MaxMap)
    return((unsigned long) MaxMap);
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((unsigned long) quantum);
#else
  return((unsigned long) (quantum+0.5));
#endif
}

static inline unsigned short ScaleQuantumToShort(const Quantum quantum)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((unsigned short) (257UL*quantum));
#else
  if (quantum <= 0.0)
    return(0);
  if ((256.0*quantum) >= 65535.0)
    return(65535);
  return((unsigned short) (256.0*quantum+0.5));
#endif
}

static inline Quantum ScaleShortToQuantum(const unsigned short value)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((Quantum) ((value+128U)/257U));
#else
  return((Quantum) (value/256.0+0.5));
#endif
}
#elif (MAGICKCORE_QUANTUM_DEPTH == 16)
static inline Quantum ScaleCharToQuantum(const unsigned char value)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((Quantum) (257U*value));
#else
  return((Quantum) (256.0*value+0.5));
#endif
}

static inline Quantum ScaleLongToQuantum(const unsigned long value)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((Quantum) ((value+MagickULLConstant(32768))/
    MagickULLConstant(65537)));
#else
  return((Quantum) (value/65536.0+0.5));
#endif
}

static inline Quantum ScaleMapToQuantum(const MagickRealType value)
{
  if (value <= 0.0)
    return((Quantum) 0);
#if defined(MAGICKCORE_HDRI_SUPPORT)
  if (value >= MaxMap)
    return((Quantum) QuantumRange);
  return((Quantum) value);
#else
  if ((value+0.5) >= MaxMap)
    return((Quantum) QuantumRange);
  return((Quantum) (value+0.5));
#endif
}

static inline unsigned long ScaleQuantumToLong(const Quantum quantum)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((unsigned long) (65537UL*quantum));
#else
  if (quantum <= 0.0)
    return(0UL);
  if ((65536.0*quantum) >= 4294967295.0)
    return(4294967295UL);
  return((unsigned long) (65536.0*quantum+0.5));
#endif
}

static inline unsigned long ScaleQuantumToMap(const Quantum quantum)
{
  if (quantum >= (Quantum) MaxMap)
    return((unsigned long) MaxMap);
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((unsigned long) quantum);
#else
  if (quantum < 0.0)
    return(0UL);
  return((unsigned long) (quantum+0.5));
#endif
}

static inline unsigned short ScaleQuantumToShort(const Quantum quantum)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((unsigned short) quantum);
#else
  if (quantum <= 0.0)
    return(0);
  if (quantum >= 65535.0)
    return(65535);
  return((unsigned short) (quantum+0.5));
#endif
}

static inline Quantum ScaleShortToQuantum(const unsigned short value)
{
  return((Quantum) value);
}
#elif (MAGICKCORE_QUANTUM_DEPTH == 32)
static inline Quantum ScaleCharToQuantum(const unsigned char value)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((Quantum) (16843009UL*value));
#else
  return((Quantum) (16843008.0*value+0.5));
#endif
}

static inline Quantum ScaleLongToQuantum(const unsigned long value)
{
  return((Quantum) value);
}

static inline Quantum ScaleMapToQuantum(const MagickRealType value)
{
  if (value <= 0.0)
    return((Quantum) 0);
#if defined(MAGICKCORE_HDRI_SUPPORT)
  if (value >= MaxMap)
    return(QuantumRange);
  return((Quantum) (65537.0*value));
#else
  if (value >= MaxMap)
    return(QuantumRange);
  return((Quantum) (65536UL*value+0.5));
#endif
}

static inline unsigned long ScaleQuantumToLong(const Quantum quantum)
{
  return((unsigned long) quantum);
}

static inline unsigned long ScaleQuantumToMap(const Quantum quantum)
{
  if (quantum < 0.0)
    return(0UL);
  if ((quantum/65537) >= MaxMap)
    return((unsigned long) MaxMap);
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((unsigned long) ((quantum+MagickULLConstant(32768))/
    MagickULLConstant(65537)));
#else
  return((unsigned long) (quantum/65536.0+0.5));
#endif
}

static inline unsigned short ScaleQuantumToShort(const Quantum quantum)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((unsigned short) ((quantum+MagickULLConstant(32768))/
    MagickULLConstant(65537)));
#else
  if (quantum <= 0.0)
    return(0);
  if ((quantum/65536.0) >= 65535.0)
    return(65535);
  return((unsigned short) (quantum/65536.0+0.5));
#endif
}

static inline Quantum ScaleShortToQuantum(const unsigned short value)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((Quantum) (65537UL*value));
#else
  return((Quantum) (65536.0*value+0.5));
#endif
}
#elif (MAGICKCORE_QUANTUM_DEPTH == 64)
static inline Quantum ScaleCharToQuantum(const unsigned char value)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((Quantum) (MagickULLConstant(71777214294589695)*value));
#else
  return((Quantum) (71777214294589694.0*value+0.5));
#endif
}

static inline Quantum ScaleLongToQuantum(const unsigned long value)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((Quantum) (4294967295UL*value));
#else
  return((Quantum) (4294967294.0*value+0.5));
#endif
}

static inline Quantum ScaleMapToQuantum(const MagickRealType value)
{
  if (value <= 0.0)
    return((Quantum) 0);
#if defined(MAGICKCORE_HDRI_SUPPORT)
  if (value >= MaxMap)
    return(QuantumRange);
  return((Quantum) (281479271612415.0*value));
#else
  if (value >= MaxMap)
    return(QuantumRange);
  return((Quantum) (MagickULLConstant(281479271612414)*value+0.5));
#endif
}

static inline unsigned long ScaleQuantumToLong(const Quantum quantum)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((unsigned long) ((quantum+2147483648.0)/4294967297.0));
#else
  return((unsigned long) (quantum/4294967296.0+0.5));
#endif
}

static inline unsigned long ScaleQuantumToMap(const Quantum quantum)
{
  if (quantum < 0.0)
    return(0UL);
  if ((quantum/281479271612415.0) >= MaxMap)
    return((unsigned long) MaxMap);
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((unsigned long) ((quantum+2147450879.0)/281479271612415.0));
#else
  return((unsigned long) (quantum/281479271612414.0)+0.5);
#endif
}

static inline unsigned short ScaleQuantumToShort(const Quantum quantum)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((unsigned short) ((quantum+2147450879.0)/281479271612415.0));
#else
  return((unsigned short) (quantum/281479271612414.0+0.5));
#endif
}

static inline Quantum ScaleShortToQuantum(const unsigned short value)
{
#if !defined(MAGICKCORE_HDRI_SUPPORT)
  return((Quantum) (MagickULLConstant(281479271612415)*value));
#else
  return((Quantum) (281479271612414.0*value));
#endif
}
#endif

static inline unsigned short SinglePrecisionToHalf(const float value)
{
  typedef union _SinglePrecision
  {
    unsigned int
      fixed_point;

    float
      single_precision;
  } SinglePrecision;

  register int
    exponent;

  register unsigned int
    significand,
    sign_bit;

  SinglePrecision
    map;

  unsigned short
    half;

  /*
    The IEEE 754 standard specifies half precision as having:

      Sign bit: 1 bit
      Exponent width: 5 bits
      Significand precision: 11 (10 explicitly stored)
  */
  map.single_precision=value;
  sign_bit=(map.fixed_point >> 16) & 0x00008000;
  exponent=(int) ((map.fixed_point >> ExponentShift) & 0x000000ff)-ExponentBias;
  significand=map.fixed_point & 0x007fffff;
  if (exponent <= 0)
    {
      long
        shift;

      if (exponent < -10)
        return((unsigned short) sign_bit);
      significand=significand | 0x00800000;
      shift=14-exponent;
      significand=(unsigned int) ((significand+((1 << (shift-1))-1)+
        ((significand >> shift) & 0x01)) >> shift);
      return((unsigned short) (sign_bit | significand));
    }
  else
    if (exponent == (0xff-ExponentBias))
      {
        if (significand == 0)
          return((unsigned short) (sign_bit | ExponentMask));
        else
          {
            significand>>=SignificandShift;
            half=(unsigned short) (sign_bit | significand |
              (significand == 0) | ExponentMask);
            return(half);
          }
      }
  significand=significand+((significand >> SignificandShift) & 0x01)+0x00000fff;
  if ((significand & 0x00800000) != 0)
    {
      significand=0;
      exponent++;
    }
  if (exponent > 30)
    {
      float
        alpha;

      register long
        i;

      /*
        Float overflow.
      */
      alpha=1.0e10;
      for (i=0; i < 10; i++)
        alpha*=alpha;
      return((unsigned short) (sign_bit | ExponentMask));
    }
  half=(unsigned short) (sign_bit | (exponent << 10) |
    (significand >> SignificandShift));
  return(half);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
