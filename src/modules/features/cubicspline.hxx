/************************************************************************/
/*                                                                      */
/*               Copyright 2008-2016 by Benjamin Seppke                 */
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

#ifndef GRAIPE_FEATURES_CUBICSPLINE_HXX
#define GRAIPE_FEATURES_CUBICSPLINE_HXX

#include "features/config.hxx"
#include "features/featurelist.hxx"

namespace graipe {

/**
 * This class represents a CubicSpline by means of a 1D interpolation
 * function over an arbitrary number of supporting (x,y)-values.
 */
class GRAIPE_FEATURES_EXPORT CubicSpline1D
{
    public:
        //Internally used types
        typedef PointFeatureList2D::PointType PointType;
        typedef QVector<PointType> PointListType;
        
        /**
         * Default constructor. Constructs an empty 1D cubic spline.
         */
        CubicSpline1D();

        /**
         * A more convenient constructor. Constructs a 1D cubic spline by means of
         * a set of function values (x,y) and the first and last derivative at 
         * the boundaries of this spline.
         *
         * \param points The list of points (x,y) values. Need to be ordered (increasing x).
         * \param first_derivative The starting gradient.
         * \param last_derivative The finishing gradient.
         */
        CubicSpline1D(const PointListType & points, float first_derivative, float last_derivative);
    
        /**
         * Virtual destructor of this class, needed due to non-final QVector class.
         */
        virtual ~CubicSpline1D();
    
        /**
         * Return the first derivative (just before the first point).
         *
         * \return The first order derivative of the 1D cubic spline just before the first point.
         */
        inline float firstDerivative() const;
    
        /**
         * Resets the first derivative (just before the first point).
         *
         * \param first_derivative The first order derivative of the 1D cubic spline just before the first point.
         */
        inline void setFirstDerivative(float first_derivative);
    
        /**
         * Return the last derivative (just after the last point).
         *
         * \return The first order derivative of the 1D cubic spline just after the last point.
         */
        inline float lastDerivative() const;
    
        /**
         * Resets the last derivative (just after the last point).
         *
         * \param first_derivative The first order derivative of the 1D cubic spline just after the last point.
         */
        inline void setLastDerivative(float last_derivative);
    
        /**
         * Interpolation: Return the resulting (interpolated) value.
         *
         * \param x The ordinate, for which we shall interpolate.
         * \return The value of the 1D cubic spline at that given ordinate.
         */
        inline float interpolate(float x) const;
    
        /**
         * Interpolation: Return the first order derived (interpolated) value.
         *
         * \param x The ordinate, for which we shall interpolate.
         * \return The first order derivative of the 1D cubic spline at that given ordinate.
         */
        inline float derive(float x) const;
        
        /**
         * Interpolation: Return the second order derived (interpolated) value.
         *
         * \param x The ordinate, for which we shall interpolate.
         * \return The second order derivative of the 1D cubic spline at that given ordinate.
         */
        inline float derive2(float x) const;
        
        /**
         * Update/replace a point in the point list with/by a new one. Does nothing
         * if the index is out of range w.r.t. the list of points.
         *
         * \param index The index, for which we want to replace the point.
         * \param point The new point, which will replace the old one
         */
        void updatePoint(unsigned int index, const PointType& point);
        
        /**
         * Returns the number of points in the point list.
         *
         * \return The number of points in the point list.
         */
        inline unsigned int size() const;
        
        /**
         * Returns a constant reference access to the point list. This can be used for
         * read-access only!
         *
         * \return The const reference to the point list.
         */
        const PointListType & points() const;
        
    protected:
        /**
         * Updates the second order derivatives, which are needed and thus chached
         * for the cubic interpolation.
         * Adapted from Numerical Recipies... Cubic spline interpolation
         */
        inline void updateSecondDerivatives();
        
        //Point (x,y) list
        PointListType m_points;
        
        //Gradients before and after the given points
        float m_first_derivative;
        float m_last_derivative;
        
        //Second order derivatives at each point
        QVector<float> m_y2;
};
    
    
    
    
/**
 * This class represents a parameteric 2D CubicSpline by means of two coupled 1D cubic splines.
 * One 2D spline interpolates over the x-, the other one over the y-axis. Thus,
 * the CubicSpline is mainly a pointfeaturelist, but with changed semantics.
 * In addition, the first and last (first order) deriviatives become points containing
 * their partial derivatives in x- and y-direction.
 */
class GRAIPE_FEATURES_EXPORT CubicSpline2D
{
    public:
        //Internally used types
        typedef CubicSpline1D::PointType PointType;
        typedef CubicSpline1D::PointListType PointListType;
        
        /**
         * Default constructor. Constructs an empty 2D cubic spline.
         */
        CubicSpline2D();
    
        /**
         * A more convenient constructor. Constructs a 2D cubic spline by means of
         * a set of points (x,y) and the first and last derivative at
         * the boundaries of this 2D spline.
         *
         * \param points The list of points (x,y) values.
         * \param first_derivative The starting gradient, given as (d_x, d_y).
         * \param last_derivative The finishing gradient, given as (d_x, d_y).
         */
        CubicSpline2D(const PointListType& points, const PointType& first_derivative, const PointType& last_derivative);
                    
        /**
         * Virtual destructor of this class, needed due to non-final QVector class.
         */
        virtual ~CubicSpline2D();
    
        /**
         * Return the first derivative (just before the first point).
         *
         * \return The first order derivative of the 2D cubic spline just before the first point.
         */
        inline const PointType& firstDerivative() const;
    
        /**
         * Resets the first derivative (just before the first point).
         *
         * \param first_derivative The first order derivative of the 2D cubic spline just before the first point.
         */
        inline void setFirstDerivative(const PointType& first_derivative);
    
        /**
         * Return the last derivative (just after the last point).
         *
         * \return The first order derivative of the 2D cubic spline just after the last point.
         */
        inline const PointType& lastDerivative() const;
    
        /**
         * Resets the last derivative (just after the last point).
         *
         * \param first_derivative The first order derivative of the 2D cubic spline just after the last point.
         */
        inline void setLastDerivative(const PointType& last_derivative);
    
        /**
         * Interpolation: Return the resulting (interpolated) point.
         *
         * \param t The parametric value, for which we want to interpolate.
         *          A t of zero means the first point, a t of one the last point.
         * \return The point of the 2D cubic spline at that given parameter.
         */
        PointType interpolate(float t) const;
        /**
         * Interpolation: Return the first order derived (interpolated) point.
         *
         * \param t The parametric value, for which we want to interpolate.
         *          A t of zero means the first point, a t of one the last point.
         * \return The first order derivative (d_x, d_y) of 2D cubic spline at that given parameter.
         */
        PointType derive(float t) const;

        /**
         * Interpolation: Return the second order derived (interpolated) point.
         *
         * \param t The parametric value, for which we want to interpolate.
         *          A t of zero means the first point, a t of one the last point.
         * \return The second order derivative (d_xx, d_yy) of the 2D cubic spline at that given parameter.
         */
        PointType derive2(float t) const;
        
        /**
         * Returns the number of points in the point list.
         *
         * \return the number of points in the point list.
         */
        virtual unsigned int size() const;
        
        /**
         * Returns a constant reference access to the point list. This can be used for
         * read-access only!
         *
         * \return The const reference to the point list.
         */
        const PointListType & points() const;
		
        /**
         * Returns the point of a given index in the point list. Throws an error, if the
         * index is out of range w.r.t. the list of points.
         *
         * \param index The index, for which we want to replace the point.
         * \return The point, which is listed a the given index.
         */
		virtual PointType point(unsigned int index) const;
    
        /**
         * Update/replace a point in the point list with/by a new one. Does nothing
         * if the index is out of range w.r.t. the list of points.
         *
         * \param index The index, for which we want to replace the point.
         * \param point The new point, which will replace the old one
         */
		virtual void setPoint(unsigned int idx, const PointType& new_p);
		
        /**
         * Add a point to the end of the point list.
         *
         * \param x The ordinate of the new point.
         * \param y The coordinate ot the new point.
         */
		virtual void addPoint(float x, float y);
        
        /**
         * Add a point to the end of the point list.
         *
         * \param p The new point.
         */
		virtual void addPoint(const PointType& p);
	
        /**
         * Returns the bounding rectangle of this 2D cubic spline.
         *
         * \return The bounding rectangle of this 2D cubic spline.
         */
		const QRectF& boundingRect() const;
		
	protected:
        /**
         * Internal update function to update the cubic spline interpolations after
         * the update/addition/removal of points
         */
        void updateFromPoints();
    
        //One cubic spline for each dimension
        CubicSpline1D m_splineX;
        CubicSpline1D m_splineY;
   
        //Point (x,y) list
        PointListType m_points;
    
        //Gradients before and after the given points
        PointType m_first_derivative;
        PointType m_last_derivative;
    
        //Cached bounding rect of this Cubic spline
        QRectF m_boundingRect;    
};
    
} //end of namespace graipe

#endif //GRAIPE_FEATURES_CUBICSPLINE_HXX
