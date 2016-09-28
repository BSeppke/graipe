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

#include "features2d/cubicspline.hxx"

namespace graipe {

/**
 * Default constructor. Constructs an empty 1D cubic spline.
 */
CubicSpline1D::CubicSpline1D()
{
}

/**
 * A more convenient constructor. Constructs a 1D cubic spline by means of
 * a set of function values (x,y) and the first and last derivative at 
 * the boundaries of this spline.
 *
 * \param points The list of points (x,y) values. Need to be ordered (increasing x).
 * \param first_derivative The starting gradient.
 * \param last_derivative The finishing gradient.
 */
CubicSpline1D::CubicSpline1D(const PointListType & points, float first_derivative, float last_derivative)
:   m_points(points),
    m_first_derivative(first_derivative),
    m_last_derivative(last_derivative),
    m_y2(points.size())
{
    updateSecondDerivatives();
}

/**
 * Virtual destructor of this class, needed due to non-final QVector class.
 */
CubicSpline1D::~CubicSpline1D()
{
}

/**
 * Return the first derivative (just before the first point).
 *
 * \return The first order derivative of the 1D cubic spline just before the first point.
 */
float CubicSpline1D::firstDerivative() const
{
    return m_first_derivative;
}

/**
 * Resets the first derivative (just before the first point).
 *
 * \param first_derivative The first order derivative of the 1D cubic spline just before the first point.
 */
void CubicSpline1D::setFirstDerivative(float first_derivative)
{
    m_first_derivative = first_derivative;
    updateSecondDerivatives();
}

/**
 * Return the last derivative (just after the last point).
 *
 * \return The first order derivative of the 1D cubic spline just after the last point.
 */
float CubicSpline1D::lastDerivative() const
{
    return m_last_derivative;
}

/**
 * Resets the last derivative (just after the last point).
 *
 * \param first_derivative The first order derivative of the 1D cubic spline just after the last point.
 */
void CubicSpline1D::setLastDerivative(float last_derivative)
{
    m_last_derivative = last_derivative;
    updateSecondDerivatives();
}
/**
 * Interpolation: Return the resulting (interpolated) value.
 *
 * \param x The ordinate, for which we shall interpolate.
 * \return The value of the 1D cubic spline at that given ordinate.
 */
float CubicSpline1D::interpolate(float x) const
{
    int n = m_points.size();
    
    int klo, khi, k;
    float a, b, c, d, h;
    
    klo = 0;
    khi = n - 1;
    
    while(khi - klo > 1)
    {
        k = (khi + klo) >> 1;
        if(m_points[k].x() > x)
        {
            khi = k;
        }
        else
        {
            klo = k;
        }
    }
    
    h = m_points[khi].x() - m_points[klo].x();
    if(h != 0.0)
    {
        qCritical() << "CubicSpline1D::interpolate: Bad point set given for spline interpolation. Needs to be ordered!";
        throw std::runtime_error("Bad point set given for spline interpolation.");
    }
    
    a = (m_points[khi].x() - x)/h;
    b = (x - m_points[klo].x())/h;
    c = (a*a*a-a)*(h*h) / 6.0;
    d = (b*b*b-b)*(h*h) / 6.0;
    
    return a*m_points[klo].y() + b*m_points[khi].y() + c*m_y2[klo] + d*m_y2[khi];
}
    
/**
 * Interpolation: Return the first order derived (interpolated) value.
 *
 * \param x The ordinate, for which we shall interpolate.
 * \return The first order derivative of the 1D cubic spline at that given ordinate.
 */
float CubicSpline1D::derive(float x) const
{
    int n = m_points.size();
    
    int klo, khi, k;
    float a, b, c, d, h;
    
    klo = 0;
    khi = n - 1;
    
    while(khi - klo > 1)
    {
        k = (khi + klo) >> 1;
        if(m_points[k].x() > x)
        {
            khi = k;
        }
        else
        {
            klo = k;
        }
    }
    
    h = m_points[khi].x() - m_points[klo].x();
    if(h != 0.0)
    {
        qCritical() << "CubicSpline1D::derive: Bad point set given for spline derivation. Needs to be ordered!";
        throw std::runtime_error("Bad point set given for spline derivation.");
    }
    
    a = (m_points[khi].x() - x)/h;
    b = (x - m_points[klo].x())/h;
    c = (a*a*a-a)*(h*h) / 6.0;
    d = (b*b*b-b)*(h*h) / 6.0;
    
    return  -1.0/h*m_points[klo].y() + 1.0/h*m_points[khi].y() + (-3.0*a*a+1.0)*h/6.0*m_y2[klo] + (3.0*b*b-1.0)*h/6.0*m_y2[khi];
    /*      da/dx                    db/dx                 '------ db/dx -----'                '------ db/dx -----'      */
}

/**
 * Interpolation: Return the second order derived (interpolated) value.
 *
 * \param x The ordinate, for which we shall interpolate.
 * \return The second order derivative of the 1D cubic spline at that given ordinate.
 */
float CubicSpline1D::derive2(float x) const
{
    int n = m_points.size();
    
    int klo, khi, k;
    float a, b, c, d, h;
    
    klo = 0;
    khi = n - 1;
    
    while(khi - klo > 1)
    {
        k = (khi + klo) >> 1;
        if(m_points[k].x() > x)
        {
            khi = k;
        }
        else
        {
            klo = k;
        }
    }
    
    h = m_points[khi].x() - m_points[klo].x();
    if(h != 0.0)
    {
        qCritical() << "CubicSpline1D::derive2: Bad point set given for spline derviation of order 2. Needs to be ordered!";
        throw std::runtime_error("Bad point set given for spline derivation of order 2.");
    }
    
    a = (m_points[khi].x() - x)/h;
    b = (x - m_points[klo].x())/h;
    c = (a*a*a-a)*(h*h) / 6.0;
    d = (b*b*b-b)*(h*h) / 6.0;
    
    return a*m_y2[klo] + b*m_y2[khi];
}

/**
 * Update/replace a point in the point list with/by a new one. Does nothing
 * if the index is out of range w.r.t. the list of points.
 *
 * \param index The index, for which we want to replace the point.
 * \param point The new point, which will replace the old one
 */
void CubicSpline1D::updatePoint(unsigned int index, const PointType& point)
{
    if (index < (unsigned int)m_points.size())
    {
        m_points[index]=point;
        updateSecondDerivatives();
    }
}

/**
 * Returns the number of points in the point list.
 *
 * \return The number of points in the point list.
 */
unsigned int CubicSpline1D::size() const
{
    return m_points.size();
}

/**
 * Returns a constant reference access to the point list. This can be used for
 * read-access only!
 *
 * \return The const reference to the point list.
 */
const CubicSpline1D::PointListType & CubicSpline1D::points() const
{
    return m_points;
}

/**
 * Update/replace a point in the point list with/by a new one. Does nothing
 * if the index is out of range w.r.t. the list of points.
 *
 * \param index The index, for which we want to replace the point.
 * \param point The new point, which will replace the old one
 */
void CubicSpline1D::updateSecondDerivatives()
{
    int n = m_points.size();
    
    QVector<float> u(n);
    m_y2.resize(n);
    
    
    if(m_first_derivative > 0.99e30)
    {
        m_y2[0] = u[0] = 0.0;
    }
    else
    {
        m_y2[0] = -0.5;
        u[0]  =   3.0/(m_points[1].x()-m_points[0].x())
                * ((m_points[1].y()-m_points[0].y())
                  /(m_points[1].x()-m_points[0].x()) - m_first_derivative);
    }
    
    PointType pa, pb, pc;
    float p, sig;
    
    for(int i=1; i<n-1;i++)
    {
        PointType pa = m_points[i-1];
        PointType pb = m_points[i];
        PointType pc = m_points[i+1];
        
        sig   = (pb.x()-pa.x())/(pc.x()-pa.x());
        p     = sig*m_y2[i-1]+2.0;
        m_y2[i] = (sig-1.0)/p;
        u[i]  = (pc.y()-pb.y())/(pc.x()-pb.x()) - (pb.y()-pa.y())/(pb.x()-pa.x());
        u[i]  = (6.0*u[i]/(pc.x()-pa.x())-sig*u[i-1])/p;
    }
    
    if(m_last_derivative > 0.99e30)
    {
        m_y2[n-1]=u[n-1]=0.0;
    }
    else
    {
        m_y2[n-1] = 0.5;
        u[n-1] = (3.0/(m_points[n-1].x()-m_points[n-2].x()))
                *(m_last_derivative - (m_points[n-1].y()-m_points[n-2].y())
                  /(m_points[n-1].x()-m_points[n-2].x()));
    }
    m_y2[n-1] = (u[n-1]-m_y2[n-1]*u[n-2])/(m_y2[n-1]*m_y2[n-2]+1.0);
    
    for(int k=n-2; k>=0; k--)
    {
        m_y2[k] = m_y2[k]*m_y2[k+1]+u[k];
    }
}




/**
 * Default constructor. Constructs an empty 2D cubic spline.
 */
CubicSpline2D::CubicSpline2D()
{
}

/**
 * A more convenient constructor. Constructs a 2D cubic spline by means of
 * a set of points (x,y) and the first and last derivative at
 * the boundaries of this 2D spline.
 *
 * \param points The list of points (x,y) values.
 * \param first_derivative The starting gradient, given as (d_x, d_y).
 * \param last_derivative The finishing gradient, given as (d_x, d_y).
 */
CubicSpline2D::CubicSpline2D(const PointListType& points, const PointType & first_derivative, const PointType & last_derivative)
: m_points(points),
  m_first_derivative(first_derivative),
  m_last_derivative(last_derivative)
{
    updateFromPoints();
}
   
/**
 * Virtual destructor of this class, needed due to non-final QVector class.
 */
CubicSpline2D::~CubicSpline2D()
{
}
    
/**
 * Return the first derivative (just before the first point).
 *
 * \return The first order derivative of the 2D cubic spline just before the first point.
 */
const CubicSpline2D::PointType& CubicSpline2D::firstDerivative() const
{
    return m_first_derivative;
}

/**
 * Resets the first derivative (just before the first point).
 *
 * \param first_derivative The first order derivative of the 2D cubic spline just before the first point.
 */
void CubicSpline2D::setFirstDerivative(const PointType& first_derivative)
{
    m_first_derivative = first_derivative;
    updateFromPoints();
}

/**
 * Return the last derivative (just after the last point).
 *
 * \return The first order derivative of the 2D cubic spline just after the last point.
 */
const CubicSpline2D::PointType& CubicSpline2D::lastDerivative() const
{
    return m_last_derivative;
}

/**
 * Resets the last derivative (just after the last point).
 *
 * \param first_derivative The first order derivative of the 2D cubic spline just after the last point.
 */
void CubicSpline2D::setLastDerivative(const PointType& last_derivative)
{
    m_last_derivative = last_derivative;
    updateFromPoints();
    
}

/**
 * Interpolation: Return the resulting (interpolated) point.
 *
 * \param t The parametric value, for which we want to interpolate.
 *          A t of zero means the first point, a t of one the last point.
 * \return The point of the 2D cubic spline at that given parameter.
 */
CubicSpline2D::PointType CubicSpline2D::interpolate(float t) const
{
    return PointType(m_splineX.interpolate(t), m_splineY.interpolate(t));
}

/**
 * Interpolation: Return the first order derived (interpolated) point.
 *
 * \param t The parametric value, for which we want to interpolate.
 *          A t of zero means the first point, a t of one the last point.
 * \return The first order derivative (d_x, d_y) of 2D cubic spline at that given parameter.
 */
CubicSpline2D::PointType CubicSpline2D::derive(float t) const
{
    return PointType(m_splineX.derive(t), m_splineY.derive(t));
}

/**
 * Interpolation: Return the second order derived (interpolated) point.
 *
 * \param t The parametric value, for which we want to interpolate.
 *          A t of zero means the first point, a t of one the last point.
 * \return The second order derivative (d_xx, d_yy) of the 2D cubic spline at that given parameter.
 */
CubicSpline2D::PointType CubicSpline2D::derive2(float t) const
{
    return PointType(m_splineX.derive2(t), m_splineY.derive2(t));
}

/**
 * Returns the number of points in the point list.
 *
 * \return The number of points in the point list.
 */
unsigned int CubicSpline2D::size() const
{
	return m_splineX.size();
}

/**
 * Returns a constant reference access to the point list. This can be used for
 * read-access only!
 *
 * \return The const reference to the point list.
 */
const CubicSpline2D::PointListType & CubicSpline2D::points() const
{
    return m_points;
}

/**
 * Returns the point of a given index in the point list. Throws an error, if the
 * index is out of range w.r.t. the list of points.
 *
 * \param index The index, for which we want to replace the point.
 * \return The point, which is listed a the given index.
 */
CubicSpline2D::PointType CubicSpline2D::point(unsigned int index) const
{
    return PointType((m_splineX.points()[index]).x(), (m_splineY.points()[index]).y());
}

/**
 * Update/replace a point in the point list with/by a new one. Does nothing
 * if the index is out of range w.r.t. the list of points.
 *
 * \param index The index, for which we want to replace the point.
 * \param point The new point, which will replace the old one
 */
void CubicSpline2D::setPoint(unsigned int index, const PointType& new_p)
{
    if(index < (unsigned int)m_points.size())
    {
        PointType p(float(index)/(m_splineX.size()-1), new_p.x());
        m_splineX.updatePoint(index, p);
        
        p.setY(new_p.y());
        m_splineY.updatePoint(index, p);
        
        m_boundingRect.setLeft( std::min(qreal(new_p.x()), m_boundingRect.left()) );
        m_boundingRect.setTop( std::min(qreal(new_p.y()), m_boundingRect.top()) );
        
        m_boundingRect.setRight( std::max(qreal(new_p.x()), m_boundingRect.right()) );
        m_boundingRect.setBottom( std::max(qreal(new_p.y()), m_boundingRect.bottom()) );
    }
}

/**
 * Add a point to the end of the point list.
 *
 * \param x The ordinate of the new point.
 * \param y The coordinate ot the new point.
 */
void CubicSpline2D::addPoint(float x, float y)
{
	addPoint(PointType(x,y));
}

/**
 * Add a point to the end of the point list.
 *
 * \param p The new point.
 */
void CubicSpline2D::addPoint(const PointType& new_p)
{
	if (m_splineX.size()==0) //first point
	{
		m_boundingRect = QRectF(new_p.x(),new_p.y(),1,1);
	}
    
    m_points.push_back(new_p);
	
	m_boundingRect.setLeft( std::min(qreal(new_p.x()), m_boundingRect.left()) );
	m_boundingRect.setTop( std::min(qreal(new_p.y()), m_boundingRect.top()) );
	
	m_boundingRect.setRight( std::max(qreal(new_p.x()), m_boundingRect.right()) );
	m_boundingRect.setBottom( std::max(qreal(new_p.y()), m_boundingRect.bottom()) );
}

/**
 * Returns the bounding rectangle of this 2D cubic spline.
 *
 * \return The bounding rectangle of this 2D cubic spline.
 */
const QRectF& CubicSpline2D::boundingRect() const
{
	return m_boundingRect;
}

/**
 * Internal update function to update the cubic spline interpolations after
 * the update/addition/removal of points
 */
void CubicSpline2D::updateFromPoints()
{
    int n=m_points.size();
    
    PointListType points_x(n);
    PointListType points_y(n);
    
    //Fill x- and y-lists for parametric representation
    for (int i=0; i< n; ++i)
    {
        points_x[i].setX(float(i)/(n-1));
        points_x[i].setY(m_points[i].x());
        points_y[i].setX(float(i)/(n-1));
        points_y[i].setY(m_points[i].y());
    }
    
    //Compute second derivatives
    m_splineX = CubicSpline1D(points_x, m_first_derivative.x(), m_last_derivative.x());
    m_splineY = CubicSpline1D(points_y, m_first_derivative.x(), m_last_derivative.y());
}
    
} //End of namespace graipe
