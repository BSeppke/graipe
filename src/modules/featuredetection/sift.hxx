#ifndef GRAIPE_FEATUREDETECTION_SIFT_HXX
#define GRAIPE_FEATUREDETECTION_SIFT_HXX

#include <vector>
#include <list>
#include <string>
#include <algorithm>

#include <vigra/impex.hxx>
#include <vigra/multi_array.hxx>
#include <vigra/convolution.hxx>
#include <vigra/multi_math.hxx>
#include <vigra/linear_algebra.hxx>
#include <vigra/splineimageview.hxx>

namespace graipe {

struct SIFTFeature
{
    vigra::TinyVector<float,2> position;
    float scale;
    float contrast;
    float orientation;
    std::vector<float> descriptor;
};

/**
 * Inline function to determine if there is a local extremum or not
 * present in the DoG stack...
 *
 * \param dog Constant reference to the DoG stack.
 * \param i The scale, where we want to look for an extremum.
 * \param x The x-position, where we want to look for an extremum.
 * \param y The y-position, where we want to look for an extremum.
 * \return True, if a local minimum or maximum is found.
 */
template <class T>
inline bool localExtremum(const std::vector<vigra::MultiArray<2, T> > & dog, int i, int x, int y)
{
    const vigra::MultiArray<2, float> & prev = dog[i-3];
    const vigra::MultiArray<2, float> & curr = dog[i-2];
    const vigra::MultiArray<2, float> & next = dog[i-1];
    
    float v = curr(x,y);
    return (    (    v < curr(x-1,y-1) && v < curr(x,y-1) && v < curr(x+1,y-1)
                 &&  v < curr(x-1,y)                      && v < curr(x+1,y)
                 &&  v < curr(x-1,y+1) && v < curr(x,y+1) && v < curr(x+1,y+1)
                 
                 &&  v < prev(x-1,y-1) && v < prev(x,y-1) && v < prev(x+1,y-1)
                 &&  v < prev(x-1,y)   && v < prev(x-1,y) && v < prev(x+1,y)
                 &&  v < prev(x-1,y+1) && v < prev(x,y+1) && v < prev(x+1,y+1)
                 
                 &&  v < next(x-1,y-1) && v < next(x,y-1) && v < next(x+1,y-1)
                 &&  v < next(x-1,y)   && v < next(x-1,y) && v < next(x+1,y)
                 &&  v < next(x-1,y+1) && v < next(x,y+1) && v < next(x+1,y+1))
            
            ||  (    v > curr(x-1,y-1) && v > curr(x,y-1) && v > curr(x+1,y-1)
                 &&  v > curr(x-1,y)                      && v > curr(x+1,y)
                 &&  v > curr(x-1,y+1) && v > curr(x,y+1) && v > curr(x+1,y+1)
                 
                 &&  v > prev(x-1,y-1) && v > prev(x,y-1) && v > prev(x+1,y-1)
                 &&  v > prev(x-1,y)   && v > prev(x-1,y) && v > prev(x+1,y)
                 &&  v > prev(x-1,y+1) && v > prev(x,y+1) && v > prev(x+1,y+1)
                 
                 &&  v > next(x-1,y-1) && v > next(x,y-1) && v > next(x+1,y-1)
                 &&  v > next(x-1,y)   && v > next(x-1,y) && v > next(x+1,y)
                 &&  v > next(x-1,y+1) && v > next(x,y+1) && v > next(x+1,y+1)));
}

/**
 * Inline function to adjust a local feature at the dog stack with respect to
 * subpixel accuracy. Since we compute the Hessian matrix as well as some gradients here,
 * we will also perform the check for contrast and curvature_thresholds here.
 *
 * \param dog Constant reference to the DoG stack.
 * \param feature Non-const reference to the sift feature. Position and scale will be changed by
 *        this function if possible.
 * \param contrast_threshold The contrast threshold.
 * \param curvature_threshold The curvature threshold.
 * \return True, if a the extremum can be refined and passes the threshold checks.
 */
template <class T>
inline bool adjustLocalExtremum(const std::vector<vigra::MultiArray<2, T> > & dog,
                                SIFTFeature & feature,
                                float contrast_threshold, float curvature_threshold)
{
    using namespace vigra;
    using namespace vigra::linalg;
    
    unsigned int i = feature.scale;
    unsigned int x = feature.position[0];
    unsigned int y = feature.position[1];
    
    const vigra::MultiArray<2, float> & prev = dog[i-3];
    const vigra::MultiArray<2, float> & curr = dog[i-2];
    const vigra::MultiArray<2, float> & next = dog[i-1];
    
    float v = curr(x,y);
    
    vigra::Matrix<double> grad(3,1);
    vigra::Matrix<double> hess(3,3);
    vigra::Matrix<double> offset(3,1);

    grad(0,0) = curr(x+1,y) - curr(x-1,y);
    grad(1,0) = curr(x,y+1) - curr(x,y-1);
    grad(2,0) = next(x,y)   - prev(x,y);
    grad*=0.5;
    
    float v2 = curr(x,y)*2;
    
    hess(0,0) = curr(x+1,y) + curr(x-1,y) - v2; //Dxx
    hess(1,1) = curr(x,y+1) + curr(x,y-1) - v2; //Dyy
    hess(2,2) = next(x,y)   + prev(x,y)   - v2; //Dss
    hess*=4.0;
    
    hess(0,1) = hess(1,0) = curr(x+1, y+1) - curr(x-1, y+1) - curr(x+1, y-1) + curr(x-1, y-1); //Dxy
    hess(0,2) = hess(2,0) = next(x+1, y)   - next(x-1, y)   - prev(x+1, y)   + prev(x-1, y);   //Dxs
    hess(1,2) = hess(2,1) = next(y, y+1)   - next(x, y-1)   - prev(x, y+1)   + prev(x, y-1);   //Dys
    hess/=4.0;
    
    if(!linearSolve(hess, grad, offset))
    {
        //qDebug("SIFT technique: Point (%d,%d,%d) did not pass optimization!" , x, y, i);
        return false;
    }
    
    for(int i = 0; i < 3; i++)
	{
		if(std::abs(offset(i,0)) < 0.0001)
		{
			offset(i,0) = 0;
		}
	}
    
    offset = -offset;
    
    feature.position[0] += offset(0,0);
    feature.position[1] += offset(1,0);
    feature.scale += offset(2,0);
    
    if( std::abs(offset(0,0)) >= 0.5 || std::abs(offset(1,0)) >= 0.5 || std::abs(offset(2,0)) >= 0.5 )
    {
        //qDebug("SIFT technique: Point (%f,%f,%f) did not pass offset<0.5 check: %f >= 0.5 ||  %f >= 0.5 ||  %f >= 0.5 || " , res_x, res_y, res_i, std::abs(offset(0,0)), std::abs(offset(1,0)), std::abs(offset(2,0)));
        return false;
    }
    
    if( std::abs(v + dot(grad.transpose(),offset)*0.5) < contrast_threshold)
    {
        //qDebug("SIFT technique: Point (%f,%f,%f) did not passed contrast_thresholding: %f < %f" , res_x, res_y, res_i, std::abs(v + dot(grad.transpose(),offset)*0.5), contrast_threshold);
        return false;
    }
    
    float det = hess(0,0) + hess(1,1);
    float tra = hess(0,0)*hess(1,1) - hess(1,0)*hess(0,1);
    
    if(det <= 0 || tra*tra*curvature_threshold > (curvature_threshold+1)*(curvature_threshold+1)*det)
    {
        //qDebug("SIFT technique: Point (%f,%f,%f) did not passed curvature_thresholding:  %f <= 0 || %f > %f" , res_x, res_y, res_i, det, tra*tra/det, (curvature_threshold+1)*(curvature_threshold+1)/curvature_threshold);
        return false;
    }
    
    return true;
}

/**
 * Inline function to fill the orientation histograms for a given DoG
 * extermum position. The result will be used to determine the rotation
 * angle of the feature at this position.
 *
 * \param img Constant reference to the image.
 * \param feature Const reference to the sift feature.
 * \param radius The radius used for the collection of gradients
 * \param histogram_bins The sampling bins for 0..360 degrees.
 * \return A filled vector with all gaussian distance weighted gradient directions.
 */
template <class T>
std::vector<float> computeOrientationHistogram(const vigra::MultiArray<2,T> & img,
                                               const SIFTFeature & feature,
                                               int radius=8, unsigned int histogram_bins=36)
{
    float x = feature.position[0];
    float y = feature.position[1];
 
    vigra::Gaussian<double> gauss((feature.scale-2)*1.5);
    
    std::vector<float> histogram(histogram_bins);
    
    for (int j = -radius; j<= radius; ++j)
    {
        int img_y = std::floor((y+j) +0.5);
        if( img_y < 1 || img_y > img.height()-2 )
            continue;
        
        for (int i = -radius; i<= radius; ++i)
        {
            int img_x = std::floor((x+i)+0.5);
            if( img_x < 1 || img_x > img.width()-2 )
                continue;
            
            float   dx = (img(img_x+1,   img_y  ) - img(img_x-1, img_y  ))/2.0,
                    dy = (img(img_x  ,   img_y+1) - img(img_x  , img_y-1))/2.0,
                    angle = fmod(2*M_PI + atan2(dy,dx), 2*M_PI),
                    weight = sqrt(dx*dx+dy*dy)*gauss(sqrt(i*i+j*j));
            
            unsigned int hist_bin = angle/(2.0*M_PI)*histogram_bins;
            histogram[hist_bin]+=weight;
        }
    }
    
    return histogram;
}

/**
 * Inline function to fill the final discriptor histograms for a given feature.
 *
 * \param img Constant reference to the image.
 * \param feature Const reference to the sift feature. Position and orientation will used.
 * \param blocks The block count (defaults to 4x4=16).
 * \param block_size The size of each block (defaults to 4x4=16)
 * \param histogram_bins The sampling bins for 0..360 degrees.
 * \return A filled flat vector with all gaussian distance weighted gradient directions.
 */
template <class T, int ORDER>
inline std::vector<float>  computeSIFTHistograms(const vigra::SplineImageView<ORDER, T> & siv,
                                                 const SIFTFeature & feature,
                                                 unsigned int blocks=16, unsigned int block_size=16, unsigned int histogram_bins=8)
{
    float orientation = feature.orientation;
    float x = feature.position[0];
    float y = feature.position[1];
    
    //Compute overall angles and magnitudes of first derivatives (Gauss weighted):
    int blocks_per_row=sqrt(blocks),
        block_width=sqrt(block_size),
        radius=blocks_per_row*block_width/2.0;
    
    vigra::Gaussian<double> gauss(blocks_per_row*block_width/3.0);
    
    //Histograms are ordered as follows:
    //       block0_bin0, block0_bin1, ... , block1_bin0, ... , blockN_bin0, ..
    std::vector<float> histograms(blocks*histogram_bins);
    
    float hist_row = 0;
    
    for (int j = -radius; j<= radius; ++j, hist_row+=blocks_per_row/(2.0*radius+1))
    {
        float hist_col = 0;
        
        for (int i = -radius; i<= radius; ++i, hist_col+=blocks_per_row/(2.0*radius+1))
        {
            float siv_y = y + cos(orientation)*i + sin(orientation)*j;
            if( siv_y <= 0 || siv_y >= siv.height() - 1 )
                continue;
            
            float siv_x = x + sin(orientation)*i + sin(orientation)*j;
            if( siv_x <= 0 || siv_x >= siv.width() - 1 )
                continue;
            
            float   dx = siv.dx(siv_x,siv_y),
                    dy = siv.dy(siv_x,siv_y),
                    gradient_orientation = atan2(dy,dx)+M_PI,
                    gradient_weight = sqrt(dx*dx+dy*dy)*gauss(sqrt(i*i+j*j));
            
            unsigned int hist = blocks_per_row*int(hist_col) + int(hist_row),
                         hist_bin = gradient_orientation/(2.0*M_PI)*histogram_bins,
                         index = std::min(hist*histogram_bins + (hist_bin % histogram_bins), blocks*histogram_bins);
            
            histograms[index]+=gradient_weight;
        }
    }
    
    return histograms;
}

/**
 * The main SIFT method. Computes the feature descriptors.
 *
 * \param sigma The (gaussian scale) sigma by means of a scale step. Defaults to 1.0
 * \param octaves The number of octaves. If zero (=default), it will auto-estimate using a min size of 8x8
 * \param levels The number of levels per octave, for which keypoint may be found
 * \param contrast_threshold The keypoint's contrast threshold, Defaults to 0.03 = 3%
 * \param curvature_threshold The keypoint's edge threshold. Defaults to 10.0 (radius of corner)
 * \param double_image_size It true, it doubles the image size for 0th scale
 * \param normalize_image It true, the image will be normalized to 0..1 first.
 * \return The representation of a vector of single SIFT features, which are vectors, too.
 *         Each vector is ordered as follows:
 *              0  1    2      3        4            5            ......           132
 *              x  y  scale  angle   contrast  hist:block0_bin0   ......  hist:block15_bin7
 *                           (deg.)
 *
 */
template <class T>
std::vector<SIFTFeature> computeSIFTDescriptors(const vigra::MultiArrayView<2,T> & image,
                                                float sigma = 1.0, unsigned int octaves=0, unsigned int levels=3,
                                                float contrast_threshold=0.03, float curvature_threshold=10.0, bool double_image_size=true, bool normalize_image=true)
{
    using namespace std;
    using namespace vigra;
    using namespace vigra::multi_math;
    
    //If we rescale the image (double in each direction), we need to adjust the
    //octave offset - thus resulting DoG positions will be divided by two at the
    //lowermost scale if needed
    int o_offset=0;
    
    vigra::MultiArray<2, float> work_image=image;
    
    if(double_image_size)
    {
        work_image.reshape(2*image.shape());
        
        resizeImageLinearInterpolation(image, work_image);
        gaussianSmoothing(work_image, work_image, sigma);
        o_offset=-1;
    }
    
    //Determine the number of Octaves
    if(octaves<1)
    {
        octaves = log(std::min(work_image.width(),work_image.height()))/log(2.0)-3;
    }

    //Further parameters
    unsigned int         s = levels;
    float                k = pow(2.0,1.0/s);
    unsigned int intervals = s+3;
    
    //Data containers:
    std::vector<MultiArray<2, float> > octave(intervals);
    std::vector<MultiArray<2, float> > dog(intervals-1);
    
    std::vector<SIFTFeature> result;
    
    //Find min and max of image
    vigra::FindMinMax<T> minmax;   // init functor
    vigra::inspectImage(work_image, minmax);
    
    if(normalize_image)
    {
        //Normalize image to 0..1
        vigra::transformImage(work_image, work_image,
                              vigra::linearRangeMapping(
                                                        minmax.min, minmax.max,  // src range
                                                        0.0, 1.0)				// dest range
                              );
    }
    else
    {
        contrast_threshold *= (minmax.max - minmax.min);
    }
    
    //initialise first octave with current (maybe doubled) image
    octave[0] = work_image;
    
    unsigned int counter_phase1=0;
    unsigned int counter_phase2=0;
    unsigned int counter_phase3=0;
    
    //Run the loop
    for(unsigned int o=0; o<octaves; ++o)
    {
        /**
         * 1. Step create the Octaves and DoGs:
         */
        for (unsigned int i=1; i<intervals; ++i)
        {
            //initialize curent interval image array
            octave[i].reshape(octave[i-1].shape());

            // (total_sigma)^2 = sigma^2 + (last_sigma)^2
            // --> sigma = sqrt((total_sigma)^2 - (last_sigma)^2)!
            //determine the last sigma
            double last_sigma  = pow(k, i-1.0)*sigma,
                   total_sigma = last_sigma*k,
                   current_sigma = sqrt(total_sigma*total_sigma - last_sigma*last_sigma);
            
            
            gaussianSmoothing(octave[i-1], octave[i], current_sigma);

            //Compute the dog
            dog[i-1].reshape(octave[i].shape());
            dog[i-1] = octave[i]-octave[i-1];
        }
        
        /**
          * 2. Step: Find features on each DoG level,
          *          adjust them with subpixel accuray
          *          filter out most of them,
          *          create orientation histogram,
          *          align according to max orientation,
          *          create descriptors
          */
        for (unsigned int i=3; i<intervals; ++i)
        {
            //if we have at least three DoGs, we can search for local extrema
            for (unsigned int y=1; y<dog[i-2].height()-1; ++y)
            {
                for (unsigned int x=1; x<dog[i-2].width()-1; ++x)
                {
                    float v = dog[i-2](x,y);
                    
                    if ( abs(v) > contrast_threshold)
                    {
                        counter_phase1++;
                        
                        if(localExtremum(dog, i, x, y))
                        {
                        
                            //Create a new feature and initialize it.
                            SIFTFeature new_feature;
                            new_feature.position[0] = x;
                            new_feature.position[1] = y;
                            new_feature.scale = i;
                            new_feature.contrast = abs(v);
                        
                            counter_phase2++;
                            
                            if ( adjustLocalExtremum(dog, new_feature, contrast_threshold, curvature_threshold) )
                            {
                                counter_phase3++;
                                
                                //switch from DoG to scale space
								unsigned int best_i = std::floor(new_feature.scale+.5);
                                std::vector<float> hist = computeOrientationHistogram(octave[best_i], new_feature);

                                int x2 = 0;
                                double y2 = hist[0];
                                
                                for(unsigned int bin=1; bin<hist.size(); ++bin)
                                {
                                    if(hist[bin] > y2)
                                    {
                                        x2 = bin;
                                        y2 = hist[bin];
                                    }
                                }
                                
                                if(y2!=0)
                                {
                                    //Interpolate angle using parabola:
                                    //Collecting values
                                    int x1 = x2-1;
                                    int x3 = x2+1;
                                    double y1 = hist[(36+x1)%36];
                                    double y3 = hist[(36+x3)%36];
                                    
                                    //Estimate parabola
                                    double denom = (x1 - x2) * (x1 - x3) * (x2 - x3);
                                    double A     = (x3 * (y2 - y1) + x2 * (y1 - y3) + x1 * (y3 - y2)) / denom;
                                    double B     = (x3*x3 * (y1 - y2) + x2*x2 * (y3 - y1) + x1*x1 * (y2 - y3)) / denom;
                                    //double C     = (x2 * x3 * (x2 - x3) * y1 + x3 * x1 * (x3 - x1) * y2 + x1 * x2 * (x1 - x2) * y3) / denom;

                                    double xv = -B / (2*A);
                                    //double yv = C - B*B / (4*A);
                        
                                    //Assign value:
                                    new_feature.orientation = xv*10.0/180.0*M_PI;
                                }
                                else
                                {
                                    new_feature.orientation = x2*10.0/180.0*M_PI;
                                }
                                
                                vigra::SplineImageView<2,float> siv(octave[best_i]);
                                
                                new_feature.descriptor = computeSIFTHistograms(siv, new_feature);

                                //Rescale from local DoG size to global image size
                                new_feature.position *= pow(2,o+o_offset);                          //global position
                                new_feature.scale = pow(2,o+o_offset)*pow(k, new_feature.scale)*sigma;  //global scale
                                
                                //Add to results:
                                result.push_back(new_feature);
                            }
                        }
                    }
                }
            }
        }
        
        //rescale for next pyramid step and resize old image (3rd from top)
        octave[0].reshape(octave[0].shape()/2);
        resizeImageNoInterpolation(octave[s], octave[0]);
    }
    qDebug("SIFT feature detector: %d features are found (%d after phase1, %d after phase 2)", counter_phase3, counter_phase1, counter_phase2);
    
    return result;
}

} //end of namespace graipe

#endif //GRAIPE_FEATUREDETECTION_SIFT_HXX
