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

#include "gui/qlistwidgetitems.hxx"

#include <QtDebug>

namespace graipe {

/**
 * Default constructor of the QListWidgetModelItem class.
 *
 * \param text  A QString determining the text of this item. Dafaults to "".
 * \param model A Model pointer to the model attached to this item.
 *              Note, that the ownership changes here - the model pointer
 *              will belong to the item after creation.
 */
QListWidgetModelItem::QListWidgetModelItem(QString text, Model* model)
:	QListWidgetItem(text),
    m_model(model)
{
}

/**
 * Destructor of the QListWidgetModelItem class.
 * Since the item owns the model pointer, it will be deleted if the model is
 * not locked. If the model is locked, it will be kept alive but a debug 
 * message will be printed out.
 */
QListWidgetModelItem::~QListWidgetModelItem()
{
    if(m_model!=NULL)
    {
        if(m_model->locked()==false)
        {
            delete m_model;
            m_model = NULL;
        }
        else
        {
            qCritical() << QString("Error: QListWidgetModelItem shall be deleted, but model is locked! Model %1 will be let alive.").arg(m_model->name().toStdString().c_str());
        }
    }
}

/**
 * Const accessor to the models pointer. You may change the model, but not 
 * pointer itself.
 *
 * \return the assigned Model pointer.
 */
Model* QListWidgetModelItem::model() const
{
    return m_model;
}

    
    
    

/**
 * Default constructor of the QListWidgetViewControllerItem class.
 *
 * \param text A QString determining the text of this item. Dafaults to "".
 * \param viewController   A ViewController pointer to the View attached to this item.
 *                         Note, that the ownership changes here - the ViewController
 *                         will belong to the item after creation.
 */
QListWidgetViewControllerItem::QListWidgetViewControllerItem(QString text, ViewController* viewController)
:	QListWidgetItem(text),
    m_viewController(viewController)
{
    if (m_viewController != NULL)
    {
        m_viewController->setName(text);
    }
}

/**
 * Destructor of the QListWidgetViewItem class.
 * Since the item owns the ViewController pointer, it will be deleted at destruction
 */
QListWidgetViewControllerItem::~QListWidgetViewControllerItem()
{
    if(m_viewController != NULL)
    {
        delete m_viewController;
        m_viewController = NULL;
    }
}

/**
 * Const accessor to the ViewController pointer. You may change the ViewController, but not
 * the pointer itself.
 *
 * \return the assigned ViewController pointer.
 */
ViewController* QListWidgetViewControllerItem::viewController() const
{
    return m_viewController;
}




/**
 * Default constructor of the QListWidgetAlgorithmItem class.
 *
 * \param text A QString determining the text of this item. Dafaults to "".
 * \param alg  A Algorithm pointer to the Algorithm attached to this item.
 *             Note, that the ownership does not changes here.
 */
QListWidgetAlgorithmItem::QListWidgetAlgorithmItem(QString text, Algorithm* alg )
:	QListWidgetItem(text),
    m_algorithm(alg)
{
}
/**
 * Const accessor to the Algorithm pointer. You may edit the Algorithm, but not
 * the pointer itself.
 *
 * \return the assigned Algorithm pointer.
 */
Algorithm* QListWidgetAlgorithmItem::algorithm() const
{
    return m_algorithm;
}
  
}//end of namespace graipe
