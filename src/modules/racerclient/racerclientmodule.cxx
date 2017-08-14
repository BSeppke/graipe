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

#include "images/images.h"
#include "vectorfields/vectorfields.h"
#include "features2d/features2d.h"
#include "core/core.h"

#include "racerclient/racerconnection.hxx"

#include <QFile>

namespace graipe {

/**
 * @addtogroup graipe_racerclient
 * @{
 *
 * @file
 * @brief Implementation file for the racer client module
 *
 * The racer client module, which contains the connection and reasoning using Racer.
 */
 
/**
 * Small helper funtion to computer the great circle distance between two points
 * on a standard earth sphere of radium 6371 km.
 *
 * \param p1deg First position in lat, lon (degrees).
 * \param p2deg Second position in lat, lon (degrees).
 * \return distance between both points in km.
 */
inline float distanceOnEarth(const QPointF & p1deg, const QPointF & p2deg)
{
	double r = 6371; // km
	
	double degToRad = M_PI/180.0;
	
	double lon1rad = double(p1deg.x())*degToRad, lon2rad = double(p2deg.x())*degToRad;
	double lat1rad = double(p1deg.y())*degToRad, lat2rad = double(p2deg.y())*degToRad;
	
	return acos(sin(lat1rad)*sin(lat2rad) + cos(lat1rad)*cos(lat2rad) * cos(lon2rad-lon1rad)) * r;
}

/**
 * This class implements a classical reasoning case for the analysis of derived sea surface currents
 * (by means of a vectorfield), other sources of knowledge and a common reasoning base
 * (by means of a T-Box). To answer the validity question of the measurement, the connection to a
 * Racer DL system is used.
 */
class RacerInterpreter
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        RacerInterpreter(Workspace* wsp)
        : Algorithm(wsp)
        { 
            m_param_measured_vectorfield = new ModelParameter("Derived Current Vectorfield", "SparseVectorfield2D|SparseMultiVectorfield2D|SparseWeightedVectorfield2D|SparseWeightedMultiVectorfield2D|DenseVectorfield2D|DenseWeightedVectorfield2D", NULL, false, wsp);
            m_param_tbox_filename = new FilenameParameter("T-Box in Racer-format");
            
            m_param_velocity1	=  new FloatParameter("low velocity [cm/s] <= ", 0,999999, 10);
            m_param_velocity2	= new FloatParameter("moderate velocity [cm/s] <=", 0,999999, 30);
            m_param_velocity3  = new FloatParameter("high velocity [cm/s]<=", 0,999999, 100);
            
            m_param_distance1 = new FloatParameter("touching distance [km] <=", 0,999999, 1);
            m_param_distance2 = new FloatParameter("next-to distance [km] <=", 0,999999, 5);
            m_param_distance3 = new FloatParameter("far distance [km] <=", 0,999999, 10);
            
            m_param_use_wind = new BoolParameter("Wind vectorfield available?", false);
            m_param_wind_vectorfield = new ModelParameter("Wind vectorfield", "SparseVectorfield2D|SparseMultiVectorfield2D|SparseWeightedVectorfield2D|SparseWeightedMultiVectorfield2D|DenseVectorfield2D|DenseWeightedVectorfield2D", NULL, m_param_use_wind, wsp);
            
            m_param_use_modelled_currents = new BoolParameter("Modelled current available?", false);
            m_param_modelled_vectorfield = new ModelParameter("Modelled current vectorfield", "SparseVectorfield2D|SparseMultiVectorfield2D|SparseWeightedVectorfield2D|SparseWeightedMultiVectorfield2D|DenseVectorfield2D|DenseWeightedVectorfield2D", NULL, m_param_use_modelled_currents, wsp);

            m_param_save_abox = new BoolParameter("Save resulting A-Box?", false);
            m_param_abox_filename = new FilenameParameter("A-Box in Racer-format", "", m_param_save_abox);	
            
            m_parameters->addParameter("vf", m_param_measured_vectorfield );
            m_parameters->addParameter("tbox-filename", m_param_tbox_filename );
            
            m_parameters->addParameter("v1", m_param_velocity1);
            m_parameters->addParameter("v2", m_param_velocity2);
            m_parameters->addParameter("v3", m_param_velocity3);
            
            m_parameters->addParameter("d1", m_param_distance1);
            m_parameters->addParameter("d2", m_param_distance2);
            m_parameters->addParameter("d3", m_param_distance3);
            
            
            m_parameters->addParameter("use_wind?", m_param_use_wind);
            m_parameters->addParameter("wind",      m_param_wind_vectorfield);
            
            m_parameters->addParameter("use_model?", m_param_use_modelled_currents);
            m_parameters->addParameter("model",      m_param_modelled_vectorfield);

            m_parameters->addParameter("save_abox?",    m_param_save_abox);
            m_parameters->addParameter("abox-filename", m_param_abox_filename);
        }
    
        QString typeName() const
        {
            return "RacerInterpreter";
        }
    
    
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    emit statusMessage(0.0, QString("started"));
                    
                    Vectorfield2D* measured_vf = static_cast<Vectorfield2D*>(  m_param_measured_vectorfield->value() );
                    
                    if (measured_vf->scale() == 0)
                    {
                        emit errorMessage("Error: No scale given to compute the 'cm/s' for each vector of the measured vf.");
                    }
                    
                    QString abox_filename = m_param_abox_filename->value();
                    QString tbox_filename = m_param_tbox_filename->value();
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    RacerConnection conn;
                    
                    unsigned int connection_timeout =		 5*	1000; //5 secs
                    unsigned int tbox_timeout =				60*	1000; //1 min
                    unsigned int abox_timeout =				60*	1000; //1 min
                    unsigned int query_timeout =		30*	60*	1000; //30 min
                    
                    qDebug("Trying to connect to the RACER server");
                    conn.connectToServer(connection_timeout);
                    
