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

#ifndef GRAIPE_FEATURES2D_CUBICSPLINELIST_HXX
#define GRAIPE_FEATURES2D_CUBICSPLINELIST_HXX

#include "core/model.hxx"
#include "features2d/cubicspline.hxx"
#include "features2d/config.hxx"

#include <QVector>

namespace graipe {

/**
 * This class represents a list of 2D cubic splines. It conformes to a
 * graipe::Model and may thus be widely used.
 */
class GRAIPE_FEATURES2D_EXPORT CubicSplineList2D 
:	public Model
{
	
	public:
		//Internally used type
		typedef CubicSpline2D CubicSplineType;
    
        /**
         * Default constructor. Constructs an empty list of 2D cubic splines.
         */
		CubicSplineList2D();
    
        /**
         * Copy constructor. Creates a copy from another list of 2D cubic splines.
         *
         * \param spline_list The other CubicSplineList2D.
         */
		CubicSplineList2D(const CubicSplineList2D& spline_list);
		
        /**
         * Returns the typeName of a 2d cubic spline list.
         *
         * \return Always "CubicSplineList2D".
         */
		QString typeName() const;
		
        /**
         * Returns the number of 2D cubic splines in this list.
         *
         * \return The number of 2D cubic splines in this list.
         */
		virtual unsigned int size() const;
    
        /**
         * Completely erases this list of 2D cubic splines. Does nothing if the list is locked.
         */
		virtual void clear();
		
		/*
         * Constant access a 2D cubic spline inside this list at a given index. May throw
         * an error, if the index is out of range.
         *
         * \param index The index of the spline in the list.
         */
		virtual const CubicSplineType & spline(unsigned int index) const;
    
		/*
         * Reset/replace a 2D cubic spline inside this list at a given index. If the index is
         * out of range or the model is locked, this function will do nothing.
         *
         * \param index The index of the replaced spline in the list.
         * \param new_spline The replacement spline.
         */
		virtual void setSpline(unsigned int index, const CubicSplineType& new_spline);
    
        /*
         * Add a 2D cubic spline at the end of this list. 
         * If the model is locked, this function will do nothing.
         *
         * \param spl The replacement spline.
         */
		virtual void addSpline(const CubicSplineType& spl);
	
		/**
         * Return the item header for this list of 2D cubic splines.
         *
         * \return Always "dp0/dx, dp0/dy, p0_x, p0_y, p1_x, p1_y, ... , pN_x, pN_y, dpN/dx, dpN/dy".
         */
		virtual QString item_header() const;
    
        /**
         * Serialization of one 2D cubic spline at a given list index to a string. This function will
         * throw an error if the index is out of range.
         *
         * \param index The index of the 2D cubic spline to be serialized.
         * \return A QString containing the searialization of the 2D cubic spline.
         */
        virtual QString serialize_item(unsigned int index) const;
    
        /**
         * Deserialization/addition of a 2D cubic spline from a string to this list.
         *
         * \param serial A QString containing the searialization of the 2D cubic spline.
         * \return True, if the item could be deserialized and the model is not locked.
         *         The serialization should be given as: dp0/dx, dp0/dy, p0_x, p0_y, ... , pN_x, pN_y, dpN/dx, dpN/dy
         */
        virtual bool deserialize_item(const QString & serial);
    
        /**
         * Serialization the list of 2D cubic splines to a QIODevice.
         * The first line is the header as given in item_header(). Each following
         * line represents one 2D cubic spline serialization.
         *
         * \param out The QIODevice, where we will put our output on.
         */
		void serialize_content(QXmlStreamWriter& xmlWriter) const;
    
        /**
         * Deserializion of a list of 2D cubic splines from a QIODevice.
         * The first line is the header as given in item_header(), which is ignored however.
         * Each following line has to be one valide 2D cubic spline serialization.
         *
         * \param in The QIODevice, where we will read from.
         */
		bool deserialize_content(QXmlStreamReader& xmlReader);
	
    protected:
        //The list of 2D cubic splines
        QVector<CubicSplineType> m_splines;
};





/**
 * This class extends the list of 2D cubic splines. 
 * For each 2D cubic spline, it keeps an additional weight, which may indicate
 * a fitting error or anything else.
 */
class GRAIPE_FEATURES2D_EXPORT WeightedCubicSplineList2D 
:	public CubicSplineList2D
{
    public:
        /**
         * Default constructor. Constructs an empty list of weighted 2D cubic splines.
         */
        WeightedCubicSplineList2D();
    
        /**
         * Copy constructor. Creates a copy from another list of weighted 2D cubic splines.
         *
         * \param spline_list The other WeightedCubicSplineList2D.
         */
        WeightedCubicSplineList2D(const WeightedCubicSplineList2D& spline_list);
        
        /**
         * Returns the typeName of a weighted 2D cubic spline list.
         *
         * \return Always "WeightedCubicSplineList2D".
         */
        QString typeName() const;
    
        /**
         * Getter of the weight of a 2D cubic spline at a given index. May throw an error,
         * if the index is out of bounds.
         * 
         * \param index The index, for qhich we query the weight.
         * \return The weight at the given index.
         */
        float weight(unsigned int index) const;
    
        /**
         * Setter of the weight of a 2D cubic spline at a given index. If the index is
         * out of range or the model is locked, this function will do nothing.
         *
         * \param index The index of the replaced weight in the list.
         * \param new_w The replacement weight.
         */
        void setWeight(unsigned int index, float new_w);
    
        /*
         * Reset/replace a 2D cubic spline inside this list at a given index. 
         * The resetted spline will get a weight of zero. If the index is
         * out of range or the model is locked, this function will do nothing.
         *
         * \param index The index of the replaced spline in the list.
         * \param new_spline The replacement spline.
         */
        void setSpline(unsigned int index, const CubicSplineType& new_spline);
    
        /*
         * Reset/replace a 2D cubic spline inside this list at a given index and a given weight.
         * If the index is out of range or the model is locked, this function will do nothing.
         *
         * \param index The index of the replaced spline in the list.
         * \param new_spline The replacement spline.
         * \param new_w The weight of the replacement spline.
         */
        virtual void setSpline(unsigned int index, const CubicSplineType& new_spline, float new_w);
        
        /*
         * Add a 2D cubic spline at the end of this list. 
         * The new spline will have a weight of zero.
         * If the model is locked, this function will do nothing.
         *
         * \param new_spline The replacement spline.
         */
        void addSpline(const CubicSplineType& new_spline);
    
        /*
         * Add a 2D cubic spline with a given weight at the end of this list.
         * If the model is locked, this function will do nothing.
         *
         * \param index The index of the replaced spline in the list.
         * \param new_spline The replacement spline.
         * \param new_w The weight of the replacement spline.
         */
        virtual void addSpline(const CubicSplineType& new_spline, float new_w);
        
		/**
         * Return the item header for this list of 2D cubic splines.
         *
         * \return Always "weight, dp0/dx, dp0/dy, p0_x, p0_y, p1_x, p1_y, ... , pN_x, pN_y, dpN/dx, dpN/dy".
         */
		virtual QString item_header() const;
    
        /**
         * Serialization of one 2D cubic spline at a given list index to a string. This function will
         * throw an error if the index is out of range.
         *
         * \param index The index of the 2D cubic spline to be serialized.
         * \return A QString containing the searialization of the 2D cubic spline.
         *         The serialization should be given as: weight, dp0/dx, dp0/dy, p0_x, p0_y, ... , pN_x, pN_y, dpN/dx, dpN/dy
         */
        virtual QString serialize_item(unsigned int index) const;
    
        /**
         * Deserialization/addition of a 2D cubic spline from a string to this list.
         *
         * \param serial A QString containing the searialization of the 2D cubic spline.
         * \return True, if the item could be deserialized and the model is not locked.
         */
        virtual bool deserialize_item(const QString & serial);
        
    protected:
        //The list of weights:
        QVector<float> m_weights;
};
    
} //end of namespace graipe

#endif //GRAIPE_FEATURES2D_CUBICSPLINELIST_HXX
