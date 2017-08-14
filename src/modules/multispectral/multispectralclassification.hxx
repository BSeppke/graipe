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

#ifndef GRAIPE_MULTISPECTRAL_MULTISPECTRALCLASSIFICATION_HXX
#define GRAIPE_MULTISPECTRAL_MULTISPECTRALCLASSIFICATION_HXX

#include "core/algorithm.hxx"
#include <vigra/multi_array.hxx>

namespace graipe {

/**
 * @addtogroup graipe_multispectral
 * @{
 *
 * @file
 * @brief Header file for multispectral classification algorithms.
 */

/**
 * This function implements the Normalized Diffence Vegetation Index (NDVI)
 * from two different image bands. If an algorithm pointer is provided, the 
 * status is updated to show progress.
 *
 * \param[in] s_nir The source band at the near infrared.
 * \param[in] s_red The source band at the red part of the spectrum.
 * \param[out] dest The resulting values of the NDVI for each pixel.
 * \param alg Pointer to the algorithm, used for update status.
 */
template <class T1, class T2>
void computeNDVI(const vigra::MultiArrayView<2,T1> & s_nir, const vigra::MultiArrayView<2,T1> & s_red, vigra::MultiArrayView<2,T2> dest,
                 Algorithm* alg = NULL)
{

    vigra_precondition(s_nir.shape() == s_red.shape() ,"channel sizes differ!");
    vigra_precondition(s_red.shape() == dest.shape() ,"channel and dest sizes differ!");
    
	unsigned int i=0;
	
    auto nir_iter = s_nir.begin();
    auto red_iter = s_red.begin();
    auto dest_iter = dest.begin();
    
    for( ; nir_iter != s_nir.end(); ++nir_iter, ++red_iter, ++dest_iter, ++i)
    {
        T2 temp = *nir_iter + *red_iter;
			
        if(temp != 0)
        {
			*dest_iter = (*nir_iter - *red_iter)/ temp;
        }
		else
        {
			*dest_iter = 0;
		}
		if (i%dest.width()==0 && alg)
		{
			alg->status_update(100.0*i/dest.size());
		}
	}
}

/**
 * This function implements the Enhanced Vegetation Index (EVI)
 * from two different image bands. If an algorithm pointer is provided, the 
 * status is updated to show progress.
 *
 * \param[in] s_nir The source band at the near infrared.
 * \param[in] s_red The source band at the red part of the spectrum.
 * \param[in] c The c parameter of the EVI algorithm.
 * \param[in] l The l parameter of the EVI algorithm.
 * \param[in] g The g parameter of the EVI algorithm.
 * \param[out] dest The resulting values of the NDVI for each pixel.
 * \param alg Pointer to the algorithm, used for update status.
 */
template <class T1, class T2>
void computeEVI2(const vigra::MultiArrayView<2,T1> & s_nir, const vigra::MultiArrayView<2,T1> & s_red, vigra::MultiArrayView<2,T2> dest,
				 double c, double l, double g,
				 Algorithm* alg = NULL)
{

    vigra_precondition(s_nir.shape() == s_red.shape() ,"channel sizes differ!");
    vigra_precondition(s_red.shape() == dest.shape() ,"channel and dest sizes differ!");
    
	unsigned int i=0;
	
    auto nir_iter = s_nir.begin();
    auto red_iter = s_red.begin();
    auto dest_iter = dest.begin();
    
    for( ; nir_iter != s_nir.end(); ++nir_iter, ++red_iter, ++dest_iter, ++i)
    {
        T2 temp = *nir_iter + c *(*red_iter) + l;
			
        if(temp != 0)
        {
            *dest_iter = g*(*nir_iter - *red_iter)/ temp;
        }
        else
        {
            *dest_iter = 0;
		}
		if (i%dest.width()==0 && alg)
		{
			alg->status_update(100.0*i/dest.size());
		}
	}
}

/**
 * This function implements the Enhanced Vegetation Index (EVI)
 * from three different image bands. If an algorithm pointer is provided, the
 * status is updated to show progress.
 *
 * \param[in] s_nir The source band at the near infrared.
 * \param[in] s_red The source band at the red part of the spectrum.
 * \param[in] s_blue The source band at the blue part of the spectrum.
 * \param[in] c1 The c1 parameter of the EVI algorithm.
 * \param[in] c2 The c2 parameter of the EVI algorithm.
 * \param[in] l The l parameter of the EVI algorithm.
 * \param[in] g The g parameter of the EVI algorithm.
 * \param[out] dest The resulting values of the NDVI for each pixel.
 * \param alg Pointer to the algorithm, used for update status.
 */
template <class T1, class T2>
void computeEVI(const vigra::MultiArrayView<2,T1> & s_nir, const vigra::MultiArrayView<2,T1> & s_red, const vigra::MultiArrayView<2,T1> & s_blue, vigra::MultiArrayView<2,T2> dest,
				 double c1, double c2, double l, double g,
				 Algorithm* alg = NULL)
{

    vigra_precondition(s_nir.shape() == s_red.shape() ,"channel sizes differ!");
    vigra_precondition(s_nir.shape() == s_blue.shape() ,"channel sizes differ!");
    vigra_precondition(s_red.shape() == dest.shape() ,"channel and dest sizes differ!");
    
	unsigned int i=0;
	
    auto nir_iter = s_nir.begin();
    auto red_iter = s_red.begin();
    auto blue_iter = s_blue.begin();
    auto dest_iter = dest.begin();
    
    for( ; nir_iter != s_nir.end(); ++nir_iter, ++red_iter, ++blue_iter, ++dest_iter, ++i)
    {
        T2 temp = *nir_iter + c1 *(*red_iter) + c2 *(*blue_iter) + l;
			
        if(temp != 0)
        {
            *dest_iter = g*(*nir_iter - *red_iter)/ temp;
        }
        else
        {
            *dest_iter = 0;
		}
		if (i%dest.width()==0 && alg)
		{
			alg->status_update(100.0*i/dest.size());
		}
	}
}

/**
 * @}
 */
 
} //end of namespace graipe

#endif //GRAIPE_MULTISPECTRAL_MULTISPECTRALCLASSIFICATION_HXX
