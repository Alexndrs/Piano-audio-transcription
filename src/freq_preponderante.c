#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>


float frequence_preponderante(double* tab_amplitude,float* tab_frequence,float seuil, int H, int len_tab)
{
    float fprep;
    float amp_max = seuil;
    float f;
    float prod_spec;
    for(int k = 0; k < (len_tab/H) ;k++)
    {
        f = tab_frequence[k];
        prod_spec = 1;
        for (int i = 1; i <= H; i++)
        {
            double r = REAL(tab_amplitude,i*k);
            double im = IMAG(tab_amplitude,i*k);
            prod_spec *=(r*r + im*im);
        }
        if(log10(prod_spec) > amp_max)
        {
            amp_max = log10(prod_spec);
            fprep = f;
        }
    }
    return (fprep); 
}
