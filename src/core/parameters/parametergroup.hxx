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

#ifndef GRAIPE_CORE_PARAMETERS_PARAMETERGROUP_HXX
#define GRAIPE_CORE_PARAMETERS_PARAMETERGROUP_HXX

#include "core/parameters/parameter.hxx"

#include <QPointer>
#include <QFormLayout>

/**
 * @file
 * @brief Header file for the ParameterGroup class
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * This is the ParameterGroup class, which inherits from the
 * Parameter base class to act like a parameter, but provide a
 * container for different parameters. As datastructure of the collection,
 * an std::map is used. This makes it very easy to access the
 * collected parameters by their ids/keys, which need to be provided on insert.
 *
 * ATTENTION: When adding a parameter to a group it ownership changes to this group!
 *            Thus, on deletion of the group, the item will be deleted, too!
 *
 * Further, since every single parameter is (de-)serializable, we also provide one
 * common (de-)serialization mechanism for a complete ParameterGroup, which
 * mainly reads/writes one Parameter "id: serial" per line.
 */
class GRAIPE_CORE_EXPORT ParameterGroup
:   public Parameter
{
    Q_OBJECT
    
    public:
    
        //public typedefs for convenience
        typedef std::map<QString, Parameter*> storage_type;
        typedef std::pair<QString,Parameter*> item_type;
        
        /**
         * Default constructor of the ParameterGroup class with a setting of the
         * most important values directly.
         *
         * \param name          The name (label) of this parameter group.
         * \param items         A std::map from QString to Parameter pointers containing all parameters.
         * \param policy        Layout settings for the used QFormLayout.
         * \param parent        If given (!= NULL), this parameter has a parent and will
         *                      be enabled/disabled, if the parent is a BoolParameter.
         * \param invert_parent If true, the enables/disabled dependency to the parent will be swapped.
         */
        ParameterGroup(const QString & name="", storage_type items=storage_type(), QFormLayout::RowWrapPolicy policy = QFormLayout::WrapLongRows, Parameter * parent = NULL, bool invert_parent=false);
    
        /**
         * Destructor of the ParameterGroup class. 
         * On destruction, every parameter of the group will be deleted, too.
         * as we assume to have the ownership of the added parameter pointers.
         */
        ~ParameterGroup();
    
        /**
         * The (immutable) type name of this parameter class.
         *
         * \return "ParameterGroup".
         */
        QString typeName() const
        {
            return "ParameterGroup";
        }
    
        /**
         * Add an already existing parameter to the ParameterGroup.
         *
         * \param id     The key for this parameter for easy access inside the group
         * \param param  The pointer to the existing parameter.
         * \param hidden If true, the parameter will be displayed by the group.
         * \return The index of the inserted parameter (size()-1)
         */
        unsigned int addParameter(const QString& id, Parameter* param, bool hidden=false);
    
        /**
         * Accessor to a parameter specified by the (QString) id.
         *
         * \param id The id of the parameter.
         * \return The pointer to the parameter inside the group or NULL, if not found.
         */
        Parameter* operator[](const QString& id);
    
        /**
         * Const accessor to a parameter specified by the (QString) id.
         *
         * \param id The id of the parameter.
         * \return The const pointer to the parameter inside the group or NULL, if not found.
         */
        Parameter const * operator[](const QString& id) const;
    
        /**
         * Const iterator pointing to the beginning of the QString -> parameter pointer
         * map storage. This may be used to support const for-looping in stl-style.
         *
         * \return The const iterator to the beginning of the QString -> parameter pointer map.
         */
        storage_type::const_iterator begin() const;
        
        /**
         * Const iterator pointing after the end of the QString -> parameter pointer
         * map storage. This may be used to support const for-looping in stl-style.
         *
         * \return The const iterator after the end of the QString -> parameter pointer map.
         */
         storage_type::const_iterator end() const;
        
        /**
         * Iterator pointing to the beginning of the QString -> parameter pointer
         * map storage. This may be used to support for-looping in stl-style.
         *
         * \return The iterator to the beginning of the QString -> parameter pointer map.
         */
        storage_type::iterator begin();
        
        /**
         * Iterator pointing after the end of the QString -> parameter pointer
         * map storage. This may be used to support for-looping in stl-style.
         *
         * \return The iterator after the end of the QString -> parameter pointer map.
         */
        storage_type::iterator end();
    
        /**
         * This function gives the parameter count of all existing parameters.
         *
         * \return The size of the QString -> parameter pointer map.
         */
        unsigned int size() const;
    
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
         * The value converted to a QString. Please note, that this can vary from the 
         * serialize() result, which also returns a QString. This is due to the fact,
         * that serialize also may perform encoding of QStrings to avoid special chars
         * inside the QString.
         *
         * \param filter_types Only special parameters are given out - filtered by their type. 
         * \return The value of the parameter converted to an QString.
         */
        QString valueText(const QString & filter_types) const;    

        /**
         * Serialization of the parameter groups's state to a xml stream.
         * Writes the following XML code by default:
         * 
         * <ParameterGroup>
         *     <Name>NAME</Name>
         *     <Parameters>N</Parameters>
         *     <Parameter ID="ID_PARAM_0">
         *         PARAM_0_SERIALIZATION
         *     </Parameter>
         *     ...
         *     <Parameter ID="ID_PARAM_N-1">
         *         PARAM_N-1_SERIALIZATION
         *      </Parameter>
         * </ParameterGroup>
         *
         * with                NAME = name(), and
         *               ID_PARAM_0 = m_parameters.front()->first.
         *    PARAM_0_SERIALIZATION = m_parameters.front()->second->serialize().
         *
         * \param xmlWriter The QXMLStreamWriter, which we use serialize the 
         *                  parameter's type, name and value.
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
         * The delegate widget of this parameter. 
         * Each parameter generates such a widget on demand, which refers to the
         * first call of this function. This is needed due to the executability of
         * classes using parameters (like the Algorithm class) in different threads.
         *
         * \return The delegate widget to control the values of this parameter.
         */
        QWidget * delegate();
        
    protected:
        /** The QString -> parameter pointer map **/
        storage_type m_parameters;
    
        /** The order of the parameters, since maps are stricly ordered. **/
        QStringList m_parameter_order;
    
        /** The delegate widget **/
        QPointer<QWidget> m_delegate;
    
        /** The layout of the widget **/
        QFormLayout * m_layout;
    
        /** The RowWrapPolicy (WrapLongRows by default) **/
        QFormLayout::RowWrapPolicy m_policy;
};

} //end of namespace graipe

/**
 * @}
 */

#endif //GRAIPE_CORE_PARAMETERS_PARAMETERGROUP_HXX
