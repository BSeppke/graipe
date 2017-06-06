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
#include "analysis/analysis.h"
#include <vigra/separableconvolution.hxx>

#include "core/core.h"

namespace graipe {

/**
 * Simple algorithm to compute the mean of several dense vectorfields
 */
class MeanVectorfield
:   public Algorithm
{
    public:
        /**
         * Default constructor for the dense mean vectorfield computation.
         * Initializes only one parameter - the vectorfield selection
         */
        MeanVectorfield()
        {
            m_parameters->addParameter("vf", new MultiModelParameter("(Dense) Vectorfields",  "DenseVectorfield2D"));
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
                    
                    MultiModelParameter		* param_vectorfields      = static_cast<MultiModelParameter*> ((*m_parameters)["vf"]);
                    
                    std::vector<Model*> selected_vectorfields = param_vectorfields->value();
                    
                    if (selected_vectorfields.size() == 0)
                    {
                        emit errorMessage(QString("Explainable error occured: No vectorfields have been selected"));
                        
                    }
                    else
                    {
                        
                        //for each pair of images: add them				
                        emit statusMessage(1.0, QString("starting computation"));
                        
                        //take the first image as a master for size and channels
                        DenseVectorfield2D* vf = static_cast<DenseVectorfield2D*>( selected_vectorfields[0] );
                        
                        unsigned int ref_w = vf->width(), ref_h = vf->height();
                        vigra::MultiArray<2,float>temp_u(ref_w,ref_h), temp_v(ref_w,ref_h);
                        
                        //iterate ->add vfs
                        for(unsigned int i = 0; i < selected_vectorfields.size(); ++i)
                        {
                            
                            vf = static_cast<DenseVectorfield2D*>( selected_vectorfields[i] );
                            
                            vigra_precondition(vf->width() == ref_w && vf->height() == ref_h, "vectorfields are of different size");
                            
                            temp_u += vf->u();
                            temp_v += vf->v();
                        }
                        
                        //divide by count
                        temp_u /= selected_vectorfields.size();
                        temp_v /= selected_vectorfields.size();
                        
                        //create new image to do the addition
                        DenseVectorfield2D* new_vf = new DenseVectorfield2D(temp_u, temp_v);
                        
                        //Copy all metadata from current image (will be overwritten later)
                        vf->copyMetadata(*new_vf);
                        new_vf->setName(QString("mean vectorfield of ") + param_vectorfields->toString());
                                        
                        m_results.push_back(new_vf);
                        
                        emit statusMessage(100.0, QString("finished computation"));
                        emit finished();
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
};
/**
 * Creates a dense mean vectorfield algorithm instance.
 *
 * \return A new instance of the dense mean vectorfield algorithm.
 */
Algorithm* createMeanVectorfield()
{
	return new MeanVectorfield;
}



/**
 * An algorithm for the Vectorfield comparision
 * by means of one vectorfield (any kind) w.r.t. a dense reference vectorfield
 */
class Vectorfield2DDenseModelComparison
:   public Algorithm
{
	
    public:
        /**
         * Default constructor. Initialized additional parameters.
         */
        Vectorfield2DDenseModelComparison()
        {
            m_parameters->addParameter("vf1", new ModelParameter("Derived Vectorfield",	"SparseVectorfield2D | SparseWeightedVectorfield2D | SparseMultiVectorfield2D | SparseWeightedMultiVectorfield2D | DenseVectorfield2D | DenseWeightedVectorfield2D"));
            m_parameters->addParameter("vf2", new ModelParameter("Reference Vectorfield to compare with", "DenseVectorfield2D | DenseWeightedVectorfield2D"));
            m_parameters->addParameter("degree", new IntParameter("Use spline interpolation of degree", 0, 5,1));
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
                    
                    ModelParameter	* param_vf = static_cast<ModelParameter*> ( (*m_parameters)["vf1"]),
                                        * param_reference_vf = static_cast<ModelParameter*> ((*m_parameters)["vf2"]);
                    
                    IntParameter		* param_interpolation  =  static_cast<IntParameter*> ((*m_parameters)["degree"]);
                    
                    Vectorfield2D*		first_vf     = static_cast<Vectorfield2D*>(param_vf->value());
                    DenseVectorfield2D* reference_vf = static_cast<DenseVectorfield2D*>(param_reference_vf->value());
                    
                    vigra_precondition(first_vf		!= NULL, "bad first vf pointer");
                    vigra_precondition(reference_vf != NULL, "bad reference vf pointer");
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    AverageAngularErrorFunctor f_aae;
                    QString report_aae;
                    WeightedPointFeatureList2D* comparison_aae;
                    
                    switch (param_interpolation->value()) {
                        case 0:
                            comparison_aae = compareVectorfieldToDenseModel<0>(first_vf, reference_vf, f_aae, report_aae);
                            break;
                        case 1:
                            comparison_aae = compareVectorfieldToDenseModel<1>(first_vf, reference_vf, f_aae, report_aae);
                            break;
                        case 2:
                            comparison_aae = compareVectorfieldToDenseModel<2>(first_vf, reference_vf, f_aae, report_aae);
                            break;
                        case 3:
                            comparison_aae = compareVectorfieldToDenseModel<3>(first_vf, reference_vf, f_aae, report_aae);
                            break;
                        case 4:
                            comparison_aae = compareVectorfieldToDenseModel<4>(first_vf, reference_vf, f_aae, report_aae);
                            break;
                        case 5:
                            comparison_aae = compareVectorfieldToDenseModel<5>(first_vf, reference_vf, f_aae, report_aae);
                            break;
                        default:
                            comparison_aae = compareVectorfieldToDenseModel<4>(first_vf, reference_vf, f_aae, report_aae);
                            break;
                    }
                            
                    comparison_aae->setName(f_aae.shortName() + QString(" Vectorfield comparison: ") + first_vf->name() + QString(" and ") + reference_vf->name());
                    
                    QString descr("The following parameters were used for comparison:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    comparison_aae->setDescription(descr + report_aae);
                    
                    first_vf->copyGeometry(*comparison_aae);
                    
                    m_results.push_back(comparison_aae);
                    
                    emit statusMessage(50.0, QString("half-way done"));
                    
                    AverageVelocityErrorFunctor f_ave;
                    QString report_ave;
                    WeightedPointFeatureList2D* comparison_ave;
                    
                    switch (param_interpolation->value()) {
                        case 0:
                            comparison_ave = compareVectorfieldToDenseModel<0>(first_vf, reference_vf, f_ave, report_ave);
                            break;
                        case 1:
                            comparison_ave = compareVectorfieldToDenseModel<1>(first_vf, reference_vf, f_ave, report_ave);
                            break;
                        case 2:
                            comparison_ave = compareVectorfieldToDenseModel<2>(first_vf, reference_vf, f_ave, report_ave);
                            break;
                        case 3:
                            comparison_ave = compareVectorfieldToDenseModel<3>(first_vf, reference_vf, f_ave, report_ave);
                            break;
                        case 4:
                            comparison_ave = compareVectorfieldToDenseModel<4>(first_vf, reference_vf, f_ave, report_ave);
                            break;
                        case 5:
                            comparison_ave = compareVectorfieldToDenseModel<5>(first_vf, reference_vf, f_ave, report_ave);
                            break;
                        default:
                            comparison_ave = compareVectorfieldToDenseModel<4>(first_vf, reference_vf, f_ave, report_ave);
                            break;
                    }
                    comparison_ave->setName(f_ave.shortName() + QString(" Vectorfield comparison: ") + first_vf->name() + QString(" and ") + reference_vf->name());
                    comparison_ave->setDescription(descr + report_ave);
                    
                    first_vf->copyGeometry(*comparison_ave);
                    
                    m_results.push_back(comparison_ave);
                    
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

/** 
 * Creates one instance of the vectorfield -> dense vectorfield comparison
 * algorithm defined above.
 *
 * \return A new instance of the Vectorfield2DDenseModelComparison.
 */
Algorithm* createVectorfield2DDenseModelComparison()
{
	return new Vectorfield2DDenseModelComparison;
};




/**
 * An algorithm for a more generic Vectorfield comparision
 * by means of two vectorfields of any kind
 */
class Vectorfield2DGenericComparison
:   public Algorithm
{
	
    public:
        /**
         * Default constructor. Initialized additional parameters.
         */
        Vectorfield2DGenericComparison()
        {
            m_parameters->addParameter("vf1", new ModelParameter("First Vectorfield", "SparseVectorfield2D | SparseWeightedVectorfield2D | SparseMultiVectorfield2D | SparseWeightedMultiVectorfield2D | DenseVectorfield2D | DenseWeightedVectorfield2D"));
            m_parameters->addParameter("vf2", new ModelParameter("Reference Vectorfield to compare with",  "SparseVectorfield2D | SparseWeightedVectorfield2D | SparseMultiVectorfield2D | SparseWeightedMultiVectorfield2D | DenseVectorfield2D | DenseWeightedVectorfield2D"));
            m_parameters->addParameter("n", new IntParameter("use n nearest neighbors in reference vf", 0, 100));
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
                    
                    ModelParameter	* param_vf = static_cast<ModelParameter*> ((*m_parameters)["vf1"]),
                                        * param_reference_vf = static_cast<ModelParameter*> ((*m_parameters)["vf2"]);
                    
                    IntParameter		* param_n_neighbors  =  static_cast<IntParameter*> ((*m_parameters)["n"]);
                    
                    Vectorfield2D* first_vf     = static_cast<Vectorfield2D*>(param_vf->value());
                    Vectorfield2D* reference_vf = static_cast<Vectorfield2D*>(param_reference_vf->value());
                    
                    vigra_precondition(first_vf		!= NULL, "bad first vf pointer");
                    vigra_precondition(reference_vf != NULL, "bad reference vf pointer");
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    AverageAngularErrorFunctor f_aae;
                    QString report_aae;
                    WeightedPointFeatureList2D* comparison_aae =  compareVectorfieldsGeneric(first_vf, reference_vf, param_n_neighbors->value(), f_aae, report_aae);
                    
                    comparison_aae->setName(f_aae.shortName() + QString(" Vectorfield comparison: ") + first_vf->name() + QString(" and ") + reference_vf->name());
                
                    QString descr("The following parameters were used for comparison:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    
                    comparison_aae->setDescription(descr + report_aae);
                    
                    first_vf->copyGeometry(*comparison_aae);
                    
                    m_results.push_back(comparison_aae);
                    
                    
                    emit statusMessage(50.0, QString("half-way done"));
                    
                    
                    AverageVelocityErrorFunctor f_ave;
                    QString report_ave;
                    WeightedPointFeatureList2D* comparison_ave =  compareVectorfieldsGeneric(first_vf, reference_vf, param_n_neighbors->value(), f_ave, report_ave);
                    
                    comparison_ave->setName(f_ave.shortName() + QString(" Vectorfield comparison: ") + first_vf->name() + QString(" and ") + reference_vf->name());
                    comparison_ave->setDescription(descr + report_ave);
                    
                    first_vf->copyGeometry(*comparison_ave);
                    
                    m_results.push_back(comparison_ave);
                    
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

/** 
 * Creates one instance of the generic vectorfield -> vectorfield
 * comparison algorithm defined above.
 *
 * \return A new instance of the Vectorfield2DGenericComparison.
 */
Algorithm* createVectorfield2DGenericComparison()
{
	return new Vectorfield2DGenericComparison;
};




/**
 * This algorithm estimates the global motion of any vectorfield and resturns 
 * a separated version of the vectorfield, where the global motion is stored, too.
 */
class Vectorfield2DSeparation
:   public Algorithm
{
    public:
        /**
         * Default constructor. Initializes only one additional parameter, the vectorfield.
         */
        Vectorfield2DSeparation()
        {
            m_parameters->addParameter("vf", new ModelParameter("Vectorfield", "SparseVectorfield2D | SparseWeightedVectorfield2D | SparseMultiVectorfield2D | SparseWeightedMultiVectorfield2D | DenseVectorfield2D | DenseWeightedVectorfield2D"));
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
                    
                    ModelParameter	* param_vf = static_cast<ModelParameter*> ((*m_parameters)["vf"]);
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    if(param_vf->value()->typeName() == "SparseVectorfield2D")
                    {
                        SparseVectorfield2D* result_vf = computeGlobalMotionOfVectorfield(static_cast<SparseVectorfield2D*>(param_vf->value()));
                        m_results.push_back(result_vf);
                    }
                    else if(param_vf->value()->typeName() == "SparseWeightedVectorfield2D")
                    {
                        SparseWeightedVectorfield2D* result_vf = computeGlobalMotionOfVectorfield(static_cast<SparseWeightedVectorfield2D*>(param_vf->value()));
                        m_results.push_back(result_vf);
                    }
                    else if(param_vf->value()->typeName() == "SparseMultiVectorfield2D")
                    {
                        SparseMultiVectorfield2D* result_vf = computeGlobalMotionOfVectorfield(static_cast<SparseMultiVectorfield2D*>(param_vf->value()));
                        m_results.push_back(result_vf);
                    }
                    else if(param_vf->value()->typeName() == "SparseWeightedMultiVectorfield2D")
                    {
                        SparseWeightedMultiVectorfield2D* result_vf = computeGlobalMotionOfVectorfield(static_cast<SparseWeightedMultiVectorfield2D*>(param_vf->value()));
                        m_results.push_back(result_vf);
                    }
                    else if(param_vf->value()->typeName() == "DenseVectorfield2D")
                    {
                        DenseVectorfield2D* result_vf = computeGlobalMotionOfVectorfield(static_cast<DenseVectorfield2D*>(param_vf->value()));
                        m_results.push_back(result_vf);
                    }
                    else if(param_vf->value()->typeName() == "DenseWeightedVectorfield2D")
                    {
                        DenseWeightedVectorfield2D* result_vf = computeGlobalMotionOfVectorfield(static_cast<DenseWeightedVectorfield2D*>(param_vf->value()));
                        m_results.push_back(result_vf);
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
};

/** 
 * Creates one instance of the vectorfield separation (gme-adding)
 * algorithm defined above.
 *
 * \return A new instance of the Vectorfield2DSeparation.
 */
Algorithm* createVectorfield2DSeparation()
{
	return new Vectorfield2DSeparation;
};




/**
 * This algorithm estimates the curl of a given dense vectorfield as an output image.
 */
class Vectorfield2DCurl
:   public Algorithm
{
    public:
        /**
         * Default constructor. Initializes only one additional parameter, the vectorfield.
         */
        Vectorfield2DCurl()
        {
            m_parameters->addParameter("vf", new ModelParameter("Vectorfield", "DenseVectorfield2D | DenseWeightedVectorfield2D"));
            m_parameters->addParameter("sigma", new FloatParameter("Sigma for gaussian gradient:",	0.5, 100, 1));
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
                    
                    ModelParameter	* param_vf = static_cast<ModelParameter*> ((*m_parameters)["vf"]);
                    FloatParameter	* param_sigma = static_cast<FloatParameter*> ((*m_parameters)["sigma"]);
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    DenseVectorfield2D* vf = static_cast<DenseVectorfield2D*>(param_vf->value());
                    
                    unsigned int w = vf->width(),
                                 h = vf->height();
                    
                    Image<float>* img = new Image<float>(Image<float>::Size_Type(w,h), 1);
                    vf->copyGeometry(*img);
                    
                    vigra::Kernel1D<double> kernel;
                    kernel.initGaussianDerivative(param_sigma->value(),1);
                    
                    vigra::MultiArray<2,float>m_uy(w,h), m_vx(w,h), res(w,h);
                    
                    vigra::separableConvolveY(vf->u(), m_uy, kernel);
                    vigra::separableConvolveX(vf->v(), m_vx, kernel);
                    
                    for (unsigned int y=0 ; y < h; ++y)
                    {
                        for (unsigned int x=0 ; x < w; ++x)
                        {
                            res(x,y)  = m_vx(x,y) - m_uy(x,y);
                        }
                    }
                    
                    img->setBand(0,res);
                    img->setName("Scalar Curl of: " + vf->name());
                    img->setDescription("A gaussian sigma of: " + param_sigma->toString() + " has been used to derive the gradents of the vectorfield.");
                    
                    m_results.push_back(img);
                    
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

/** 
 * Creates one instance of the vectorfield curl extraction
 * algorithm defined above.
 *
 * \return A new instance of the Vectorfield2DCurl.
 */
Algorithm* computeVectorfield2DCurl()
{
	return new Vectorfield2DCurl;
};




/**
 * This algorithm estimates the divergence of a given dense vectorfield as an output image.
 */
class Vectorfield2DDiv
:   public Algorithm
{
    public:
        /**
         * Default constructor. Initializes only one additional parameter, the vectorfield.
         */
        Vectorfield2DDiv()
        {
            m_parameters->addParameter("vf", new ModelParameter("Vectorfield", "DenseVectorfield2D | DenseWeightedVectorfield2D"));
            m_parameters->addParameter("sigma", new FloatParameter("Sigma for gaussian gradient:",	0.5, 100, 1));
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
                    
                    ModelParameter	* param_vf = static_cast<ModelParameter*> ((*m_parameters)["vf"]);
                    FloatParameter	* param_sigma = static_cast<FloatParameter*> ((*m_parameters)["sigma"]);
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    DenseVectorfield2D* vf = static_cast<DenseVectorfield2D*>(param_vf->value());
                    
                    unsigned int w = vf->width(),
                                 h = vf->height();
                    
                    Image<float>* img = new Image<float>(Image<float>::Size_Type(w,h), 1);
                    vf->copyGeometry(*img);
                    
                    vigra::Kernel1D<double> kernel;
                    kernel.initGaussianDerivative(param_sigma->value(),1);
                    
                    vigra::MultiArray<2,float> m_ux(w,h), m_vy(w,h), res(w,h);
                    
                    vigra::separableConvolveX(vf->u(), m_ux, kernel);
                    vigra::separableConvolveY(vf->v(), m_vy, kernel);
                    
                    for (unsigned int y=0 ; y < h; ++y)
                    {
                        for (unsigned int x=0 ; x < w; ++x)
                        {
                            res(x,y)  = m_ux(x,y) + m_vy(x,y);
                        }
                    }
                    
                    img->setBand(0,res);
                    img->setName("Divergence of: " + vf->name());
                    img->setDescription("A gaussian sigma of: " + param_sigma->toString() + " has been used to derive the gradents of the vectorfield.");
                    
                    m_results.push_back(img);
                    
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

/** 
 * Creates one instance of the vectorfield divergence extraction
 * algorithm defined above.
 *
 * \return A new instance of the Vectorfield2DDiv.
 */
Algorithm* computeVectorfield2DDiv()
{
	return new Vectorfield2DDiv;
};




/**
 * This class encapsulates all the functionality of this module in a 
 * way that it can be used within graipe. To achieve this, it extends
 * graipe's Module class by introducing new algorithms.
 */
class AnalysisModule
:   public Module
{
    public:
        /**
         * Default constructor of the AnalysisModule.
         */
		AnalysisModule()
		{
        }
	

        /**
         * Returns the ModelFactory of the AnalysisModule.
         * Since this module does not introduce any new Models, it will
         * return an empty ModelFactory.
         *
         * \return An empty ModelFactory of the AnalysisModule.
         */
		ModelFactory modelFactory() const
		{
			return ModelFactory();
		}
				
        /**
         * Returns the ViewControllerFactory of the AnalysisModule.
         * Since this module does not introduce any new Models,  no views are
         * needed. Thus, it will return an empty ViewControllerFactory.
         *
         * \return An empty ViewControllerFactory of the AnalysisModule.
         */
		ViewControllerFactory viewControllerFactory() const
		{
			return ViewControllerFactory();
		}
    
        /**
         * Returns the AlgorithmFactory of the AnalysisModule.
         * Here, we provide the tree algorithms defined above.
         *
         * \return An AlgorithmFactory containing the analysis algorithms defined herein.
         */
		AlgorithmFactory algorithmFactory() const
		{
			AlgorithmFactory alg_factory;
			
			AlgorithmFactoryItem alg_item;
			alg_item.topic_name = "Analysis";
			
			//1. specialized vectorfield -> dense model vf comparison method
			alg_item.algorithm_name = "Vectorfield Comparison w.r.t. dense model data";	
			alg_item.algorithm_fptr = &createVectorfield2DDenseModelComparison;
			alg_factory.push_back(alg_item);
			
			//2. generic vectorfield -> vectorfield comparison method
			alg_item.algorithm_name = "Generic Vectorfield Comparison";	
			alg_item.algorithm_fptr = &createVectorfield2DGenericComparison;
			alg_factory.push_back(alg_item);
			
			
			//3. vectorfield separation -> estimation of affine motion from vf
			alg_item.algorithm_name = "Generic Vectorfield Separation";	
			alg_item.algorithm_fptr = &createVectorfield2DSeparation;
			alg_factory.push_back(alg_item);
			
			alg_item.algorithm_name = "Create mean vectorfield";	
			alg_item.algorithm_fptr = &createMeanVectorfield;
			alg_factory.push_back(alg_item);
            
			alg_item.algorithm_name = "Compute curl of vectorfield";
			alg_item.algorithm_fptr = &computeVectorfield2DCurl;
			alg_factory.push_back(alg_item);
			
            
			alg_item.algorithm_name = "Compute divergence of vectorfield";
			alg_item.algorithm_fptr = &computeVectorfield2DDiv;
			alg_factory.push_back(alg_item);
			
			return alg_factory;
		}		
    
        /**
         * Returns the name of this Module.
         * 
         * \return Always: "AnalysisModule"
         */
		QString name() const
        {
            return "AnalysisModule";
        }
};

} //end of namespace graipe


/**
 *Interface to the ModuleHandler
 */

/**
 *  The initialization procedure returns a pointer to the
 *  FeatureDetectionModule (which inherits from Module) acutal
 *  implementation of the class above
 *
 *	@return the pointer to the plugin 
 */
#include <QtCore/QtGlobal>
extern "C"{
    Q_DECL_EXPORT graipe::Module* initialize()
	{
        return new graipe::AnalysisModule;
	}
}



