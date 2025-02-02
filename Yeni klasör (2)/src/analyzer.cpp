#include "analyzer.h"
#include <cmath>
#include <algorithm>
#include <numeric>

// PIMPL implementation
struct Analyzer::Impl {
    // FFT için parametreler
    static constexpr int FFT_SIZE = 4096;
    static constexpr int WINDOW_TYPE = 1;  // Hanning
};

Analyzer::Analyzer(QObject *parent)
    : QObject(parent)
    , pimpl(std::make_unique<Impl>())
{
}

Analyzer::~Analyzer() = default;

double Analyzer::measureChannelPower(const QVector<double>& frequencies,
                                   const QVector<double>& amplitudes,
                                   double startFreq,
                                   double stopFreq)
{
    int startIndex = findFrequencyIndex(frequencies, startFreq);
    int stopIndex = findFrequencyIndex(frequencies, stopFreq);

    return calculatePower(amplitudes, startIndex, stopIndex);
}

double Analyzer::measureOBW(const QVector<double>& frequencies,
                          const QVector<double>& amplitudes,
                          double percentPower)
{
    // Toplam gücü hesapla
    double totalPower = calculatePower(amplitudes, 0, amplitudes.size() - 1);
    double targetPower = totalPower * percentPower / 100.0;

    // Merkez frekansı bul
    int centerIndex = amplitudes.size() / 2;
    double currentPower = 0.0;
    int bandwidth = 0;

    // Güç eşiğine ulaşana kadar bant genişliğini artır
    while (currentPower < targetPower && bandwidth < amplitudes.size()) {
        int startIndex = std::max(centerIndex - bandwidth/2, 0);
        int stopIndex = std::min(centerIndex + bandwidth/2, amplitudes.size() - 1);
        currentPower = calculatePower(amplitudes, startIndex, stopIndex);
        bandwidth++;
    }

    return std::abs(frequencies[centerIndex + bandwidth/2] -
                   frequencies[centerIndex - bandwidth/2]);
}

ACPRResult Analyzer::measureACPR(const QVector<double>& frequencies,
                               const QVector<double>& amplitudes,
                               double channelBW,
                               double channelSpacing)
{
    ACPRResult result;

    // Ana kanal indekslerini bul
    int centerIndex = frequencies.size() / 2;
    double halfBW = channelBW / 2.0;
    int mainStart = findFrequencyIndex(frequencies, frequencies[centerIndex] - halfBW);
    int mainStop = findFrequencyIndex(frequencies, frequencies[centerIndex] + halfBW);

    // Yan kanal indekslerini bul
    int lowerStart = findFrequencyIndex(frequencies, frequencies[mainStart] - channelSpacing - channelBW);
    int lowerStop = findFrequencyIndex(frequencies, frequencies[mainStart] - channelSpacing);
    int upperStart = findFrequencyIndex(frequencies, frequencies[mainStop] + channelSpacing);
    int upperStop = findFrequencyIndex(frequencies, frequencies[mainStop] + channelSpacing + channelBW);

    // Güç hesapla
    result.mainChannelPower = calculatePower(amplitudes, mainStart, mainStop);
    result.lowerChannelPower = calculatePower(amplitudes, lowerStart, lowerStop);
    result.upperChannelPower = calculatePower(amplitudes, upperStart, upperStop);

    // Oranları hesapla
    result.lowerRatio = result.mainChannelPower - result.lowerChannelPower;
    result.upperRatio = result.mainChannelPower - result.upperChannelPower;

    return result;
}

QVector<SpurResult> Analyzer::findSpurs(const QVector<double>& frequencies,
                                      const QVector<double>& amplitudes,
                                      double threshold)
{
    QVector<SpurResult> spurs;
    
    // Tepe noktalarını bul
    for (int i = 1; i < amplitudes.size() - 1; ++i) {
        if (amplitudes[i] > threshold &&
            amplitudes[i] > amplitudes[i-1] &&
            amplitudes[i] > amplitudes[i+1]) {
            
            SpurResult spur;
            spur.frequency = frequencies[i];
            spur.amplitude = amplitudes[i];
            spur.relativePower = spur.amplitude - *std::max_element(amplitudes.begin(),
                                                                  amplitudes.end());
            spurs.push_back(spur);
        }
    }

    // Genliğe göre sırala
    std::sort(spurs.begin(), spurs.end(),
              [](const SpurResult& a, const SpurResult& b) {
                  return a.amplitude > b.amplitude;
              });

    return spurs;
}

QVector<double> Analyzer::measurePhaseNoise(const QVector<std::complex<float>>& iqData,
                                          double sampleRate,
                                          const QVector<double>& offsets)
{
    QVector<double> phaseNoise(offsets.size());
    
    // Güç spektral yoğunluğunu hesapla
    auto psd = calculatePSD(iqData, sampleRate);
    
    // Her offset için faz gürültüsünü hesapla
    for (int i = 0; i < offsets.size(); ++i) {
        int offsetIndex = static_cast<int>(offsets[i] * psd.size() / sampleRate);
        if (offsetIndex < psd.size()) {
            phaseNoise[i] = psd[offsetIndex];
        }
    }
    
    return phaseNoise;
}

double Analyzer::calculatePower(const QVector<double>& amplitudes,
                              int startIndex,
                              int stopIndex)
{
    double power = 0.0;
    for (int i = startIndex; i <= stopIndex; ++i) {
        power += std::pow(10.0, amplitudes[i] / 10.0);  // dBm -> mW
    }
    return 10.0 * std::log10(power);  // mW -> dBm
}

int Analyzer::findFrequencyIndex(const QVector<double>& frequencies,
                               double frequency)
{
    auto it = std::lower_bound(frequencies.begin(), frequencies.end(), frequency);
    return static_cast<int>(std::distance(frequencies.begin(), it));
}

QVector<double> Analyzer::calculatePSD(const QVector<std::complex<float>>& iqData,
                                     double sampleRate)
{
    // Basit bir güç spektral yoğunluk hesabı
    QVector<double> psd(Impl::FFT_SIZE/2);
    
    // TODO: FFT ve pencere fonksiyonu uygula
    
    return psd;
}
