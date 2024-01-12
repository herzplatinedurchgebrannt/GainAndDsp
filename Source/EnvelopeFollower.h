#pragma once

#include <JuceHeader.h>

#define TINY 0.0000000001

/**

This is an envelope follower with user adjustable attack and release time constants. It squares
the incoming signal or takes the absolute value, smoothes this signal via an attack-release (AR)
averager and (possibly) extracts the square-root. So the output value represents the
instantaneous mean-abs, mean-squared or RMS-value of the incoming signal. The averaging time is
determined by the attack and release time constants which are set up in milliseconds.

References: Udo Zoelzer - DAFX (page 84)

*/

class EnvelopeFollower /*: public SlewRateLimiter*/
{

  public:
    enum detectorModes
    {
        MEAN_ABS,
        MEAN_SQUARE,
        ROOT_MEAN_SQUARE,

        NUM_MODES
    };

    /** \name Construction/Destruction */

    /** Constructor. */
    EnvelopeFollower();

    /** Destructor. */
    ~EnvelopeFollower();

    void setSampleRate(float newSampleRate);

    void setAttackTime(float newAttackTime);

    void setReleaseTime(float newReleaseTime);

    /** \name Setup */

    /** Chooses the mode for the envelope detector. @see detectorModes */
    void setMode(int newMode);

    void reset();

    /** \name Audio Processing */

    /** Smoothes the input value vith the AR-averager. */
    inline float applySmoothing(float in);

    void prepare(const juce::dsp::ProcessSpec &spec) noexcept;

    void process(const juce::dsp::ProcessContextNonReplacing<float> &context) noexcept;

    /** Estimates the signal envelope via one of the functions getSampleMeanAbsolute(),
    getSampleMeanSquare() or getSampleRootMeanSquare() depending on the chosen mode. */
    inline float getSample(float in);

    /** Estimates the signal envelope via AR-averaging the mean absolute value. */
    inline float getSampleMeanAbsolute(float in);

    /** Estimates the signal envelope via AR-averaging the mean squared value. */
    inline float getSampleMeanSquare(float in);

    /** Estimates the signal envelope via AR-averaging the mean squared value and extracting the
    square root. */
    inline float getSampleRootMeanSquare(float in);

  protected:
    /** \name Data */

    float y_1; // previous ouput sample of the unit

    float coeffAttack, coeffRelease;
    int mode; /** @see detectorModes */


    float sampleRate;
    float attackTime, releaseTime; // time constants in seconds
};

// inlined functions

inline float EnvelopeFollower::applySmoothing(float in)
{
    static float temp, coeff;

    temp = in;

    // decide, if signal is rising or falling and choose appropriate filter
    // coefficient:
    if (y_1 < temp)
        coeff = coeffAttack;
    else
        coeff = coeffRelease;

    // perform the envelope detection:
    temp = (1.0f - coeff) * temp + coeff * y_1 + TINY;

    // update the memorized output for next call:
    y_1 = temp;

    return temp;
}

inline float EnvelopeFollower::getSample(float in)
{
    switch (mode)
    {
    case MEAN_ABS:
        return getSampleMeanAbsolute(in);
    case MEAN_SQUARE:
        return getSampleMeanSquare(in);
    case ROOT_MEAN_SQUARE:
        return getSampleRootMeanSquare(in);
    default:
        return getSampleMeanAbsolute(in);
    }
}

inline float EnvelopeFollower::getSampleMeanAbsolute(float in)
{
    return (applySmoothing(std::fabs(in)));
}

inline float EnvelopeFollower::getSampleMeanSquare(float in)
{
    return (applySmoothing(in * in));
}

inline float EnvelopeFollower::getSampleRootMeanSquare(float in)
{
    return std::sqrt(getSampleMeanSquare(in));
}
