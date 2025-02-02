#ifndef ANALYZER_H
#define ANALYZER_H

#include <QObject>
#include <QVector>

class Analyzer : public QObject {
    Q_OBJECT
public:
    explicit Analyzer(QObject *parent = nullptr);
    
    // Ölçüm fonksiyonları
    double measureChannelPower(const QVector<double>& freqs, 
                             const QVector<double>& powers,
                             double startFreq,
                             double stopFreq);
                             
    double measureOBW(const QVector<double>& freqs,
                     const QVector<double>& powers,
                     double percentage);
                     
    struct ACPRResult {
        double mainChannelPower;
        double lowerChannelPower;
        double upperChannelPower;
        double lowerRatio;
        double upperRatio;
    };
    
    ACPRResult measureACPR(const QVector<double>& freqs,
                          const QVector<double>& powers,
                          double channelBW,
                          double channelSpacing);
                          
    struct SpurResult {
        double frequency;
        double amplitude;
        double relativePower;
    };
    
    QVector<SpurResult> findSpurs(const QVector<double>& freqs,
                                 const QVector<double>& powers,
                                 double threshold);
                                 
    QVector<double> measurePhaseNoise(const QVector<std::complex<float>>& iqData,
                                     double sampleRate,
                                     const QVector<double>& offsets);
};

#endif // ANALYZER_H 