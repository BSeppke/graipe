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

#ifndef GRAIPE_ANALYSIS_VECTORFIELDSEPARATION_HXX
#define GRAIPE_ANALYSIS_VECTORFIELDSEPARATION_HXX

#include "vectorfields/vectorfields.h"
#include <vigra/linear_solve.hxx>

namespace graipe {

/**
 * Estimate a global motion matrix (rotation and translation) from a 
 * vectorfield of any kind by using mean square error solution of the 
 * corresponding system of linear equations.
 *
 * \param vf The vectorfield, for which we want to estimate the global motion.
 * \return The global motion (rot. + trans.) of that vectorfield.
 */
vigra::linalg::TemporaryMatrix<double> estimateGlobalMotionOfVectorfield(const Vectorfield2D* vf)
{
	
	//Estimate Affine Transform from all vf->points:
	//
	unsigned int size = vf->size();
	
	vigra::Matrix<double> A(size,3), b(size,2), res(3,2);
	for (unsigned int i =0; i<size; ++i)
	{
		//s_x/y				r1					t_x			b_x/y
		//----------------------------------------------------------------------------------
		A(i,0)=vf->origin(i).x();		A(i,1)=vf->origin(i).y();		A(i,2)=1;	b(i,0)=vf->target(i).x();
		b(i,1)=vf->target(i).y();
	}
	
	if(!linearSolve(A, b, res))
		vigra_fail("splitMotion(): singular solution matrix.");
	
    vigra::linalg::TemporaryMatrix<double> affineMat(3,3);
	
	affineMat(0,0) = res(0,0);	affineMat(0,1)=res(1,0);	affineMat(0,2) = res(2,0);
	affineMat(1,0) = res(0,1);	affineMat(1,1)=res(1,1);	affineMat(1,2) = res(2,1);
	affineMat(2,0) =		0;	affineMat(2,1)=       0;	affineMat(2,2) =        1;
	
	return affineMat;	
}



/**
 * This algorithm estimates the global motion of any vectorfield and resturns 
 * a separated version of the vectorfield, where the global motion is stored, too.
 *
 * \param vf The vectorfield, for which we want to estimate the global motion.
 * \return A vectorfield, where the global motion (rot. + trans.) is stored as
 *         the member function globalMotion() of that vectorfield.
 */
template<class VECTORFIELD_CLASS>
VECTORFIELD_CLASS* computeGlobalMotionOfVectorfield(const VECTORFIELD_CLASS* vf)
{
	VECTORFIELD_CLASS* result_vf = new VECTORFIELD_CLASS(*vf);
	
    vigra::linalg::TemporaryMatrix<double> mat = estimateGlobalMotionOfVectorfield(vf);
    
    QTransform transform(mat(0,0), mat(1,0), mat(2,0),
                         mat(0,1), mat(1,1), mat(2,1),
                         mat(0,2), mat(1,2), mat(2,2));

    QString mat_str = "";
        //TODO:TransformParameter::valueText(transform);
    
	result_vf->setGlobalMotion(transform);
	result_vf->setName(QString("Separated motion of: ") + vf->name());
    
	QTextStream ss;
	
	ss  << "Separated motion of: " <<  vf->name() << "\n"
		<< "Matrix was:\n"
        << mat_str;
	
	result_vf->setDescription(ss.readAll());
							  
	return result_vf;
}

} //end of namespace graipe

#endif //GRAIPE_ANALYSIS_VECTORFIELDSEPARATION_HXX
