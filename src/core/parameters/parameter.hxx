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

#ifndef GRAIPE_CORE_PARAMETERS_PARAMETER_HXX
#define GRAIPE_CORE_PARAMETERS_PARAMETER_HXX

#include "core/serializable.hxx"

#include <vector>

#include <QString>
#include <QWidget>
#include <QFormLayout>
#include <QSizePolicy>


namespace graipe {

//Forward declaration of Model class to resolve circular dependency
class Model;


/**
 * This is the base class of all Parameters.
 * Make sure to fullfil this base's api when impelementing own
 * (derived) Parameter classes.
 * To get an idea, how such specializations look like, you may refer
 * to the parameters already provided by GRAIPE
 */
class GRAIPE_CORE_EXPORT Parameter
:   public QObject,
    public Serializable
{
    Q_OBJECT
    
    public:
        /**
         * Default constructor of the Parameter class
         */
        Parameter();
    
        /**
         * More usable constructor of the Parameter class with a setting of the 
         * most important values directly.
         *
         * \param name          The name (label) of this parameter.
         * \param parent        If given (!= NULL), this parameter has a parent and will
         *                      be enabled/disabled, if the parent is a BoolParameter.
         * \param invert_parent If true, the enables/disabled dependency to the parent will be swapped.
         */
        Parameter(const QString& name, Parameter* parent=NULL, bool invert_parent=false);
    
        /**
         * (Of course virtual) Destructor of the Parameter class.
         */
        virtual ~Parameter();
    
        /**
         * The (immutable) type name of this parameter class.
         * Implemented to fullfil the Serializable interface.
         *
         * \return "Parameter".
         */
        QString typeName() const;
    
        /**
         * The name of this parameter. This name is used a label for the parameter.
         *
         * \return The name of the parameter.
         */
        virtual QString name() const;
    
        /**
         * Writer for the name/label of this parameter.
         *
         * \param name The new name of the parameter.
         */
        void setName(const QString& name);
    
        /**
         * Potentially non-const access to the parent of this parameter. 
         *
         * \return The pointer to the parent parameter.
         */
        virtual Parameter* parent();
    
        /**
         * Is the parent's checked state linked to setEnabled slot of this parameter's
         * widget delegate or is it inverted linked?
         *
         * \return True, if an inverted linkage is established.
         */
        virtual bool invertParent() const;
    
        /**
         * The value converted to a QString. Please note, that this can vary from the 
         * serialize() result, which also returns a QString. This is due to the fact,
         * that serialize also may perform encoding of QStrings to avoid special chars.
         *
         * \return The value of the parameter converted to an QString
         */
        virtual QString valueText() const;
    
        /**
         * The magicID of this parameter class. 
         * Implemented to fullfil the Serializable interface.
         *
         * \return The same as the typeName() function.
         */
        QString magicID() const;
    
        /**
         * Serialization of the parameter's state to an output device.
         * Just writes the magicID a.k.a. typeName() on the device.
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
         * Const access to the model list, which is currently assigned to 
         * this parameter.
         *
         * \return A pointer to the current model list.
         */
        const std::vector<Model*> * modelList() const;
        
        /**
         * Writing access to the model list, which may be used to assign a new
         * or update the currently used model list of this parameter.
         *
         * \param new_model_list A pointer to the new model list.
         */
        void setModelList(const std::vector<Model*> * new_model_list);
        
        /**
         * This method is called after each (re-)assignment of the model list
         * e.g. after a call of the setModelList() function. It may be implemented
         * by means of the subclasses to handle these updates.
         */
        virtual void refresh();
    
        /**
         * This function locks the parameters value. 
         * This means, that after a lock() call, only const acess to the parameter is 
         * possible until someone unlocks it. 
         * To work properly, the inner parameter class has to be designed accordingly.
         * As an example, you may look at the Model class, which supports locking and
         * unlocking - so do the parameter classes based on models!
         */
        virtual void lock();
    
        /**
         * This function unlocks the parameters value.
         * This means, that after a lock() call, only const acess to the parameter is 
         * possible until someone unlocks it. 
         * To work properly, the inner parameter class has to be designed accordingly.
         * As an example, you may look at the Model class, which supports locking and
         * unlocking - so do the parameter classes based on models!
         */
        virtual void unlock();
    
        /**
         * This function indicates whether the value of a parameter is valid or not.
         *
         * \return True, if the parameter's value is valid.
         */
        virtual bool isValid() const;
    
        /**
         * Sets a parameter to be hidden. 
         * Hidden parameters behave like visible parameters unless they are added to a
         * parameter group, where their delegates will not be shown.
         *
         * \param hide If true, the parameter will be hidden in a parameter group.
         */
        virtual void hide(bool hide);
    
        /**
         * Is a parameter marked as "hidden" with respect to a parameter group?
         *
         * \return True, if the parameter will be hidden in a parameter group.
         */
        virtual bool isHidden() const;
    
        /**
         * The delegate widget of this parameter. 
         * Each parameter generates such a widget on demand, which refers to the
         * first call of this function. This is needed due to the executability of
         * classes using parameters (like the Algorithm class) in different threads.
         *
         * \return The delegate widget to control the values of this parameter.
         */
        virtual QWidget * delegate();
        
    public slots:
        /**
         * This slot is called everytime, the delegate has changed. It may be used to synchronize
         * the internal value of the parameter with the current delegate's value. 
         * For the base class, it simply emits the valueChanged() signal.
         */
        void updateValue();

    signals:
        /**
         * This signal should be emitted by the Parameter everytime the value has changed.
         */
        void valueChanged();
        
    protected:
        /**
         * Initializes the connections (signal<->slot) between the parameter class and
         * the delegate widget. This will be done after the first call of the delegate()
         * function, since the delegate is NULL until then.
         */
        virtual void initConnections();
    
        /** The name/label of the parameter **/
        QString m_name;
    
        /** The parent of the parameter **/
        Parameter* m_parent;
    
        /** Should the enabled/disabled by parent rule be inverted? **/
        bool m_invert_parent;
    
        /** Shall the parameter be hidden inside a parameter group? **/
        bool m_hide;
    
        /** The list of all currently loaded models **/
        const std::vector<Model*> * m_modelList;
};

} //end of namespace graipe

#endif //GRAIPE_CORE_PARAMETERS_PARAMETER_HXX
