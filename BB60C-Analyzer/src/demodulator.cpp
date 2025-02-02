#include "demodulator.h"
#include <cmath>
#include <algorithm>

// PIMPL implementation
struct Demodulator::Impl {
    // Filtre katsayıları ve geçici tamponlar
    QVector<float> filterCoeffs;
    QVector<float> delayLine;
    
    // FM demodülasyon için önceki faz
    float prevPhase{0.0f};
};

Demodulator::Demodulator(QObject *parent)
    : QObject(parent)
    , pimpl(std::make_unique<Impl>())
    , mode(AM)
    , centerFreq(1e6)  // 1 MHz
    , bw(10e3)         // 10 kHz
    , vol(1.0f)
{
    // Varsayılan alçak geçiren filtre katsayıları
    pimpl->filterCoeffs = {0.1f, 0.2f, 0.4f, 0.2f, 0.1f};
    pimpl->delayLine.resize(pimpl->filterCoeffs.size());
    updateFilterCoeffs();
}

Demodulator::~Demodulator() = default;

void Demodulator::setMode(Mode newMode)
{
    if (mode != newMode) {
        mode = newMode;
        updateFilterCoeffs();
    }
}

void Demodulator::setFrequency(double freq)
{
    if (centerFreq != freq) {
        centerFreq = freq;
        updateFilterCoeffs();
    }
}

void Demodulator::setBandwidth(double bandwidth)
{
    if (bw != bandwidth) {
        bw = bandwidth;
        updateFilterCoeffs();
    }
}

void Demodulator::setVolume(float volume)
{
    vol = std::clamp(volume, 0.0f, 1.0f);
}

QVector<float> Demodulator::demodulate(const QVector<std::complex<float>>& iqData)
{
    switch (mode) {
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
        return QVector<float>();
    }
}

QVector<float> Demodulator::demodulateAM(const QVector<std::complex<float>>& iqData)
{
    QVector<float> audioData(iqData.size());
    
    // AM demodülasyon: Genlik hesapla
    for (int i = 0; i < iqData.size(); ++i) {
        float magnitude = std::sqrt(std::norm(iqData[i]));
        audioData[i] = magnitude * vol * 100.0f;
    }
    
    applyBandwidth(audioData);
    return audioData;
}

QVector<float> Demodulator::demodulateFM(const QVector<std::complex<float>>& iqData)
{
    QVector<float> audioData(iqData.size());
    
    // FM demodülasyon: Faz farkını hesapla
    for (int i = 0; i < iqData.size(); ++i) {
        float phase = std::arg(iqData[i]);
        float phaseDiff = phase - pimpl->prevPhase;
        
        // Faz farkını normalize et
        if (phaseDiff > M_PI) phaseDiff -= 2*M_PI;
        if (phaseDiff < -M_PI) phaseDiff += 2*M_PI;
        
        audioData[i] = phaseDiff * vol * 100.0f;
        pimpl->prevPhase = phase;
    }
    
    applyBandwidth(audioData);
    return audioData;
}

QVector<float> Demodulator::demodulateSSB(const QVector<std::complex<float>>& iqData, bool upperSideband)
{
    QVector<float> audioData(iqData.size());
    
    // SSB demodülasyon: Hilbert dönüşümü ile
    for (int i = 0; i < iqData.size(); ++i) {
        float real = iqData[i].real();
        float imag = iqData[i].imag();
        
        // USB için topla, LSB için çıkar
        audioData[i] = upperSideband ? (real + imag) : (real - imag);
        audioData[i] *= vol * 100.0f;
    }
    
    return audioData;
}

void Demodulator::updateFilterCoeffs()
{
    // Basit bir alçak geçiren filtre tasarımı
    const int ORDER = 64;
    pimpl->filterCoeffs.resize(ORDER);

    double fc = bw / 2.0;  // Kesim frekansı
    double sum = 0.0;

    for (int i = 0; i < ORDER; ++i) {
        if (i == ORDER/2) {
            pimpl->filterCoeffs[i] = 2.0 * fc;
        } else {
            double x = M_PI * (i - ORDER/2);
            pimpl->filterCoeffs[i] = std::sin(2.0 * fc * x) / x;
        }
        // Blackman penceresi uygula
        pimpl->filterCoeffs[i] *= 0.42 - 0.5 * std::cos(2.0 * M_PI * i / ORDER)
                           + 0.08 * std::cos(4.0 * M_PI * i / ORDER);
        sum += pimpl->filterCoeffs[i];
    }

    // Normalize et
    for (float& coeff : pimpl->filterCoeffs) {
        coeff /= sum;
    }
}

void Demodulator::applyBandwidth(QVector<float>& audio)
{
    QVector<float> filtered(audio.size());

    // Konvolüsyon ile filtreleme
    for (size_t i = 0; i < audio.size(); ++i) {
        float sum = 0.0f;
        for (size_t j = 0; j < pimpl->filterCoeffs.size(); ++j) {
            if (i >= j) {
                sum += audio[i-j] * pimpl->filterCoeffs[j];
            }
        }
        filtered[i] = sum;
    }

    audio = std::move(filtered);
}

void Demodulator::applyVolume(QVector<float>& audio)
{
    for (float& sample : audio) {
        sample *= vol;
    }
}
