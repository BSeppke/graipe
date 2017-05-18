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

#ifndef GRAIPE_CORE_PARAMETERS_COLORPARAMETER_HXX
#define GRAIPE_CORE_PARAMETERS_COLORPARAMETER_HXX

#include "core/parameters/parameter.hxx"

#include <QPointer>
#include <QPushButton>
#include <QColorDialog>

/**
 * @file
 * @brief Header file for the ColorParameter class
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * This is the ColorParameter class.
 * It inherits from the Parameter base class to:
 * - hold a QColor value, and
 * - provide editing facilities by means of a QPushButton 
 *   (with a color pixmap as label).
 */
class GRAIPE_CORE_EXPORT ColorParameter
:   public Parameter
{    
    Q_OBJECT
    
    public:
        /**
         * Default constructor of the ColorParameter class with a setting of the
         * most important values directly.
         *
         * \param name          The name (label) of this parameter.
         * \param value         The initial value of this parameter.
         * \param parent        If given (!= NULL), this parameter has a parent and will
         *                      be enabled/disabled, if the parent is a BoolParameter.
         * \param invert_parent If true, the enables/disabled dependency to the parent will be swapped.
         */
        ColorParameter(const QString& name, QColor value=QColor(), Parameter* parent=NULL, bool invert_parent=false);
    
        /**
         * The destructor of the ColorParameter class.
         */
        ~ColorParameter();
    
        /**
         * The (immutable) type name of this parameter class.
         *
         * \return "ColorParameter".
         */
        virtual QString typeName() const;
        
        /** 
         * The current value of this parameter in the correct, most special type.
         *
         * \return The value of this parameter.
         */
        const QColor& value() const;
    
        /**
         * Writing accessor of the current value of this parameter.
         *
         * \param value The new value of this parameter.
         */
        void setValue(const QColor& value);
            
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
         * Basically, just: "ColorParameter, " + value().rgba()
         *
         * \param out The output device on which we serialize the parameter's state.
         */
        void serialize(QXmlStreamWriter& xmlWriter) const;
    
        /**
         * Deserialization of a parameter's state from an input device.
         *
         * \param in the input device.
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
         * This slot is called on every button click on the color button. It mainly 
         * presents the color selection dialog. Then the user can select between
         * the different colors.
         */
        void updateValue();
        
    protected:    
        /** The storage for the value of this parameter **/
        QColor m_value;
        
        /** The delegate **/
        QPointer<QPushButton> m_delegate;
    
};

} //end of namespace graipe

/**
 * @}
 */

#endif //GRAIPE_CORE_PARAMETERS_COLORPARAMETER_HXX
