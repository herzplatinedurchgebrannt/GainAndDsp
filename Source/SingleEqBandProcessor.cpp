#include "SingleEqBandProcessor.h"

//==============================================================================
SingleEqBandProcessor::SingleEqBandProcessor()
    : band(SingleEqBandProcessor::Band(TRANS("High Mids"), SingleEqBandProcessor::Peak, 1000.0f, 0.707f))
{
}

SingleEqBandProcessor::~SingleEqBandProcessor()
{
}

void SingleEqBandProcessor::reset() noexcept
{
}

void SingleEqBandProcessor::setPluginProcessor(juce::AudioProcessor* processorToUse) noexcept
{
    pluginProcessor = processorToUse;
}

//==============================================================================
void SingleEqBandProcessor::prepare(const juce::dsp::ProcessSpec& spec) noexcept
{
    sampleRate = spec.sampleRate;
    updateBand();
    filter.prepare(spec);
    filter.reset();
}

void SingleEqBandProcessor::releaseResources()
{
}

void SingleEqBandProcessor::process(const juce::dsp::ProcessContextReplacing<float>& context) noexcept
{
    filter.process(context);
}

void SingleEqBandProcessor::setBandGain(float gainToUse)
{
    band.gain = gainToUse;
    updateBand();
}

void SingleEqBandProcessor::setBand(const SingleEqBandProcessor::Band& newBand)
{
    band.type = newBand.type;
    band.frequency = newBand.frequency;
    band.quality = newBand.quality;
    band.gain = newBand.gain;
    band.active = newBand.active;
    updateBand();
}

void SingleEqBandProcessor::updateBand()
{
    if (sampleRate > 0)
    {
        juce::dsp::IIR::Coefficients<float>::Ptr newCoefficients;
        switch (band.type)
        {
        case NoFilter:
            newCoefficients = new juce::dsp::IIR::Coefficients<float>(1, 0, 1, 0);
            break;
        case LowPass:
            newCoefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, band.frequency, band.quality);
            break;
        case LowPass1st:
            newCoefficients = juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(sampleRate, band.frequency);
            break;
        case LowShelf:
            newCoefficients =
                juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, band.frequency, band.quality, band.gain);
            break;
        case BandPass:
            newCoefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, band.frequency, band.quality);
            break;
        case AllPass:
            newCoefficients = juce::dsp::IIR::Coefficients<float>::makeAllPass(sampleRate, band.frequency, band.quality);
            break;
        case AllPass1st:
            newCoefficients = juce::dsp::IIR::Coefficients<float>::makeFirstOrderAllPass(sampleRate, band.frequency);
            break;
        case Notch:
            newCoefficients = juce::dsp::IIR::Coefficients<float>::makeNotch(sampleRate, band.frequency, band.quality);
            break;
        case Peak:
            newCoefficients =
                juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, band.frequency, band.quality, band.gain);
            break;
        case HighShelf:
            newCoefficients =
                juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, band.frequency, band.quality, band.gain);
            break;
        case HighPass1st:
            newCoefficients = juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(sampleRate, band.frequency);
            break;
        case HighPass:
            newCoefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, band.frequency, band.quality);
            break;
        default:
            break;
        }

        if (newCoefficients)
        {
            if (pluginProcessor)
            {
                juce::ScopedLock processLock(pluginProcessor->getCallbackLock());
                *filter.state = *newCoefficients;
            }
            else
            {
                *filter.state = *newCoefficients;
            }
        }
    }
}
