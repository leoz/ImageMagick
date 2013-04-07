/*
  Copyright 1999-2013 ImageMagick Studio LLC, a non-profit organization
  dedicated to making software imaging solutions freely available.
  
  You may not use this file except in compliance with the License.
  obtain a copy of the License at
  
    http://www.imagemagick.org/script/license.php
  
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  MagickCore private graphic gems methods.
*/
#ifndef _MAGICKCORE_GEM_PRIVATE_H
#define _MAGICKCORE_GEM_PRIVATE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#include "magick/pixel-private.h"

#define D65X  (0.950456f)
#define D65Y  (1.0f)
#define D65Z  (1.088754f)
#define CIEEpsilon  (216.0f/24389.0f)
#define CIEK  (24389.0f/27.0f)

static inline void ConvertLabToXYZ(const double L,const double a,const double b,
  double *X,double *Y,double *Z)
{
  double
    x,
    y,
    z;

  assert(X != (double *) NULL);
  assert(Y != (double *) NULL);
  assert(Z != (double *) NULL);
  y=(100.0f*L+16.0f)/116.0f;
  x=y+255.0f*(a-0.5f)/500.0f;
  z=y-255.0f*(b-0.5f)/200.0f;
  if ((x*x*x) > CIEEpsilon)
    x=(x*x*x);
  else
    x=(116.0f*x-16.0f)/CIEK;
  if ((y*y*y) > CIEEpsilon)
    y=(y*y*y);
  else
    y=(100.0f*L)/CIEK;
  if ((z*z*z) > CIEEpsilon)
    z=(z*z*z);
  else
    z=(116.0f*z-16.0f)/CIEK;
  *X=D65X*x;
  *Y=D65Y*y;
  *Z=D65Z*z;
}

static inline void ConvertXYZToLuv(const double X,const double Y,const double Z,
  double *L,double *u,double *v)
{
  double
    alpha;

  assert(L != (double *) NULL);
  assert(u != (double *) NULL);
  assert(v != (double *) NULL);
  if ((Y/D65Y) > CIEEpsilon)
    *L=(double) (116.0f*pow(Y/D65Y,1.0/3.0)-16.0f);
  else
    *L=CIEK*(Y/D65Y);
  alpha=PerceptibleReciprocal(X+15.0f*Y+3.0f*Z);
  *u=13.0f*(*L)*((4.0f*alpha*X)-(4.0f*D65X/(D65X+15.0f*D65Y+3.0f*D65Z)));
  *v=13.0f*(*L)*((9.0f*alpha*Y)-(9.0f*D65Y/(D65X+15.0f*D65Y+3.0f*D65Z)));
  *L/=100.0f;
  *u=(*u+134.0f)/354.0f;
  *v=(*v+140.0f)/262.0f;
}

static inline void ConvertRGBToXYZ(const Quantum red,const Quantum green,
  const Quantum blue,double *X,double *Y,double *Z)
{
  double
    b,
    g,
    r;

  assert(X != (double *) NULL);
  assert(Y != (double *) NULL);
  assert(Z != (double *) NULL);
  r=QuantumScale*red;
  g=QuantumScale*green;
  b=QuantumScale*blue;
  *X=0.41239558896741421610*r+0.35758343076371481710*g+0.18049264738170157350*b;
  *Y=0.21258623078559555160*r+0.71517030370341084990*g+0.07220049864333622685*b;
  *Z=0.01929721549174694484*r+0.11918386458084853180*g+0.95049712513157976600*b;
}

static inline void ConvertXYZToLab(const double X,const double Y,const double Z,
  double *L,double *a,double *b)
{
  double
    x,
    y,
    z;

  assert(L != (double *) NULL);
  assert(a != (double *) NULL);
  assert(b != (double *) NULL);
  if ((X/D65X) > CIEEpsilon)
    x=pow(X/D65X,1.0/3.0);
  else
    x=(CIEK*X/D65X+16.0f)/116.0f;
  if ((Y/D65Y) > CIEEpsilon)
    y=pow(Y/D65Y,1.0/3.0);
  else
    y=(CIEK*Y/D65Y+16.0f)/116.0f;
  if ((Z/D65Z) > CIEEpsilon)
    z=pow(Z/D65Z,1.0/3.0);
  else
    z=(CIEK*Z/D65Z+16.0f)/116.0f;
  *L=((116.0f*y)-16.0f)/100.0f;
  *a=(500.0f*(x-y))/255.0f+0.5f;
  *b=(200.0f*(y-z))/255.0f+0.5f;
}

static inline void ConvertLuvToXYZ(const double L,const double u,const double v,
  double *X,double *Y,double *Z)
{
  assert(X != (double *) NULL);
  assert(Y != (double *) NULL);
  assert(Z != (double *) NULL);
  if ((100.0f*L) > (CIEK*CIEEpsilon))
    *Y=(double) pow(((100.0*L)+16.0)/116.0,3.0);
  else
    *Y=(100.0f*L)/CIEK;
  *X=((*Y*((39.0f*(100.0f*L)/((262.0f*v-140.0f)+13.0f*(100.0f*L)*(9.0f*D65Y/
    (D65X+15.0f*D65Y+3.0f*D65Z))))-5.0f))+5.0f*(*Y))/((((52.0f*(100.0f*L)/
    ((354.0f*u-134.0f)+13.0f*(100.0f*L)*(4.0f*D65X/(D65X+15.0f*D65Y+3.0f*
    D65Z))))-1.0f)/3.0f)-(-1.0f/3.0f));
  *Z=(*X*(((52.0f*(100.0f*L)/((354.0f*u-134.0f)+13.0f*(100.0f*L)*(4.0f*D65X/
    (D65X+15.0f*D65Y+3.0f*D65Z))))-1.0f)/3.0f))-5.0f*(*Y);
}

static inline void ConvertXYZToRGB(const double x,const double y,const double z,
  Quantum *red,Quantum *green,Quantum *blue)
{
  double
    b,
    g,
    r;

  /*
    Convert XYZ to RGB colorspace.
  */
  assert(red != (Quantum *) NULL);
  assert(green != (Quantum *) NULL);
  assert(blue != (Quantum *) NULL);
  r=3.2406f*x-1.5372f*y-0.4986f*z;
  g=(-0.9689f*x+1.8758f*y+0.0415f*z);
  b=0.0557f*x-0.2040f*y+1.0570f*z;
  *red=ClampToQuantum((MagickRealType) QuantumRange*r);
  *green=ClampToQuantum((MagickRealType) QuantumRange*g);
  *blue=ClampToQuantum((MagickRealType) QuantumRange*b);
}


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif