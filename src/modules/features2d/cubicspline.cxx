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

#include "features2d/cubicspline.hxx"

namespace graipe {

/**
 * @addtogroup graipe_features2d
 * @{
 *     @file
 *     @brief Implementation file for cubic splines (1d and 2d)
 * @}
 */

CubicSpline1D::CubicSpline1D()
{
}

CubicSpline1D::CubicSpline1D(const PointListType & points, float first_derivative, float last_derivative)
:   m_points(points),
    m_first_derivative(first_derivative),
    m_last_derivative(last_derivative),
    m_y2(points.size())
{
    updateSecondDerivatives();
}

CubicSpline1D::~CubicSpline1D()
{
}

float CubicSpline1D::firstDerivative() const
{
    return m_first_derivative;
}

void CubicSpline1D::setFirstDerivative(float first_derivative)
{
    m_first_derivative = first_derivative;
    updateSecondDerivatives();
}

float CubicSpline1D::lastDerivative() const
{
    return m_last_derivative;
}

void CubicSpline1D::setLastDerivative(float last_derivative)
{
    m_last_derivative = last_derivative;
    updateSecondDerivatives();
}

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

float CubicSpline1D::derive(float x) const
{
    int n = m_points.size();
    
    int klo, khi, k;
    float a, b, h;//c, d;
    
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
    //c = (a*a*a-a)*(h*h) / 6.0;
    //d = (b*b*b-b)*(h*h) / 6.0;
    
    return  -1.0/h*m_points[klo].y() + 1.0/h*m_points[khi].y() + (-3.0*a*a+1.0)*h/6.0*m_y2[klo] + (3.0*b*b-1.0)*h/6.0*m_y2[khi];
    /*      da/dx                    db/dx                 '------ db/dx -----'                '------ db/dx -----'      */
}

float CubicSpline1D::derive2(float x) const
{
    int n = m_points.size();
    
    int klo, khi, k;
    float a, b, h;//c, d;
    
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
    //c = (a*a*a-a)*(h*h) / 6.0;
    //d = (b*b*b-b)*(h*h) / 6.0;
    
    return a*m_y2[klo] + b*m_y2[khi];
}

void CubicSpline1D::updatePoint(unsigned int index, const PointType& point)
{
    if (index < (unsigned int)m_points.size())
    {
        m_points[index]=point;
        updateSecondDerivatives();
    }
}

unsigned int CubicSpline1D::size() const
{
    return m_points.size();
}

const CubicSpline1D::PointListType & CubicSpline1D::points() const
{
    return m_points;
}

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










CubicSpline2D::CubicSpline2D()
{
}

CubicSpline2D::CubicSpline2D(const PointListType& points, const PointType & first_derivative, const PointType & last_derivative)
: m_points(points),
  m_first_derivative(first_derivative),
  m_last_derivative(last_derivative)
{
    updateFromPoints();
}

CubicSpline2D::~CubicSpline2D()
{
}

const CubicSpline2D::PointType& CubicSpline2D::firstDerivative() const
{
    return m_first_derivative;
}

void CubicSpline2D::setFirstDerivative(const PointType& first_derivative)
{
    m_first_derivative = first_derivative;
    updateFromPoints();
}

const CubicSpline2D::PointType& CubicSpline2D::lastDerivative() const
{
    return m_last_derivative;
}

void CubicSpline2D::setLastDerivative(const PointType& last_derivative)
{
    m_last_derivative = last_derivative;
    updateFromPoints();
    
}

CubicSpline2D::PointType CubicSpline2D::interpolate(float t) const
{
    return PointType(m_splineX.interpolate(t), m_splineY.interpolate(t));
}

CubicSpline2D::PointType CubicSpline2D::derive(float t) const
{
    return PointType(m_splineX.derive(t), m_splineY.derive(t));
}

CubicSpline2D::PointType CubicSpline2D::derive2(float t) const
{
    return PointType(m_splineX.derive2(t), m_splineY.derive2(t));
}

unsigned int CubicSpline2D::size() const
{
	return m_splineX.size();
}

const CubicSpline2D::PointListType & CubicSpline2D::points() const
{
    return m_points;
}

CubicSpline2D::PointType CubicSpline2D::point(unsigned int index) const
{
    return PointType((m_splineX.points()[index]).x(), (m_splineY.points()[index]).y());
}

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

void CubicSpline2D::addPoint(float x, float y)
{
	addPoint(PointType(x,y));
}

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

const QRectF& CubicSpline2D::boundingRect() const
{
	return m_boundingRect;
}

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
