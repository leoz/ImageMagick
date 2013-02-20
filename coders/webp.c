/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                         W   W  EEEEE  BBBB   PPPP                           %
%                         W   W  E      B   B  P   P                          %
%                         W W W  EEE    BBBB   PPPP                           %
%                         WW WW  E      B   B  P                              %
%                         W   W  EEEEE  BBBB   P                              %
%                                                                             %
%                                                                             %
%                         Read/Write WebP Image Format                        %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 March 2011                                  %
%                                                                             %
%                                                                             %
%  Copyright 1999-2013 ImageMagick Studio LLC, a non-profit organization      %
%  dedicated to making software imaging solutions freely available.           %
%                                                                             %
%  You may not use this file except in compliance with the License.  You may  %
%  obtain a copy of the License at                                            %
%                                                                             %
%    http://www.imagemagick.org/script/license.php                            %
%                                                                             %
%  Unless required by applicable law or agreed to in writing, software        %
%  distributed under the License is distributed on an "AS IS" BASIS,          %
%  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   %
%  See the License for the specific language governing permissions and        %
%  limitations under the License.                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/blob.h"
#include "magick/blob-private.h"
#include "magick/client.h"
#include "magick/display.h"
#include "magick/exception.h"
#include "magick/exception-private.h"
#include "magick/image.h"
#include "magick/image-private.h"
#include "magick/list.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/monitor-private.h"
#include "magick/memory_.h"
#include "magick/option.h"
#include "magick/pixel-accessor.h"
#include "magick/quantum-private.h"
#include "magick/static.h"
#include "magick/string_.h"
#include "magick/string-private.h"
#include "magick/module.h"
#include "magick/utility.h"
#include "magick/xwindow.h"
#include "magick/xwindow-private.h"
#if defined(MAGICKCORE_WEBP_DELEGATE)
#include <webp/decode.h>
#include <webp/encode.h>
#endif

