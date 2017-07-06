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

#include "images/images.h"
#include "vectorfields/vectorfields.h"
#include "core/core.h"

#include "registration/warpingfunctors.hxx"

#include <vigra/affine_registration_fft.hxx>

#include <QElapsedTimer>

namespace graipe {


/**
 * This class implements the global motion estimation image->image algorithm 
 * as specialization of the graipe::Algorithm class
 */
class GlobalMotionCorrector
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        GlobalMotionCorrector(Workspace* wsp)
        : Algorithm(wsp)
        {
            m_param_imageBand1 = new ImageBandParameter<float>("Reference Image", NULL, false, wsp);
            m_param_imageBand2 = new ImageBandParameter<float>("Second Image",	NULL, false, wsp);
            
            m_parameters->addParameter("image1", m_param_imageBand1 );
            m_parameters->addParameter("image2", m_param_imageBand2 );
        }
    
        QString typeName() const
        {
            return "GlobalMotionCorrector";
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
                    
                    vigra::MultiArrayView<2,float> imageband1 =  m_param_imageBand1->value();
                    vigra::MultiArrayView<2,float> imageband2 =  m_param_imageBand2->value();
                    
                    vigra_precondition(imageband1.size() && imageband2.size(), "At least one image size is 0x0!");
                    vigra_precondition(imageband1.size()==imageband2.size(),   "Image sizes differ!");
                    
                    emit statusMessage(1.0, QString("started computation"));
                    
                    vigra::Matrix<double> mat(3,3);
                    double rotation_correlation;
                    double translation_correlation;
                    
                    QElapsedTimer timer;
                    timer.start();
                    
                    estimateGlobalRotationTranslation(imageband1,
                                                      imageband2,
                                                      mat,
                                                      rotation_correlation,
                                                      translation_correlation);
                    
                    Image<float>* displaced_image = new Image<float>(imageband2.shape(), m_param_imageBand1->image()->numBands(), m_workspace);
                    
                    for( unsigned int c=0; c < m_param_imageBand1->image()->numBands(); c++)
                    {
                        vigra::affineWarpImage(vigra::SplineImageView<3, float>(m_param_imageBand1->image()->band(c)),
                                               displaced_image->band(c),
                                               mat);
                    }
                    
                    qint64 processing_time = timer.elapsed();
                    
                    m_param_imageBand2->image()->copyMetadata(*displaced_image);
                    
                    displaced_image->setName("GME corrected image band " + m_param_imageBand1->toString());
                    
                    
                    QTransform transform(mat(0,0), mat(1,0), mat(2,0),
                                         mat(0,1), mat(1,1), mat(2,1),
                                         mat(0,2), mat(1,2), mat(2,2));

                    QString mat_str = "";//TODO:TransformParameter::valueText(transform);
                    
                    qDebug() << "GME: Matrix=" << transform << "\nMatString="<< mat_str <<"\n";
                    
                    QString descr = QString("First image band: %1\n"
                                            "Second image band: %2\n"
                                            "Computed global motion matrix (I1 -> I2): %3\n"
                                            "rotation accuracy: %4\n"
                                            "translation accuracy: %5\n"
                                            "processing time: %6 seconds").arg(m_param_imageBand1->toString()).arg(m_param_imageBand2->toString())
                                            .arg(mat_str).arg(rotation_correlation).arg(translation_correlation).arg(processing_time/1000.0);
                                        
                    displaced_image->setDescription(descr);
                    
                    m_results.push_back(displaced_image);
                    
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
        //Additional parameters
        ImageBandParameter<float> * m_param_imageBand1;
        ImageBandParameter<float> * m_param_imageBand2;
};

/** 
 * Creates one instance of the global motion estimation (image->image)
 * algorithm defined above.
 *
 * \return A new instance of the GlobalMotionCorrector.
 */
Algorithm* createGlobalMotionCorrector(Workspace* wsp)
{
    return new GlobalMotionCorrector(wsp);
}




/**
 * This templated class provides the base for all registration algorithms. The registration 
 * itself is given as the template argument (a functor) and will be called using the specialized
 * graipe::Algorithm class.
 */
template<class WARPING_FUNCTOR>
class GenericRegistration 
:	public Algorithm 
{
	public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
		GenericRegistration(Workspace* wsp)
        : Algorithm(wsp)
		{
			m_parameters->addParameter("image1", new ModelParameter("Image to be warped",  "Image", NULL, false, wsp));
			m_parameters->addParameter("image2", new ModelParameter("Reference Image",  "Image", NULL, false, wsp));
			m_parameters->addParameter("vf", new ModelParameter("Correspondence Map",  "SparseVectorfield2D", NULL, false, wsp));
		}
    
        QString typeName() const
        {
            return "GenericRegistration";
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
                    
                    ModelParameter	* param_image1 = static_cast<ModelParameter*> ((*m_parameters)["image1"]),
                                    * param_image2 = static_cast<ModelParameter*> ((*m_parameters)["image2"]),
                                    * param_vf = static_cast<ModelParameter*> ((*m_parameters)["vf"]);
                    
                    Image<float>* image1 = static_cast<Image<float>*>(  param_image1->value() );	
                    Image<float>* image2 = static_cast<Image<float>*>(  param_image2->value() );	
                    
                    Vectorfield2D* vf =  static_cast<Vectorfield2D*> (param_vf->value());
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    WARPING_FUNCTOR func_a;
                    
                    Image<float>* new_image = new Image<float>(image2->size(), image1->numBands(), m_workspace);
                    
                    vigra::MultiArray<2,float> res_img_band(image2->size());
                    std::vector<vigra::TinyVector<double,2> > src_points(vf->size()), dest_points(vf->size());
            
                    for(unsigned int i=0; i<vf->size(); ++i)
                    {
                        src_points[i][0] = vf->origin(i).x();
                        src_points[i][1] = vf->origin(i).x();
                
                        dest_points[i][0] = vf->target(i).x();
                        dest_points[i][1] = vf->target(i).y();
                    }
                    
                    for(unsigned int c=0; c<image1->numBands(); c++)
                    {
                        func_a(image1->band(c), new_image->band(c), src_points.begin(), src_points.end(), dest_points.begin());
            
                    }
                    
                    new_image->setName(func_a.name() + QString(" of ") + image1->name() + QString(" to ") + image2->name());
                    QString descr("The following components were used to compute the ");
                    
                    descr += func_a.name() + QString(":\n");
                    descr +=  QString("First image: ") + image1->name()  + QString("\n");
                    descr +=  QString("Reference image: ") + image2->name()  + QString("\n");
                    descr +=  QString("Correspondence vectorfield: ") + vf->name()  + QString("\n");
                    
                    new_image->setDescription(descr);
                    
                    image2->copyGeometry(*new_image);
                    
                    m_results.push_back(new_image);
                    
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
};

template<>
QString GenericRegistration<WarpAffineFunctor>::typeName() const
{
    return "AffineRegistration";
}

/**
 * Creates one instance of the point->point affine registration
 * algorithm using the framework defined above.
 *
 * \return A new instance of the GenericRegistration<WarpAffineFunctor>.
 */
Algorithm* createAffineRegistration(Workspace* wsp)
{
	return new GenericRegistration<WarpAffineFunctor>(wsp);
}

template<>
QString GenericRegistration<WarpProjectiveFunctor>::typeName() const
{
    return "ProjectiveRegistration";
}

/** 
 * Creates one instance of the point->point projective registration
 * algorithm using the framework defined above.
 *
 * \return A new instance of the GenericRegistration<WarpProjectiveFunctor>.
 */
Algorithm* createProjectiveRegistration(Workspace* wsp)
{
	return new GenericRegistration<WarpProjectiveFunctor>(wsp);
}

template<>
QString GenericRegistration<WarpBilinearFunctor>::typeName() const
{
    return "BilinearRegistration";
}

/** 
 * Creates one instance of the point->point bilinear registration
 * algorithm using the framework defined above.
 *
 * \return A new instance of the GenericRegistration<WarpBilinearFunctor>.
 */
Algorithm* createBilinearRegistration(Workspace* wsp)
{
	return new GenericRegistration<WarpBilinearFunctor>(wsp);
}

template<>
QString GenericRegistration<WarpBiquadraticFunctor>::typeName() const
{
    return "BiquadraticRegistration";
}

/** 
 * Creates one instance of the point->point biquadratic registration
 * algorithm using the framework defined above.
 *
 * \return A new instance of the GenericRegistration<WarpBiquadraticFunctor>.
 */
Algorithm* createBiquadraticRegistration(Workspace* wsp)
{
	return new GenericRegistration<WarpBiquadraticFunctor>(wsp);
}

template<>
QString GenericRegistration<WarpBicubicFunctor>::typeName() const
{
    return "GBicubicRegistration";
}

/** 
 * Creates one instance of the point->point bicubic registration
 * algorithm using the framework defined above.
 *
 * \return A new instance of the GenericRegistration<WarpBicubicFunctor>.
 */
Algorithm* createBicubicRegistration(Workspace* wsp)
{
	return new GenericRegistration<WarpBicubicFunctor>(wsp);
}

template<>
QString GenericRegistration<WarpPiecewiseAffineFunctor>::typeName() const
{
    return "PiecewiseAffineRegistration";
}
/** 
 * Creates one instance of the point->point piecewise affine registration
 * algorithm using the framework defined above.
 *
 * \return A new instance of the GenericRegistration<WarpPiecewiseAffineFunctor>.
 */
Algorithm* createPiecewiseAffineRegistration(Workspace* wsp)
{
	return new GenericRegistration<WarpPiecewiseAffineFunctor>(wsp);
}

template<>
QString GenericRegistration<WarpRBF1Functor>::typeName() const
{
    return "RBF1Registration";
}

/** 
 * Creates one instance of the point->point radial basis registration
 * algorithm using a linear distance power functor and the framework defined above.
 *
 * \return A new instance of the GenericRegistration<WarpRadialBasisFunctor<vigra::DistancePowerFunctor<1> > >.
 */
Algorithm* createRBF1Registration(Workspace* wsp)
{
	return new GenericRegistration<WarpRBF1Functor>(wsp);
}

template<>
QString GenericRegistration<WarpRBF2Functor>::typeName() const
{
    return "RBF2Registration";
}

/** 
 * Creates one instance of the point->point radial basis registration
 * algorithm using a quadratic distance power functor and the framework defined above.
 *
 * \return A new instance of theGenericRegistration<WarpRadialBasisFunctor<vigra::DistancePowerFunctor<2> > >.
 */
Algorithm* createRBF2Registration(Workspace* wsp)
{
	return new GenericRegistration<WarpRBF2Functor>(wsp);
}
template<>
QString GenericRegistration<WarpRBF3Functor>::typeName() const
{
    return "RBF3Registration";
}
/** 
 * Creates one instance of the point->point radial basis registration
 * algorithm using a cubic distance power functor and the framework defined above.
 *
 * \return A new instance of the GenericRegistration<WarpRadialBasisFunctor<vigra::DistancePowerFunctor<3> > >.
 */
Algorithm* createRBF3Registration(Workspace* wsp)
{
	return new GenericRegistration<WarpRBF3Functor>(wsp);
}

template<>
QString GenericRegistration<WarpRadialBasisFunctor<vigra::ThinPlateSplineFunctor> >::typeName() const
{
    return "TPSRegistration";
}
/** 
 * Creates one instance of the point->point radial basis registration
 * algorithm using a thin plate spline functor and the framework defined above.
 *
 * \return A new instance of the GenericRegistration<WarpRadialBasisFunctor<vigra::ThinPlateSplineFunctor> >.
 */
Algorithm* createTPSRegistration(Workspace* wsp)
{
	return new GenericRegistration<WarpRadialBasisFunctor<vigra::ThinPlateSplineFunctor> >(wsp);
}




/**
 * This class encapsulates all the functionality of this module in a 
 * way that it can be used within graipe. To achieve this, it extends
 * graipe's Module class by introducing new algorithms.
 */
class RegistrationModule
:   public Module
{
	public:
        /**
         * Default constructor for the RegistrationModule.
         */
		RegistrationModule()
		{
        }
    
        /**
         * Returns the ModelFactory of the RegistrationModule.
         * Since this module does not introduce any new Models, it will
         * return an empty ModelFactory.
         *
         * \return An empty ModelFactory of the RegistrationModule.
         */
		ModelFactory modelFactory() const
		{
			return ModelFactory();
		}
    
        /**
         * Returns the ViewControllerFactory of the RegistrationModule.
         * Since this module does not introduce any new Models,  no views are
         * needed. Thus, it will return an empty ViewControllerFactory.
         *
         * \return An empty ViewControllerFactory of the RegistrationModule.
         */
		ViewControllerFactory viewControllerFactory() const
		{
			return ViewControllerFactory();
		}
		
        /**
         * Returns the AlgorithmFactory of the RegistrationModule.
         * Here, we provide the algorithms defined above.
         *
         * \return An AlgorithmFactory containing the analysis algorithms defined herein.
         */
		AlgorithmFactory algorithmFactory() const
		{
			AlgorithmFactory alg_factory;
			
			AlgorithmFactoryItem alg_item;
			alg_item.topic_name = "Registration (image -> image)";
            
            //1. Global Motion Correction
            alg_item.algorithm_name = "Global motion correction (rot. + transl.)";
            alg_item.algorithm_type = "GlobalMotionCorrector";
            alg_item.algorithm_fptr = &createGlobalMotionCorrector;
            alg_factory.push_back(alg_item);
            
            
            
            
            alg_item.topic_name = "Registration (points -> points)";
			
			//1. Affine Registration
			alg_item.algorithm_name = "Affine registration";
            alg_item.algorithm_type = "AffineRegistration";
			alg_item.algorithm_fptr = &createAffineRegistration;
			alg_factory.push_back(alg_item);	
			
			//2. Projective Registration
			alg_item.algorithm_name = "Projective registration";
            alg_item.algorithm_type = "ProjectiveRegistration";
			alg_item.algorithm_fptr = &createProjectiveRegistration;
			alg_factory.push_back(alg_item);
			
			//3. Bilinear Registration
			alg_item.algorithm_name = "Bilinear registration";
            alg_item.algorithm_type = "BilinearRegistration";
			alg_item.algorithm_fptr = &createBilinearRegistration;
			alg_factory.push_back(alg_item);	
			
			//4. Biquadratic Registration
			alg_item.algorithm_name = "Biquadratic registration";
            alg_item.algorithm_type = "BiquadraticRegistration";
			alg_item.algorithm_fptr = &createBiquadraticRegistration;
			alg_factory.push_back(alg_item);	
			
			//5. Bicubic Registration
			alg_item.algorithm_name = "Bicubic registration";
            alg_item.algorithm_type = "BicubicRegistration";
			alg_item.algorithm_fptr = &createBicubicRegistration;
			alg_factory.push_back(alg_item);	
			
			//6. Piecewise affine Registration
			alg_item.algorithm_name = "Piecewise affine registration";
            alg_item.algorithm_type = "PiecewiseAffineRegistration";
			alg_item.algorithm_fptr = &createPiecewiseAffineRegistration;
			alg_factory.push_back(alg_item);	
			
			//7. RBF linear Registration
			alg_item.algorithm_name = "RBF linear registration";
            alg_item.algorithm_type = "RBF1Registration";
			alg_item.algorithm_fptr = &createRBF1Registration;
			alg_factory.push_back(alg_item);
						
			//8. RBF quadratic Registration
			alg_item.algorithm_name = "RBF quadratic registration";
            alg_item.algorithm_type = "RBF2Registration";
			alg_item.algorithm_fptr = &createRBF2Registration;
			alg_factory.push_back(alg_item);
			
			//9. RBF cubic Registration
			alg_item.algorithm_name = "RBF cubic registration";
            alg_item.algorithm_type = "RBF3Registration";
			alg_item.algorithm_fptr = &createRBF3Registration;
			alg_factory.push_back(alg_item);
			
			//10. RBF TPS Registration
			alg_item.algorithm_name = "Thin plate spline registration";	
            alg_item.algorithm_type = "TPSRegistration";
			alg_item.algorithm_fptr = &createTPSRegistration;
			alg_factory.push_back(alg_item);	
			
			return alg_factory;
		}
		
        /**
         * Returns the name of this Module.
         * 
         * \return Always: "RegistrationModule"
         */
		QString name() const
		{
			return "RegistrationModule";
		}
};

} //end of namespace graipe

/**
 *Interface to the ModuleHandler
 */

/**
 *  The initialization procedure returns a pointer to the
 *  RegistrationModule (which inherits from Module) acutal
 *  implementation of the class above
 *
 *	\return The pointer to a new instance of this module.
 */

#include <QtCore/QtGlobal>
extern "C"{
    Q_DECL_EXPORT graipe::Module* initialize()
	{
        return new graipe::RegistrationModule;
	}
}




