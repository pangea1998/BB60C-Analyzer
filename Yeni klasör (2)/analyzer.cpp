#include "analyzer.h"
#include <cmath>
#include <algorithm>
#include <numeric>

Analyzer::Analyzer(QObject *parent) : QObject(parent) {
}

double Analyzer::measureChannelPower(const QVector<double>& freqs,
                                   const QVector<double>& powers,
                                   double startFreq,
                                   double stopFreq) {
    double totalPower = 0;
    int count = 0;
    
    for (int i = 0; i < freqs.size(); ++i) {
        if (freqs[i] >= startFreq && freqs[i] <= stopFreq) {
            // dBm'den Watt'a dönüştür
            totalPower += std::pow(10.0, powers[i] / 10.0);
            count++;
        }
    }
    
    if (count == 0) return -INFINITY;
    
    // Ortalama gücü dBm'e geri dönüştür
    return 10.0 * std::log10(totalPower / count);
}

double Analyzer::measureOBW(const QVector<double>& freqs,
                          const QVector<double>& powers,
                          double percentage) {
    // Toplam gücü hesapla
    double totalPower = 0;
    for (const double& power : powers) {
        totalPower += std::pow(10.0, power / 10.0);
    }
    
    double targetPower = totalPower * percentage / 100.0;
    double currentPower = 0;
    
    int startIndex = 0;
    int stopIndex = powers.size() - 1;
    
    // İteratif olarak bandwidth'i bul
    while (startIndex < stopIndex) {
        double powerAtStart = std::pow(10.0, powers[startIndex] / 10.0);
        double powerAtStop = std::pow(10.0, powers[stopIndex] / 10.0);
        
        currentPower = std::accumulate(powers.begin() + startIndex,
                                     powers.begin() + stopIndex + 1,
                                     0.0,
                                     [](double sum, double p) {
                                         return sum + std::pow(10.0, p / 10.0);
                                     });
        
        if (currentPower >= targetPower) break;
        
        if (powerAtStart < powerAtStop)
            startIndex++;
        else
            stopIndex--;
    }
    
    return freqs[stopIndex] - freqs[startIndex];
}

Analyzer::ACPRResult Analyzer::measureACPR(const QVector<double>& freqs,
                                         const QVector<double>& powers,
                                         double channelBW,
                                         double channelSpacing) {
    ACPRResult result;
    
    // Ana kanal gücünü ölç
    double centerFreq = (freqs.first() + freqs.last()) / 2.0;
    result.mainChannelPower = measureChannelPower(freqs, powers,
                                                centerFreq - channelBW/2,
                                                centerFreq + channelBW/2);
    
    // Alt kanal gücünü ölç
    result.lowerChannelPower = measureChannelPower(freqs, powers,
                                                 centerFreq - channelSpacing - channelBW/2,
                                                 centerFreq - channelSpacing + channelBW/2);
    
    // Üst kanal gücünü ölç
    result.upperChannelPower = measureChannelPower(freqs, powers,
                                                 centerFreq + channelSpacing - channelBW/2,
                                                 centerFreq + channelSpacing + channelBW/2);
    
    // ACPR oranlarını hesapla
    result.lowerRatio = result.mainChannelPower - result.lowerChannelPower;
    result.upperRatio = result.mainChannelPower - result.upperChannelPower;
    
    return result;
}

QVector<Analyzer::SpurResult> Analyzer::findSpurs(const QVector<double>& freqs,
                                                const QVector<double>& powers,
                                                double threshold) {
    QVector<SpurResult> spurs;
    
    // Maksimum gücü bul
    double maxPower = *std::max_element(powers.begin(), powers.end());
    
    // Yerel maksimumları bul
    for (int i = 1; i < powers.size() - 1; ++i) {
        if (powers[i] > powers[i-1] && powers[i] > powers[i+1] && // Yerel maksimum
            powers[i] > threshold && // Eşik üstünde
            powers[i] < maxPower - 20) { // Ana sinyalden en az 20dB düşük
            
            SpurResult spur;
            spur.frequency = freqs[i];
            spur.amplitude = powers[i];
            spur.relativePower = powers[i] - maxPower;
            
            spurs.append(spur);
        }
    }
    
    return spurs;
}

QVector<double> Analyzer::measurePhaseNoise(const QVector<std::complex<float>>& iqData,
                                          double sampleRate,
                                          const QVector<double>& offsets) {
    QVector<double> phaseNoiseResults;
    // Phase noise ölçümü için FFT ve güç spektral yoğunluğu hesaplaması gerekiyor
    // Bu kısım karmaşık olduğu için şimdilik boş bırakıyorum
    return phaseNoiseResults;
} 