/*
  Forward declarations.
*/
#if defined(MAGICKCORE_WEBP_DELEGATE)
static MagickBooleanType
  WriteWEBPImage(const ImageInfo *,Image *);
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s W E B P                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsWEBP() returns MagickTrue if the image format type, identified by the
%  magick string, is WebP.
%
%  The format of the IsWEBP method is:
%
%      MagickBooleanType IsWEBP(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o magick: compare image format pattern against these bytes.
%
%    o length: Specifies the length of the magick string.
%
*/
static MagickBooleanType IsWEBP(const unsigned char *magick,const size_t length)
{
  if (length < 12)
    return(MagickFalse);
  if (LocaleNCompare((const char *) magick+8,"WEBP",4) == 0)
    return(MagickTrue);
  return(MagickFalse);
}

#if defined(MAGICKCORE_WEBP_DELEGATE)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d W E B P I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadWEBPImage() reads an image in the WebP image format.
%
%  The format of the ReadWEBPImage method is:
%
%      Image *ReadWEBPImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: the image info.
%
%    o exception: return any errors or warnings in this structure.
%
*/
static Image *ReadWEBPImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  MagickBooleanType
    status;

  register unsigned char
    *p;

  size_t
    length;

  ssize_t
    count,
    y;

  unsigned char
    *stream;

  WebPDecoderConfig
    configure;

  WebPDecBuffer
    *const webp_image = &configure.output;

  WebPBitstreamFeatures
    *const features = &configure.input;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AcquireImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  if (WebPInitDecoderConfig(&configure) == 0)
    ThrowReaderException(ResourceLimitError,"UnableToDecodeImageFile");
  length=(size_t) GetBlobSize(image);
  stream=(unsigned char *) AcquireQuantumMemory(length,sizeof(*stream));
  if (stream == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  count=ReadBlob(image,length,stream);
  if (count != (ssize_t) length)
    ThrowReaderException(CorruptImageError,"InsufficientImageDataInFile");
  if (WebPGetFeatures(stream,length,features) != 0)
    {
      stream=(unsigned char*) RelinquishMagickMemory(stream);
      ThrowReaderException(ResourceLimitError,"UnableToDecodeImageFile");
    }
  webp_image->colorspace=MODE_RGBA;
  if (WebPDecode(stream,length,&configure) != 0)
    {
      stream=(unsigned char*) RelinquishMagickMemory(stream);
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
    }
  image->columns=(size_t) webp_image->width;
  image->rows=(size_t) webp_image->height;
  image->matte=features->has_alpha != 0 ? MagickTrue : MagickFalse;
  if (stream[15] == 'L')
    image->quality=100;
  p=webp_image->u.RGBA.rgba;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register PixelPacket
      *q;

    register ssize_t
      x;

    q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      SetPixelRed(q,ScaleCharToQuantum(*p++));
      SetPixelGreen(q,ScaleCharToQuantum(*p++));
      SetPixelBlue(q,ScaleCharToQuantum(*p++));
      SetPixelAlpha(q,ScaleCharToQuantum(*p++));
      q++;
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
    status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  WebPFreeDecBuffer(webp_image);
  stream=(unsigned char*) RelinquishMagickMemory(stream);
  return(image);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r W E B P I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  RegisterWEBPImage() adds attributes for the WebP image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterWEBPImage method is:
%
%      size_t RegisterWEBPImage(void)
%
*/
ModuleExport size_t RegisterWEBPImage(void)
{
  char
    version[MaxTextExtent];

  MagickInfo
    *entry;

  *version='\0';
  entry=SetMagickInfo("WEBP");
#if defined(MAGICKCORE_WEBP_DELEGATE)
  entry->decoder=(DecodeImageHandler *) ReadWEBPImage;
  entry->encoder=(EncodeImageHandler *) WriteWEBPImage;
  (void) FormatLocaleString(version,MaxTextExtent,"libwebp %d.%d.%d",
    (WebPGetDecoderVersion() >> 16) & 0xff,
    (WebPGetDecoderVersion() >> 8) & 0xff,
    (WebPGetDecoderVersion() >> 0) & 0xff);
#endif
  entry->description=ConstantString("WebP Image Format");
  entry->adjoin=MagickFalse;
  entry->module=ConstantString("WEBP");
  entry->magick=(IsImageFormatHandler *) IsWEBP;
  if (*version != '\0')
    entry->version=ConstantString(version);
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r W E B P I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  UnregisterWEBPImage() removes format registrations made by the WebP module
%  from the list of supported formats.
%
%  The format of the UnregisterWEBPImage method is:
%
%      UnregisterWEBPImage(void)
%
*/
ModuleExport void UnregisterWEBPImage(void)
{
  (void) UnregisterMagickInfo("WEBP");
}
#if defined(MAGICKCORE_WEBP_DELEGATE)

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e W E B P I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WriteWEBPImage() writes an image in the WebP image format.
%
%  The format of the WriteWEBPImage method is:
%
%      MagickBooleanType WriteWEBPImage(const ImageInfo *image_info,
%        Image *image)
%
%  A description of each parameter follows.
%
%    o image_info: the image info.
%
%    o image:  The image.
%
*/

static int WebPWriter(const unsigned char *stream,size_t length,
  const WebPPicture *const picture)
{
  Image
    *image;

  image=(Image *) picture->custom_ptr;
  return(length != 0 ? (int) WriteBlob(image,length,stream) : 1);
}

static MagickBooleanType WriteWEBPImage(const ImageInfo *image_info,
  Image *image)
{
  const char
    *value;

  int
    webp_status;

  MagickBooleanType
    status;

  register uint32_t
    *restrict q;

  ssize_t
    y;

  uint32_t
    *pixels;

  WebPConfig
    configure;

  WebPPicture
    picture;

  WebPAuxStats
    statistics;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if ((image->columns > 16383UL) || (image->rows > 16383UL))
    ThrowWriterException(ImageError,"WidthOrHeightExceedsLimit");
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFalse)
    return(status);
  if ((WebPPictureInit(&picture) == 0) || (WebPConfigInit(&configure) == 0))
    ThrowWriterException(ResourceLimitError,"UnableToEncodeImageFile");
  picture.writer=WebPWriter;
  picture.custom_ptr=(void *) image;
  picture.stats=(&statistics);
  picture.width=(int) image->columns;
  picture.height=(int) image->rows;
  picture.argb_stride=(int) image->columns;
  picture.use_argb=1;
  if (image->quality != UndefinedCompressionQuality)
    configure.quality=(float) image->quality;
  if (image->quality >= 100)
    configure.lossless=1;
  value=GetImageOption(image_info,"webp:lossless");
  if (value != (char *) NULL)
    configure.lossless=ParseCommandOption(MagickBooleanOptions,MagickFalse,
      value);
  value=GetImageOption(image_info,"webp:method");
  if (value != (char *) NULL)
    configure.method=StringToInteger(value);
  value=GetImageOption(image_info,"webp:image-hint");
  if (value != (char *) NULL)
    {
      if (LocaleCompare(value,"graph") == 0)
        configure.image_hint=WEBP_HINT_GRAPH;
      if (LocaleCompare(value,"photo") == 0)
        configure.image_hint=WEBP_HINT_PHOTO;
      if (LocaleCompare(value,"picture") == 0)
        configure.image_hint=WEBP_HINT_PICTURE;
    }
  value=GetImageOption(image_info,"webp:target-size");
  if (value != (char *) NULL)
    configure.target_size=StringToInteger(value);
  value=GetImageOption(image_info,"webp:target-psnr");
  if (value != (char *) NULL)
    configure.target_PSNR=(float) StringToDouble(value,(char **) NULL);
  value=GetImageOption(image_info,"webp:segments");
  if (value != (char *) NULL)
    configure.segments=StringToInteger(value);
  value=GetImageOption(image_info,"webp:sns-strength");
  if (value != (char *) NULL)
    configure.sns_strength=StringToInteger(value);
  value=GetImageOption(image_info,"webp:filter-strength");
  if (value != (char *) NULL)
    configure.filter_strength=StringToInteger(value);
  value=GetImageOption(image_info,"webp:filter-sharpness");
  if (value != (char *) NULL)
    configure.filter_sharpness=StringToInteger(value);
  value=GetImageOption(image_info,"webp:filter-type");
  if (value != (char *) NULL)
    configure.filter_type=StringToInteger(value);
  value=GetImageOption(image_info,"webp:auto-filter");
  if (value != (char *) NULL)
    configure.autofilter=ParseCommandOption(MagickBooleanOptions,MagickFalse,
      value);
  value=GetImageOption(image_info,"webp:alpha-compression");
  if (value != (char *) NULL)
    configure.alpha_compression=StringToInteger(value);
  value=GetImageOption(image_info,"webp:alpha-filtering");
  if (value != (char *) NULL)
    configure.alpha_filtering=StringToInteger(value);
  value=GetImageOption(image_info,"webp:alpha-quality");
  if (value != (char *) NULL)
    configure.alpha_quality=StringToInteger(value);
  value=GetImageOption(image_info,"webp:pass");
  if (value != (char *) NULL)
    configure.pass=StringToInteger(value);
  value=GetImageOption(image_info,"webp:show-compressed");
  if (value != (char *) NULL)
    configure.show_compressed=StringToInteger(value);
  value=GetImageOption(image_info,"webp:preprocessing");
  if (value != (char *) NULL)
    configure.preprocessing=StringToInteger(value);
  value=GetImageOption(image_info,"webp:partitions");
  if (value != (char *) NULL)
    configure.partitions=StringToInteger(value);
  value=GetImageOption(image_info,"webp:partition-limit");
  if (value != (char *) NULL)
    configure.partition_limit=StringToInteger(value);
  if (WebPValidateConfig(&configure) == 0)
    ThrowWriterException(ResourceLimitError,"UnableToEncodeImageFile");
  /*
    Allocate memory for pixels.
  */
  pixels=(uint32_t *) AcquireQuantumMemory(image->columns,image->rows*
    sizeof(*pixels));
  if (pixels == (uint32_t *) NULL)
    ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
  /*
    Convert image to WebP raster pixels.
  */
  q=pixels;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register const PixelPacket
      *restrict p;

    register ssize_t
      x;

    p=GetVirtualPixels(image,0,y,image->columns,1,&image->exception);
    if (p == (PixelPacket *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      *q++=(uint32_t) (image->matte != MagickFalse ?
        ScaleQuantumToChar(GetPixelAlpha(p)) << 24 : 0xff000000u) |
        (ScaleQuantumToChar(GetPixelRed(p)) << 16) |
        (ScaleQuantumToChar(GetPixelGreen(p)) << 8) |
        (ScaleQuantumToChar(GetPixelBlue(p)));
      p++;
    }
    status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  picture.argb=pixels;
  webp_status=WebPEncode(&configure,&picture);
  WebPPictureFree(&picture);
  pixels=(uint32_t *) RelinquishMagickMemory(pixels);
  (void) CloseBlob(image);
  return(webp_status == 0 ? MagickFalse : MagickTrue);
}
#endif
