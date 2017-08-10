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

#ifndef GRAIPE_OPTICALFLOW_OPTICALFLOWFRAMEWORK_HXX
#define GRAIPE_OPTICALFLOW_OPTICALFLOWFRAMEWORK_HXX

//debug output
#include <QtDebug>

//for hierarchical processing and global motion estimation
#include "registration/registration.h"

//image representation
#include "vigra/stdimage.hxx"

//separable filters
#include "vigra/convolution.hxx"
#include "vigra/stdconvolution.hxx"
#include <vigra/affine_registration_fft.hxx>

namespace graipe {

/**
 * Calculation of a single OFCE Algorithm run. Maybe with GME and correction.
 *
 * \param[in] flow The flow field.
 * \param[out] mat The global mothion estimation for the given flow field.
 */
template <	class T, int N>
void correctOFCEWithGlobalDisplacement(vigra::MultiArrayView<2,vigra::TinyVector<T,N> > flow,
                                       const vigra::Matrix<double>& mat)
{
    vigra_assert( N >= 2, "flow field must contain (u,v)-components at least" );
    
    for(unsigned int y=0; y != flow.height(); ++y)
    {
        for(unsigned int x=0; x != flow.width(); ++x)
        {
            flow(x,y)[0] += x*mat(0,0) + y*mat(0,1) + mat(0,2) - x;
            flow(x,y)[1] += x*mat(1,0) + y*mat(1,1) + mat(1,2) - y;
        }
    }
}




/**
 * The most basic case of an Optical Flow computation:
 * Just call the functor without any masks or hierarchical processing scheme, but
 * if selected with global motion estimation.
 *
 * \param[in] src1 First image of the series.
 * \param[in] src2 Second image of the series.
 * \param[out] flow The resulting Optical Flow field.
 * \param[in] flowFunc The used functor to compute the Optical Flow.
 * \param[in] use_global If true, the global motion estimation be used prior.
 * \param[out] mat If use_global is true, this contains the global motion estimation matrix (rot+trans).
 * \param[out] rotation_correlation If use_global is true, this contains the rotation correlation.
 * \param[out] translation_correlation If use_global is true, this contains the transflation correlation.
 */
template <class T1, class T2, class OpticalFlowFunctor>
void calculateOFCE(const vigra::MultiArrayView<2,T1> & src1,
                   const vigra::MultiArrayView<2,T2> & src2,
				   vigra::MultiArrayView<2,typename OpticalFlowFunctor::FlowValueType> flow,
				   OpticalFlowFunctor flowFunc,
                   bool use_global,
                   vigra::Matrix<double>& mat,
                   double & rotation_correlation, double & translation_correlation)
{
    vigra_precondition(src1.shape() == src2.shape() ,"image sizes differ!");
    vigra_precondition(src1.shape() == flow.shape() ,"image and flow array sizes differ!");
    
	//1. step: estimate global displacement of image
	vigra::MultiArray<2,T1> src1_t(src1.shape());
    
    mat = vigra::identityMatrix<double>(3);
    
    if(use_global)
    {
        estimateGlobalRotationTranslation(src1, src2, mat, rotation_correlation, translation_correlation);
        
        //mat is an affine transfrom from I2->I1, thus affineWarping is possible without inversion
        affineWarpImage(vigra::SplineImageView<3, T1>(src1), src1_t, mat);
    }
	else
    {
		//copy image 1 to displacd image
		src1_t = src1;
	}
	
	//2. step: Perform the OFCE computiation
	flowFunc(src1_t, src2, flow);
	
	//3. step: "add" the global displacement back to the result
	if(use_global)
	{
        //Mat is from I2->I1 invert for I1->I2
        vigra::Matrix<double> imat = vigra::identityMatrix<double>(3);
        imat(0,0) = mat(0,0); imat(0,1) = mat(1,0);
        imat(1,0) = mat(0,1); imat(1,1) = mat(1,1);
        imat(0,2) = - (imat(0,0)*mat(0,2) + imat(1,0)*mat(1,2));
        imat(1,2) = - (imat(0,1)*mat(0,2) + imat(1,1)*mat(1,2));
        
        mat = imat;
        correctOFCEWithGlobalDisplacement(flow, mat);
	}
}




/**
 * The second most basic case of an Optical Flow computation:
 * Just call the functor with a masks, no hierarchical processing scheme, but
 * if selected with global motion estimation.
 *
 * \param[in] src1 First image of the series.
 * \param[in] src2 Second image of the series.
 * \param[in] mask THe mask, where pixel values are assumed to be valid.
 * \param[out] flow The resulting Optical Flow field.
 * \param[in] flowFunc The used functor to compute the Optical Flow.
 * \param[in] use_global If true, the global motion estimation be used prior.
 * \param[out] mat If use_global is true, this contains the global motion estimation matrix (rot+trans).
 * \param[out] rotation_correlation If use_global is true, this contains the rotation correlation.
 * \param[out] translation_correlation If use_global is true, this contains the transflation correlation.
 */
template <class T1, class T2, class T3, class OpticalFlowFunctor>
void calculateOFCE(const vigra::MultiArrayView<2,T1> & src1,
                  const vigra::MultiArrayView<2,T2> & src2,
                  const vigra::MultiArrayView<2,T3> & mask,
				  vigra::MultiArrayView<2,typename OpticalFlowFunctor::FlowValueType> flow,
                  OpticalFlowFunctor flowFunc,
                  bool use_global,
                  vigra::Matrix<double>& mat,
                  double & rotation_correlation, double & translation_correlation)
{
    vigra_precondition(src1.shape() == src2.shape() ,"image sizes differ!");
    vigra_precondition(src1.shape() == mask.shape() ,"image and mask sizes differ!");
    vigra_precondition(src1.shape() == flow.shape() ,"image and flow array sizes differ!");
    
	//1. step: estimate global displacement of image
	vigra::MultiArray<2,T1> src1_t(src1.shape());
    
    mat = vigra::identityMatrix<double>(3);
    
    if(use_global)
    {
        estimateGlobalRotationTranslation(src1, src2, mat, rotation_correlation, translation_correlation);
        
        //mat is an affine transfrom from I2->I1, thus affineWarping is possible without inversion
        affineWarpImage(vigra::SplineImageView<3, T1>(src1), src1_t, mat);
    }
	else
    {
		//copy image 1 to displacd image
		src1_t = src1;
	}
	
	//2. step: Perform the OFCE computiation
	flowFunc(src1_t, src2, mask, flow);
	
	//3. step: "add" the global displacement back to the result
	if(use_global)
	{
        //Mat is from I2->I1 invert for I1->I2
        vigra::Matrix<double> imat = vigra::identityMatrix<double>(3);
        imat(0,0) = mat(0,0); imat(0,1) = mat(1,0);
        imat(1,0) = mat(0,1); imat(1,1) = mat(1,1);
        imat(0,2) = - (imat(0,0)*mat(0,2) + imat(1,0)*mat(1,2));
        imat(1,2) = - (imat(0,1)*mat(0,2) + imat(1,1)*mat(1,2));
        
        mat = imat;
        correctOFCEWithGlobalDisplacement(flow, mat);
	}
}

/**
 *  Hierarchical Framework for optical flow calculation to work on an gaussian scale-space image pyramid.
 */ 

/**
 * Helper-function to get (via Gaussian reduction) from one image to next pyramid level's image
 * Next means smaller image.
 * \param[in] in The input image
 * \param[out] out The reduced image.
 */
template <class T>
void reduceToNextLevel(const vigra::MultiArrayView<2,T> & in, vigra::MultiArray<2,T> & out)
{    
    // image size at current level
    unsigned int width  = (unsigned int)in.width(),
                 height = (unsigned int)in.height();
    
    // image size at next smaller level
    unsigned int newwidth  = (width + 1) / 2,
                 newheight = (height + 1) / 2;
    
    // resize result image to appropriate size
    out.reshape(vigra::Shape2(newwidth, newheight));
    
    // define a Gaussian kernel (size 5x1)
    vigra::Kernel1D<double> filter;
    filter.initExplicitly(-2, 2) = 0.05, 0.25, 0.4, 0.25, 0.05;
    
    vigra::MultiArray<2,T> temp(in.shape()), temp2(in.shape());
    
    // smooth (band limit) input image
    vigra::separableConvolveX(in,   temp, filter);
    vigra::separableConvolveY(temp, temp2, filter);
                       
    // downsample smoothed image
    vigra::resizeImageNoInterpolation(temp2, out);
}

/**
 * Helper function to build the step list for different scale space traversal stratigies.
 *
 * \param steps Step count.
 * \param break_level On wich level shall we finish/break the traversal.
 * \param hmode The hierarchical traversal mode: (0: V, 1: Single W, 2: Full W)
 * \return the list containing the layers in the traversal order.
 */
std::list<unsigned int> buildStepList(unsigned int steps, unsigned int break_level, unsigned int hmode)
{
	std::list<unsigned int> step_list;

	//qDebug() << "Building step list for hierarchical processing";
	unsigned int remaining_steps = (steps-break_level)*2;
	switch(hmode)
	{
		case 2: // 'Full W':
			
			while( remaining_steps/2>break_level)
			{
				for (int i=int(remaining_steps/2); i>=int(break_level); --i)
				{
					step_list.push_back(i);
					//qDebug() << "Adding level " << i;
				}
				remaining_steps= (remaining_steps-break_level)/2;
			}
			break;
			
		case 1: // 'Single W':			
			for (int i=int(steps); i>=int(break_level); --i)
			{
				step_list.push_back(i);
				//qDebug() << "Adding level " << i;
			}
			for (int i=int(steps-break_level)/2; i>=int(break_level); --i)
			{
				step_list.push_back(i);
				//qDebug() << "Adding level " << i;
			}
			break;
			
		case  0:  // 'V':
		default:
			for (int i=int(steps); i>=int(break_level); --i)
			{
				step_list.push_back(i);
				//qDebug() << "Adding level " << i;
			}
			break;
	}

	return step_list;
}


/**
 * The first hierarchical Optical Flow estimation approach:
 * a) Detect flow at level n
 * b) use that (probably scaled) flow as initial flow for the iterations
 *     at level (n+1)
 * For each (a) the functor is called without a mask, but if selected with global motion estimation.
 *
 * \param[in] src1 First image of the series.
 * \param[in] src2 Second image of the series.
 * \param[in] mask THe mask, where pixel values are assumed to be valid.
 * \param[out] flow_list The resulting Optical Flow fields during the steps.
 * \param[in] flowFunc The used functor to compute the Optical Flow.
 * \param[in] use_global If true, the global motion estimation be used prior to each computation.
 * \param[out] mat_list If use_global is true, this contains the global motion estimation matrices (rot+trans).
 * \param[out] rotation_correlation_list If use_global is true, this contains the rotation correlations.
 * \param[out] translation_correlation_list If use_global is true, this contains the transflation correlations.
 * \param[in] steps Step count.
 * \param[in] break_level On wich level shall we finish/break the traversal.
 * \param[in] hmode The hierarchical traversal mode: (0: V, 1: Single W, 2: Full W)
 */
template <	class T1, class T2, class MatrixType, class OpticalFlowFunctor>
void calculateOFCEHierarchicallyInitialiser(const vigra::MultiArrayView<2,T1> & src1, 
                                            const vigra::MultiArrayView<2,T2> & src2, 
                                            std::vector<vigra::MultiArray<2,typename OpticalFlowFunctor::FlowValueType> > & flow_list,
                                            OpticalFlowFunctor flow_func,
                                            bool use_gme,
                                            std::vector<MatrixType>& mat_list,
                                            std::vector<double>& rotation_correlation_list,
                                            std::vector<double>& translation_correlation_list,
                                            unsigned int steps,  
                                            unsigned int break_level, 
                                            unsigned int hmode)
{
    vigra_precondition(src1.shape() == src2.shape() ,"image sizes differ!");
    
	steps = std::min((double)steps, log((double)std::min(src1.width(), src1.height()))/log(2.0)-3);
	std::list<unsigned int> step_list = buildStepList(steps, break_level, hmode);
	
    //create gaussian pyramid hierarchy bottom->up
	std::vector<vigra::MultiArray<2,T1> >	img_list(steps+1);
    std::vector<vigra::MultiArray<2,T2> >	img2_list(steps+1);
	
	qDebug() << "Building gaussian pyramid for both images with " << steps << " levels";
	
	img_list[0]  = src1;
	img2_list[0] = src2;
		
	for (unsigned int level=1; level<=steps; ++level)
	{
		reduceToNextLevel(img_list[level-1], img_list[level]);
		reduceToNextLevel(img2_list[level-1], img2_list[level]);
		
		flow_list.push_back(vigra::MultiArray<2,typename OpticalFlowFunctor::FlowValueType>(img_list[level].shape()));
		mat_list.push_back(MatrixType(3,3));
		
	}
	
	//work on that hierarchy	
	for (std::list<unsigned int>::iterator iter = step_list.begin(); iter != step_list.end(); ++iter)
	{
		std::list<unsigned int>::iterator next_iter = iter; next_iter++;
		unsigned int s = *iter;
		unsigned int next_s = *next_iter;
		
		qDebug() << "Running OFCE on level " << s;
		
		flow_func.setLevel(s);
		
		calculateOFCE(img_list[s], img2_list[s],
                      flow_list[s],
					  flow_func, 
					  use_gme,
                      mat_list[s],
                      rotation_correlation_list[s],
                      translation_correlation_list[s]);
		
		if(next_iter != step_list.end() )
		{
			//progate u's and v's for the next lower step
			vigra::resizeImageLinearInterpolation(flow_list[s], flow_list[next_s]);
			
            //rescale vector length only!
            double rescale_factor = pow(2.0, double(s-next_s));
            flow_list[next_s].bindElementChannel(0) *= rescale_factor;
            flow_list[next_s].bindElementChannel(1) *= rescale_factor;
			
		}
	}	
	if(break_level!=0)
	{
		vigra::resizeImageLinearInterpolation(flow_list[break_level], flow_list[0]);
		
		//rescale vector length only!
		double rescale_factor = pow(2.0, double(break_level));
        flow_list[0].bindElementChannel(0) *= rescale_factor;
        flow_list[0].bindElementChannel(1) *= rescale_factor;
	}
}


/**
 * The second hierarchical Optical Flow estimation approach:
 * a) Detect flow at level n
 * b) use that (probably scaled) flow as initial flow for the iterations
 *    at level (n+1)
 * For each (a) the functor is called with a mask and if selected with global motion estimation.
 *
 * \param[in] src1 First image of the series.
 * \param[in] src2 Second image of the series.
 * \param[in] mask THe mask, where pixel values are assumed to be valid.
 * \param[out] flow_list The resulting Optical Flow fields during the steps.
 * \param[in] flowFunc The used functor to compute the Optical Flow.
 * \param[in] use_global If true, the global motion estimation be used prior to each computation.
 * \param[out] mat_list If use_global is true, this contains the global motion estimation matrices (rot+trans).
 * \param[out] rotation_correlation_list If use_global is true, this contains the rotation correlations.
 * \param[out] translation_correlation_list If use_global is true, this contains the transflation correlations.
 * \param steps Step count.
 * \param[in] break_level On wich level shall we finish/break the traversal.
 * \param[in] hmode The hierarchical traversal mode: (0: V, 1: Single W, 2: Full W)
 */
template <	class T1, class T2, class T3, class MatrixType, class OpticalFlowFunctor>
void calculateOFCEHierarchicallyInitialiser(const vigra::MultiArrayView<2,T1> & src1,
											const vigra::MultiArrayView<2,T2> & src2,
											const vigra::MultiArrayView<2,T3> & mask,
                                            std::vector<vigra::MultiArray<2,typename OpticalFlowFunctor::FlowValueType> > & flow_list,
											OpticalFlowFunctor flow_func,
											bool use_gme,
                                            std::vector<MatrixType>& mat_list,
                                            std::vector<double>& rotation_correlation_list,
                                            std::vector<double>& translation_correlation_list,
											unsigned int steps,
											unsigned int break_level,
											unsigned int hmode)
{
    vigra_precondition(src1.shape() == src2.shape() ,"image sizes differ!");
    vigra_precondition(src1.shape() == mask.shape() ,"image and mask sizes differ!");
    
	steps = std::min((double)steps, log((double)std::min(src1.width(),src1.height()))/log(2.0)-3);
	std::list<unsigned int> step_list = buildStepList(steps, break_level, hmode);
    
	//create gaussian pyramid hierarchy bottom->up
	std::vector<vigra::MultiArray<2,T1> >	img_list(steps+1);
    std::vector<vigra::MultiArray<2,T2> >	img2_list(steps+1);
	std::vector<vigra::MultiArray<2,T3> >	mask_list(steps+1);
	
	qDebug() << "Building gaussian pyramid for both images with " << steps << " levels";
	
	img_list[0]  = src1;;
	img2_list[0] = src2;
	mask_list[0] = mask;
	
	for (unsigned int level=1; level<=steps; ++level)
	{
		reduceToNextLevel(img_list[level-1], img_list[level]);
		reduceToNextLevel(img2_list[level-1], img2_list[level]);
		reduceToNextLevel(mask_list[level-1], mask_list[level]);
		
		flow_list.push_back(vigra::MultiArray<2,typename OpticalFlowFunctor::FlowValueType>(img_list[level].shape()));
		mat_list.push_back(MatrixType(3,3));
		
	}
	
	//work on that hierarchy	
	for (std::list<unsigned int>::iterator iter = step_list.begin(); iter != step_list.end(); ++iter)
	{
		std::list<unsigned int>::iterator next_iter = iter; next_iter++;
		unsigned int s = *iter;
		unsigned int next_s = *next_iter;
		
		qDebug() << "Running OFCE on level " << s;
		
		flow_func.setLevel(s);
		
		calculateOFCE(img_list[s],
					  img2_list[s],
					  mask_list[s],
					  flow_list[s],
					  flow_func,
					  use_gme,
                      mat_list[s],
                      rotation_correlation_list[s],
                      translation_correlation_list[s]);
		
		if(next_iter != step_list.end() )
		{
			//progate u's and v's for the next lower step
            vigra::resizeImageLinearInterpolation(flow_list[s], flow_list[next_s]);
			
            //rescale vector length only!
            double rescale_factor = pow(2.0, double(s-next_s));
            flow_list[next_s].bindElementChannel(0) *= rescale_factor;
            flow_list[next_s].bindElementChannel(1) *= rescale_factor;
			
		}
	}	
	if(break_level!=0)
	{
		vigra::resizeImageLinearInterpolation(flow_list[break_level], flow_list[0]);
		
		//rescale vector length only!
		double rescale_factor = pow(2.0, double(break_level));
        flow_list[0].bindElementChannel(0) *= rescale_factor;
        flow_list[0].bindElementChannel(1) *= rescale_factor;
	}
}




/**
 * The third hierarchical Optical Flow estimation approach:
 *  
 *  a) Detect flow at level n
 *  b) use that (probably scaled) flow to warp the img_list image of next level.
 *  c) save the flow at this level
 *  d) proceed with zero assumption flow at next level (n+1)
 *  e) ...
 *  f) at the end: Add all motion increments to obtain complete flow
 *
 * For each (a) the functor is called without a mask, but if selected with global motion estimation.
 *
 * \param[in] src1 First image of the series.
 * \param[in] src2 Second image of the series.
 * \param[in] mask THe mask, where pixel values are assumed to be valid.
 * \param[out] flow_list The resulting Optical Flow fields during the steps.
 * \param[in] flowFunc The used functor to compute the Optical Flow.
 * \param[in] use_global If true, the global motion estimation be used prior to each computation.
 * \param[out] mat_list If use_global is true, this contains the global motion estimation matrices (rot+trans).
 * \param[out] rotation_correlation_list If use_global is true, this contains the rotation correlations.
 * \param[out] translation_correlation_list If use_global is true, this contains the transflation correlations.
 * \param[in] steps Step count.
 * \param[in] break_level On wich level shall we finish/break the traversal.
 * \param[in] hmode The hierarchical traversal mode: (0: V, 1: Single W, 2: Full W)
 * \param[in] warp The functor, which is used for warping
 * \param[in] warp_subsampling The subsampling, wich is used for warping
 * \param[in] warp_sigma The sigma, which is used for smoothing the result before subsampling.
 */
template <class T1, class T2, class MatrixType, class OpticalFlowFunctor, class WarpingFunctor>
void calculateOFCEHierarchicallyWarping(const vigra::MultiArrayView<2,T1> & src1, 
										const vigra::MultiArrayView<2,T2> & src2, 
										std::vector<vigra::MultiArray<2, T1> >& img_list,
                                        std::vector<vigra::MultiArray<2,typename OpticalFlowFunctor::FlowValueType> >& flow_list,
										OpticalFlowFunctor flow_func,
										bool use_gme, 
                                        std::vector<MatrixType>& mat_list,
                                        std::vector<double>& rotation_correlation_list,
                                        std::vector<double>& translation_correlation_list,
										unsigned int steps,  
										unsigned int break_level, 
										unsigned int hmode,
										WarpingFunctor warp,
										unsigned int warp_subsampling,
										float warp_sigma)
{
    vigra_precondition(src1.shape() == src2.shape() ,"image sizes differ!");
    
    using namespace ::vigra::multi_math;
    
	steps = std::min((double)steps, log((double)std::min(src1.width(), src1.height()))/log(2.0)-3);
	std::list<unsigned int> step_list = buildStepList(steps, break_level, hmode);
	
    //create gaussian pyramid hierarchy bottom->up
	img_list.resize(steps+1);
    std::vector<vigra::MultiArray<2,T2> >	img2_list(steps+1);
    
	vigra::MultiArray<2,typename OpticalFlowFunctor::FlowValueType> flow_res(src1.shape()), temp_res(src1.shape());
	
	qDebug() << "Building gaussian pyramid for both images with " << steps << " levels";
	
	img_list[0]  = src1;
	img2_list[0] = src2;
	
	for (unsigned int level=1; level<=steps; ++level)
	{
		reduceToNextLevel(img_list[level-1], img_list[level]);
		reduceToNextLevel(img2_list[level-1], img2_list[level]);
		
		flow_list.push_back(vigra::MultiArray<2,typename OpticalFlowFunctor::FlowValueType>(img_list[level].shape()));
		mat_list.push_back(MatrixType(3,3));
		
	}
	
	//work on that hierarchy	
	for (std::list<unsigned int>::iterator iter = step_list.begin(); iter != step_list.end(); ++iter)
	{
		std::list<unsigned int>::iterator next_iter = iter; next_iter++;
		unsigned int s = *iter;
		unsigned int next_s = *next_iter;
		
		qDebug() << "Running OFCE on level " << s;
		
		flow_func.setLevel(s);
		
		calculateOFCE(img_list[s], img2_list[s],
                      flow_list[s],
					  flow_func, 
					  use_gme,
                      mat_list[s],
                      rotation_correlation_list[s],
                      translation_correlation_list[s]);
        
		if(next_iter != step_list.end() )
		{
			//rescale vector length
            double rescale_factor = pow(2.0, double(s));
            
			//store computed u and v to the result
			vigra::resizeImageLinearInterpolation(flow_list[s], temp_res);
            
            //rescale values:
            flow_res += temp_res*rescale_factor;
            
            //use u and v (in a subsampled way) to call the warping
			//progate u's and v's for the next lower step
			vigra::resizeImageLinearInterpolation(flow_list[s], flow_list[next_s]);
			
			//rescale vector length
            rescale_factor = pow(2.0, double(s-next_s));
            flow_list[next_s].bindElementChannel(0) *= rescale_factor;
            flow_list[next_s].bindElementChannel(1) *= rescale_factor;
			
			//smooth further if necessary
			if (warp_sigma != 0.0)
			{
				vigra::gaussianSmoothing(flow_list[next_s], flow_list[next_s], warp_sigma);
			}
			
			//Prepare point list for warping
			std::vector<Vectorfield2D::PointType> src_points, dest_points;
			for(unsigned int y=0; y<(unsigned int)(flow_list[next_s].height()); y+=warp_subsampling)
			{
				for(unsigned int x=0; x<(unsigned int)(flow_list[next_s].width()); x+=warp_subsampling)
				{
					src_points.push_back(Vectorfield2D::PointType(x,y));
					dest_points.push_back(Vectorfield2D::PointType(x+flow_list[next_s](x,y)[0], y+flow_list[next_s](x,y)[1]));
				} 
			}
			//Call the warping functor
			warp(img_list[next_s], img_list[next_s], src_points.begin(), src_points.end(), dest_points.begin());
			
			//Delete (already corrected) motion estimate
			flow_list[next_s] = typename OpticalFlowFunctor::FlowValueType();
		}
	}	
	if(break_level!=0)
	{
        vigra::resizeImageLinearInterpolation(flow_list[break_level], flow_list[0]);
		
		//rescale vector length
		double rescale_factor = pow(2.0, double(break_level));
		
		//store computed u and v to the result
		vigra::resizeImageLinearInterpolation(flow_list[break_level], temp_res);
        flow_res += temp_res*rescale_factor;
	}
	flow_list[0] = flow_res;
}

/**
 * The fourth hierarchical Optical Flow estimation approach:
 *  a) Detect flow at level n
 *  b) use that (probably scaled) flow to warp the img_list image of next level.
 *  c) save the flow at this level
 *  d) proceed with zero assumption flow at next level (n+1)
 *  e) ...
 *  f) at the end: Add all motion increments to obtain complete flow
 *
 * For each (a) the functor is called with a mask and if selected with global motion estimation.
 *
 * \param[in] src1 First image of the series.
 * \param[in] src2 Second image of the series.
 * \param[in] mask THe mask, where pixel values are assumed to be valid.
 * \param[out] flow_list The resulting Optical Flow fields during the steps.
 * \param[in] flowFunc The used functor to compute the Optical Flow.
 * \param[in] use_global If true, the global motion estimation be used prior to each computation.
 * \param[out] mat_list If use_global is true, this contains the global motion estimation matrices (rot+trans).
 * \param[out] rotation_correlation_list If use_global is true, this contains the rotation correlations.
 * \param[out] translation_correlation_list If use_global is true, this contains the transflation correlations.
 * \param steps Step count.
 * \param[in] break_level On wich level shall we finish/break the traversal.
 * \param[in] hmode The hierarchical traversal mode: (0: V, 1: Single W, 2: Full W)
 * \param[in] warp The functor, which is used for warping
 * \param[in] warp_subsampling The subsampling, wich is used for warping
 * \param[in] warp_sigma The sigma, which is used for smoothing the result before subsampling.
 */
template <class T1, class T2, class T3, class MatrixType, class OpticalFlowFunctor, class WarpingFunctor>
void calculateOFCEHierarchicallyWarping(const vigra::MultiArrayView<2,T1> & src1,
										const vigra::MultiArrayView<2,T2> & src2,
										const vigra::MultiArrayView<2,T3> & mask,
										std::vector<vigra::MultiArray<2,T1> >& img_list,
                                        std::vector<vigra::MultiArray<2,typename OpticalFlowFunctor::FlowValueType> >& flow_list,
										OpticalFlowFunctor flow_func,
										bool use_gme,
                                        std::vector<MatrixType>& mat_list,
                                        std::vector<double>& rotation_correlation_list,
                                        std::vector<double>& translation_correlation_list,
                                        unsigned int steps,
                                        unsigned int break_level,
                                        unsigned int hmode,
                                        WarpingFunctor warp,
                                        unsigned int warp_subsampling,
                                        float warp_sigma)
{
    vigra_precondition(src1.shape() == src2.shape() ,"image sizes differ!");
    vigra_precondition(src1.shape() == mask.shape() ,"image and mask sizes differ!");
    
    using namespace ::vigra::multi_math;
    
	steps = std::min((double)steps, log((double)std::min(src1.width(), src1.height()))/log(2.0)-3);
	std::list<unsigned int> step_list = buildStepList(steps, break_level, hmode);
	
    //create gaussian pyramid hierarchy bottom->up
	img_list.resize(steps+1);
	std::vector<vigra::MultiArray<2,T2> >	img2_list(steps+1);
	std::vector<vigra::MultiArray<2,T3> >	mask_list(steps+1);
	
	vigra::MultiArray<2,typename OpticalFlowFunctor::FlowValueType> flow_res(src1.shape()), temp_res(src1.shape());
	
	qDebug() << "Building gaussian pyramid for both images with " << steps << " levels";
	
	img_list[0] = src1;
	img2_list[0] = src2;
	mask_list[0] = mask;
	
	for (unsigned int level=1; level<=steps; ++level)
	{
		reduceToNextLevel(img_list[level-1], img_list[level]);
		reduceToNextLevel(img2_list[level-1], img2_list[level]);
		reduceToNextLevel(mask_list[level-1], mask_list[level]);
		
		flow_list.push_back(vigra::MultiArray<2,typename OpticalFlowFunctor::FlowValueType>(img_list[level].shape()));
		mat_list.push_back(MatrixType(3,3));
		
	}
	
	//work on that hierarchy	
	for (std::list<unsigned int>::iterator iter = step_list.begin(); iter != step_list.end(); ++iter)
	{
		std::list<unsigned int>::iterator next_iter = iter; next_iter++;
		unsigned int s = *iter;
		unsigned int next_s = *next_iter;
		
		qDebug() << "Running OFCE on level " << s;
		
		flow_func.setLevel(s);
		
		calculateOFCE(img_list[s],
					  img2_list[s],
					  mask_list[s],
					  flow_list[s],
					  flow_func,
					  use_gme,
                      mat_list[s],
                      rotation_correlation_list[s],
                      translation_correlation_list[s]);
        
		if(next_iter != step_list.end() )
		{
			//rescale vector length
            double rescale_factor = pow(2.0, double(s));
            
			//store computed u and v to the result
			vigra::resizeImageLinearInterpolation(flow_list[s], temp_res);
            
            //rescale values:
            flow_res += temp_res*rescale_factor;
            
            //use u and v (in a subsampled way) to call the warping
			//progate u's and v's for the next lower step
			vigra::resizeImageLinearInterpolation(flow_list[s], flow_list[next_s]);
			
			//rescale vector length
            rescale_factor = pow(2.0, double(s-next_s));
            
            using namespace vigra::multi_math;
			flow_list[next_s].bindElementChannel(0) *= rescale_factor;
			flow_list[next_s].bindElementChannel(1) *= rescale_factor;
			
			//smooth further if necessary
			if (warp_sigma != 0.0)
			{
				vigra::gaussianSmoothing(flow_list[next_s], flow_list[next_s], warp_sigma);
			}
			
			//Prepare point list for warping
			std::vector<Vectorfield2D::PointType> src_points, dest_points;
			for(unsigned int y=0; y<(unsigned int)(flow_list[next_s].height()); y+=warp_subsampling)
			{
				for(unsigned int x=0; x<(unsigned int)(flow_list[next_s].width()); x+=warp_subsampling)
				{
					src_points.push_back(Vectorfield2D::PointType(x,y));
					dest_points.push_back(Vectorfield2D::PointType(x+flow_list[next_s](x,y)[0], y+flow_list[next_s](x,y)[1]));
				} 
			}
			//Call the warping functor
			warp(img_list[next_s], img_list[next_s], src_points.begin(), src_points.end(), dest_points.begin());
			
			//Delete (already corrected) motion estimate
			flow_list[next_s] = typename OpticalFlowFunctor::FlowValueType();
		}
	}	
	if(break_level!=0)
	{
        vigra::resizeImageLinearInterpolation(flow_list[break_level], flow_list[0]);
		
		//rescale vector length
		double rescale_factor = pow(2.0, double(break_level));
		
		//store computed u and v to the result
		vigra::resizeImageLinearInterpolation(flow_list[break_level], temp_res);
        flow_res += temp_res*rescale_factor;
	}
	flow_list[0] = flow_res;
}

} //end of namespace graipe

#endif //GRAIPE_OPTICALFLOW_OPTICALFLOWFRAMEWORK_HXX
