/*
  Copyright 1999-2012 ImageMagick Studio LLC, a non-profit organization
  dedicated to making software imaging solutions freely available.

  You may not use this file except in compliance with the License.
  obtain a copy of the License at

    http://www.imagemagick.org/script/license.php

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  MagickCore pixel accessor methods.
*/
#ifndef _MAGICKCORE_PIXEL_ACCESSOR_H
#define _MAGICKCORE_PIXEL_ACCESSOR_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#include <math.h>
#include <magick/pixel.h>

#define ClampPixelRed(pixel) ClampToQuantum((pixel)->red)
#define ClampPixelGreen(pixel) ClampToQuantum((pixel)->green)
#define ClampPixelBlue(pixel) ClampToQuantum((pixel)->blue)
#define ClampPixelIndex(indexes) ClampToQuantum(*(indexes))
#define ClampPixelOpacity(pixel) ClampToQuantum((pixel)->opacity)
#define GetPixela(pixel) ((pixel)->green)
#define GetPixelb(pixel) ((pixel)->blue)
#define GetPixelAlpha(pixel) (QuantumRange-(pixel)->opacity)
#define GetPixelBlack(indexes) (*(indexes))
#define GetPixelBlue(pixel) ((pixel)->blue)
#define GetPixelCb(pixel) ((pixel)->green)
#define GetPixelCr(pixel) ((pixel)->blue)
#define GetPixelCyan(pixel) ((pixel)->red)
#define GetPixelGray(pixel) ((pixel)->red)
#define GetPixelGreen(pixel) ((pixel)->green)
#define GetPixelIndex(indexes) (*(indexes))
#define GetPixelL(pixel) ((pixel)->red)
#define GetPixelMagenta(pixel) ((pixel)->green)
#define GetPixelNext(pixel)  ((pixel)+1)
#define GetPixelOpacity(pixel) ((pixel)->opacity)
#define GetPixelRed(pixel) ((pixel)->red)
#define GetPixelRGB(pixel,packet) \
{ \
  (packet)->red=GetPixelRed((pixel)); \
  (packet)->green=GetPixelGreen((pixel)); \
  (packet)->blue=GetPixelBlue((pixel)); \
}
#define GetPixelRGBO(pixel,packet) \
{ \
  (packet)->red=GetPixelRed((pixel)); \
  (packet)->green=GetPixelGreen((pixel)); \
  (packet)->blue=GetPixelBlue((pixel)); \
  (packet)->opacity=GetPixelOpacity((pixel)); \
}
#define GetPixelY(pixel) ((pixel)->red)
#define GetPixelYellow(pixel) ((pixel)->blue)
#define SetPixela(pixel,value) ((pixel)->green=(Quantum) (value))
#define SetPixelAlpha(pixel,value) \
  ((pixel)->opacity=(Quantum) (QuantumRange-(value)))
#define SetPixelb(pixel,value) ((pixel)->blue=(Quantum) (value))
#define SetPixelBlack(indexes,value) (*(indexes)=(Quantum) (value))
#define SetPixelBlue(pixel,value) ((pixel)->blue=(Quantum) (value))
#define SetPixelCb(pixel,value) ((pixel)->green=(Quantum) (value))
#define SetPixelCr(pixel,value) ((pixel)->blue=(Quantum) (value))
#define SetPixelCyan(pixel,value) ((pixel)->red=(Quantum) (value))
#define SetPixelGray(pixel,value) \
  ((pixel)->red=(pixel)->green=(pixel)->blue=(Quantum) (value))
#define SetPixelGreen(pixel,value) ((pixel)->green=(Quantum) (value))
#define SetPixelIndex(indexes,value) (*(indexes)=(IndexPacket) (value))
#define SetPixelL(pixel,value) ((pixel)->red=(Quantum) (value))
#define SetPixelMagenta(pixel,value) ((pixel)->green=(Quantum) (value))
#define SetPixelOpacity(pixel,value) \
  ((pixel)->opacity=(Quantum) (value))
