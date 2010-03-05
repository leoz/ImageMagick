/*
  Copyright 1999-2010 ImageMagick Studio LLC, a non-profit organization
  dedicated to making software imaging solutions freely available.

  You may not use this file except in compliance with the License.
  obtain a copy of the License at

    http://www.imagemagick.org/script/license.php

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  MagickWand image Methods.
*/

#ifndef _MAGICKWAND_MAGICK_IMAGE_H
#define _MAGICKWAND_MAGICK_IMAGE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern WandExport ChannelFeatures
  *MagickGetImageChannelFeatures(MagickWand *,const unsigned long);

extern WandExport ChannelStatistics
  *MagickGetImageChannelStatistics(MagickWand *);

extern WandExport char
  *MagickGetImageFilename(MagickWand *),
  *MagickGetImageFormat(MagickWand *),
  *MagickGetImageSignature(MagickWand *),
  *MagickIdentifyImage(MagickWand *);

extern WandExport ColorspaceType
  MagickGetImageColorspace(MagickWand *);

extern WandExport CompositeOperator
  MagickGetImageCompose(MagickWand *);

extern WandExport CompressionType
  MagickGetImageCompression(MagickWand *);

extern WandExport DisposeType
  MagickGetImageDispose(MagickWand *);

extern WandExport double
  *MagickGetImageChannelDistortions(MagickWand *,const MagickWand *,
    const MetricType),
  MagickGetImageFuzz(MagickWand *),
  MagickGetImageGamma(MagickWand *),
  MagickGetImageTotalInkDensity(MagickWand *);

extern WandExport GravityType
  MagickGetImageGravity(MagickWand *);

extern WandExport Image
  *MagickDestroyImage(Image *),
  *GetImageFromMagickWand(const MagickWand *);

extern WandExport ImageType
  MagickGetImageType(MagickWand *);

extern WandExport InterlaceType
  MagickGetImageInterlaceScheme(MagickWand *);

extern WandExport InterpolatePixelMethod
  MagickGetImageInterpolateMethod(MagickWand *);

extern WandExport MagickBooleanType
  MagickAdaptiveBlurImage(MagickWand *,const double,const double),
  MagickAdaptiveBlurImageChannel(MagickWand *,const ChannelType,const double,
    const double),
  MagickAdaptiveResizeImage(MagickWand *,const unsigned long,
    const unsigned long),
  MagickAdaptiveSharpenImage(MagickWand *,const double,const double),
  MagickAdaptiveSharpenImageChannel(MagickWand *,const ChannelType,const double,
    const double),
  MagickAdaptiveThresholdImage(MagickWand *,const unsigned long,
    const unsigned long,const long),
  MagickAddImage(MagickWand *,const MagickWand *),
  MagickAddNoiseImage(MagickWand *,const NoiseType),
  MagickAddNoiseImageChannel(MagickWand *,const ChannelType,const NoiseType),
  MagickAffineTransformImage(MagickWand *,const DrawingWand *),
  MagickAnnotateImage(MagickWand *,const DrawingWand *,const double,
    const double,const double,const char *),
  MagickAnimateImages(MagickWand *,const char *),
  MagickAutoGammaImage(MagickWand *),
  MagickAutoGammaImageChannel(MagickWand *,const ChannelType),
  MagickAutoLevelImage(MagickWand *),
  MagickAutoLevelImageChannel(MagickWand *,const ChannelType),
  MagickBlackThresholdImage(MagickWand *,const PixelWand *),
  MagickBlueShiftImage(MagickWand *,const double),
  MagickBlurImage(MagickWand *,const double,const double),
  MagickBlurImageChannel(MagickWand *,const ChannelType,const double,
    const double),
  MagickBorderImage(MagickWand *,const PixelWand *,const unsigned long,
    const unsigned long),
  MagickBrightnessContrastImage(MagickWand *,const double,const double),
  MagickBrightnessContrastImageChannel(MagickWand *,const ChannelType,
    const double,const double),
  MagickCharcoalImage(MagickWand *,const double,const double),
  MagickChopImage(MagickWand *,const unsigned long,const unsigned long,
    const long,const long),
  MagickClampImage(MagickWand *),
  MagickClampImageChannel(MagickWand *,const ChannelType),
  MagickClipImage(MagickWand *),
  MagickClipImagePath(MagickWand *,const char *,const MagickBooleanType),
  MagickClutImage(MagickWand *,const MagickWand *),
  MagickClutImageChannel(MagickWand *,const ChannelType,const MagickWand *),
  MagickColorDecisionListImage(MagickWand *,const char *),
  MagickColorizeImage(MagickWand *,const PixelWand *,const PixelWand *),
  MagickCommentImage(MagickWand *,const char *),
  MagickCompositeImage(MagickWand *,const MagickWand *,const CompositeOperator,
    const long,const long),
  MagickCompositeImageChannel(MagickWand *,const ChannelType,const MagickWand *,    const CompositeOperator,const long,const long),
  MagickConstituteImage(MagickWand *,const unsigned long,const unsigned long,
    const char *,const StorageType,const void *),
  MagickContrastImage(MagickWand *,const MagickBooleanType),
  MagickContrastStretchImage(MagickWand *,const double,const double),
  MagickContrastStretchImageChannel(MagickWand *,const ChannelType,const double,
    const double),
  MagickConvolveImage(MagickWand *,const unsigned long,const double *),
  MagickConvolveImageChannel(MagickWand *,const ChannelType,const unsigned long,
    const double *),
  MagickCropImage(MagickWand *,const unsigned long,const unsigned long,
    const long,const long),
  MagickCycleColormapImage(MagickWand *,const long),
  MagickDecipherImage(MagickWand *,const char *),
  MagickDeskewImage(MagickWand *,const double),
  MagickDespeckleImage(MagickWand *),
  MagickDisplayImage(MagickWand *,const char *),
  MagickDisplayImages(MagickWand *,const char *),
  MagickDistortImage(MagickWand *,const DistortImageMethod,const unsigned long,
    const double *,const MagickBooleanType),
  MagickDrawImage(MagickWand *,const DrawingWand *),
  MagickEdgeImage(MagickWand *,const double),
  MagickEmbossImage(MagickWand *,const double,const double),
  MagickEncipherImage(MagickWand *,const char *),
  MagickEnhanceImage(MagickWand *),
  MagickEqualizeImage(MagickWand *),
  MagickEqualizeImageChannel(MagickWand *,const ChannelType),
  MagickEvaluateImage(MagickWand *,const MagickEvaluateOperator,const double),
  MagickEvaluateImageChannel(MagickWand *,const ChannelType,
    const MagickEvaluateOperator,const double),
  MagickExportImagePixels(MagickWand *,const long,const long,
    const unsigned long,const unsigned long,const char *,const StorageType,
    void *),
  MagickExtentImage(MagickWand *,const unsigned long,const unsigned long,
    const long,const long),
  MagickFilterImage(MagickWand *,const KernelInfo *),
  MagickFilterImageChannel(MagickWand *,const ChannelType,const KernelInfo *),
  MagickFlipImage(MagickWand *),
  MagickFloodfillPaintImage(MagickWand *,const ChannelType,const PixelWand *,
    const double,const PixelWand *,const long,const long,
    const MagickBooleanType),
  MagickFlopImage(MagickWand *),
  MagickForwardFourierTransformImage(MagickWand *,const MagickBooleanType),
  MagickFrameImage(MagickWand *,const PixelWand *,const unsigned long,
    const unsigned long,const long,const long),
  MagickFunctionImage(MagickWand *,const MagickFunction,const unsigned long,
    const double *),
  MagickFunctionImageChannel(MagickWand *,const ChannelType,
    const MagickFunction,const unsigned long,const double *),
  MagickGammaImage(MagickWand *,const double),
  MagickGammaImageChannel(MagickWand *,const ChannelType,const double),
  MagickGaussianBlurImage(MagickWand *,const double,const double),
  MagickGaussianBlurImageChannel(MagickWand *,const ChannelType,const double,
    const double),
  MagickGetImageAlphaChannel(MagickWand *),
  MagickGetImageBackgroundColor(MagickWand *,PixelWand *),
  MagickGetImageBluePrimary(MagickWand *,double *,double *),
  MagickGetImageBorderColor(MagickWand *,PixelWand *),
  MagickGetImageChannelDistortion(MagickWand *,const MagickWand *,
    const ChannelType,const MetricType,double *),
  MagickGetImageChannelKurtosis(MagickWand *,const ChannelType,double *,
    double *),
  MagickGetImageChannelMean(MagickWand *,const ChannelType,double *,double *),
  MagickGetImageChannelRange(MagickWand *,const ChannelType,double *,double *),
  MagickGetImageColormapColor(MagickWand *,const unsigned long,PixelWand *),
  MagickGetImageDistortion(MagickWand *,const MagickWand *,const MetricType,
    double *),
  MagickGetImageGreenPrimary(MagickWand *,double *,double *),
  MagickGetImageMatteColor(MagickWand *,PixelWand *),
  MagickGetImageLength(MagickWand *,MagickSizeType *),
  MagickGetImagePage(MagickWand *,unsigned long *,unsigned long *,long *,
    long *),
  MagickGetImagePixelColor(MagickWand *,const long,const long,PixelWand *),
  MagickGetImageRange(MagickWand *,double *,double *),
  MagickGetImageRedPrimary(MagickWand *,double *,double *),
  MagickGetImageResolution(MagickWand *,double *,double *),
  MagickGetImageWhitePoint(MagickWand *,double *,double *),
  MagickHaldClutImage(MagickWand *,const MagickWand *),
  MagickHaldClutImageChannel(MagickWand *,const ChannelType,const MagickWand *),
  MagickHasNextImage(MagickWand *),
  MagickHasPreviousImage(MagickWand *),
  MagickImplodeImage(MagickWand *,const double),
  MagickImportImagePixels(MagickWand *,const long,const long,
    const unsigned long,const unsigned long,const char *,const StorageType,
    const void *),
  MagickInverseFourierTransformImage(MagickWand *,MagickWand *,
    const MagickBooleanType),
  MagickLabelImage(MagickWand *,const char *),
  MagickLevelImage(MagickWand *,const double,const double,const double),
  MagickLevelImageChannel(MagickWand *,const ChannelType,const double,
    const double,const double),
  MagickLinearStretchImage(MagickWand *,const double,const double),
  MagickLiquidRescaleImage(MagickWand *,const unsigned long,const unsigned long,
    const double,const double),
  MagickMagnifyImage(MagickWand *),
  MagickMedianFilterImage(MagickWand *,const double),
  MagickMinifyImage(MagickWand *),
  MagickModulateImage(MagickWand *,const double,const double,const double),
  MagickMorphologyImage(MagickWand *,MorphologyMethod,const long,KernelInfo *),
  MagickMorphologyImageChannel(MagickWand *,const ChannelType,MorphologyMethod,
    const long,KernelInfo *),
  MagickMotionBlurImage(MagickWand *,const double,const double,const double),
  MagickMotionBlurImageChannel(MagickWand *,const ChannelType,const double,
    const double,const double),
  MagickNegateImage(MagickWand *,const MagickBooleanType),
  MagickNegateImageChannel(MagickWand *,const ChannelType,
    const MagickBooleanType),
  MagickNewImage(MagickWand *,const unsigned long,const unsigned long,
    const PixelWand *),
  MagickNextImage(MagickWand *),
  MagickNormalizeImage(MagickWand *),
  MagickNormalizeImageChannel(MagickWand *,const ChannelType),
  MagickOilPaintImage(MagickWand *,const double),
  MagickOpaquePaintImage(MagickWand *,const PixelWand *,const PixelWand *,
    const double,const MagickBooleanType),
  MagickOpaquePaintImageChannel(MagickWand *,const ChannelType,
    const PixelWand *,const PixelWand *,const double,const MagickBooleanType),
  MagickOrderedPosterizeImage(MagickWand *,const char *),
  MagickOrderedPosterizeImageChannel(MagickWand *,const ChannelType,
    const char *),
  MagickTransparentPaintImage(MagickWand *,const PixelWand *,
    const double,const double,const MagickBooleanType invert),
  MagickPingImage(MagickWand *,const char *),
  MagickPingImageBlob(MagickWand *,const void *,const size_t),
  MagickPingImageFile(MagickWand *,FILE *),
  MagickPolaroidImage(MagickWand *,const DrawingWand *,const double),
  MagickPosterizeImage(MagickWand *,const unsigned long,
    const MagickBooleanType),
  MagickPreviousImage(MagickWand *),
  MagickQuantizeImage(MagickWand *,const unsigned long,const ColorspaceType,
    const unsigned long,const MagickBooleanType,const MagickBooleanType),
  MagickQuantizeImages(MagickWand *,const unsigned long,const ColorspaceType,
    const unsigned long,const MagickBooleanType,const MagickBooleanType),
  MagickRadialBlurImage(MagickWand *,const double),
  MagickRadialBlurImageChannel(MagickWand *,const ChannelType,const double),
  MagickRaiseImage(MagickWand *,const unsigned long,const unsigned long,
    const long,const long,const MagickBooleanType),
  MagickRandomThresholdImage(MagickWand *,const double,const double),
  MagickRandomThresholdImageChannel(MagickWand *,const ChannelType,const double,
    const double),
  MagickReadImage(MagickWand *,const char *),
  MagickReadImageBlob(MagickWand *,const void *,const size_t),
  MagickReadImageFile(MagickWand *,FILE *),
  MagickRecolorImage(MagickWand *,const unsigned long,const double *),
  MagickReduceNoiseImage(MagickWand *,const double),
  MagickRemapImage(MagickWand *,const MagickWand *,const DitherMethod),
  MagickRemoveImage(MagickWand *),
  MagickResampleImage(MagickWand *,const double,const double,const FilterTypes,
    const double),
  MagickResetImagePage(MagickWand *,const char *),
  MagickResizeImage(MagickWand *,const unsigned long,const unsigned long,
    const FilterTypes,const double),
  MagickRollImage(MagickWand *,const long,const long),
  MagickRotateImage(MagickWand *,const PixelWand *,const double),
  MagickSampleImage(MagickWand *,const unsigned long,const unsigned long),
  MagickScaleImage(MagickWand *,const unsigned long,const unsigned long),
  MagickSegmentImage(MagickWand *,const ColorspaceType,const MagickBooleanType,
    const double,const double),
  MagickSelectiveBlurImage(MagickWand *,const double,const double,const double),
  MagickSelectiveBlurImageChannel(MagickWand *,const ChannelType,const double,
    const double,const double),
  MagickSeparateImageChannel(MagickWand *,const ChannelType),
  MagickSepiaToneImage(MagickWand *,const double),
  MagickSetImage(MagickWand *,const MagickWand *),
  MagickSetImageAlphaChannel(MagickWand *,const AlphaChannelType),
  MagickSetImageBackgroundColor(MagickWand *,const PixelWand *),
  MagickSetImageBias(MagickWand *,const double),
  MagickSetImageBluePrimary(MagickWand *,const double,const double),
  MagickSetImageBorderColor(MagickWand *,const PixelWand *),
  MagickSetImageChannelDepth(MagickWand *,const ChannelType,
    const unsigned long),
  MagickSetImageClipMask(MagickWand *,const MagickWand *),
  MagickSetImageColormapColor(MagickWand *,const unsigned long,
    const PixelWand *),
  MagickSetImageColorspace(MagickWand *,const ColorspaceType),
  MagickSetImageCompose(MagickWand *,const CompositeOperator),
  MagickSetImageCompression(MagickWand *,const CompressionType),
  MagickSetImageDelay(MagickWand *,const unsigned long),
  MagickSetImageDepth(MagickWand *,const unsigned long),
  MagickSetImageDispose(MagickWand *,const DisposeType),
  MagickSetImageCompressionQuality(MagickWand *,const unsigned long),
  MagickSetImageExtent(MagickWand *,const unsigned long,const unsigned long),
  MagickSetImageFilename(MagickWand *,const char *),
  MagickSetImageFormat(MagickWand *,const char *),
  MagickSetImageFuzz(MagickWand *,const double),
  MagickSetImageGamma(MagickWand *,const double),
  MagickSetImageGravity(MagickWand *,const GravityType),
  MagickSetImageGreenPrimary(MagickWand *,const double,const double),
  MagickSetImageInterlaceScheme(MagickWand *,const InterlaceType),
  MagickSetImageInterpolateMethod(MagickWand *,const InterpolatePixelMethod),
  MagickSetImageIterations(MagickWand *,const unsigned long),
  MagickSetImageMatte(MagickWand *,const MagickBooleanType),
  MagickSetImageMatteColor(MagickWand *,const PixelWand *),
  MagickSetImageOpacity(MagickWand *,const double),
  MagickSetImageOrientation(MagickWand *,const OrientationType),
  MagickSetImagePage(MagickWand *,const unsigned long,const unsigned long,
    const long,const long),
  MagickSetImageRedPrimary(MagickWand *,const double,const double),
  MagickSetImageRenderingIntent(MagickWand *,const RenderingIntent),
  MagickSetImageResolution(MagickWand *,const double,const double),
  MagickSetImageScene(MagickWand *,const unsigned long),
  MagickSetImageTicksPerSecond(MagickWand *,const long),
  MagickSetImageType(MagickWand *,const ImageType),
  MagickSetImageUnits(MagickWand *,const ResolutionType),
  MagickSetImageWhitePoint(MagickWand *,const double,const double),
  MagickShadeImage(MagickWand *,const MagickBooleanType,const double,
    const double),
  MagickShadowImage(MagickWand *,const double,const double,const long,
    const long),
  MagickSharpenImage(MagickWand *,const double,const double),
  MagickSharpenImageChannel(MagickWand *,const ChannelType,const double,
    const double),
  MagickShaveImage(MagickWand *,const unsigned long,const unsigned long),
  MagickShearImage(MagickWand *,const PixelWand *,const double,const double),
  MagickSigmoidalContrastImage(MagickWand *,const MagickBooleanType,
    const double,const double),
  MagickSigmoidalContrastImageChannel(MagickWand *,const ChannelType,
    const MagickBooleanType,const double,const double),
  MagickSketchImage(MagickWand *,const double,const double,const double),
  MagickSolarizeImage(MagickWand *,const double),
  MagickSparseColorImage(MagickWand *,const ChannelType,const SparseColorMethod,
    const unsigned long,const double *),
  MagickSpliceImage(MagickWand *,const unsigned long,const unsigned long,
    const long,const long),
  MagickSpreadImage(MagickWand *,const double),
  MagickStripImage(MagickWand *),
  MagickSwirlImage(MagickWand *,const double),
  MagickTintImage(MagickWand *,const PixelWand *,const PixelWand *),
  MagickTransformImageColorspace(MagickWand *,const ColorspaceType),
  MagickTransposeImage(MagickWand *),
  MagickTransverseImage(MagickWand *),
  MagickThresholdImage(MagickWand *,const double),
  MagickThresholdImageChannel(MagickWand *,const ChannelType,const double),
  MagickThumbnailImage(MagickWand *,const unsigned long,const unsigned long),
  MagickTrimImage(MagickWand *,const double),
  MagickUniqueImageColors(MagickWand *),
  MagickUnsharpMaskImage(MagickWand *,const double,const double,const double,
    const double),
  MagickUnsharpMaskImageChannel(MagickWand *,const ChannelType,const double,
    const double,const double,const double),
  MagickVignetteImage(MagickWand *,const double,const double,
    const long,const long),
  MagickWaveImage(MagickWand *,const double,const double),
  MagickWhiteThresholdImage(MagickWand *,const PixelWand *),
  MagickWriteImage(MagickWand *,const char *),
  MagickWriteImageFile(MagickWand *,FILE *),
  MagickWriteImages(MagickWand *,const char *,const MagickBooleanType),
  MagickWriteImagesFile(MagickWand *,FILE *);

WandExport MagickProgressMonitor
  MagickSetImageProgressMonitor(MagickWand *,const MagickProgressMonitor,
    void *);

extern WandExport MagickWand
  *MagickAppendImages(MagickWand *,const MagickBooleanType),
  *MagickAverageImages(MagickWand *),
  *MagickCoalesceImages(MagickWand *),
  *MagickCombineImages(MagickWand *,const ChannelType),
  *MagickCompareImageChannels(MagickWand *,const MagickWand *,const ChannelType,
    const MetricType,double *),
  *MagickCompareImages(MagickWand *,const MagickWand *,const MetricType,
    double *),
  *MagickCompareImageLayers(MagickWand *,const ImageLayerMethod),
  *MagickDeconstructImages(MagickWand *),
  *MagickFxImage(MagickWand *,const char *),
  *MagickFxImageChannel(MagickWand *,const ChannelType,const char *),
  *MagickGetImage(MagickWand *),
  *MagickGetImageClipMask(MagickWand *),
  *MagickGetImageRegion(MagickWand *,const unsigned long,const unsigned long,
    const long,const long),
  *MagickIntensityProjectionImages(MagickWand *,const MagickBooleanType),
  *MagickMergeImageLayers(MagickWand *,const ImageLayerMethod),
  *MagickMorphImages(MagickWand *,const unsigned long),
  *MagickMontageImage(MagickWand *,const DrawingWand *,const char *,
    const char *,const MontageMode,const char *),
  *MagickOptimizeImageLayers(MagickWand *),
  *MagickPreviewImages(MagickWand *wand,const PreviewType),
  *MagickSimilarityImage(MagickWand *,const MagickWand *,RectangleInfo *,
    double *),
  *MagickSteganoImage(MagickWand *,const MagickWand *,const long),
  *MagickStereoImage(MagickWand *,const MagickWand *),
  *MagickTextureImage(MagickWand *,const MagickWand *),
  *MagickTransformImage(MagickWand *,const char *,const char *);

extern WandExport OrientationType
  MagickGetImageOrientation(MagickWand *);

extern WandExport PixelWand
  **MagickGetImageHistogram(MagickWand *,unsigned long *);

extern WandExport RenderingIntent
  MagickGetImageRenderingIntent(MagickWand *);

extern WandExport ResolutionType
  MagickGetImageUnits(MagickWand *);

extern WandExport unsigned char
  *MagickGetImageBlob(MagickWand *,size_t *),
  *MagickGetImagesBlob(MagickWand *,size_t *);

extern WandExport unsigned long
  MagickGetImageColors(MagickWand *),
  MagickGetImageCompressionQuality(MagickWand *),
  MagickGetImageDelay(MagickWand *),
  MagickGetImageChannelDepth(MagickWand *,const ChannelType),
  MagickGetImageDepth(MagickWand *),
  MagickGetImageHeight(MagickWand *),
  MagickGetImageIterations(MagickWand *),
  MagickGetImageScene(MagickWand *),
  MagickGetImageTicksPerSecond(MagickWand *),
  MagickGetImageWidth(MagickWand *),
  MagickGetNumberImages(MagickWand *);

extern WandExport VirtualPixelMethod
  MagickGetImageVirtualPixelMethod(MagickWand *),
  MagickSetImageVirtualPixelMethod(MagickWand *,const VirtualPixelMethod);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
