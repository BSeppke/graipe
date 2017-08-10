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

#ifndef GRAIPE_REGISTRATION_WARPINGFUNCTORS_HXX
#define GRAIPE_REGISTRATION_WARPINGFUNCTORS_HXX

#include <vigra/stdimage.hxx>
#include <vigra/splineimageview.hxx>
#include <vigra/convolution.hxx>
#include <vigra/linear_solve.hxx>
#include <vigra/affinegeometry.hxx>

#include "registration/piecewiseaffine_registration.hxx"

#include <vigra/projective_registration.hxx>
#include <vigra/polynomial_registration.hxx>
#include <vigra/rbf_registration.hxx>

namespace graipe {

/**
 * This class represents the affine registration functor.
 * It will create a functor with a affine registration matrix, that can be used for registration purpose
 * by means of the affineWarpImage function.
 */
class WarpAffineFunctor
{
    public:
        /**
         * The functor call. It transforms the first image with respect to the given point correspondences
         * to match the second image as best as possible, given the affine model.
         *
         * \param src   The first image.
         * \param dest  The second (reference) image.
         * \param s     The begin() iterator of the source points.
         * \param s_end The end() iterator of the source points.
         * \param d    The begin() iterator of the corresponding dest points.
         */
        template <class T1, class T2, class SrcPointIterator, class DestPointIterator>
        void operator()(const vigra::MultiArrayView<2, T1> & src, vigra::MultiArrayView<2, T2> dest,
                        SrcPointIterator s, SrcPointIterator s_end,
                        DestPointIterator d)
        {
            vigra::affineWarpImage(vigra::SplineImageView<4, T1>(src), dest,
                                   vigra::affineMatrix2DFromCorrespondingPoints(s, s_end, d));
        }
        /**
         * The static name of this functor.
         *
         * \return Always "Affine reg.".
         */
        static QString name()
        {
            return "Affine reg.";
        }
};




/**
 * This class represents the projective registration functor.
 * It will create a functor with a projective registration matrix, that can be used for registration purpose
 * by means of the projectiveWarpImage function.
 */
class WarpProjectiveFunctor
{
    public:
        /**
         * The functor call. It transforms the first image with respect to the given point correspondences
         * to match the second image as best as possible, given the projective model.
         *
         * \param src   The first image.
         * \param dest  The second (reference) image.
         * \param s     The begin() iterator of the source points.
         * \param s_end The end() iterator of the source points.
         * \param d    The begin() iterator of the corresponding dest points.
         */
       template <class T1, class T2, class SrcPointIterator, class DestPointIterator>
        void operator()(const vigra::MultiArrayView<2, T1> & src, vigra::MultiArrayView<2, T2> dest,
                        SrcPointIterator s, SrcPointIterator s_end,
                        DestPointIterator d)
        {
            vigra::projectiveWarpImage(vigra::SplineImageView<4, T1>(src), dest,
                                       vigra::projectiveMatrix2DFromCorrespondingPoints(s, s_end, d));
        }
    
        /**
         * The static name of this functor.
         *
         * \return Always "Projective reg.".
         */
        static QString name()
        {
            return "Projective reg.";
        }
};




/**
 * This class represents the piecewise affine registration functor.
 * It will create a functor with a piecewise affine registration  model, that can be used for registration purpose
 * by means of the piecewiseAffineWarpImage function.
 */
class WarpPiecewiseAffineFunctor
{
    public:
        /**
         * The functor call. It transforms the first image with respect to the given point correspondences
         * to match the second image as best as possible, given the piecewise affine model.
         *
         * \param src   The first image.
         * \param dest  The second (reference) image.
         * \param s     The begin() iterator of the source points.
         * \param s_end The end() iterator of the source points.
         * \param d    The begin() iterator of the corresponding dest points.
         */
        template <class T1, class T2, class SrcPointIterator, class DestPointIterator>
        void operator()(const vigra::MultiArrayView<2, T1> & src, vigra::MultiArrayView<2, T2> dest,
                        SrcPointIterator s, SrcPointIterator s_end,
                        DestPointIterator d)
        {
            piecewiseAffineWarpImage(vigra::SplineImageView<4, T1>(src), dest,
                                     computePiecewiseAffineTransformations(s, s_end, d));
        }

        /**
         * The static name of this functor.
         *
         * \return Always "Piecewise affine reg.".
         */
        static QString name()
        {
            return "Piecewise affine reg.";
        }
};




/**
 * This class is the hull for all polynomial registration functors of any degree N.
 * Given an N, this will create a functor with a polynomial model, that can be used for registration purpose
 * by means of the polynomialWarpImage function.
 */
template <unsigned int N>
class WarpPolynomialFunctor
{
    public:
        /**
         * The functor call. It transforms the first image with respect to the given point correspondences
         * and the polynom degree to match the second image as best as possible, given the polynomial model.
         *
         * \param src   The first image.
         * \param dest  The second (reference) image.
         * \param s     The begin() iterator of the source points.
         * \param s_end The end() iterator of the source points.
         * \param d    The begin() iterator of the corresponding dest points.
         */
        template <class T1, class T2, class SrcPointIterator, class DestPointIterator>
        void operator()(const vigra::MultiArrayView<2, T1> & src, vigra::MultiArrayView<2, T2> dest,
                        SrcPointIterator s, SrcPointIterator s_end,
                        DestPointIterator d)
        {
            vigra::polynomialWarpImage<N>(vigra::SplineImageView<4, T1>(src), dest,
                                          vigra::polynomialMatrix2DFromCorrespondingPoints<N>(s, s_end, d));
        }
	
        /**
         * The static name of this functor. It mainly depends on the degree of the polynom.
         *
         * \return The name of this functor.
         */
        static QString name()
        {
            switch (N)
            {
                case 1:
                    return "Bilinear reg.";
                    
                case 2:
                    return "Biquadratic reg.";
                    
                case 3:
                    return "Bicubic reg.";
                    
                default:
                    return QString("Polynomial reg. of deg. %1").arg(N);
            }
        }
};




/**
 * Name traits for Radial Basis Functors:
 */
template<class T> QString rbfName()                                 { return "Unknown RBF functor"; }
template<>        QString rbfName<vigra::ThinPlateSplineFunctor>()  { return "Thin Plate Splines";  }
template<>        QString rbfName<vigra::DistancePowerFunctor<1> >(){ return "Distance Power<1>";   }
template<>        QString rbfName<vigra::DistancePowerFunctor<2> >(){ return "Distance Power<2>";   }
template<>        QString rbfName<vigra::DistancePowerFunctor<3> >(){ return "Distance Power<3>";   }




/**
 * This class is the hull for a Radial Basis Function (RBF) functor.
 * Given an RBF, this will create a functor, that can be used for registration purpose
 * by means of the rbfWarpIamge function.
 */
template <class RadialBasisFunctor>
class WarpRadialBasisFunctor
{
    public:
        /**
         * The functor call. It transforms the first image with respect to the given point correspondences
         * and the RBF functor to match the second image as best as possible, given the RBF model.
         *
         * \param src   The first image.
         * \param dest  The second (reference) image.
         * \param s     The begin() iterator of the source points.
         * \param s_end The end() iterator of the source points.
         * \param d    The begin() iterator of the corresponding dest points.
         */
        template <class T1, class T2, class SrcPointIterator, class DestPointIterator>
        void operator()(const vigra::MultiArrayView<2, T1> & src, vigra::MultiArrayView<2, T2> dest,
                        SrcPointIterator s, SrcPointIterator s_end,
                        DestPointIterator d)
        {
            RadialBasisFunctor rbf;
            
            rbfWarpImage(vigra::SplineImageView<4, T1>(src), dest,
                         d,  d+ (s_end-s),
                         vigra::rbfMatrix2DFromCorrespondingPoints(s, s_end, d, rbf),
                         rbf);
        }
    
        /**
         * The static name of this functor. It mainly depends on the Name traits above the 
         * class definition.
         *
         * \return The name of this functor.
         */
        static QString name()
        {
            return rbfName<RadialBasisFunctor>();
        }
};

//Some more typedefs for convenience
typedef WarpPolynomialFunctor<1> WarpBilinearFunctor;
typedef WarpPolynomialFunctor<2> WarpBiquadraticFunctor;
typedef WarpPolynomialFunctor<3> WarpBicubicFunctor;
typedef WarpRadialBasisFunctor<vigra::ThinPlateSplineFunctor  > WarpTPSFunctor;
typedef WarpRadialBasisFunctor<vigra::DistancePowerFunctor<1> > WarpRBF1Functor;
typedef WarpRadialBasisFunctor<vigra::DistancePowerFunctor<2> > WarpRBF2Functor;
typedef WarpRadialBasisFunctor<vigra::DistancePowerFunctor<3> > WarpRBF3Functor;

    
} //end of namespace graipe

#endif //GRAIPE_REGISTRATION_WARPINGFUNCTORS_HXX
