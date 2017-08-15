/************************************************************************/
/*                                                                      */
/*               Copyright 2008-2017 by Benjamin Seppke                 */
/*       Cognitive Systems Group, University of Hamburg, Germany        */
/*                                                                      */
/*    Additional Copyright for most parts of the Delaunay lib, which    */
/*    I ported to use VIGRA style and types is due to:                  */
/*           2005, Sjaak Priester, Amsterdam (sjaak@sjaakpriester.nl)   */
/*    See his copyright notice below the GRAIPE preamble for more       */
/*    information and the license.                                      */
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

#ifndef GRAIPE_REGISTRATION_PIECEWISEAFFINE_REGISTRATION_HXX
#define GRAIPE_REGISTRATION_PIECEWISEAFFINE_REGISTRATION_HXX

#include <vigra/mathutil.hxx>
#include <vigra/matrix.hxx>
#include <vigra/affine_registration.hxx>
#include <vigra/tinyvector.hxx>
#include <vigra/splineimageview.hxx>

#include "registration/delaunay.hxx"

namespace graipe {

/**
 * @addtogroup graipe_registration
 * @{
 *
 * @file
 * @brief Header file for the piecewise affine registration approach
 */
 
/** The used type for points **/
typedef Vertex PointType;

/** The used type for triangles **/
typedef Triangle<double> TriangleType;

/** The used triangle transformation type **/
typedef std::pair<TriangleType, vigra::Matrix<double> > TriangleTransformationType;


/**
 * This function computes the piecewise affine transmations for a set of points
 * It first Delaunay triangluates the source points and uses the same triangle structure on
 * source and target points. 
 * For each of the triangles, it computes the affine matrix.
 *
 * \param s     The begin() iterator of the source points.
 * \param s_end The end() iterator of the source points.
 * \param d    The begin() iterator of the corresponding dest points.
 * \return A Vector containing pairs of triangles and affine transformation matrices.
 */
template <class SrcPointIterator, class DestPointIterator>
std::vector<TriangleTransformationType> computePiecewiseAffineTransformations(SrcPointIterator s, SrcPointIterator s_end, DestPointIterator d)
{
    unsigned int point_count = (unsigned int)(s_end - s);
    
    VertexVector vertices(point_count);
    TriangleSet triangles;
    
    unsigned int i=0;
    
    for (i=0; i<point_count;++i)
    {
        vertices[i] = Vertex((float)s[i][0], (float)s[i][1]);
    }
    
    delaunay_triangulation(vertices, triangles);
    
    std::vector<PointType> s_points(3), d_points(3);
    
    std::vector<TriangleTransformationType> result;
    
	for (TriangleSet::iterator iter = triangles.begin(); iter!=triangles.end(); iter++) 
    {
        for(unsigned int i=0; i<3; ++i)
        {
            unsigned long idx = iter->vertex(i) - vertices.data();
            s_points[i] = s[idx];
            s_points[i] = d[idx];
        }
        result.push_back(TriangleTransformationType(*iter,
                                                    vigra::affineMatrix2DFromCorrespondingPoints(d_points.begin(), d_points.end(), s_points.begin())));
    }
    
    return result;
}

/**
 * Given a piecewise affine transformation structure as returned by computePiecewiseAffineTransformations
 * this function returns the transformed image.
 * 
 * \param src The source image.
 * \param dest The destination image.
 * \param tri_trans The triangles and their transformations as returned by computePiecewiseAffineTransformations.
 */
template <int ORDER, class T1, class T2>
void piecewiseAffineWarpImage(vigra::SplineImageView<ORDER, T1> const & src, vigra::MultiArrayView<2,T2> dest,
                              const std::vector<TriangleTransformationType> & tri_trans)
{

    for(unsigned int y = 0; y<dest.height(); ++y)
    {
        for(unsigned int x=0; x<dest.width(); ++x)
        {
            for(std::vector<TriangleTransformationType>::const_iterator iter=tri_trans.begin(); iter!=tri_trans.end(); ++iter)
            {
                if(iter->first.isInside(PointType(x,y)))
                {
                    vigra::Matrix<double> transformation = iter->second;
                    double sx = transformation(0,0)*x + transformation(0,1)*y + transformation(0,2);
                    double sy = transformation(1,0)*x + transformation(1,1)*y + transformation(1,2); 
                    
                    if(src.isInside(sx, sy))
                        dest(x,y) =  src(sx, sy);
                }
            }
        }
    }
}

/**
 * @}
 */

} //end of namespace graipe

#endif // GRAIPE_REGISTRATION_PIECEWISEAFFINE_REGISTRATION_HXX
