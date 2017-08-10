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

#include "core/algorithm.hxx"

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *     @file
 *     @brief Implementation file for the Algorithm class
 * @}
 */

Algorithm::Algorithm(Workspace* wsp)
:   m_parameters(new ParameterGroup),
    m_workspace(wsp)
{
}

Algorithm::~Algorithm() 
{
    delete m_parameters;
}

bool Algorithm::deserialize(QXmlStreamReader& xmlReader)
{
    if(     xmlReader.name() == typeName()
        &&  xmlReader.attributes().hasAttribute("ID"))
    {
        setID(xmlReader.attributes().value("ID").toString());
        return m_parameters->deserialize(xmlReader);
    }
    return false;
}

void Algorithm::serialize(QXmlStreamWriter& xmlWriter) const
{
    xmlWriter.setAutoFormatting(true);
    xmlWriter.setAutoFormattingIndent(4);
    
    bool fullFile = (xmlWriter.device()->pos() == 0);
    
    if (fullFile)
    {
        xmlWriter.writeStartDocument();
    }
        xmlWriter.writeStartElement(typeName());
        xmlWriter.writeAttribute("ID", id());
            m_parameters->serialize(xmlWriter);
        xmlWriter.writeEndElement();
        
    if (fullFile)
    {
        xmlWriter.writeEndDocument();
    }
}

ParameterGroup* Algorithm::parameters()
{
	return m_parameters;
}

bool Algorithm::parametersValid() const
{
    unsigned int i=0;
    
    for(auto item : *m_parameters)
    {
        //pointer to parameter not existent or parameter not valid?
		if ( item.second == NULL)
        {
            qDebug() << "ERR for Alg: Null pointer for parameter " << i << "\n";
            return false;
        }
        if( !item.second->isValid() )
		{
            qDebug() << "ERR for Alg: parameter " << i << " Name: " << item.second->name() << " is not valid!\n";
        	return false;
        }
        i++;
    }
	return true;
}

void Algorithm::run()
{
	//Tell the caller about status updates
	emit statusMessage(0.0,   QString("Beginning processing"));
	
	if (!parametersValid())
	{
        //Parameters set incorrectly
		emit errorMessage(QString("Some parameters are not available"));
     }
     else
     {
        //Parameters set correctly set
		emit statusMessage(1.0,   QString("Parameters are all available"));
		
		//Calculate Result
		emit statusMessage(100.0, QString("Finished processing"));
	
		//Tell the upper instance (the separately started thread) to quit
		emit finished();
	}
}

void Algorithm::lockModels()
{
    for(auto item : *m_parameters)
	{
        item.second->lock();
    }
}

void Algorithm::unlockModels()
{
    for(auto item : *m_parameters)
    {
        item.second->unlock();
    }
}

void Algorithm::status_update(float percent)
{
	//restrict to 99.9% because otherwise the processing of the algorithm 
	//could be interuppted unwanted
	float p_overall = 100.0*m_phase/std::max(m_phase_count,(unsigned int)1);
	p_overall += percent/std::max(m_phase_count,(unsigned int)1);
	
	emit statusMessage(std::min(p_overall, 99.9f), QString("processing"));	
}

std::vector<Model *>  Algorithm::results()
{
	return m_results;
}

}//end of namespace graipe
