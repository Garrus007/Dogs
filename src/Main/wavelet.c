#include "wavelet.h"
#include "gauss.h"


#define N_FREQS 85

#define wavelet_f0      6.0f
#define wavelet_dofmin  2.0f
#define wavelet_gamma   2.320f
#define PI_1_4          0.75112554f //powf(PI, -0.25)
#define wavelet_flambda 1.03304365f //((4 * PI) / (wavelet_f0 + sqrtf(2.0f + wavelet_f0 * wavelet_f0)))

float BUFFER[MAX_BUFFER_SIZE];
float FFT_RE[MAX_BUFFER_SIZE];
float FFT_IM[MAX_BUFFER_SIZE];
float FFT_SQR[MAX_BUFFER_SIZE];
uint16_t BUFFER_SIZE = 0;

////////////////////////////////////////////////////////////////////////////////



float wavelet_psi_ft(float f) {
    
    float f_diff = f - wavelet_f0;
    return PI_1_4 * gauss(f_diff);
}

uint16_t bit_reverse(uint16_t a, uint16_t b) {
    uint16_t result = 0;
    while (b) {
        result <<= 1;
        result |= a & 1;
        a >>= 1;
        b >>= 1;
    }
    return result;
}

/********** FFT ********/
void fft(float const *in,
         float *out_re,
         float *out_im,
         uint16_t n) {

    for (uint16_t i = 0; i < n; ++i) {
        out_re[i] = in[bit_reverse(i, n - 1)];
        out_im[i] = 0;
    }
    for (uint16_t m = 1; m < n; m *= 2) {
        float em_re = cosf(PI / m);
        float em_im = sinf(PI / m);
        for (uint16_t k = 0; k < n; k += 2 * m) {
            float e_re = 1;
            float e_im = 0;
            for (uint16_t j = 0; j < m; ++j) {
                float u_re = out_re[k + j];
                float u_im = out_im[k + j];
                float v_re = e_re * out_re[k + j + m] - e_im * out_im[k + j + m];
                float v_im = e_re * out_im[k + j + m] + e_im * out_re[k + j + m];
                out_re[k + j] = u_re + v_re;
                out_im[k + j] = u_im + v_im;
                out_re[k + j + m] = u_re - v_re;
                out_im[k + j + m] = u_im - v_im;

                float e_re_new = e_re * em_re - e_im * em_im;
                e_im = e_re * em_im + e_im * em_re;
                e_re = e_re_new;
            }
        }
    }
}

/********* FFT ********/

// Estimate of the lag-one autocorrelation.
float ar1(const float *signal, uint16_t n) {
    // Estimates the lag zero and one covariance
    float c0 = 0, c1 = 0;
    for (uint16_t i = 0; i < n - 1; ++i) {
        c0 += signal[i] * signal[i];
        c1 += signal[i] * signal[i + 1];
    }
    c0 += signal[n - 1] * signal[n - 1];
    c0 /= n;
    c1 /= (n - 1);

    // According to A. Grinsteds' substitutions
    float A = c0 * n * n;
    float B = -c1 * n - c0 * n * n - 2 * c0 + 2 * c1 - c1 * n * n + c0 * n;
    float C = n * (c0 + c1 * n - c1);
    float D = B * B - 4 * A * C;
    if (D < 0) { return 0.99; } /* It isn't science! */
    return (-B - sqrtf(D)) / (2 * A);
}


// A chi-squared percent point function for 95%
float chi2_ppf_95(float dof) {
    if (dof < 20) {
        return 2.10308f + 1.98911f * dof - 0.0464057f * powf(dof, 2)
               + 0.00102171f * powf(dof, 3);
    }
    return 9.27863f + 1.157f * dof;
}

float ftfreq(uint16_t i, uint16_t n) {
    if (2 * i < n) {
        return 2 * PI * i / n;
    }
    return -2 * PI * (n - i) / n;
}

