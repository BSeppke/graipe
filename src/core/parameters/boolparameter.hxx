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

#ifndef GRAIPE_CORE_PARAMETERS_BOOLPARAMETER_HXX
#define GRAIPE_CORE_PARAMETERS_BOOLPARAMETER_HXX

#include "core/parameters/parameter.hxx"

#include <QPointer>
#include <QCheckBox>

/**
 * @file
 * @brief Header file for the BoolParameter class
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * This is the BoolParameter class.
 * It inherits from the Parameter base class to:
 * - hold a bool value, and
 * - provide editing facilities by means of a QCheckBox.
 */
class GRAIPE_CORE_EXPORT BoolParameter
:   public Parameter
{
    Q_OBJECT
    
    public:
        /**
         * Default constructor of the BoolParameter class with a setting of the
         * most important values directly.
         *
         * \param name          The name (label) of this parameter.
         * \param value         The initial value of this parameter.
         * \param parent        If given (!= NULL), this parameter has a parent and will
         *                      be enabled/disabled, if the parent is a BoolParameter.
         * \param invert_parent If true, the enables/disabled dependency to the parent will be swapped.
         */
        BoolParameter(const QString& name, bool value=false, Parameter* parent=NULL, bool invert_parent=false);
    
        /**
         * The destructor of the BoolParameter class.
         */
        ~BoolParameter();
    
        /**
         * The (immutable) type name of this parameter class.
         *
         * \return "BoolParameter".
         */
        static QString typeName();
    
        /** 
         * The current value of this parameter in the correct, most special type
         *
         * \return The value of this parameter.
         */
        bool value() const;
    
        /**
         * Writing accessor of the current value of this parameter.
         *
         * \param value The new value of this parameter.
         */
        void setValue(bool value);
            
        /**
         * The value converted to a QString. Either true or false.
         *
         * \return The value of the parameter converted to an QString
         */
        QString toString() const;
    
        /**
         * Deserialization of a parameter's state from a string.
         *
         * \param str The input QString.
         * \return True, if the deserialization was successful, else false.
         */
        bool fromString(QString& str);
    
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
        /** The storage for the value of this parameter **/
        bool m_value;
        
        /** The delegate widget for booleans **/
        QPointer<QCheckBox> m_delegate;
};

} //end of namespace graipe

/**
 * @}
 */

#endif //GRAIPE_CORE_PARAMETERS_BOOLPARAMETER_HXX
