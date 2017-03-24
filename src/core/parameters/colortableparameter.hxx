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

#ifndef GRAIPE_CORE_PARAMETERS_COLORTABLEPARAMETER_HXX
#define GRAIPE_CORE_PARAMETERS_COLORTABLEPARAMETER_HXX

#include "core/colortables.hxx"
#include "core/parameters/parameter.hxx"
#include "core/parameters/parametergroup.hxx"

#include <QComboBox>

namespace graipe {

class ParameterSelection;

/**
 * This is the ColorTableParameter class.
 * It inherits from the Parameter base class to:
 * - hold a QColor value, and
 * - provide editing facilities by means of a QPushButton 
 *   (with a color pixmap as label).
 */
class GRAIPE_CORE_EXPORT ColorTableParameter
:   public Parameter
{    
    Q_OBJECT
    
    public:
        /**
         * Default constructor of the ColorTableParameter class with a setting of the
         * most important values directly.
         *
         * \param name          The name (label) of this parameter.
         * \param value         The initial value of this parameter.
         * \param parent        If given (!= NULL), this parameter has a parent and will
         *                      be enabled/disabled, if the parent is a BoolParameter.
         * \param invert_parent If true, the enables/disabled dependency to the parent will be swapped.
         */
        ColorTableParameter(const QString& name, QVector<QRgb> value=(colorTables()[0]), Parameter* parent=NULL, bool invert_parent=false);
    
        /**
         * The destructor of the ColorTableParameter class.
         */
        ~ColorTableParameter();
    
        /**
         * The (immutable) type name of this parameter class.
         *
         * \return "ColorTableParameter".
         */
        virtual QString typeName() const;
        
        /** 
         * The current value of this parameter in the correct, most special type.
         *
         * \return The value of this parameter.
         */
        QVector<QRgb> value() const;
    
        /**
         * Gives information if a colortable is already knwon or not,
         * either in the system-wide colorTables or in the extraTables
         *
         * \param ct The color table to be checked for.
         * \return Positive index, if the color table is either in colorTables() or in m_extra_tables.
         *         The index corresponds to the comboBox index. Else, -1
         */
        int colorTableIndex(const QVector<QRgb> & ct) const;
    
        /**
         * Writing accessor of the current value of this parameter.
         *
         * \param value The new value of this parameter.
         */
        void setValue(const QVector<QRgb>& value);
    
        /**
         * Add another (user defined) color table to this parameter.
         *
         * \param ct The new user defined ct of this parameter.
         * \return The new ComboBox id of the inserted or found ct. 
         */
        int addCustomColorTable(const QVector<QRgb>& ct);
            
        /**
         * The value converted to a QString. Please note, that this can vary from the 
         * serialize() result, which also returns a QString. This is due to the fact,
         * that serialize also may perform encoding of QStrings to avoid special chars
         * inside the QString.
         *
         * \return The value of the parameter converted to an QString.
         */
        QString valueText() const;
            
        /**
         * Serialization of the parameter's state to an output device.
         * Basically, just: "ColorTableParameter, " + valueText()
         *
         * \param out The output device on which we serialize the parameter's state.
         */
        void serialize(QIODevice& out) const;
    
        /**
         * Deserialization of a parameter's state from an input device.
         *
         * \param in the input device.
         * \return True, if the deserialization was successful, else false.
         */
        bool deserialize(QIODevice& in);

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
        /** The storage for the value of this parameter (index of ct) **/
        int m_ct_idx;
    
        /** The delegate **/
        QPointer<QComboBox> m_delegate;
    
        /** The starage of extra color tables **/
        QVector<QVector<QRgb> > m_extra_tables;
};

} //end of namespace graipe

#endif //GRAIPE_CORE_PARAMETERS_COLORPARAMETER_HXX
