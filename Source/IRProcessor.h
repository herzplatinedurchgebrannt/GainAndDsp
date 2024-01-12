#pragma once

#include <JuceHeader.h>

using namespace juce::dsp;

class IRProcessor
{
public:
    struct BufferWithSampleRate
    {
        BufferWithSampleRate() = default;

        BufferWithSampleRate(juce::AudioBuffer<float>&& bufferIn, double sampleRateIn)
            : buffer(std::move(bufferIn))
            , sampleRate(sampleRateIn)
        {
        }

        juce::AudioBuffer<float> buffer;
        double sampleRate = 0.0;
    };

    class BufferTransfer
    {
    public:
        void set(BufferWithSampleRate&& p)
        {
            const juce::SpinLock::ScopedLockType lock(mutex);
            buffer = std::move(p);
            newBuffer = true;
        }

        // Call `fn` passing the new buffer, if there's one available
        template <typename Fn> void get(Fn&& fn)
        {
            const juce::SpinLock::ScopedTryLockType lock(mutex);

            if (lock.isLocked() && newBuffer)
            {
                fn(buffer);
                newBuffer = false;
            }
        }

    private:
        BufferWithSampleRate buffer;
        bool newBuffer = false;
        juce::SpinLock mutex;
    };

    void prepare(const ProcessSpec& spec);

    void process(ProcessContextReplacing<float> context);

    void reset();

    void setBypass(bool bypassValue);

    //void setSelection(bool cabSelectionValue);

    void setCabSelectId(int cabSelectIdNewValue);

    void updateParameters();

    double sampleRate = 0.0;
    bool bypass = false;
    //bool cabOneIsSelected = true;
    int cabSelectId = 1;

    juce::MemoryBlock currentCabinetData;
    Convolution convolution;
    Gain<float> gain;

    BufferTransfer bufferTransfer;



//    inline juce::File getExamplesDirectory() noexcept
//    {
//#ifdef PIP_JUCE_EXAMPLES_DIRECTORY
//        MemoryOutputStream mo;
//
//        auto success = Base64::convertFromBase64(mo, JUCE_STRINGIFY(PIP_JUCE_EXAMPLES_DIRECTORY));
//        ignoreUnused(success);
//        jassert(success);
//
//        return mo.toString();
//#elif defined PIP_JUCE_EXAMPLES_DIRECTORY_STRING
//        return File{ CharPointer_UTF8 { PIP_JUCE_EXAMPLES_DIRECTORY_STRING } };
//#else
//        auto currentFile = juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentApplicationFile);
//        auto exampleDir = currentFile.getParentDirectory().getChildFile("examples");
//
//        if (exampleDir.exists())
//            return exampleDir;
//
//        // keep track of the number of parent directories so we don't go on endlessly
//        for (int numTries = 0; numTries < 15; ++numTries)
//        {
//            if (currentFile.getFileName() == "examples")
//                return currentFile;
//
//            const auto sibling = currentFile.getSiblingFile("examples");
//
//            if (sibling.exists())
//                return sibling;
//
//            currentFile = currentFile.getParentDirectory();
//        }
//
//        return currentFile;
//#endif
//    }
//
//
//    inline std::unique_ptr<juce::InputStream> createAssetInputStream(const char* resourcePath)
//    {
//#if JUCE_ANDROID
//        ZipFile apkZip(File::getSpecialLocation(File::invokedExecutableFile));
//        return std::unique_ptr<InputStream>(apkZip.createStreamForEntry(apkZip.getIndexOfFileName("assets/" + String(resourcePath))));
//#else
//#if JUCE_IOS
//        auto assetsDir = File::getSpecialLocation(File::currentExecutableFile)
//            .getParentDirectory().getChildFile("Assets");
//#elif JUCE_MAC
//        auto assetsDir = File::getSpecialLocation(File::currentExecutableFile)
//            .getParentDirectory().getParentDirectory().getChildFile("Resources").getChildFile("Assets");
//
//        if (!assetsDir.exists())
//            assetsDir = getExamplesDirectory().getChildFile("Assets");
//#else
//        auto assetsDir = getExamplesDirectory().getChildFile("Assets");
//#endif
//
//        auto resourceFile = assetsDir.getChildFile(resourcePath);
//        jassert(resourceFile.existsAsFile());
//
//        return resourceFile.createInputStream();
//#endif
//    }

};

