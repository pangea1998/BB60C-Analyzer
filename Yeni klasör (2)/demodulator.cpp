#include "demodulator.h"
#include <cmath>
#include <algorithm>

Demodulator::Demodulator(QObject *parent) 
    : QObject(parent)
    , currentMode(AM)
    , centerFreq(0)
    , bandwidth(0)
{
}

void Demodulator::setMode(Mode mode) {
    currentMode = mode;
}

void Demodulator::setFrequency(double freq) {
    centerFreq = freq;
}

void Demodulator::setBandwidth(double bw) {
    bandwidth = bw;
}

std::vector<float> Demodulator::demodulate(const std::vector<std::complex<float>>& iqData) {
    switch (currentMode) {
        case AM:
            return demodulateAM(iqData);
        case FM:
            return demodulateFM(iqData);
        case USB:
            return demodulateSSB(iqData, true);
        case LSB:
            return demodulateSSB(iqData, false);
        case CW:
            return demodulateCW(iqData);
        default:
            return std::vector<float>();
    }
}

std::vector<float> Demodulator::demodulateAM(const std::vector<std::complex<float>>& iqData) {
    std::vector<float> output;
    output.reserve(iqData.size());
    
    for (const auto& sample : iqData) {
        float magnitude = std::sqrt(sample.real() * sample.real() + 
                                  sample.imag() * sample.imag());
        output.push_back(magnitude);
    }
    
    return output;
}

std::vector<float> Demodulator::demodulateFM(const std::vector<std::complex<float>>& iqData) {
    std::vector<float> output;
    output.reserve(iqData.size());
    
    if (iqData.empty()) return output;
    
    std::complex<float> prev = iqData[0];
    for (size_t i = 1; i < iqData.size(); ++i) {
        float phase = std::arg(iqData[i] * std::conj(prev));
        output.push_back(phase);
        prev = iqData[i];
    }
    
    return output;
}

std::vector<float> Demodulator::demodulateSSB(const std::vector<std::complex<float>>& iqData, bool upperSideband) {
    std::vector<float> output;
    output.reserve(iqData.size());
    
    for (const auto& sample : iqData) {
        float demodulated = upperSideband ? 
            sample.real() + sample.imag() : 
            sample.real() - sample.imag();
        output.push_back(demodulated);
    }
    
    return output;
}

std::vector<float> Demodulator::demodulateCW(const std::vector<std::complex<float>>& iqData) {
    // CW için basit bir SSB demodülasyonu kullan
    return demodulateSSB(iqData, true);
} 