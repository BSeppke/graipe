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

#ifndef GRAIPE_VECTORFIELDS_VECTORS_HXX
#define GRAIPE_VECTORFIELDS_VECTORS_HXX

#include "core/core.h"
#include <vector>

namespace graipe {

class Vector2D
    : public Serializable
{
public:
    typedef QPointFX PointType;
    
    QString typeName() const
    {
        return "Vector2D";
    }
    
    void serialize(QXmlStreamWriter& xmlWriter) const;
    bool deserialize(QXmlStreamReader& xmlReader);
    
    PointType origin;
    PointType direction;
};

class WeightedVector2D
    : public Vector2D
{
public:
    QString typeName() const
    {
        return "WeightedVector2D";
    }
    
    void serialize(QXmlStreamWriter& xmlWriter) const;
    bool deserialize(QXmlStreamReader& xmlReader);
    
    float weight;
};

class MultiVector2D
    : public Vector2D
{
public:
    QString typeName() const
    {
        return "MultiVector2D";
    }
    
    void serialize(QXmlStreamWriter& xmlWriter) const;
    bool deserialize(QXmlStreamReader& xmlReader);
    
    std::vector<Vector2D::PointType> altDirections;
};

class WeightedMultiVector2D
    : public MultiVector2D
{
public:
    QString typeName() const
    {
        return "WeightedMultiVector2D";
    }
    
    void serialize(QXmlStreamWriter& xmlWriter) const;
    bool deserialize(QXmlStreamReader& xmlReader);
    
    float weight;
    std::vector<float> altWeights;
};

} //end of namespace graipe

#endif
