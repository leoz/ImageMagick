/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%             CCCC   OOO   N   N  V   V   OOO   L      V   V  EEEEE           %
%            C      O   O  NN  N  V   V  O   O  L      V   V  E               %
%            C      O   O  N N N  V   V  O   O  L      V   V  EEE             %
%            C      O   O  N  NN   V V   O   O  L       V V   E               %
%             CCCC   OOO   N   N    V     OOO   LLLLL    V    EEEEE           %
%                                                                             %
%                              Convolve An Image                              %
%                                                                             %
%                               Software Design                               %
%                                 John Cristy                                 %
%                                November 2009                                %
%                                                                             %
%                                                                             %
%  Copyright 1999-2010 ImageMagick Studio LLC, a non-profit organization      %
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
% Convolve an image by executing in concert across heterogeneous platforms
% consisting of CPUs, GPUs, and other processors (in development).
%
*/

/*
  Include declarations.
*/
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <math.h>
#include <magick/studio.h>
#include <magick/MagickCore.h>

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   c o n v o l v e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  convolveImage() convolves an image by utilizing the OpenCL framework to
%  execute the algorithm across heterogeneous platforms consisting of CPUs,
%  GPUs, and other processors.  The format of the convolveImage method is:
%
%      unsigned long convolveImage(Image *images,const int argc,
%        char **argv,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: the address of a structure of type Image.
%
%    o argc: Specifies a pointer to an integer describing the number of
%      elements in the argument vector.
%
%    o argv: Specifies a pointer to a text array containing the command line
%      arguments.
%
%    o exception: return any errors or warnings in this structure.
%
*/

#if defined(MAGICKCORE_OPENCL_SUPPORT)

#if (MAGICKCORE_QUANTUM_DEPTH == 8)
#define CLQuantumRange  "255.0"
#if !defined(MAGICKCORE_HDRI_SUPPORT)
#define CLPixelPacketString  "uchar4"
#define CLPixelPacket  cl_uchar4
#else
#define CLPixelPacketString  "float4"
#define CLPixelPacket  cl_float4
#endif
#elif (MAGICKCORE_QUANTUM_DEPTH == 16)
#define CLQuantumRange  "65535.0"
#if !defined(MAGICKCORE_HDRI_SUPPORT)
#define CLPixelPacketString  "ushort4"
#define CLPixelPacket  cl_ushort4
#else
#define CLPixelPacketString  "float4"
#define CLPixelPacket  cl_float4
#endif
#elif (MAGICKCORE_QUANTUM_DEPTH == 32)
#define CLQuantumRange  "4294967295.0"
#if !defined(MAGICKCORE_HDRI_SUPPORT)
#define CLPixelPacketString  "uint4"
#define CLPixelPacket  cl_uint4
#else
#define CLPixelPacketString  "float4"
#define CLPixelPacket  cl_float4
#endif
#else
#define CLQuantumRange  "18446744073709551615.0"
#if !defined(MAGICKCORE_HDRI_SUPPORT)
#define CLPixelPacketString  "ulong4"
#define CLPixelPacket  cl_ulong4
#else
#define CLPixelPacketString  "float4"
#define CLPixelPacket  cl_float4
#endif
#endif

typedef struct _CLInfo
{
  cl_context
    context;

  cl_device_id
    *devices;

  cl_command_queue
    command_queue;

  cl_kernel
    kernel;

  cl_program
    program;

  cl_mem
    pixels,
    convolve_pixels;

  cl_uint
    width,
    height,
    matte;

  cl_mem
    mask;
} CLInfo;

static char
  *convolve_program =
    "#define QuantumRange  " CLQuantumRange "\n"
    "#define QuantumScale  (1.0/QuantumRange)\n"
    "\n"
    "static uint AuthenticPixel(const int value,const uint range)\n"
    "{\n"
    "  if (value < 0)\n"
    "    return(0);\n"
    "  if (value >= range)\n"
    "    return(range-1);\n"
    "  return(value);\n"
    "}\n"
    "\n"
    "static ushort AuthenticQuantum(const float value)\n"
    "{\n"
    "  if (value < 0)\n"
    "    return(0);\n"
    "  if (value >= " CLQuantumRange ")\n"
    "    return(" CLQuantumRange ");\n"
    "  return(value+0.5);\n"
    "}\n"
    "\n"
    "__kernel void Convolve(const __global " CLPixelPacketString " *input,\n"
    "  __constant float *mask,const uint width,const uint height,\n"
    "  const uint matte,__global " CLPixelPacketString " *output)\n"
    "{\n"
    "  const uint columns = get_global_size(0);\n"
    "  const uint rows = get_global_size(1);\n"
    "\n"
    "  const int x = get_global_id(0);\n"
    "  const int y = get_global_id(1);\n"
    "\n"
    "  float4 sum = { 0.0, 0.0, 0.0, 0.0 };\n"
    "  float gamma = 0.0;\n"
    "  const int hstep = (width-1)/2;\n"
    "  const int vstep = (height-1)/2;\n"
    "  uint i = 0;\n"
    "\n"
    "  for (int v=(-vstep); v <= vstep; v++)\n"
    "  {\n"
    "    for (int u=(-hstep); u <= vstep; u++)\n"
    "    {\n"
    "      const uint index=AuthenticPixel(y+v,rows)*columns+\n"
    "        AuthenticPixel(x+u,columns);\n"
    "      float alpha = 1.0;\n"
    "      if (matte != 0)\n"
    "        alpha = (float) (QuantumScale*(QuantumRange-input[index].w));\n"
    "      sum.x+=alpha*mask[i]*input[index].x;\n"
    "      sum.y+=alpha*mask[i]*input[index].y;\n"
    "      sum.z+=alpha*mask[i]*input[index].z;\n"
    "      sum.w+=mask[i]*input[index].w;\n"
    "      gamma+=alpha*mask[i];\n"
    "      i++;\n"
    "    }\n"
    "  }\n"
    "  const uint index=y*columns+x;\n"
    "  gamma=1.0/((gamma <= 0.000001) && (gamma >= -0.000001) ? 1.0 : gamma);\n"
    "  output[index].x=AuthenticQuantum(gamma*sum.x);\n"
    "  output[index].y=AuthenticQuantum(gamma*sum.y);\n"
    "  output[index].z=AuthenticQuantum(gamma*sum.z);\n"
    "  output[index].w=AuthenticQuantum(sum.w);\n"
    "}\n";

static void OpenCLNotify(const char *message,const void *data,size_t length,
  void *user_context)
{
  ExceptionInfo
    *exception;

  (void) data;
  (void) length;
  exception=(ExceptionInfo *) user_context;
  (void) ThrowMagickException(exception,GetMagickModule(),FilterError,
    "","`%s'",message);
}

static MagickBooleanType BindCLParameters(CLInfo *cl_info,Image *image,
  void *pixels,float *mask,const unsigned long width,const unsigned long height,
  void *convolve_pixels)
{
  cl_int
    status;

  /*
    Bind OpenCL buffers.
  */
  cl_info->pixels=clCreateBuffer(cl_info->context,CL_MEM_READ_ONLY |
    CL_MEM_USE_HOST_PTR,image->columns*image->rows*sizeof(CLPixelPacket),
    pixels,&status);
  if ((cl_info->pixels == (cl_mem) NULL) || (status != CL_SUCCESS))
    return(MagickFalse);
  status=clSetKernelArg(cl_info->kernel,0,sizeof(cl_mem),(void *)
    &cl_info->pixels);
  if (status != CL_SUCCESS)
    return(MagickFalse);
  cl_info->mask=clCreateBuffer(cl_info->context,CL_MEM_READ_ONLY |
    CL_MEM_USE_HOST_PTR,width*height*sizeof(cl_float),mask,&status);
  if ((cl_info->mask == (cl_mem) NULL) || (status != CL_SUCCESS))
    return(MagickFalse);
  status=clSetKernelArg(cl_info->kernel,1,sizeof(cl_mem),(void *)
    &cl_info->mask);
  if (status != CL_SUCCESS)
    return(MagickFalse);
  cl_info->width=(cl_uint) width;
  status=clSetKernelArg(cl_info->kernel,2,sizeof(cl_uint),(void *)
    &cl_info->width);
  if (status != CL_SUCCESS)
    return(MagickFalse);
  cl_info->height=(cl_uint) height;
  status=clSetKernelArg(cl_info->kernel,3,sizeof(cl_uint),(void *)
    &cl_info->height);
  if (status != CL_SUCCESS)
    return(MagickFalse);
  cl_info->matte=(cl_uint) image->matte;
  status=clSetKernelArg(cl_info->kernel,4,sizeof(cl_uint),(void *)
    &cl_info->matte);
  if (status != CL_SUCCESS)
    return(MagickFalse);
  cl_info->convolve_pixels=clCreateBuffer(cl_info->context,CL_MEM_WRITE_ONLY |
    CL_MEM_USE_HOST_PTR,image->columns*image->rows*sizeof(CLPixelPacket),
    convolve_pixels,&status);
  if ((cl_info->convolve_pixels == (cl_mem) NULL) || (status != CL_SUCCESS))
    return(MagickFalse);
  status=clSetKernelArg(cl_info->kernel,5,sizeof(cl_mem),(void *)
    &cl_info->convolve_pixels);
  if (status != CL_SUCCESS)
    return(MagickFalse);
  clFinish(cl_info->command_queue);
  return(MagickTrue);
}

static void DestroyCLBuffers(CLInfo *cl_info)
{
  cl_int
    status;

  if (cl_info->convolve_pixels != (cl_mem) NULL)
    status=clReleaseMemObject(cl_info->convolve_pixels);
  if (cl_info->pixels != (cl_mem) NULL)
    status=clReleaseMemObject(cl_info->pixels);
  if (cl_info->mask != (cl_mem) NULL)
    status=clReleaseMemObject(cl_info->mask);
}

static CLInfo *DestroyCLInfo(CLInfo *cl_info)
{
  cl_int
    status;

  if (cl_info->kernel != (cl_kernel) NULL)
    status=clReleaseKernel(cl_info->kernel);
  if (cl_info->program != (cl_program) NULL)
    status=clReleaseProgram(cl_info->program);
  if (cl_info->command_queue != (cl_command_queue) NULL)
    status=clReleaseCommandQueue(cl_info->command_queue);
  if (cl_info->context != (cl_context) NULL)
    status=clReleaseContext(cl_info->context);
  cl_info=(CLInfo *) RelinquishMagickMemory(cl_info);
  return(cl_info);
}

static MagickBooleanType EnqueueKernel(CLInfo *cl_info,Image *image)
{
  cl_event
    events[1];

  cl_int
    status;

  size_t
    global_work_size[2];

  global_work_size[0]=image->columns;
  global_work_size[1]=image->rows;
  status=clEnqueueNDRangeKernel(cl_info->command_queue,cl_info->kernel,2,NULL,
    global_work_size,NULL,0,NULL,&events[0]);
  if (status != CL_SUCCESS)
    return(MagickFalse);
  status=clWaitForEvents(1,&events[0]);
  if (status != CL_SUCCESS)
    return(MagickFalse);
  clFinish(cl_info->command_queue);
  return(MagickTrue);
}

static CLInfo *GetCLInfo(Image *image,const char *name,const char *source,
  ExceptionInfo *exception)
{
  cl_int
    status;

  CLInfo
    *cl_info;

  size_t
    length,
    lengths[] = { strlen(source) };

  /*
    Create OpenCL info.
  */
  cl_info=(CLInfo *) AcquireAlignedMemory(1,sizeof(*cl_info));
  if (cl_info == (CLInfo *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",image->filename);
      return((CLInfo *) NULL);
    }
  (void) ResetMagickMemory(cl_info,0,sizeof(*cl_info));
  /*
    Create OpenCL context.
  */
  cl_info->context=clCreateContextFromType((cl_context_properties *) NULL,
    CL_DEVICE_TYPE_DEFAULT,OpenCLNotify,exception,&status);
  if ((cl_info->context == (cl_context) NULL) || (status != CL_SUCCESS))
    {
      DestroyCLInfo(cl_info);
      return((CLInfo *) NULL);
    }
  /*
    Detect OpenCL devices.
  */
  status=clGetContextInfo(cl_info->context,CL_CONTEXT_DEVICES,0,NULL,&length);
  if ((status != CL_SUCCESS) || (length == 0))
    {
      DestroyCLInfo(cl_info);
      return((CLInfo *) NULL);
    }
  cl_info->devices=(cl_device_id *) AcquireMagickMemory(length);
  if (cl_info->devices == (cl_device_id *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",image->filename);
      DestroyCLInfo(cl_info);
      return((CLInfo *) NULL);
    }
  status=clGetContextInfo(cl_info->context,CL_CONTEXT_DEVICES,length,
    cl_info->devices,NULL);
  if (status != CL_SUCCESS)
    {
      DestroyCLInfo(cl_info);
      return((CLInfo *) NULL);
    }
  /*
    Create OpenCL queue.
  */
  cl_info->command_queue=clCreateCommandQueue(cl_info->context,
    cl_info->devices[0],0,&status);
  if ((cl_info->command_queue == (cl_command_queue) NULL) ||
      (status != CL_SUCCESS))
    {
      DestroyCLInfo(cl_info);
      return((CLInfo *) NULL);
    }
  /*
    Build OpenCL program.
  */
  cl_info->program=clCreateProgramWithSource(cl_info->context,1,&source,
    lengths,&status);
  if ((cl_info->program == (cl_program) NULL) || (status != CL_SUCCESS))
    {
      DestroyCLInfo(cl_info);
      return((CLInfo *) NULL);
    }
  status=clBuildProgram(cl_info->program,1,cl_info->devices,NULL,NULL,NULL);
  if ((cl_info->program == (cl_program) NULL) || (status != CL_SUCCESS))
    {
      char
        *log;

      status=clGetProgramBuildInfo(cl_info->program,cl_info->devices[0],
        CL_PROGRAM_BUILD_LOG,0,NULL,&length);
      log=(char *) AcquireMagickMemory(length);
      if (log == (char *) NULL)
        {
          DestroyCLInfo(cl_info);
          return((CLInfo *) NULL);
        }
      status=clGetProgramBuildInfo(cl_info->program,cl_info->devices[0],
        CL_PROGRAM_BUILD_LOG,length,log,&length);
      (void) ThrowMagickException(exception,GetMagickModule(),FilterError,
        "failed to build OpenCL program","`%s' (%s)",image->filename,log);
      log=DestroyString(log);
      DestroyCLInfo(cl_info);
      return((CLInfo *) NULL);
    }
  /*
    Get a kernel object.
  */
  cl_info->kernel=clCreateKernel(cl_info->program,name,&status);
  if ((cl_info->kernel == (cl_kernel) NULL) || (status != CL_SUCCESS))
    {
      DestroyCLInfo(cl_info);
      return((CLInfo *) NULL);
    }
  return(cl_info);
}

static float *ParseMask(const char *value,unsigned long *order)
{
  char
    token[MaxTextExtent];

  const char
    *p;

  float
    *mask,
    normalize;

  register long
    i;

  /*
    Parse convolution mask.
  */
  p=(const char *) value;
  if (*p == '\'')
    p++;
  for (i=0; *p != '\0'; i++)
  {
    GetMagickToken(p,&p,token);
    if (*token == ',')
      GetMagickToken(p,&p,token);
  }
  *order=(unsigned long) sqrt((double) i+1.0);
  mask=(float *) AcquireQuantumMemory(*order,*order*sizeof(*mask));
  if (mask == (float *) NULL)
    return(mask);
  p=(const char *) value;
  if (*p == '\'')
    p++;
  for (i=0; (i < (long) (*order**order)) && (*p != '\0'); i++)
  {
    GetMagickToken(p,&p,token);
    if (*token == ',')
      GetMagickToken(p,&p,token);
    mask[i]=strtod(token,(char **) NULL);
  }
  for ( ; i < (long) (*order**order); i++)
    mask[i]=0.0;
  normalize=0.0;
  for (i=0; i < (long) (*order**order); i++)
    normalize+=mask[i];
  if (normalize != 0.0)
    for (i=0; i < (long) (*order**order); i++)
      mask[i]/=normalize;
  return(mask);
}

#endif

ModuleExport unsigned long convolveImage(Image **images,const int argc,
  const char **argv,ExceptionInfo *exception)
{
  assert(images != (Image **) NULL);
  assert(*images != (Image *) NULL);
  assert((*images)->signature == MagickSignature);
#if !defined(MAGICKCORE_OPENCL_SUPPORT)
  (void) argc;
  (void) argv;
  (void) ThrowMagickException(exception,GetMagickModule(),MissingDelegateError,
    "DelegateLibrarySupportNotBuiltIn","`%s' (OpenCL)",(*images)->filename);
#else
  {
    float
      *mask;

    Image
      *image;

    MagickBooleanType
      status;

    CLInfo
      *cl_info;

    unsigned long
      order;

    if (argc < 1)
      return(MagickImageFilterSignature);
    /*
      Convolve image.
    */
    mask=ParseMask(argv[0],&order);
    if (mask == (float *) NULL)
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",(*images)->filename);
    cl_info=GetCLInfo(*images,"Convolve",convolve_program,exception);
    if (cl_info == (CLInfo *) NULL)
      {
        mask=(float *) RelinquishMagickMemory(mask);
        return(MagickImageFilterSignature);
      }
    image=(*images);
    for ( ; image != (Image *) NULL; image=GetNextImageInList(image))
    {
      Image
        *convolve_image;

      MagickSizeType
        length;

      void
        *convolve_pixels,
        *pixels;

      if (SetImageStorageClass(image,DirectClass) == MagickFalse)
        continue;
      pixels=GetPixelCachePixels(image,&length,exception);
      if (pixels == (void *) NULL)
        {
          (void) ThrowMagickException(exception,GetMagickModule(),CacheError,
            "UnableToReadPixelCache","`%s'",image->filename);
          continue;
        }
      convolve_image=CloneImage(image,image->columns,image->rows,MagickTrue,
        exception);
      convolve_pixels=GetPixelCachePixels(convolve_image,&length,exception);
      if (convolve_pixels == (void *) NULL)
        {
          (void) ThrowMagickException(exception,GetMagickModule(),CacheError,
            "UnableToReadPixelCache","`%s'",image->filename);
          convolve_image=DestroyImage(convolve_image);
          continue;
        }
      status=BindCLParameters(cl_info,image,pixels,mask,order,order,
        convolve_pixels);
      if (status == MagickFalse)
        continue;
      status=EnqueueKernel(cl_info,image);
      if (status == MagickFalse)
        continue;
      (void) CopyMagickMemory(pixels,convolve_pixels,length);
      DestroyCLBuffers(cl_info);
      convolve_image=DestroyImage(convolve_image);
    }
    mask=(float *) RelinquishMagickMemory(mask);
    cl_info=DestroyCLInfo(cl_info);
  }
#endif
  return(MagickImageFilterSignature);
}