void remove_trend(float *signal, uint16_t n) {
    float sumx = (n - 1) * n / 2,
            sumx2 = n * (n - 1) * (2 * n - 1) / 6,
            sumy = 0, sumxy = 0;
    for (uint16_t i = 0; i < n; ++i) {
        sumy += signal[i];
        sumxy += i * signal[i];
    }

    float a = (n * sumxy - sumx * sumy) / (n * sumx2 - sumx * sumx);
    float b = -(sumx * sumxy - sumx2 * sumy) / (n * sumx2 - sumx * sumx);

    for (uint16_t i = 0; i < n; ++i) {
        signal[i] -= a * i + b;
    }
}


uint16_t steps() 
{
    remove_trend(BUFFER, BUFFER_SIZE);

    /* calculate autocorrelation */
    float alpha = ar1(BUFFER, BUFFER_SIZE);

    /* calculate variance */
    float mean = 0, variance = 0;
    for (uint16_t i = 0; i < BUFFER_SIZE; ++i) {
        float delta = BUFFER[i] - mean;
        mean += delta / (i + 1);
        float delta2 = BUFFER[i] - mean;
        variance += (delta * delta2 - variance) / (i + 1);
    }

    /* scale step */
    float dj = 1.0f / 12;
    /* Minimal wavelet scale */
    float s0 = 2.0f / wavelet_flambda;

    for (uint16_t i = BUFFER_SIZE; i < MAX_BUFFER_SIZE; ++i) {
        BUFFER[i] = 0;
    }

    /* perform FT */
    fft(BUFFER, FFT_RE, FFT_IM, MAX_BUFFER_SIZE);    
    for(int i = 0; i<MAX_BUFFER_SIZE; i++)
      FFT_SQR[i] = (FFT_RE[i] * FFT_RE[i] + FFT_IM[i] * FFT_IM[i]);

    float max_power = 0;
    float max_freq = 0;
    float max_signif = 0;
    float dofmin = wavelet_dofmin;     // Degrees of freedom with no smoothing
    float gamma_fac = wavelet_gamma;   // Time-decorrelation factor
    
    float scale = s0;
    float ds = powf(2.0f, dj); //1.059463f; 
    float baseFreq = ftfreq(1, MAX_BUFFER_SIZE);
    
    
    for (int i = 0; i < N_FREQS; ++i) 
    {
        //float scale = s0 * powf(2.0, i * dj);
        float freq = 1 / (wavelet_flambda * scale);
        float power = 0;
     
        for (int k = 0; k < MAX_BUFFER_SIZE; ++k)
        {
            
            float psi_ft = wavelet_psi_ft(scale * ftfreq(k, MAX_BUFFER_SIZE));
            
            float psi_ft_bar = scale * baseFreq * psi_ft * psi_ft;
            float w2 = psi_ft_bar * FFT_SQR[k];
            power += (w2 - power) / (k + 1);
        }
        
        if (power > max_power) 
        {
            max_power = power;
            max_freq = freq;

            float dof = BUFFER_SIZE;
            if (dof < 1) { dof = 1; }
            
            float wtf =  dof / gamma_fac / scale;
            float sqrtRes;
            arm_sqrt_f32(1 + wtf*wtf, &sqrtRes);
            dof = dofmin * sqrtRes;
            
            if (dof < dofmin) { dof = dofmin; }
            
            float chisquare = chi2_ppf_95(dof) / dof;
            float fft_theor = variance * (1 - alpha*alpha) / (1 + alpha * alpha - 2 * alpha * cosf(2 * PI * freq));
            
            max_signif = fft_theor * chisquare;
        }
        
        scale *= ds;
    }

    if (max_power > max_signif) {
        return (uint16_t) roundf(max_freq * BUFFER_SIZE);
    }
    

    return 0;
}


void waveletProcessNewData(int16_t x, int16_t y, int16_t z, uint16_t time) {
    if (BUFFER_SIZE < MAX_BUFFER_SIZE) {
        float xf = x;
        float yf = y;
        float zf = z;
        float length;
        arm_sqrt_f32(xf * xf + yf * yf + zf * zf, &length);
        BUFFER[BUFFER_SIZE++] = length;
    }
}

uint16_t waveletGetStepsCount(void) {
    uint16_t count = steps();
    BUFFER_SIZE = 0;
    return count;
}
