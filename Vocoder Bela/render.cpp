#include <Bela.h>
#include <cmath>
#include <libraries/Gui/Gui.h>
#include <libraries/GuiController/GuiController.h>

const int kNumBands = 16;

// Center frequencies (log spaced)
float bandFreqs[kNumBands] = {
    120, 180, 260, 380,
    550, 800, 1200, 1800,
    2600, 3500, 4500, 5500,
    6500, 8000, 10000, 12000
};

float envelopes[kNumBands];

// Filter state arrays
float mod_z1[kNumBands], mod_z2[kNumBands];
float car_z1[kNumBands], car_z2[kNumBands];

// Filter coefficients
float bp_a0[kNumBands], bp_a1[kNumBands], bp_a2[kNumBands];
float bp_b1[kNumBands], bp_b2[kNumBands];

float envCoeff; // envelope smoothing
float sampleRate;

Gui gui;
GuiController controller;

void calcBandpass(int i, float freq, float Q)
{
    float w0 = 2.0f * M_PI * freq / sampleRate;
    float alpha = sinf(w0) / (2.0f * Q);

    float b0 = alpha;
    float b1_ = 0.0f;
    float b2 = -alpha;
    float a0_ = 1.0f + alpha;
    float a1_ = -2.0f * cosf(w0);
    float a2_ = 1.0f - alpha;

    bp_a0[i] = b0 / a0_;
    bp_a1[i] = b1_ / a0_;
    bp_a2[i] = b2 / a0_;
    bp_b1[i] = a1_ / a0_;
    bp_b2[i] = a2_ / a0_;
}

inline float processBiquad(
    float x,
    float& z1,
    float& z2,
    int i)
{
    float y = bp_a0[i]*x + z1;
    z1 = bp_a1[i]*x - bp_b1[i]*y + z2;
    z2 = bp_a2[i]*x - bp_b2[i]*y;
    return y;
}

bool setup(BelaContext *context, void *userData)
{
    sampleRate = context->audioSampleRate;

    float Q = 6.0f;
    for(int i = 0; i < kNumBands; i++)
    {
        calcBandpass(i, bandFreqs[i], Q);
        mod_z1[i] = mod_z2[i] = 0.0f;
        car_z1[i] = car_z2[i] = 0.0f;
        envelopes[i] = 0.0f;
    }

    float envFreq = 500.0f;
    envCoeff = expf(-2.0f * M_PI * envFreq / sampleRate);
    
    gui.setup(context->projectName);
	controller.setup(&gui, "Vocoder Controls");

	// Sliders
	controller.addSlider("Envelope speed", 500, 50, 2000, 1);
	controller.addSlider("Mic gain", 2500, 100, 5000, 1);
	controller.addSlider("Oscillator level", 0.05, 0.005, 0.2, 0.001);

    return true;
}

void render(BelaContext *context, void *userData)
{
    static float phase[kNumBands] = {0};   // one oscillator per band

    for(unsigned int n = 0; n < context->audioFrames; n++)
    {
        // --- Modulator from mic channel 2 ---
        //float mod = audioRead(context, n, 2) * 800.0f;   // mic gain
        
        float modRaw = audioRead(context, n, 2);
		static float pre = 0;
		float micGain = controller.getSliderValue(1);
		float mod = (modRaw - 0.95f * pre) * micGain;
		pre = modRaw;

        float out = 0.0f;
        float bandGain[kNumBands] = {1.0,1.0,1.2,1.5,1.5,1.3,1.2,1.0};

        for(int i = 0; i < kNumBands; i++)
        {
            // --- Modulator band ---
            float modBand = processBiquad(mod, mod_z1[i], mod_z2[i], i);

            // --- Envelope follower ---
            float rect = fabsf(modBand);
            float envSpeed = controller.getSliderValue(0);
			float coeff = expf(-2.0f * M_PI * envSpeed / sampleRate);

			envelopes[i] = coeff * envelopes[i]
             + (1.0f - coeff) * rect * 2.0f;

            envelopes[i] += 0.001f;   // noise floor

            // --- Carrier: sine wave at band frequency ---
            float oscLevel = controller.getSliderValue(2);
			float carBand = sinf(phase[i]) * oscLevel;

            phase[i] += 2.0f * M_PI * bandFreqs[i] / sampleRate;
            if(phase[i] > 2.0f * M_PI)
                phase[i] -= 2.0f * M_PI;

            // --- Apply envelope ---
            out += carBand * envelopes[i] * bandGain[i];
        }

        // --- Normalize ---
        out /= (float)kNumBands;
        out = tanhf(out);   // soft clip = protects from clipping

        // --- Output ---
        for(unsigned int ch = 0; ch < context->audioOutChannels; ch++)
            audioWrite(context, n, ch, out);
    }
}

void cleanup(BelaContext *context, void *userData)
{
}