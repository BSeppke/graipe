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
 * @addtogroup graipe_gui
 * @{
 *     @file
 *     @brief Implementation file to the graipe gui list items.
 * @}
 */
 
QListWidgetModelItem::QListWidgetModelItem(QString text, Model* model)
:	QListWidgetItem(text),
    m_model(model)
{
}

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

Model* QListWidgetModelItem::model() const
{
    return m_model;
}










QListWidgetViewControllerItem::QListWidgetViewControllerItem(QString text, ViewController* viewController)
:	QListWidgetItem(text),
    m_viewController(viewController)
{
    if (m_viewController != NULL)
    {
        m_viewController->setName(text);
    }
}

QListWidgetViewControllerItem::~QListWidgetViewControllerItem()
{
    if(m_viewController != NULL)
    {
        delete m_viewController;
        m_viewController = NULL;
    }
}

ViewController* QListWidgetViewControllerItem::viewController() const
{
    return m_viewController;
}









QListWidgetAlgorithmItem::QListWidgetAlgorithmItem(QString text, Algorithm* alg )
:	QListWidgetItem(text),
    m_algorithm(alg)
{
}

Algorithm* QListWidgetAlgorithmItem::algorithm() const
{
    return m_algorithm;
}
  
}//end of namespace graipe