#define SetPixelRed(pixel,value) ((pixel)->red=(Quantum) (value))
#define SetPixelRgb(pixel,packet) \
{ \
  SetPixelRed(pixel,(packet)->red); \
  SetPixelGreen(pixel,(packet)->green); \
  SetPixelBlue(pixel,(packet)->blue); \
}
#define SetPixelRGBA(pixel,packet) \
{ \
  SetPixelRed(pixel,(packet)->red); \
  SetPixelGreen(pixel,(packet)->green); \
  SetPixelBlue(pixel,(packet)->blue); \
  SetPixelAlpha(pixel,(QuantumRange-(packet)->opacity)); \
}
#define SetPixelRGBO(pixel,packet) \
{ \
  SetPixelRed(pixel,(packet)->red); \
  SetPixelGreen(pixel,(packet)->green); \
  SetPixelBlue(pixel,(packet)->blue); \
  SetPixelOpacity(pixel,(packet)->opacity); \
}
#define SetPixelYellow(pixel,value) ((pixel)->blue=(Quantum) (value))
#define SetPixelY(pixel,value) ((pixel)->red=(Quantum) (value))

static inline MagickRealType InversesRGBCompandor(const MagickRealType pixel)
{
  if (pixel <= (0.04045*QuantumRange))
    return(pixel/12.92);
  return(QuantumRange*pow((QuantumScale*pixel+0.055)/1.055,2.4));
}

static inline MagickRealType sRGBCompandor(const MagickRealType pixel)
{
  if (pixel <= (0.0031308*QuantumRange))
    return(12.92*pixel);
  return(QuantumRange*(1.055*pow(QuantumScale*pixel,1.0/2.4)-0.055));
}

static inline MagickRealType GetPixelIntensity(const Image *image,
  const PixelPacket *pixel)
{
  MagickRealType
    blue,
    green,
    red;

  if (image->colorspace == GRAYColorspace)
    return((MagickRealType) pixel->red);
  if (image->colorspace != sRGBColorspace)
    return(0.298839*pixel->red+0.586811*pixel->green+0.114350*pixel->blue);
  red=InversesRGBCompandor((MagickRealType) pixel->red);
  green=InversesRGBCompandor((MagickRealType) pixel->green);
  blue=InversesRGBCompandor((MagickRealType) pixel->blue);
  return((MagickRealType) (0.298839*red+0.586811*green+0.114350*blue));
}

static inline MagickBooleanType IsPixelGray(const PixelPacket *pixel)
{
  MagickRealType
    blue,
    green,
    red;

  red=(MagickRealType) pixel->red;
  green=(MagickRealType) pixel->green;
  blue=(MagickRealType) pixel->blue;
  if ((fabs(red-green) < MagickEpsilon) && (fabs(green-blue) < MagickEpsilon))
    return(MagickTrue);
  return(MagickFalse);
}

static inline Quantum PixelPacketIntensity(const PixelPacket *pixel)
{
  MagickRealType
    blue,
    green,
    red;

  red=InversesRGBCompandor((MagickRealType) pixel->red);
  green=InversesRGBCompandor((MagickRealType) pixel->green);
  blue=InversesRGBCompandor((MagickRealType) pixel->blue);
  return(ClampToQuantum(0.298839*red+0.586811*green+0.114350*blue));
}

static inline Quantum PixelIntensityToQuantum(const Image *restrict image,
  const PixelPacket *restrict pixel)
{
  MagickRealType
    blue,
    green,
    red;

  if (image->colorspace == GRAYColorspace)
    return(GetPixelGray(pixel));
  if (image->colorspace != sRGBColorspace)
    return(ClampToQuantum(0.298839*pixel->red+0.586811*pixel->green+0.114350*
      pixel->blue));
  red=InversesRGBCompandor((MagickRealType) pixel->red);
  green=InversesRGBCompandor((MagickRealType) pixel->green);
  blue=InversesRGBCompandor((MagickRealType) pixel->blue);
  return(ClampToQuantum(0.298839*red+0.586811*green+0.114350*blue));
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif