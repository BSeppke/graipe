/************************************************************************/
/*                                                                      */
/*               Copyright 2008-2017 by Benjamin Seppke                 */
/*       Cognitive Systems Group, University of Hamburg, Germany        */
/*                                                                      */
/*    This file is part of the GrAphical Image Processing Enviroment.   */
/*    The GRAIPE Website may be found at:                               */
/*        https://github.com/bseppke/graipe                             */
/*    Please direct questions, bug reports, and contributions to        */
/*    the GitHub page and use the methods provided there.               */
/*                                                                      */
/*    Permission is hereby granted, free of charge, to any person       */
/*    obtaining a copy of this software and associated documentation    */
/*    files (the "Software"), to deal in the Software without           */
/*    restriction, including without limitation the rights to use,      */
/*    copy, modify, merge, publish, distribute, sublicense, and/or      */
/*    sell copies of the Software, and to permit persons to whom the    */
/*    Software is furnished to do so, subject to the following          */
/*    conditions:                                                       */
/*                                                                      */
/*    The above copyright notice and this permission notice shall be    */
/*    included in all copies or substantial portions of the             */
/*    Software.                                                         */
/*                                                                      */
/*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND    */
/*    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES   */
/*    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND          */
/*    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT       */
/*    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,      */
/*    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      */
/*    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR     */
/*    OTHER DEALINGS IN THE SOFTWARE.                                   */
/*                                                                      */
/************************************************************************/

#ifndef GRAIPE_OPTICALFLOW_OPTICALFLOWGRADIENTS_HXX
#define GRAIPE_OPTICALFLOW_OPTICALFLOWGRADIENTS_HXX

//debug output
#include <QtDebug>

#include <vigra/multi_math.hxx>
#include <vigra/convolution.hxx>
#include <vigra/stdconvolution.hxx>

namespace graipe {

/**
 * @addtogroup graipe_opticalflow
 * @{
 *
 * @file
 * @brief Header file for the generic image series' gradients for Optical Flow approaches.
 */
 
/**
 * The generic version of a spatio-temporal Gradient estimator based on two images.
 * To avoid noise-addictiveness, one can give a gaussian sigma value to smooth
 * both images before differencing (which is indeed a subtraction of smoothed (I2-I1).
 *
 * \param[in] src1 The first image of the series.
 * \param[in] src2 The second image of the series.
 * \param[out] gX The spatio-temporal gradient in x-direction.
 * \param[out] gY The spatio-temporal gradient in y-direction.
 * \param[out] gT The spatio-temporal gradient in temporal direction.
 * \param[in] sigma The scale of the Gaussian function used to derive the partial derivatives.
 */
template <class T1, class T2, class T3>
void spatioTemporalGradient(const vigra::MultiArrayView<2, T1> & src1,
							const vigra::MultiArrayView<2, T2> & src2,
							vigra::MultiArrayView<2, T3> gX,
							vigra::MultiArrayView<2, T3> gY,
							vigra::MultiArrayView<2, T3> gT,
							double sigma)
{
    vigra_precondition(src1.shape() == src2.shape(), "image sizes differ!");
    vigra_precondition(gX.shape() == gY.shape(), "gradient array sizes differ!");
    vigra_precondition(gY.shape() == gT.shape(), "gradient array sizes differ!");
    vigra_precondition(src1.shape() == gT.shape(), "gradient array sizes differ from image sizes!");
    
    using namespace ::vigra;
    using namespace ::vigra::multi_math;
    
	//Spatial part of the spatio-temporal gradient
	vigra::MultiArray<2, T3> temp = 0.5*(src1 + src2);
	vigra::gaussianGradient(temp, gX, gY, sigma);
	
	//Temporal part of the spatio-temporal gradient
	vigra::gaussianSmoothing(src1,   gT, sigma);
	vigra::gaussianSmoothing(src2, temp, sigma);
    gT = temp - gT;
}

/**
 * The generic version of a spatial Gaussian gradient estimator for one image and
 * only considering the area under the mask.
 *
 * \param[in] src The image.
 * \param[in] mask The mask of the image (only mask != 0) will be considered.
 * \param[out] gX The spatio-temporal gradient in x-direction.
 * \param[out] gY The spatio-temporal gradient in y-direction.
 * \param[in] sigma The scale of the Gaussian function used to derive the partial derivatives.
 * \param[in] btmode The border treatment mode used for the normalized convolution. 
 *                   Defaults to vigra::BORDER_TREATMENT_CLIP.
 */
template <class T1, class T2, class T3>
void gaussianGradientWithMask(const vigra::MultiArrayView<2, T1> & src,
							  const vigra::MultiArrayView<2, T2> & mask,
							  vigra::MultiArrayView<2, T3> gX,
							  vigra::MultiArrayView<2, T3> gY,
							  double sigma, 
                              vigra::BorderTreatmentMode btmode = vigra::BORDER_TREATMENT_CLIP)
{
	vigra_precondition(src.shape() == mask.shape(), "image and mask sizes differ!");
    vigra_precondition(gX.shape() == gY.shape(), "gradient array sizes differ!");
    vigra_precondition(src.shape() == gX.shape(), "gradient array sizes differ from image sizes!");
    
    //Build the kernels for:
	vigra::Kernel1D<float> grad_dummy;
	grad_dummy.initExplicitly(0, 0) = 1;
	
	vigra::Kernel1D<float> grad;
	grad.initGaussianDerivative(sigma,1);
	
	//g_X
	vigra::Kernel2D<float> kernel_gx;
	kernel_gx.initSeparable(grad, grad_dummy);
    kernel_gx.setBorderTreatment(btmode);
    
	
	//g_Y
	vigra::Kernel2D<float> kernel_gy;
	kernel_gy.initSeparable(grad_dummy, grad);
	kernel_gy.setBorderTreatment(btmode);
    
	//gX
	vigra::normalizedConvolveImage(src, mask, gX, kernel_gx);
	//gY
	vigra::normalizedConvolveImage(src, mask, gY, kernel_gy);
}

/**
 * The masked version of the mixed (xy) 2nd order partial derivative term of the gradient.
 * It is computed using normalized convolutions w.r.t. a given mask only.
 *
 * \param[in] src The first image of the series.
 * \param[in] mask The masking of the scene (of the series).
 * \param[out] gXY The xy-part of the 2nd derivative of the image.
 * \param[in] sigma The scale of the Gaussian functions used to derive the partial derivatives of 2nd order.
 * \param[in] btmode The border treatment mode used for the normalized convolution. 
 *                   Defaults to vigra::BORDER_TREATMENT_CLIP.
 */
template <class T1, class T2, class T3>
void gaussianGradient2ndMixedTermWithMask(const vigra::MultiArrayView<2, T1> & src,
                                          const vigra::MultiArrayView<2, T2> & mask,
                                          vigra::MultiArrayView<2, T3> gXY,
                                          double sigma,
                                          vigra::BorderTreatmentMode btmode = vigra::BORDER_TREATMENT_CLIP)
{
	vigra_precondition(src.shape() == mask.shape(), "image and mask sizes differ!");
    vigra_precondition(src.shape() == gXY.shape(), "gradient array sizes differ from image sizes!");
    
    //Build the kernels for:
	vigra::Kernel1D<float> grad_dummy;
	grad_dummy.initExplicitly(0, 0) = 1;
	
	vigra::Kernel1D<float> grad;
	grad.initGaussianDerivative(sigma,1);
	
	vigra::Kernel2D<float> kernel_gx;
	kernel_gx.initSeparable(grad, grad_dummy);
	kernel_gx.setBorderTreatment(btmode);
	
	vigra::Kernel2D<float> kernel_gy;
	kernel_gy.initSeparable(grad_dummy, grad);
	kernel_gy.setBorderTreatment(btmode);
	
	//gX
	vigra::normalizedConvolveImage(src, mask, gXY, kernel_gx);
    
	//gXY
	vigra::normalizedConvolveImage(gXY, mask, gXY, kernel_gy);
}

/**
 * The masked version of the well-known Hessian matrix (of Gaussians). Computes the HM using normalized
 * convolutions w.r.t. a given mask only.
 *
 * \param[in] src The first image of the series.
 * \param[in] mask The masking of the scene (of the series).
 * \param[out] gXX The xx-part of the Hessian matrix.
 * \param[out] gXY The xy-part of the Hessian matrix.
 * \param[out] gYY The yy-part of the Hessian matrix.
 * \param[in] sigma The scale of the Gaussian functions used to derive the partial derivatives of 2nd order.
 * \param[in] btmode The border treatment mode used for the normalized convolution. 
 *                   Defaults to vigra::BORDER_TREATMENT_CLIP.
 */
template <class T1, class T2, class T3>
void hessianMatrixOfGaussianWithMask(const vigra::MultiArrayView<2, T1> & src,
                                     const vigra::MultiArrayView<2, T2> & mask,
                                     vigra::MultiArrayView<2, T3> gXX,
                                     vigra::MultiArrayView<2, T3> gXY,
                                     vigra::MultiArrayView<2, T3> gYY,
                                     double sigma,
                                     vigra::BorderTreatmentMode btmode = vigra::BORDER_TREATMENT_CLIP)
{
	vigra_precondition(src.shape() == mask.shape(), "image and mask sizes differ!");
    vigra_precondition(gXX.shape() == gXY.shape(), "gradient array sizes differ!");
    vigra_precondition(gXY.shape() == gYY.shape(), "gradient array sizes differ!");
    vigra_precondition(src.shape() == gXX.shape(), "gradient array sizes differ from image sizes!");
    
    //gX & gY
	gaussianGradientWithMask(src, mask, gXX, gYY, sigma, btmode);
    
	//gXX & gXY
	gaussianGradientWithMask(gXX, mask, gXX, gXY, sigma, btmode);
	
	//Build the kernels for:
	vigra::Kernel1D<float> grad_dummy;
	grad_dummy.initExplicitly(0, 0) = 1;
	
	vigra::Kernel1D<float> grad;
	grad.initGaussianDerivative(sigma,1);

	vigra::Kernel2D<float> kernel_gy;
	kernel_gy.initSeparable(grad_dummy, grad);
	kernel_gy.setBorderTreatment(btmode);
	
	//gYY
	vigra::normalizedConvolveImage(gYY, mask, gYY, kernel_gy);
	
}

/**
 * The generic version of a spatial Gaussian smoothing estimator for one image and
 * only considering the area under the mask.
 *
 * \param[in] src The image.
 * \param[in] mask The mask of the image (only mask != 0) will be considered.
 * \param[out] dest The smoothed result.
 * \param[in] sigma The scale of the Gaussian function used to derive the partial derivatives.
 * \param[in] btmode The border treatment mode used for the normalized convolution. 
 *                   Defaults to vigra::BORDER_TREATMENT_CLIP.
 */
template <class T1, class T2, class T3>
void gaussianSmoothingWithMask(const vigra::MultiArrayView<2, T1> & src,
                               const vigra::MultiArrayView<2, T2> & mask,
                               vigra::MultiArrayView<2, T3> dest,
                               double sigma,
							   vigra::BorderTreatmentMode btmode = vigra::BORDER_TREATMENT_CLIP)
{
    vigra_precondition(src.shape() == mask.shape(), "image and mask sizes differ!");
    vigra_precondition(src.shape() == dest.shape(), "source and dest. image sizes differ!");
    
    //Build the kernel for:
    vigra::Kernel2D<float> kernel_smooth;
	kernel_smooth.initGaussian(sigma);
	kernel_smooth.setBorderTreatment(btmode);
	
	//smoothing
	vigra::normalizedConvolveImage(src, mask, dest, kernel_smooth);
}

/**
 * The masked version of the well-known Structure Tensor. Computes the ST using normalized
 * convolutions w.r.t. a given mask only.
 *
 * \param[in] src The first image of the series.
 * \param[in] mask The masking of the scene (of the series).
 * \param[out] gXX The xx-part of the Structure Tensor.
 * \param[out] gXY The xy-part of the Structure Tensor.
 * \param[out] gYY The yy-part of the Structure Tensor.
 * \param[in] sigma_inner The scale of the inner Gaussian function used to derive the partial derivatives.
 * \param[in] sigma_outer The scale of the outer Gaussian function used to smooth the partial derivatives.
 * \param[in] btmode The border treatment mode used for the normalized convolution. 
 *                   Defaults to vigra::BORDER_TREATMENT_CLIP.
 */
template <class T1, class T2, class T3>
void structureTensorWithMask(const vigra::MultiArrayView<2, T1> & src,
                             const vigra::MultiArrayView<2, T2> & mask,
                             vigra::MultiArrayView<2, T3> gXX,
                             vigra::MultiArrayView<2, T3> gXY,
                             vigra::MultiArrayView<2, T3> gYY,
                             double sigma_inner, double sigma_outer,
                             vigra::BorderTreatmentMode btmode = vigra::BORDER_TREATMENT_CLIP)
{
	vigra_precondition(src.shape() == mask.shape(), "image and mask sizes differ!");
    vigra_precondition(gXX.shape() == gXY.shape(), "gradient array sizes differ!");
    vigra_precondition(gXY.shape() == gYY.shape(), "gradient array sizes differ!");
    vigra_precondition(src.shape() == gXX.shape(), "gradient array sizes differ from image sizes!");
    
	gaussianGradientWithMask(src, mask, gXX, gYY, sigma_inner, btmode);
	
    using namespace vigra::multi_math;
    
    gXY = gXX * gXY;
	gXX = gXX * gXX;
	gYY = gYY * gYY;
		
	gaussianSmoothingWithMask(gXX, mask, gXX, sigma_outer);
	gaussianSmoothingWithMask(gXY, mask, gXY, sigma_outer);
	gaussianSmoothingWithMask(gYY, mask, gYY, sigma_outer);
	
}

/**
 * The generic version of a spatio-temporal Gradient estimator based on two images and a mask.
 * To avoid noise-addictiveness, one can give a gaussian sigma value to smooth
 * both images before differencing (which is indeed a subtraction I2-I1).
 *
 * Note, that only masked (mask != 0) pixels are taken into account for the computation in
 * this version of the spatial-temporal gradient.
 *
 * \param[in] src1 The first image of the series.
 * \param[in] src2 The second image of the series.
 * \param[in] mask The masking of the scene (of the series).
 * \param[out] gX The spatio-temporal gradient in x-direction.
 * \param[out] gY The spatio-temporal gradient in y-direction.
 * \param[out] gT The spatio-temporal gradient in temporal direction.
 * \param[in] sigma The scale of the Gaussian function used to derive the partial derivatives.
 * \param[in] btmode The border treatment mode used for the normalized convolution. 
 *                   Defaults to vigra::BORDER_TREATMENT_CLIP.
 */
template <class T1, class T2, class T3, class T4>
void spatioTemporalGradientWithMask(const vigra::MultiArrayView<2, T1> & src1,
                                    const vigra::MultiArrayView<2, T2> & src2,
                                    const vigra::MultiArrayView<2, T3> & mask,
                                    vigra::MultiArrayView<2, T4> gX,
                                    vigra::MultiArrayView<2, T4> gY,
                                    vigra::MultiArrayView<2, T4> gT,
									double sigma,
                                    vigra::BorderTreatmentMode btmode = vigra::BORDER_TREATMENT_CLIP)
{
   
	vigra_precondition(src1.shape() == src2.shape(), "image sizes differ!");
	vigra_precondition(src1.shape() == mask.shape(), "image and mask sizes differ!");
    vigra_precondition(gX.shape() == gY.shape(), "gradient array sizes differ!");
    vigra_precondition(gY.shape() == gT.shape(), "gradient array sizes differ!");
    vigra_precondition(src1.shape() == gX.shape(), "gradient array sizes differ from image sizes!");
        
    using namespace ::vigra;
    using namespace ::vigra::multi_math;
    
	//Spatial part of the spatio-temporal gradient
	vigra::MultiArray<2, T3> temp = 0.5*(src1 + src2);
	gaussianGradientWithMask(temp, mask, gX, gY, sigma, btmode);
	
	//Temporal part of the spatio-temporal gradient
	gaussianSmoothingWithMask(src1, mask,   gT, sigma, btmode);
	gaussianSmoothingWithMask(src2, mask, temp, sigma, btmode);
    gT = temp - gT;
}

/**
 * @}
 */

} //end of namespace graipe

#endif //GRAIPE_OPTICALFLOW_OPTICALFLOWGRADIENTS_HXX
