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

#ifndef GRAIPE_CORE_PARAMETERS_POINTPARAMETER_HXX
#define GRAIPE_CORE_PARAMETERS_POINTPARAMETER_HXX

#include "core/parameters/parameter.hxx"

#include <QPointer>
#include <QSpinBox>

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *
 * @file
 * @brief Header file for the PointParameter class
 */

/**
 * This is the PointParameter class.
 * It inherits from the Parameter base class to:
 * - hold a QPoint value, and
 * - provide editing facilities by means of two QSpinBoxes.
 */
class GRAIPE_CORE_EXPORT PointParameter
:   public Parameter
{
    Q_OBJECT
    
    public:
        /**
         * Default constructor of the PointParameter class with a setting of the
         * most important values directly.
         *
         * \param name          The name (label) of this parameter.
         * \param low           The lowest allowed value of this parameter.
         * \param upp           The highest allowed value of this parameter.
         * \param value         The initial value of this parameter.
         * \param parent        If given (!= NULL), this parameter has a parent and will
         *                      be enabled/disabled, if the parent is a BoolParameter.
         * \param invert_parent If true, the enables/disabled dependency to the parent will be swapped.
         */
        PointParameter(const QString&  name, QPoint low, QPoint upp, QPoint value=QPoint(), Parameter* parent=NULL, bool invert_parent=false);
    
        /**
         * Destructor of the PointParameter class.
         */
        ~PointParameter();
    
        /**
         * The (immutable) type name of this parameter class.
         *
         * \return "PointParameter".
         */
        QString typeName() const
        {
            return "PointParameter";
        }
    
        /**
         * The lowest possible value of this parameter.
         *
         * \return The minimal value of this parameter.
         */
        QPoint lowerBound() const;
    
        /**
         * Writing accessor of the minimum value of this parameter.
         *
         * \param value The new minimum value of this parameter.
         */
        void setLowerBound(const QPoint& value);
    
        /**
         * The highest possible value of this parameter.
         *
         * \return The maximal value of this parameter.
         */
        QPoint upperBound() const;
    
        /**
         * Writing accessor of the maximum value of this parameter.
         *
         * \param value The new maximum value of this parameter.
         */
        void setUpperBound(const QPoint& value);
    
        /**
         * Writing accessor of the minimum and maximum value of this parameter.
         *
         * \param min_value The new minimum value of this parameter.
         * \param max_value The new maximum value of this parameter.
         */
        void setRange(const QPoint& min_value, const QPoint& max_value);
    
        /**
         * The current value of this parameter in the correct, most special type.
         *
         * \return The value of this parameter.
         */
        QPoint value() const;
    
        /**
         * Writing accessor of the current value of this parameter.
         *
         * \param value The new value of this parameter.
         */
        void setValue(const QPoint& value);
            
        /**
         * The value converted to a QString. Please note, that this can vary from the 
         * serialize() result, which also returns a QString. This is due to the fact,
         * that serialize also may perform encoding of QStrings to avoid special chars
         * inside the QString.
         *
         * \return The value of the parameter converted to an QString.
         */
        QString toString() const;

        /**
         * Serialization of the parameter's state to an output device.
         * Writes the following XML on the device:
         * 
         * \verbatim
           <TYPENAME>
               <Name>NAME</Name>
               <x>X</x>
               <y>Y</y>
           </TYPENAME>
           \endverbatim
         *
         * with TYPENAME = typeName(),
         *         NAME = name(),
         *            X = value().x(), and
         *            Y = value().y().
         *
         * \param xmlWriter The QXmlStreamWriter on which we serialize the parameter's state.
         */
        void serialize(QXmlStreamWriter& xmlWriter) const;
    
        /**
         * Deserialization of a parameter's state from an xml file.
         *
         * \param xmlReader The QXmlStreamReader, where we read from.
         * \return True, if the deserialization was successful, else false.
         */
        bool deserialize(QXmlStreamReader& xmlReader);
    
        /**
         * This function indicates whether the value of a parameter is valid or not.
         *
         * \return True, if the parameter's value is valid.
         */
        bool isValid() const;
        
        /**
         * The delegate widget of this parameter. 
         * Each parameter generates such a widget on demand, which refers to the
         * first call of this function. This is needed due to the executability of
         * classes using parameters (like the Algorithm class) in different threads.
         *
         * \return The delegate widget to control the values of this parameter.
         */
        QWidget * delegate();
        
    public slots:    
        /**
         * This slot is called everytime, the delegate has changed. It has to synchronize
         * the internal value of the parameter with the current delegate's value
         */
        void updateValue();
        
    protected:
        /** The value of this parameter **/
        QPoint m_value;
    
        /** The minimum value for this parameter **/
        QPoint m_min_value;        
        /** The maximum value for this parameter **/
        QPoint m_max_value;
    
        /** The parent delegate widget **/
        QPointer<QWidget> m_delegate;
    
        /** The child widget for x-control **/
        QPointer<QSpinBox> m_spbXDelegate;
        /** The child widget for y-control **/
        QPointer<QSpinBox> m_spbYDelegate;
};

/**
 * @}
 */

} //end of namespace graipe

#endif //GRAIPE_CORE_PARAMETERS_POINTPARAMETER_HXX
