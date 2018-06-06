/**************************************************************************//**
 * @file
 * 
 * Multi-scale Turing patterns based on Jonathan McCabe's work.
 *****************************************************************************/

#include "blind_quarter.h"
#include <ctime>
#include <cfloat>
#include "blur.h"

/**************************************************************************//**
 * Normalizes the image to the interval [0; 1].
 * 
 * @param[in] w Image width
 * @param[in] h Image height
 * @param[inout] im Image
 *****************************************************************************/
static void normalize(const size_t w, const size_t h, float_t* im);

void blind_quarter_init_image(const size_t w, const size_t h, float_t* im)
{
	size_t i;
	
	srand(time(NULL));
	for (i = 0; i < w*h; i++)
	{
        im[i] = (float_t)rand()/(float_t)(RAND_MAX);
	}
}

void blind_quarter_step(const struct pattern* p, const uint32_t n,
    const size_t w, const size_t h, float_t* im)
{    
    float_t* act = new float_t[w*h]; // Activator array
    float_t* inh = new float_t[w*h]; // Inhibitor array
    float_t* var = new float_t[w*h]; // Variations
    float_t var_new;
    uint32_t* best_scale = new uint32_t[w*h];
    
    // For each scale...
    for (size_t i = 0; i < n; i++)
    {
        // Compute activator and inhibitor arrays
        blur(w, h, (p+i)->act_r, (p+i)->wt, im, act);
		blur(w, h, (p+i)->inh_r, (p+i)->wt, im, inh);
        
        // For each pixel...
        for (size_t j = 0; j < w*h; j++)
        {
            // Update the variation array if the variation for this pixel is
            // smaller than the one already stored. This way, the variation
            // array always stores the smallest variation.
            // When processing the first scale (i == 0), the variation array
            // is always updated, so we don't need to initialize it beforehand.
            var_new = act[j] - inh[j];
            if ((fabs(var_new) < fabs(var[j])) || (i == 0))
            {
                var[j] = var_new;
                best_scale[j] = i;
            }
        }
    }
    
    delete [] act;
    delete [] inh;
    
    // For each pixel, add the small amount if the activator was larger than
    // the inhibitor, subtract otherwise
    for (size_t j = 0; j < w*h; j++)
    {
        if (var[j] > 0)
        {
            im[j] += (p + best_scale[j])->sa;
        }
        else
        {
            im[j] -= (p + best_scale[j])->sa;
        }
    }
    
    delete [] var;
    delete [] best_scale;
    
	normalize(w, h, im);
}

static void normalize(const size_t w, const size_t h, float_t *im)
{
	float_t max = FLT_MIN;
    float_t min = FLT_MAX;
    float_t range;
	size_t i;
    
    for (i = 0; i < w*h; i++)
	{
		if (im[i] > max)
		{
			max = im[i];
		}
        if (im[i] < min)
		{
			min = im[i];
		}
	}
	range = max - min;
	
	for (i = 0; i < w*h; i++)
	{
		im[i] = (im[i] - min)/range;
	}
}