                    if(conn.connected())
                    {
                        //qInfo() << "Connected with RACER SERVER? " << conn.connected() <<"\n"
                        //			<< "ipAddress: " << conn.ipAddress() << "\n"
                        //			<< "port:      " << conn.port() << "\n"
                        //			<< "racer version:      " << conn.racerVersion() << "\n";
                        
                        QString request,result;
                        
                        if(!tbox_filename.isEmpty())
                        {
                            request = "(racer-read-file \"" + tbox_filename + "\")";
                            result = conn.send(request, tbox_timeout);
                        }
                        
                        //If T-Box has been loaded, we proceed
                        if(result != "")
                        {					
                            //qDebug() << "Received result from RACER after loading TBox: " << result << "\n";
                            
                            std::vector<QString> directions(8);
                            directions[0] = "north";
                            directions[1] = "northeast";
                            directions[2] = "east";
                            directions[3] = "southeast";
                            directions[4] = "south";
                            directions[5] = "southwest";
                            directions[6] = "west";
                            directions[7] = "northwest";
                            
                            QTextStream* filestream  = NULL;
                            
                            if(m_param_save_abox->value())
                            {
                                QFile file(abox_filename);
                                if (file.open(QIODevice::WriteOnly | QIODevice::Text))
                                {
                                    filestream = new QTextStream(&file);
                                }
                            }
                            
                            if (m_param_use_wind->value())
                            {
                                Vectorfield2D* wind_vf = static_cast<Vectorfield2D*>(  m_param_wind_vectorfield->value() );	
                                
                                if(wind_vf != NULL)
                                {
                                    if (wind_vf->scale() == 0)
                                    {
                                        emit errorMessage("Error: No scale given to compute the 'cm/s' for each vector of the wind vf.");
                                        
                                        conn.disconnect();
                                    }
                                    else
                                    {
                                        //only add wind vectors to abox, which have a role relation with a measured vector:
                                        for(unsigned int i=0; i<wind_vf->size(); ++i)
                                        {
                                            
                                            QTransform wind_vf_transform = wind_vf->globalTransformation();
                                            
                                            QPointF t_i = wind_vf_transform.map(QPointF(wind_vf->origin(i).x(), wind_vf->origin(i).y()));
                                            QPointF t_j;
                                            
                                            for(unsigned int j=0; j<measured_vf->size(); ++j)
                                            {
                                                QTransform measured_vf_transform = measured_vf->globalTransformation();
                                                
                                                t_j = measured_vf_transform.map(QPointF(measured_vf->origin(j).x(), measured_vf->origin(j).y()));
                                                
                                                float distance = distanceOnEarth(t_i, t_j);
                                                
                                                if (distance <= m_param_distance3->value())
                                                {
                                                    QString name = QString("wind%1").arg(i);
                                                    
                                                    QTextStream datastream;
                                                    
                                                    //add the concept
                                                    datastream <<  "(instance " << name << " (and windcurrent\n";
                                                    
                                                    //find out direction (in qualitative description)
                                                    QString angle_str = directions[std::min(7.0,std::max(0.0, double(wind_vf->angle(i)) / 360 * directions.size()))];
                                                    datastream << "\t\t(some has-direction " << angle_str << ")\n";
                                                    
                                                    //find out velocity (in qualitative description)
                                                    float velocity = wind_vf->length(i)*wind_vf->scale();
                                                    
                                                    if (velocity != 0)
                                                    {
                                                        QString  velocity_str;
                                                        if (velocity<= m_param_velocity1->value())
                                                        {
                                                            velocity_str = "low";
                                                        }
                                                        else if (velocity > m_param_velocity1->value() && velocity <= m_param_velocity2->value())
                                                        {
                                                            velocity_str = "moderate";
                                                        }
                                                        else if (velocity > m_param_velocity2->value() && velocity <= m_param_velocity3->value())
                                                        {
                                                            velocity_str = "high";
                                                        }
                                                        if (!velocity_str.isEmpty())
                                                            datastream << "\t\t(some has-velocity " << velocity_str << ")\n";
                                                        
                                                        //Close idividual descriptor:
                                                        datastream << "))\n\n";
                                                        
                                                        QString request = datastream.readAll();
                                                        if(filestream)
                                                        {
                                                            *filestream << request;
                                                        }
                                                        
                                                        //Send ABox for current wind vector to RACER
                                                        result =  conn.send(request, abox_timeout);
                                                        if (result.isEmpty())
                                                        {
                                                            break;
                                                        }
                                                    }
                                                    
                                                    break; //break out of inner for loop
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            
                            if (m_param_use_modelled_currents->value())
                            {
                                Vectorfield2D* modelled_vf = static_cast<Vectorfield2D*>(  m_param_modelled_vectorfield->value() );	
                                
                                if(modelled_vf != NULL)
                                {
                                    if (modelled_vf->scale() == 0)
                                    {
                                        emit errorMessage("Error: No scale given to compute the 'cm/s' for each vector of the modelled vf.");
                                        
                                        conn.disconnect();
                                    }
                                    else
                                    {
                                        //only add modelled vectors to abox, which have a role relation with a measured vector:
                                        for(unsigned int i=0; i<modelled_vf->size(); ++i)
                                        {
                                            
                                            QTransform modelled_vf_transform = modelled_vf->globalTransformation();
                                            
                                            QPointF t_i = modelled_vf_transform.map(QPointF(modelled_vf->origin(i).x(), modelled_vf->origin(i).y()));
                                            QPointF t_j;
                                            
                                            for(unsigned int j=0; j<measured_vf->size(); ++j)
                                            {
                                                QTransform measured_vf_transform = measured_vf->globalTransformation();
                                                
                                                t_j = measured_vf_transform.map(QPointF(measured_vf->origin(j).x(), measured_vf->origin(j).y()));
                                                
                                                float distance = distanceOnEarth(t_i, t_j);
                                                
                                                if (distance <= m_param_distance3->value())
                                                {
                                                    QString name = QString("modelled%1").arg(i);
                                                    
                                                    QTextStream datastream;
                                                    
                                                    //add the concept
                                                    datastream <<  "(instance " << name << " (and modelledcurrent\n";
                                                    
                                                    //find out direction (in qualitative description)
                                                    QString angle_str = directions[std::min(7.0,std::max(0.0, double(modelled_vf->angle(i)) / 360 * directions.size()))];
                                                    datastream << "\t\t(some has-direction " << angle_str << ")\n";
                                                    
                                                    //find out velocity (in qualitative description)
                                                    float velocity = modelled_vf->length(i)*modelled_vf->scale();
                                                    
                                                    if (velocity != 0)
                                                    {
                                                        QString  velocity_str;
                                                        if (velocity<= m_param_velocity1->value())
                                                        {
                                                            velocity_str = "low";
                                                        }
                                                        else if (velocity > m_param_velocity1->value() && velocity <= m_param_velocity2->value())
                                                        {
                                                            velocity_str = "moderate";
                                                        }
                                                        else if (velocity > m_param_velocity2->value() && velocity <= m_param_velocity3->value())
                                                        {
                                                            velocity_str = "high";
                                                        }
                                                        if (!velocity_str.isEmpty())
                                                            datastream << "\t\t(some has-velocity " << velocity_str << ")\n";
                                                        
                                                        //Close idividual descriptor:
                                                        datastream << "))\n\n";
                                                        
                                                        QString request = datastream.readAll();
                                                        
                                                        if(filestream)
                                                        {
                                                            *filestream << request;
                                                        }
                                                        
                                                        //Send ABox for current modelled vector to RACER
                                                        result =  conn.send(request, abox_timeout);
                                                        if (result.isEmpty())
                                                        {
                                                            break;
                                                        }
                                                    }
                                                    
                                                    break; //break out of inner for loop
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            
                            //for each measured vector:
                            for(unsigned int i=0; i<measured_vf->size(); ++i)
                            {
                                QString name = QString("measured%1").arg(i);
                                
                                QTextStream datastream;
                                
                                //add the concept
                                datastream <<  "(instance " << name << " (and measuredcurrent\n";
                                
                                //find out direction (in qualitative description)
                                QString angle_str= directions[std::min(7.0,std::max(0.0, double(measured_vf->angle(i)) / 360 * directions.size()))];
                                datastream << "\t\t(some has-direction " << angle_str << ")\n";
                                
                                //find out velocity (in qualitative description)
                                float velocity = measured_vf->length(i)*measured_vf->scale();
                                
                                
                                QString  velocity_str;
                                if (velocity<= m_param_velocity1->value())
                                {
                                    velocity_str = "low";
                                }
                                else if (velocity > m_param_velocity1->value() && velocity <= m_param_velocity2->value())
                                {
                                    velocity_str = "moderate";
                                }
                                else if (velocity > m_param_velocity2->value() && velocity <= m_param_velocity3->value())
                                {
                                    velocity_str = "high";
                                }
                                if (!velocity_str.isEmpty())
                                    datastream << "\t\t(some has-velocity " << velocity_str << ")\n";
                                
                                //Close idividual descriptor:
                                datastream << "))\n";
                                QString request = datastream.readAll();
                                
                                if(filestream)
                                {
                                    *filestream << request;
                                }
                                
                                //Send ABox for current vector to RACER
                                result =  conn.send(request, abox_timeout);
                                if (result.isEmpty())
                                {
                                    qDebug() << "Error at datastream: " << request;
                                    break;
                                }
                            }
                            
                            //for each measured vector determine role relations:
                            for(unsigned int i=0; i<measured_vf->size(); ++i)
                            {
                                QString name = QString("measured%1").arg(i);
                                
                                QTextStream datastream;
                                QTransform measured_vf_transform = measured_vf->globalTransformation();
                                
                                QPointF t_i = measured_vf_transform.map(QPointF(measured_vf->origin(i).x(), measured_vf->origin(i).y()));
                                QPointF t_j;
                                
                                if (m_param_use_wind->value())
                                {
                                    Vectorfield2D* wind_vf = static_cast<Vectorfield2D*>(  m_param_wind_vectorfield->value() );
                                    if(wind_vf != NULL)
                                    {
                                        QTransform wind_vf_transform = wind_vf->globalTransformation();
                                        
                                        //find out distant wind individuals (in qualitative description)
                                        //for each neighbor wind vector:
                                        for(unsigned int j=0; j<wind_vf->size(); ++j)
                                        {
                                            QString neighbor_name = QString("wind%1").arg(j);
                                            
                                            t_j = wind_vf_transform.map(QPointF(wind_vf->origin(j).x(), wind_vf->origin(j).y()));
                                            
                                            float distance = distanceOnEarth(t_i, t_j);
                                            
                                            QString distance_str;
                                            if (distance <= m_param_distance1->value())
                                            {
                                                distance_str = "touches";
                                            }
                                            else if (distance > m_param_distance1->value() && distance <= m_param_distance2->value())
                                            {
                                                distance_str = "is-next-to";
                                            }
                                            else if (distance > m_param_distance2->value() && distance <= m_param_distance3->value())
                                            {
                                                distance_str = "is-far-away-from";
                                            }
                                            if(!distance_str.isEmpty())
                                                datastream << "(related " << name << " " << neighbor_name << " " << distance_str << ")\n";
                                        }
                                    }
                                }
                                
                                if (m_param_use_modelled_currents->value())
                                {
                                    Vectorfield2D* modelled_vf = static_cast<Vectorfield2D*>(  m_param_modelled_vectorfield->value() );
                                    if(modelled_vf != NULL)
                                    {
                                        QTransform modelled_vf_transform = modelled_vf->globalTransformation();
                                        
                                        //find out distant modelled current individuals (in qualitative description)
                                        //for each neighbor modelled current vector:
                                        for(unsigned int j=0; j<modelled_vf->size(); ++j)
                                        {
                                            QString neighbor_name = QString("modelled%1").arg(j);
                                            
                                            t_j = modelled_vf_transform.map(QPointF(modelled_vf->origin(j).x(), modelled_vf->origin(j).y()));
                                            
                                            float distance = distanceOnEarth(t_i, t_j);
                                            
                                            QString distance_str;
                                            if (distance <= m_param_distance1->value())
                                            {
                                                distance_str = "touches";
                                            }
                                            else if (distance > m_param_distance1->value() && distance <= m_param_distance2->value())
                                            {
                                                distance_str = "is-next-to";
                                            }
                                            else if (distance > m_param_distance2->value() && distance <= m_param_distance3->value())
                                            {
                                                distance_str = "is-far-away-from";
                                            }
                                            if(!distance_str.isEmpty())
                                                datastream << "(related " << name << " " << neighbor_name << " " << distance_str << ")\n";
                                        }
                                    }
                                }
                                
                                //find out distant measured individuals (in qualitative description)
                                //for each neighbor vector with a smaller id.
                                //This is possible due to the reflexive spatial rules!
                                for(unsigned int j=0; j<i; ++j)
                                {
                                    QString neighbor_name = QString("measured%1").arg(j);
                                    
                                    t_j = measured_vf_transform.map(QPointF(measured_vf->origin(j).x(), measured_vf->origin(j).y()));
                                    
                                    float distance = distanceOnEarth(t_i, t_j);
                                    
                                    QString distance_str;
                                    if (distance <= m_param_distance1->value())
                                    {
                                        distance_str = "touches";
                                    }
                                    else if (distance > m_param_distance1->value() && distance <= m_param_distance2->value())
                                    {
                                        distance_str = "is-next-to";
                                    }
                                    else if (distance > m_param_distance2->value() && distance <= m_param_distance3->value())
                                    {
                                        distance_str = "is-far-away-from";
                                    }
                                    if(!distance_str.isEmpty())
                                        datastream << "(related " << name << " " << neighbor_name << " " << distance_str << ")\n";
                                }
                                
                                QString request = datastream.readAll();
                                
                                if(!request.isEmpty())
                                {
                                    if(filestream)
                                    {
                                        *filestream << request;
                                    }
                                    
                                    //Send ABox for current vector to RACER
                                    result =  conn.send(request, abox_timeout);
                                    if (result.isEmpty())
                                    {
                                        qDebug() << "Error at request: " << request;
                                        break;
                                    }
                                }
                                emit statusMessage(i*100.0/measured_vf->size(), QString("creating A-Box from data"));
                            }	
                            
                            
                            //ABox is written completely - close it
                            if(filestream)
                            {
                                filestream->flush();
                                delete filestream;
                                filestream = NULL;
                            }
                            
                            
                            /**
                             *
                             * Pose questions onto that (already submitted) A- and TBox:
                             *
                             */
                            
                            //TODO Add Landmask and shiproutes
                            
                            //Find wind problems (not further distinguishable)
                            if (m_param_use_wind->value())
                            {
                                qInfo()	<< "(retrieve (?x) (?x wind-problem))\n";
                                qInfo()	<< conn.send("(retrieve (?x) (?x wind-problem))", query_timeout) << "\n\n\n\n";
                            }						
                            
                            //find wrong modellcurrents:						
                            if (m_param_use_modelled_currents->value())
                            {
                                qInfo()	<< "(retrieve (?x) (?x modelledcurrent-problem))\n";
                                qInfo()	<< conn.send("(retrieve (?x) (?x modelledcurrent-problem))", query_timeout) << "\n\n";
                                
                                //distinguish between wrong direction and wrong speed
                                qInfo()	<< "(retrieve (?x) (?x modelledcurrent-direction-problem))\n";
                                qInfo()	<< conn.send("(retrieve (?x) (?x modelledcurrent-direction-problem))", query_timeout) << "\n\n";
                                
                                qInfo()	<< "(retrieve (?x) (?x modelledcurrent-velocity-problem))\n";
                                qInfo()	<< conn.send("(retrieve (?x) (?x modelledcurrent-velocity-problem))", query_timeout) << "\n\n\n\n";
                            }
                            
                            
                            //find wrong measuredcurrents:						
                            qInfo()	<< "(retrieve (?x) (?x currentsmoothness-problem))\n";
                            qInfo()	<< conn.send("(retrieve (?x) (?x currentsmoothness-problem))", query_timeout) << "\n\n";
                            
                            //distinguish between wrong direction and wrong speed
                            qInfo()	<< "(retrieve (?x) (?x currentsmoothness-direction-problem))\n";
                            qInfo()	<< conn.send("(retrieve (?x) (?x currentsmoothness-direction-problem))", query_timeout) << "\n\n";
                            
                            qInfo()	<< "(retrieve (?x) (?x currentsmoothness-velocity-problem))\n";
                            qInfo()	<< conn.send("(retrieve (?x) (?x currentsmoothness-velocity-problem))", query_timeout) << "\n\n";
                        }
                        else
                        {
                            qCritical("RACER did not read TBox....");
                            conn.disconnect();
                            emit errorMessage(	QString("Explainable error occured: RACER Server did not read TBox"));
                        }
                        
                        
                        conn.disconnect();
                        
                        emit statusMessage(100.0, QString("finished computation"));
                        emit finished();
                    }
                    else 
                    {
                        emit errorMessage(	QString("Explainable error occured: RACER Server was not found under ")
                                          + conn.ipAddress()
                                          + QString(" (port: %1)").arg(conn.port()));
                    }
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));
                }
                unlockModels();
            }
        }

    protected:
        /** 
         * @{
         *
         * Additional parameters
         */
        ModelParameter * m_param_measured_vectorfield;
        
        FilenameParameter * m_param_tbox_filename;
        
        FloatParameter	* m_param_velocity1;
        FloatParameter	* m_param_velocity2;
        FloatParameter	* m_param_velocity3;
        
        FloatParameter	* m_param_distance1;
        FloatParameter	* m_param_distance2;
        FloatParameter	* m_param_distance3;
        
        BoolParameter	* m_param_use_wind;
        ModelParameter * m_param_wind_vectorfield;
        
        BoolParameter	* m_param_use_modelled_currents;
        ModelParameter * m_param_modelled_vectorfield;

        BoolParameter	* m_param_save_abox;
        FilenameParameter * m_param_abox_filename;
        /**
         * @}
         */
};

/**
 * Creates one instance of the non-clustered vectorfield DL interpretation
 * algorithm defined above.
 *
 * \return A new instance of the RacerClusteredInterpreter.
 */
Algorithm* createRacerInterpreter(Workspace* wsp)
{
	return new RacerInterpreter(wsp);
}




/**
 * This class implements a classical reasoning case for the analysis of derived sea surface currents
 * (by means of a clustered vectorfield), other sources of knowledge and a common reasoning base
 * (by means of a T-Box). To answer the validity question of the measurement, the connection to a
 * Racer DL system is used.
 */
class RacerClusteredInterpreter
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        RacerClusteredInterpreter(Workspace* wsp)
        : Algorithm(wsp)
        { 
            m_param_measured_vectorfield = new ModelParameter("Clustered Current Vectorfield", "SparseVectorfield2D|SparseMultiVectorfield2D|SparseWeightedVectorfield2D|SparseWeightedMultiVectorfield2D|DenseVectorfield2D|DenseWeightedVectorfield2D", NULL, false, wsp);
            m_param_clusters = new ModelParameter("Weighted Cluster borders", "WeightedPolygonList2D", NULL, false, wsp);
            m_param_tbox_filename = new FilenameParameter("T-Box in Racer-format");
            
            m_param_smoothness_threshold	=  new FloatParameter("smoothness threshold ", 0,999999, 10);
            
            m_param_velocity1	=  new FloatParameter("low velocity [cm/s] <= ", 0,999999, 10);
            m_param_velocity2	= new FloatParameter("moderate velocity [cm/s] <=", 0,999999, 30);
            m_param_velocity3  = new FloatParameter("high velocity [cm/s]<=", 0,999999, 100);
            
            m_param_distance1 = new FloatParameter("touching distance [km] <=", 0,999999, 1);
            m_param_distance2 = new FloatParameter("next-to distance [km] <=", 0,999999, 5);
            m_param_distance3 = new FloatParameter("far distance [km] <=", 0,999999, 10);
            
            m_param_use_wind = new BoolParameter("Wind vectorfield available?", false);
            m_param_wind_vectorfield = new ModelParameter("Wind vectorfield", "SparseVectorfield2D|SparseMultiVectorfield2D|SparseWeightedVectorfield2D|SparseWeightedMultiVectorfield2D|DenseVectorfield2D|DenseWeightedVectorfield2D", NULL, m_param_use_wind, wsp);
            
            m_param_use_modelled_currents = new BoolParameter("Modelled current available?", false);
            m_param_modelled_vectorfield = new ModelParameter("Modelled current vectorfield", "SparseVectorfield2D|SparseMultiVectorfield2D|SparseWeightedVectorfield2D|SparseWeightedMultiVectorfield2D|DenseVectorfield2D|DenseWeightedVectorfield2D", NULL, m_param_use_modelled_currents,wsp);

            m_param_save_abox = new BoolParameter("Save resulting A-Box?", false);
            m_param_abox_filename = new FilenameParameter("A-Box in Racer-format", "", m_param_save_abox);	
            
            m_parameters->addParameter("vf",	m_param_measured_vectorfield );
            m_parameters->addParameter("clusters",	m_param_clusters );
            m_parameters->addParameter("tbox-filename",	m_param_tbox_filename );
            
            m_parameters->addParameter("smoothT", m_param_smoothness_threshold);
            
            m_parameters->addParameter("v1", m_param_velocity1);
            m_parameters->addParameter("v2", m_param_velocity2);
            m_parameters->addParameter("v3", m_param_velocity3);
            
            m_parameters->addParameter("d1", m_param_distance1);
            m_parameters->addParameter("d2", m_param_distance2);
            m_parameters->addParameter("d3", m_param_distance3);
            
            
            m_parameters->addParameter("use_wind?", m_param_use_wind);
            m_parameters->addParameter("wind",      m_param_wind_vectorfield);
            
            m_parameters->addParameter("use_model?", m_param_use_modelled_currents);
            m_parameters->addParameter("model",      m_param_modelled_vectorfield);

            m_parameters->addParameter("save_abox?",    m_param_save_abox);
            m_parameters->addParameter("abox-filename", m_param_abox_filename);
        }
    
        QString typeName() const
        {
            return "RacerClusteredInterpreter";
        }
    
        
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    
                    emit statusMessage(0.0, QString("started"));
                    
                    Vectorfield2D* measured_vf = static_cast<Vectorfield2D*>(  m_param_measured_vectorfield->value() );	
                    WeightedPolygonList2D* measured_clusters = static_cast<WeightedPolygonList2D*>(  m_param_clusters->value() );	
                    
                    if (measured_vf->scale() == 0)
                    {
                        emit errorMessage("Error: No scale given to compute the 'cm/s' for each vector of the measured vf.");
                    }
                    
                    QString abox_filename = m_param_abox_filename->value();
                    QString tbox_filename = m_param_tbox_filename->value();
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    RacerConnection conn;
                    
                    unsigned int connection_timeout =		 5*	1000; //5 secs
                    unsigned int tbox_timeout =				60*	1000; //1 min
                    unsigned int abox_timeout =				60*	1000; //1 min
                    unsigned int query_timeout =		30*	60*	1000; //30 min
                    
                    qDebug("Trying to connect to the RACER server");
                    conn.connectToServer(connection_timeout);
                    
                    if(conn.connected())
                    {
                        
                        //qInfo() << "Connected with RACER SERVER? " << conn.connected() <<"\n"
                        //			<< "ipAddress: " << conn.ipAddress() << "\n"
                        //			<< "port:      " << conn.port() << "\n"
                        //			<< "racer version:      " << conn.racerVersion() << "\n";
                        
                        
                        QString request,result;
                        
                        if(!tbox_filename.isEmpty())
                        {
                            request = "(racer-read-file \"" + tbox_filename + "\")";
                            result = conn.send(request, tbox_timeout);
                        }
                        
                        //If T-Box has been loaded, we proceed
                        if(result != "")
                        {					
                            //qDebug() << "Received result from RACER after loading TBox: " << result << "\n";
                            
                            std::vector<QString> directions(8);
                            directions[0] = "north";
                            directions[1] = "northeast";
                            directions[2] = "east";
                            directions[3] = "southeast";
                            directions[4] = "south";
                            directions[5] = "southwest";
                            directions[6] = "west";
                            directions[7] = "northwest";
                            
                            QTextStream* filestream  = NULL;
                            
                            if(m_param_save_abox->value())
                            {
                                QFile file(abox_filename);
                                if (file.open(QIODevice::WriteOnly | QIODevice::Text))
                                {
                                    filestream = new QTextStream(&file);
                                }
                            }
                            
                            if (m_param_use_wind->value())
                            {
                                Vectorfield2D* wind_vf = static_cast<Vectorfield2D*>(  m_param_wind_vectorfield->value() );	
                                
                                if(wind_vf != NULL)
                                {
                                    if (wind_vf->scale() == 0)
                                    {
                                        emit errorMessage("Error: No scale given to compute the 'cm/s' for each vector of the wind vf.");
                                        
                                        conn.disconnect();
                                    }
                                    else
                                    {
                                        //only add wind vectors to abox, which have a role relation with a measured vector:
                                        for(unsigned int i=0; i<wind_vf->size(); ++i)
                                        {
                                            
                                            QTransform wind_vf_transform = wind_vf->globalTransformation();
                                            
                                            QPointF t_i = wind_vf_transform.map(QPointF(wind_vf->origin(i).x(), wind_vf->origin(i).y()));
                                            QPointF t_j;
                                            
                                            for(unsigned int j=0; j<measured_vf->size(); ++j)
                                            {
                                                QTransform measured_vf_transform = measured_vf->globalTransformation();
                                                
                                                t_j = measured_vf_transform.map(QPointF(measured_vf->origin(j).x(), measured_vf->origin(j).y()));
                                                
                                                float distance = distanceOnEarth(t_i, t_j);
                                                
                                                if (distance <= m_param_distance3->value())
                                                {
                                                    QString name = QString("wind%1").arg(i);
                                                    
                                                    QTextStream datastream;
                                                    
                                                    //add the concept
                                                    datastream <<  "(instance " << name << " (and windcurrent\n";
                                                    
                                                    //find out direction (in qualitative description)
                                                    QString angle_str= directions[std::min(7.0,std::max(0.0, double(wind_vf->angle(i)) / 360 * directions.size()))];
                                                    datastream << "\t\t(some has-direction " << angle_str << ")\n";
                                                    
                                                    //find out velocity (in qualitative description)
                                                    float velocity = wind_vf->length(i)*wind_vf->scale();
                                                    
                                                    if (velocity != 0)
                                                    {
                                                        QString  velocity_str;
                                                        if (velocity<= m_param_velocity1->value())
                                                        {
                                                            velocity_str = "low";
                                                        }
                                                        else if (velocity > m_param_velocity1->value() && velocity <= m_param_velocity2->value())
                                                        {
                                                            velocity_str = "moderate";
                                                        }
                                                        else if (velocity > m_param_velocity2->value() && velocity <= m_param_velocity3->value())
                                                        {
                                                            velocity_str = "high";
                                                        }
                                                        if (!velocity_str.isEmpty())
                                                            datastream << "\t\t(some has-velocity " << velocity_str << ")\n";
                                                        
                                                        //Close idividual descriptor:
                                                        datastream << "))\n\n";
                                                        
                                                       QString request = datastream.readAll();
                                
                                                        if(!request.isEmpty())
                                                        {
                                                            if(filestream)
                                                            {
                                                                *filestream << request;
                                                            }
                                                            
                                                            //Send ABox for current vector to RACER
                                                            result =  conn.send(request, abox_timeout);
                                                            if (result.isEmpty())
                                                            {
                                                                qDebug() << "Error at request: " << request;
                                                                break;
                                                            }
                                                        }
                                                    }
                                                    
                                                    break; //break out of inner for loop
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            
                            if (m_param_use_modelled_currents->value())
                            {
                                Vectorfield2D* modelled_vf = static_cast<Vectorfield2D*>(  m_param_modelled_vectorfield->value() );	
                                
                                if(modelled_vf != NULL)
                                {
                                    if (modelled_vf->scale() == 0)
                                    {
                                        emit errorMessage("Error: No scale given to compute the 'cm/s' for each vector of the modelled vf.");
                                        
                                        conn.disconnect();
                                    }
                                    else
                                    {
                                        //only add modelled vectors to abox, which have a role relation with a measured vector:
                                        for(unsigned int i=0; i<modelled_vf->size(); ++i)
                                        {
                                            
                                            QTransform modelled_vf_transform = modelled_vf->globalTransformation();
                                            
                                            QPointF t_i = modelled_vf_transform.map(QPointF(modelled_vf->origin(i).x(), modelled_vf->origin(i).y()));
                                            QPointF t_j;
                                            
                                            for(unsigned int j=0; j<measured_vf->size(); ++j)
                                            {
                                                QTransform measured_vf_transform = measured_vf->globalTransformation();
                                                
                                                t_j = measured_vf_transform.map(QPointF(measured_vf->origin(j).x(), measured_vf->origin(j).y()));
                                                
                                                float distance = distanceOnEarth(t_i, t_j);
                                                
                                                if (distance <= m_param_distance3->value())
                                                {
                                                    QString name = QString("modelled%1").arg(i);
                                                    
                                                    QTextStream datastream;
                                                    
                                                    //add the concept
                                                    datastream <<  "(instance " << name << " (and modelledcurrent\n";
                                                    
                                                    //find out direction (in qualitative description)
                                                    QString angle_str= directions[std::min(7.0,std::max(0.0, double(modelled_vf->angle(i)) / 360 * directions.size()))];
                                                    datastream << "\t\t(some has-direction " << angle_str << ")\n";
                                                    
                                                    //find out velocity (in qualitative description)
                                                    float velocity = modelled_vf->length(i)*modelled_vf->scale();
                                                    
                                                    if (velocity != 0)
                                                    {
                                                        QString  velocity_str;
                                                        if (velocity<= m_param_velocity1->value())
                                                        {
                                                            velocity_str = "low";
                                                        }
                                                        else if (velocity > m_param_velocity1->value() && velocity <= m_param_velocity2->value())
                                                        {
                                                            velocity_str = "moderate";
                                                        }
                                                        else if (velocity > m_param_velocity2->value() && velocity <= m_param_velocity3->value())
                                                        {
                                                            velocity_str = "high";
                                                        }
                                                        if (!velocity_str.isEmpty())
                                                            datastream << "\t\t(some has-velocity " << velocity_str << ")\n";
                                                        
                                                        //Close idividual descriptor:
                                                        datastream << "))\n\n";
                                                        
                                                        QString request = datastream.readAll();
                                                        
                                                        if(!request.isEmpty())
                                                        {
                                                            
                                                            if(filestream)
                                                            {
                                                                *filestream << request;
                                                            }
                                                            
                                                            //Send ABox for current vector to RACER
                                                            result =  conn.send(request, abox_timeout);
                                                            if (result.isEmpty())
                                                            {
                                                                qDebug() << "Error at request: " << request;
                                                                break;
                                                            }
                                                        }
                                                    }
                                                    
                                                    break; //break out of inner for loop
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            
                            //for each measured vector:
                            for(unsigned int i=0; i<measured_vf->size(); ++i)
                            {
                                QString name = QString("measured%1").arg(i);
                                
                                QTextStream datastream;
                                
                                //add the concept
                                datastream <<  "(instance " << name << " (and measuredcurrent\n";
                                
                                //find out direction (in qualitative description)
                                QString angle_str= directions[std::min(7.0,std::max(0.0, double(measured_vf->angle(i)) / 360 * directions.size()))];
                                datastream << "\t\t(some has-direction " << angle_str << ")\n";
                                
                                //find out velocity (in qualitative description)
                                float velocity = measured_vf->length(i)*measured_vf->scale();
                                
                                
                                QString  velocity_str;
                                if (velocity<= m_param_velocity1->value())
                                {
                                    velocity_str = "low";
                                }
                                else if (velocity > m_param_velocity1->value() && velocity <= m_param_velocity2->value())
                                {
                                    velocity_str = "moderate";
                                }
                                else if (velocity > m_param_velocity2->value() && velocity <= m_param_velocity3->value())
                                {
                                    velocity_str = "high";
                                }
                                if (!velocity_str.isEmpty())
                                    datastream << "\t\t(some has-velocity " << velocity_str << ")\n";
                                
                                //Close idividual descriptor:
                                datastream << "))\n";
                                
                                QString request = datastream.readAll();
                                
                                if(!request.isEmpty())
                                {
                                    if(filestream)
                                    {
                                        *filestream << request;
                                    }
                                    
                                    //Send ABox for current vector to RACER
                                    result =  conn.send(request, abox_timeout);
                                    if (result.isEmpty())
                                    {
                                        qDebug() << "Error at request: " << request;
                                        break;
                                    }
                                }
                            }
                            
                            //for each measured vector determine role relations:
                            for(unsigned int i=0; i<measured_vf->size(); ++i)
                            {
                                QString name = QString("measured%1").arg(i);
                                
                                QTextStream datastream;
                                QTransform measured_vf_transform = measured_vf->globalTransformation();
                                
                                QPointF t_i = measured_vf_transform.map(QPointF(measured_vf->origin(i).x(), measured_vf->origin(i).y()));
                                QPointF t_j;
                                
                                if (m_param_use_wind->value())
                                {
                                    Vectorfield2D* wind_vf = static_cast<Vectorfield2D*>(  m_param_wind_vectorfield->value() );
                                    if(wind_vf != NULL)
                                    {
                                        QTransform wind_vf_transform = wind_vf->globalTransformation();
                                        
                                        //find out distant wind individuals (in qualitative description)
                                        //for each neighbor wind vector:
                                        for(unsigned int j=0; j<wind_vf->size(); ++j)
                                        {
                                            QString neighbor_name = QString("wind%1").arg(j);
                                            
                                            t_j = wind_vf_transform.map(QPointF(wind_vf->origin(j).x(), wind_vf->origin(j).y()));
                                            
                                            float distance = distanceOnEarth(t_i, t_j);
                                            
                                            QString distance_str;
                                            if (distance <= m_param_distance1->value())
                                            {
                                                distance_str = "touches";
                                            }
                                            else if (distance > m_param_distance1->value() && distance <= m_param_distance2->value())
                                            {
                                                distance_str = "is-next-to";
                                            }
                                            else if (distance > m_param_distance2->value() && distance <= m_param_distance3->value())
                                            {
                                                distance_str = "is-far-away-from";
                                            }
                                            if(!distance_str.isEmpty())
                                                datastream << "(related " << name << " " << neighbor_name << " " << distance_str << ")\n";
                                        }
                                    }
                                }
                                
                                if (m_param_use_modelled_currents->value())
                                {
                                    Vectorfield2D* modelled_vf = static_cast<Vectorfield2D*>(  m_param_modelled_vectorfield->value() );
                                    if(modelled_vf != NULL)
                                    {
                                        QTransform modelled_vf_transform = modelled_vf->globalTransformation();
                                        
                                        //find out distant modelled current individuals (in qualitative description)
                                        //for each neighbor modelled current vector:
                                        for(unsigned int j=0; j<modelled_vf->size(); ++j)
                                        {
                                            QString neighbor_name = QString("modelled%1").arg(j);
                                            
                                            t_j = modelled_vf_transform.map(QPointF(modelled_vf->origin(j).x(), modelled_vf->origin(j).y()));
                                            
                                            float distance = distanceOnEarth(t_i, t_j);
                                            
                                            QString distance_str;
                                            if (distance <= m_param_distance1->value())
                                            {
                                                distance_str = "touches";
                                            }
                                            else if (distance > m_param_distance1->value() && distance <= m_param_distance2->value())
                                            {
                                                distance_str = "is-next-to";
                                            }
                                            else if (distance > m_param_distance2->value() && distance <= m_param_distance3->value())
                                            {
                                                distance_str = "is-far-away-from";
                                            }
                                            if(!distance_str.isEmpty())
                                                datastream << "(related " << name << " " << neighbor_name << " " << distance_str << ")\n";
                                        }
                                    }
                                }
                                
                                //find out smoothness of each individual by  means of cluster smoothness:
                                float smoothness = measured_clusters->weight(i);
                                
                                if (smoothness > m_param_smoothness_threshold->value())
                                { 
                                    datastream << "(instance " << name << " currentsmoothness-problem)\n";
                                }
                                
                                QString request = datastream.readAll();
                                
                                if(!request.isEmpty())
                                {
                                    if(filestream)
                                    {
                                        *filestream << request;
                                    }
                                    
                                    //Send ABox for current vector to RACER
                                    result =  conn.send(request, abox_timeout);
                                    if (result.isEmpty())
                                    {
                                        qDebug() << "Error at request: " << request;
                                        break;
                                    }
                                }
                                emit statusMessage(i*100.0/measured_vf->size(), QString("creating A-Box from data"));
                            }	
                            
                            
                            //ABox is written completely - close it
                            if(filestream)
                            {
                                filestream->flush();
                                delete filestream;
                                filestream = NULL;
                            }
                            
                            
                            /**
                             *
                             * Pose questions onto that (already submitted) A- and TBox:
                             *
                             */
                            
                            //TODO Add Landmask and shiproutes
                            
                            //Find wind problems (not further distinguishable)
                            if (m_param_use_wind->value())
                            {
                                qInfo()	<< "(retrieve (?x) (?x wind-problem))\n";
                                qInfo()	<< conn.send("(retrieve (?x) (?x wind-problem))", query_timeout) << "\n\n\n\n";
                            }						
                            
                            //find wrong modellcurrents:						
                            if (m_param_use_modelled_currents->value())
                            {
                                qInfo()	<< "(retrieve (?x) (?x modelledcurrent-problem))\n";
                                qInfo()	<< conn.send("(retrieve (?x) (?x modelledcurrent-problem))", query_timeout) << "\n\n";
                                
                                //distinguish between wrong direction and wrong speed
                                qInfo()	<< "(retrieve (?x) (?x modelledcurrent-direction-problem))\n";
                                qInfo()	<< conn.send("(retrieve (?x) (?x modelledcurrent-direction-problem))", query_timeout) << "\n\n";
                                
                                qInfo()	<< "(retrieve (?x) (?x modelledcurrent-velocity-problem))\n";
                                qInfo() << conn.send("(retrieve (?x) (?x modelledcurrent-velocity-problem))", query_timeout) << "\n\n\n\n";
                            }
                            
                            
                            //find wrong measuredcurrents:						
                            qInfo()	<< "(retrieve (?x) (?x currentsmoothness-problem))\n";
                            qInfo()	<< conn.send("(retrieve (?x) (?x currentsmoothness-problem))", query_timeout) << "\n\n";
                            
                            //distinguish between wrong direction and wrong speed
                            qInfo()	<< "(retrieve (?x) (?x currentsmoothness-direction-problem))\n";
                            qInfo()	<< conn.send("(retrieve (?x) (?x currentsmoothness-direction-problem))", query_timeout) << "\n\n";
                            
                            qInfo()	<< "(retrieve (?x) (?x currentsmoothness-velocity-problem))\n";
                            qInfo()	<< conn.send("(retrieve (?x) (?x currentsmoothness-velocity-problem))", query_timeout) << "\n\n";
                        }
                        else
                        {
                            qDebug("RACER did not read TBox....");
                            conn.disconnect();
                            emit errorMessage(QString("Explainable error occured: RACER Server did not read TBox"));
                        }
                        
                        
                        conn.disconnect();
                        
                        emit statusMessage(100.0, QString("finished computation"));
                        emit finished();
                    }
                    else 
                    {
                        emit errorMessage(	QString("Explainable error occured: RACER Server was not found under ")
                                          + conn.ipAddress()
                                          + QString(" (port: %1)").arg(conn.port()));
                    }
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
    
    protected:
        /**
         * @{
         *
         * Additional parameters
         */
        ModelParameter * m_param_measured_vectorfield;
        ModelParameter * m_param_clusters;
        
        FilenameParameter * m_param_tbox_filename;
        
        FloatParameter	* m_param_smoothness_threshold;
        
        FloatParameter	* m_param_velocity1;
        FloatParameter	* m_param_velocity2;
        FloatParameter	* m_param_velocity3;
        
        FloatParameter	* m_param_distance1;
        FloatParameter	* m_param_distance2;
        FloatParameter	* m_param_distance3;
        
        BoolParameter	* m_param_use_wind;
        ModelParameter * m_param_wind_vectorfield;
        
        BoolParameter	* m_param_use_modelled_currents;
        ModelParameter * m_param_modelled_vectorfield;
        
        BoolParameter	* m_param_save_abox;
        FilenameParameter * m_param_abox_filename;
        /**
         * @}
         */
};

/** 
 * Creates one instance of the clustered vectorfield DL interpretation
 * algorithm defined above.
 *
 * \return A new instance of the RacerClusteredInterpreter.
 */
Algorithm* createRacerClusteredInterpreter(Workspace* wsp)
{
	return new RacerClusteredInterpreter(wsp);
}




/**
 * This class creates and exports a classical reasoning case for the analysis of derived sea surface currents
 * (by means of a clustered vectorfield), other sources of knowledge and a common reasoning base
 * (by means of a T-Box) by means of an A-Box. Instead of transferring the A-Box to the Racer server, it is exported
 * to the file system.
 */
class ClusteredABox
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        ClusteredABox(Workspace* wsp)
        : Algorithm(wsp)
        { 
            m_param_measured_vectorfield = new ModelParameter("Clustered Current Vectorfield", "SparseVectorfield2D|SparseMultiVectorfield2D|SparseWeightedVectorfield2D|SparseWeightedMultiVectorfield2D|DenseVectorfield2D|DenseWeightedVectorfield2D", NULL, false, wsp);
            m_param_clusters = new ModelParameter("Weighted Cluster borders", "WeightedPolygonList2D", NULL, false, wsp);
            
            m_param_smoothness_threshold	=  new FloatParameter("smoothness threshold ", 0,999999, 10);
            
            m_param_velocity1	=  new FloatParameter("low velocity [cm/s] <= ", 0,999999, 10);
            m_param_velocity2	= new FloatParameter("moderate velocity [cm/s] <=", 0,999999, 30);
            m_param_velocity3  = new FloatParameter("high velocity [cm/s]<=", 0,999999, 100);
            
            m_param_distance1 = new FloatParameter("touching distance [km] <=", 0,999999, 1);
            m_param_distance2 = new FloatParameter("next-to distance [km] <=", 0,999999, 5);
            m_param_distance3 = new FloatParameter("far distance [km] <=", 0,999999, 10);
            
            m_param_use_wind = new BoolParameter("Wind vectorfield available?", false);
            m_param_wind_vectorfield = new ModelParameter("Wind vectorfield", "SparseVectorfield2D|SparseMultiVectorfield2D|SparseWeightedVectorfield2D|SparseWeightedMultiVectorfield2D|DenseVectorfield2D|DenseWeightedVectorfield2D", NULL, m_param_use_wind, wsp);
            
            m_param_use_modelled_currents = new BoolParameter("Modelled current available?", false);
            m_param_modelled_vectorfield = new ModelParameter("Modelled current vectorfield", "SparseVectorfield2D|SparseMultiVectorfield2D|SparseWeightedVectorfield2D|SparseWeightedMultiVectorfield2D|DenseVectorfield2D|DenseWeightedVectorfield2D", NULL, m_param_use_modelled_currents, wsp);

            m_param_abox_filename = new FilenameParameter("A-Box in Racer-format", "");	
            
            m_parameters->addParameter("vf",	m_param_measured_vectorfield );
            m_parameters->addParameter("clusters", m_param_clusters );

            m_parameters->addParameter("smoothT", m_param_smoothness_threshold);
            
            m_parameters->addParameter("v1", m_param_velocity1);
            m_parameters->addParameter("v2", m_param_velocity2);
            m_parameters->addParameter("v3", m_param_velocity3);
            
            m_parameters->addParameter("d1", m_param_distance1);
            m_parameters->addParameter("d2", m_param_distance2);
            m_parameters->addParameter("d3", m_param_distance3);
            
            
            m_parameters->addParameter("use_wind?", m_param_use_wind);
            m_parameters->addParameter("wind", m_param_wind_vectorfield);
            
            m_parameters->addParameter("use_model?", m_param_use_modelled_currents);
            m_parameters->addParameter("model", m_param_modelled_vectorfield);
            
            m_parameters->addParameter("abox-filename", m_param_abox_filename);
        }
    
        QString typeName() const
        {
            return "ClusteredABox";
        }
    
        
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    emit statusMessage(0.0, QString("started"));
                    
                    Vectorfield2D* measured_vf = static_cast<Vectorfield2D*>(  m_param_measured_vectorfield->value() );	
                    WeightedPolygonList2D* measured_clusters = static_cast<WeightedPolygonList2D*>(  m_param_clusters->value() );	
                    
                    if (measured_vf->scale() == 0)
                    {
                        emit errorMessage("Error: No scale given to compute the 'cm/s' for each vector of the measured vf.");
                    }
                    
                    QString abox_filename = m_param_abox_filename->value();
                    //QString tbox_filename = m_param_tbox_filename->value();
                    
                    emit statusMessage(1.0, QString("starting computation"));

                    std::vector<QString> directions(8);
                    directions[0] = "north";
                    directions[1] = "northeast";
                    directions[2] = "east";
                    directions[3] = "southeast";
                    directions[4] = "south";
                    directions[5] = "southwest";
                    directions[6] = "west";
                    directions[7] = "northwest";
            
                    QTextStream* filestream  = NULL;
                    
                    QFile file(abox_filename);
            
                    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
                    {
                        filestream = new QTextStream(&file);
                    }
            
                    if (m_param_use_wind->value())
                    {
                        Vectorfield2D* wind_vf = static_cast<Vectorfield2D*>(  m_param_wind_vectorfield->value() );	
                        
                        if(wind_vf != NULL)
                        {
                            if (wind_vf->scale() == 0)
                            {
                                emit errorMessage("Error: No scale given to compute the 'cm/s' for each vector of the wind vf.");
                                
                                //conn.disconnect();
                            }
                            else
                            {
                                //only add wind vectors to abox, which have a role relation with a measured vector:
                                for(unsigned int i=0; i<wind_vf->size(); ++i)
                                {
                                    
                                    QTransform wind_vf_transform = wind_vf->globalTransformation();
                                    
                                    QPointF t_i = wind_vf_transform.map(QPointF(wind_vf->origin(i).x(), wind_vf->origin(i).y()));
                                    QPointF t_j;
                                    
                                    for(unsigned int j=0; j<measured_vf->size(); ++j)
                                    {
                                        QTransform measured_vf_transform = measured_vf->globalTransformation();
                                        
                                        t_j = measured_vf_transform.map(QPointF(measured_vf->origin(j).x(), measured_vf->origin(j).y()));
                                        
                                        float distance = distanceOnEarth(t_i, t_j);
                                        
                                        if (distance <= m_param_distance3->value())
                                        {
                                            QString name = QString("wind%1").arg(i);
                                            
                                            QTextStream datastream;
                                            
                                            //add the concept
                                            datastream <<  "(instance " << name << " (and windcurrent\n";
                                            
                                            //find out direction (in qualitative description)
                                            QString angle_str= directions[std::min(7.0, std::max(0.0, double(wind_vf->angle(i)) / 360 * directions.size()))];
                                            datastream << "\t\t(some has-direction " << angle_str << ")\n";
                                            
                                            //find out velocity (in qualitative description)
                                            float velocity = wind_vf->length(i)*wind_vf->scale();
                                            
                                            if (velocity != 0)
                                            {
                                                QString  velocity_str;
                                                if (velocity<= m_param_velocity1->value())
                                                {
                                                    velocity_str = "low";
                                                }
                                                else if (velocity > m_param_velocity1->value() && velocity <= m_param_velocity2->value())
                                                {
                                                    velocity_str = "moderate";
                                                }
                                                else if (velocity > m_param_velocity2->value() && velocity <= m_param_velocity3->value())
                                                {
                                                    velocity_str = "high";
                                                }
                                                if (!velocity_str.isEmpty())
                                                    datastream << "\t\t(some has-velocity " << velocity_str << ")\n";
                                                
                                                //Close idividual descriptor:
                                                datastream << "))\n\n";
                                            
                                                QString request = datastream.readAll();
                                                
                                                if(filestream)
                                                {
                                                    *filestream << request;
                                                }
                                            }
                                            
                                            break; //break out of inner for loop
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    if (m_param_use_modelled_currents->value())
                    {
                        Vectorfield2D* modelled_vf = static_cast<Vectorfield2D*>(  m_param_modelled_vectorfield->value() );	
                        
                        if(modelled_vf != NULL)
                        {
                            if (modelled_vf->scale() == 0)
                            {
                                emit errorMessage("Error: No scale given to compute the 'cm/s' for each vector of the modelled vf.");
                                
                                //conn.disconnect();
                            }
                            else
                            {
                                //only add modelled vectors to abox, which have a role relation with a measured vector:
                                for(unsigned int i=0; i<modelled_vf->size(); ++i)
                                {
                                    
                                    QTransform modelled_vf_transform = modelled_vf->globalTransformation();
                                    
                                    QPointF t_i = modelled_vf_transform.map(QPointF(modelled_vf->origin(i).x(), modelled_vf->origin(i).y()));
                                    QPointF t_j;
                                    
                                    for(unsigned int j=0; j<measured_vf->size(); ++j)
                                    {
                                        QTransform measured_vf_transform = measured_vf->globalTransformation();
                                        
                                        t_j = measured_vf_transform.map(QPointF(measured_vf->origin(j).x(), measured_vf->origin(j).y()));
                                        
                                        float distance = distanceOnEarth(t_i, t_j);
                                        
                                        if (distance <= m_param_distance3->value())
                                        {
                                            QString name = QString("modelled%1").arg(i);
                                            
                                            QTextStream datastream;
                                            
                                            //add the concept
                                            datastream <<  "(instance " << name << " (and modelledcurrent\n";
                                            
                                            //find out direction (in qualitative description)
                                            QString angle_str= directions[std::min(7.0,std::max(0.0, double(modelled_vf->angle(i)) / 360 * directions.size()))];
                                            datastream << "\t\t(some has-direction " << angle_str << ")\n";
                                            
                                            //find out velocity (in qualitative description)
                                            float velocity = modelled_vf->length(i)*modelled_vf->scale();
                                            
                                            if (velocity != 0)
                                            {
                                                QString  velocity_str;
                                                if (velocity<= m_param_velocity1->value())
                                                {
                                                    velocity_str = "low";
                                                }
                                                else if (velocity > m_param_velocity1->value() && velocity <= m_param_velocity2->value())
                                                {
                                                    velocity_str = "moderate";
                                                }
                                                else if (velocity > m_param_velocity2->value() && velocity <= m_param_velocity3->value())
                                                {
                                                    velocity_str = "high";
                                                }
                                                if (!velocity_str.isEmpty())
                                                    datastream << "\t\t(some has-velocity " << velocity_str << ")\n";
                                                
                                                //Close idividual descriptor:
                                                datastream << "))\n\n";
                                                
                                                QString request = datastream.readAll();
                                                
                                                if(filestream)
                                                {
                                                    *filestream << request;
                                                }
                                            }
                                            
                                            break; //break out of inner for loop
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    //for each measured vector:
                    for(unsigned int i=0; i<measured_vf->size(); ++i)
                    {
                        QString name = QString("measured%1").arg(i);
                        
                        QTextStream datastream;
                        
                        //add the concept
                        datastream <<  "(instance " << name << " (and measuredcurrent\n";
                        
                        //find out direction (in qualitative description)
                        QString angle_str= directions[std::min(7.0,std::max(0.0, double(measured_vf->angle(i)) / 360 * directions.size()))];
                        datastream << "\t\t(some has-direction " << angle_str << ")\n";
                        
                        //find out velocity (in qualitative description)
                        float velocity = measured_vf->length(i)*measured_vf->scale();
                        
                        
                        QString  velocity_str;
                        if (velocity<= m_param_velocity1->value())
                        {
                            velocity_str = "low";
                        }
                        else if (velocity > m_param_velocity1->value() && velocity <= m_param_velocity2->value())
                        {
                            velocity_str = "moderate";
                        }
                        else if (velocity > m_param_velocity2->value() && velocity <= m_param_velocity3->value())
                        {
                            velocity_str = "high";
                        }
                        if (!velocity_str.isEmpty())
                            datastream << "\t\t(some has-velocity " << velocity_str << ")\n";
                        
                        //Close idividual descriptor:
                        datastream << "))\n";
                        
                        QString request = datastream.readAll();
                        
                        if(filestream)
                        {
                            *filestream << request;
                        }
                    }
                    
                    //for each measured vector determine role relations:
                    for(unsigned int i=0; i<measured_vf->size(); ++i)
                    {
                        QString name = QString("measured%1").arg(i);
                        
                        QTextStream datastream;
                        QTransform measured_vf_transform = measured_vf->globalTransformation();
                        
                        QPointF t_i = measured_vf_transform.map(QPointF(measured_vf->origin(i).x(), measured_vf->origin(i).y()));
                        QPointF t_j;
                        
                        if (m_param_use_wind->value())
                        {
                            Vectorfield2D* wind_vf = static_cast<Vectorfield2D*>(  m_param_wind_vectorfield->value() );
                            if(wind_vf != NULL)
                            {
                                QTransform wind_vf_transform = wind_vf->globalTransformation();
                                
                                //find out distant wind individuals (in qualitative description)
                                //for each neighbor wind vector:
                                for(unsigned int j=0; j<wind_vf->size(); ++j)
                                {
                                    QString neighbor_name = QString("wind%1").arg(j);
                                    
                                    t_j = wind_vf_transform.map(QPointF(wind_vf->origin(j).y(), wind_vf->origin(j).y()));
                                    
                                    float distance = distanceOnEarth(t_i, t_j);
                                    
                                    QString distance_str;
                                    if (distance <= m_param_distance1->value())
                                    {
                                        distance_str = "touches";
                                    }
                                    else if (distance > m_param_distance1->value() && distance <= m_param_distance2->value())
                                    {
                                        distance_str = "is-next-to";
                                    }
                                    else if (distance > m_param_distance2->value() && distance <= m_param_distance3->value())
                                    {
                                        distance_str = "is-far-away-from";
                                    }
                                    if(!distance_str.isEmpty())
                                        datastream << "(related " << name << " " << neighbor_name << " " << distance_str << ")\n";
                                }
                            }
                        }
                        
                        if (m_param_use_modelled_currents->value())
                        {
                            Vectorfield2D* modelled_vf = static_cast<Vectorfield2D*>(  m_param_modelled_vectorfield->value() );
                            if(modelled_vf != NULL)
                            {
                                QTransform modelled_vf_transform = modelled_vf->globalTransformation();
                                
                                //find out distant modelled current individuals (in qualitative description)
                                //for each neighbor modelled current vector:
                                for(unsigned int j=0; j<modelled_vf->size(); ++j)
                                {
                                    QString neighbor_name = QString("modelled%1").arg(j);
                                    
                                    t_j = modelled_vf_transform.map(QPointF(modelled_vf->origin(j).x(), modelled_vf->origin(j).y()));
                                    
                                    float distance = distanceOnEarth(t_i, t_j);
                                    
                                    QString distance_str;
                                    if (distance <= m_param_distance1->value())
                                    {
                                        distance_str = "touches";
                                    }
                                    else if (distance > m_param_distance1->value() && distance <= m_param_distance2->value())
                                    {
                                        distance_str = "is-next-to";
                                    }
                                    else if (distance > m_param_distance2->value() && distance <= m_param_distance3->value())
                                    {
                                        distance_str = "is-far-away-from";
                                    }
                                    if(!distance_str.isEmpty())
                                        datastream << "(related " << name << " " << neighbor_name << " " << distance_str << ")\n";
                                }
                            }
                        }
                        
                        //find out smoothness of each individual by  means of cluster smoothness:
                        float smoothness = measured_clusters->weight(i);
                        
                        if (smoothness > m_param_smoothness_threshold->value())
                        { 
                            datastream << "(instance " << name << " currentsmoothness-problem)\n";
                        }
                                                
                        QString request = datastream.readAll();
                        
                        if(filestream)
                        {
                            *filestream << request;
                        }
                        
                        emit statusMessage(i*100.0/measured_vf->size(), QString("creating A-Box from data"));
                    }	
                    
            
                    //ABox is written completely - close it
                    if(filestream)
                    {
                        filestream->flush();
                        delete filestream;
                        filestream = NULL;
                    }
            
            
                    emit statusMessage(100.0, QString("finished computation"));
                    emit finished();
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
    
    protected:
        /**
         * @{
         *
         * Additional parameters
         */
        ModelParameter * m_param_measured_vectorfield;
        ModelParameter * m_param_clusters;
        
        FloatParameter	* m_param_smoothness_threshold;
        
        FloatParameter	* m_param_velocity1;
        FloatParameter	* m_param_velocity2;
        FloatParameter	* m_param_velocity3;
        
        FloatParameter	* m_param_distance1;
        FloatParameter	* m_param_distance2;
        FloatParameter	* m_param_distance3;
        
        BoolParameter	* m_param_use_wind;
        ModelParameter * m_param_wind_vectorfield;
        
        BoolParameter	* m_param_use_modelled_currents;
        ModelParameter * m_param_modelled_vectorfield;
        
        FilenameParameter * m_param_abox_filename;
        /**
         * @}
         */
};

/** 
 * Creates one instance of the clustered vectorfield A-Box creation
 * algorithm defined above.
 *
 * \return A new instance of the ClusteredABox.
 */
Algorithm* createClusteredABox(Workspace* wsp)
{
	return new ClusteredABox(wsp);
}



/**
 * This class encapsulates all the functionality of this module in a 
 * way that it can be used within graipe. To achieve this, it extends
 * graipe's Module class by introducing new algorithms.
 */
class RacerClientModule
:   public Module
{
	public:
        /**
         * Default constructor for the RacerClientModule.
         */
		RacerClientModule()
		{
        }
    
        /**
         * Returns the ModelFactory of the RacerClientModule.
         * Since this module does not introduce any new Models, it will
         * return an empty ModelFactory.
         *
         * \return An empty ModelFactory of the RacerClientModule.
         */
		ModelFactory modelFactory() const
		{
			return ModelFactory();
		}
    
        /**
         * Returns the ViewControllerFactory of the RacerClientModule.
         * Since this module does not introduce any new Models,  no views are
         * needed. Thus, it will return an empty ViewControllerFactory.
         *
         * \return An empty ViewControllerFactory of the RacerClientModule.
         */
		ViewControllerFactory viewControllerFactory() const
		{
			return ViewControllerFactory();
		}
    
        /**
         * Returns the AlgorithmFactory of the RacerClientModule.
         * Here, we provide the tree algorithms defined above.
         *
         * \return An AlgorithmFactory containing the algorithms defined herein.
         */
        AlgorithmFactory algorithmFactory() const
		{
			AlgorithmFactory alg_factory;
										  
			AlgorithmFactoryItem alg_item;
			alg_item.topic_name = "High Level Vision adapters";
			
			
			//1. Interpret VF using Racer
			alg_item.algorithm_name = "measurements and vectorfields vs. A-Box (Racer)";
            alg_item.algorithm_type = "RacerInterpreter";
			alg_item.algorithm_fptr = &createRacerInterpreter;
			alg_factory.push_back(alg_item);
			
			//1. Interpret Clustered VF using Racer
			alg_item.algorithm_name = "Clustered measurements and vectorfields vs. A-Box (Racer)";	
            alg_item.algorithm_type = "RacerClusteredInterpreter";
			alg_item.algorithm_fptr = &createRacerClusteredInterpreter;
			alg_factory.push_back(alg_item);
			
			//1. Create Clustered VF ABox
			alg_item.algorithm_name = "Create Clustered measurements and vectorfields A-Box";	
            alg_item.algorithm_type = "ClusteredABox";
			alg_item.algorithm_fptr = &createClusteredABox;
			alg_factory.push_back(alg_item);
			
			return alg_factory;
		}
		
        /**
         * Returns the name of this Module.
         * 
         * \return Always: "RacerClientModule"
         */
		QString name() const
        {
            return "RacerClientModule";
        }
};


/**
 * @}
 */
 
} //end of namespace graipe

/**
 * @addtogroup graipe_racerclient
 * @{
 *
 */

#include <QtCore/QtGlobal>
extern "C"{
    /**
     *  The initialization procedure returns a pointer to the
     *  RacerClientModule (which inherits from Module) acutal
     *  implementation of the class above
     *
     *	\return The pointer to a new instance of this module.
     */
    Q_DECL_EXPORT graipe::Module* initialize()
	{
        return new graipe::RacerClientModule;
	}
} //extern "C"

/**
 * @}
 */
