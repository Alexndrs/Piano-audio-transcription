#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

float* amplitude_fenetree[nbech];

void fenetrage_hamming(float* tab_amplitude, float** amplitude_fenetree, int nb_data, float Fe, float t1, float t2)
{
    float Te = 1/Fe;
    for(k=0;k<nb_data;k++)
    {
        float t = k*Te;
        if(t>t1 && t<t2)
        {
            amplitude_fenetree[k] = tab_amplitude[k]*(0.54 - 0.46 * cos(2*3.141592653589793*(t-t1)/(t2 - t1)));
        }
        else
        { 
            amplitude_fenetree[k] = 0;
        }
    }
}