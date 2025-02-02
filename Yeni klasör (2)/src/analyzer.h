#ifndef ANALYZER_H
#define ANALYZER_H

#include <QObject>
#include <QVector>
#include <complex>
#include <memory>

// Ölçüm sonuçları için yapılar
struct ACPRResult {
    double mainChannelPower{0.0};    // Ana kanal gücü (dBm)
    double lowerChannelPower{0.0};   // Alt kanal gücü (dBm)
    double upperChannelPower{0.0};   // Üst kanal gücü (dBm)
    double lowerRatio{0.0};          // Alt kanal oranı (dB)
    double upperRatio{0.0};          // Üst kanal oranı (dB)
};

struct SpurResult {
    double frequency{0.0};       // Spur frekansı (Hz)
    double amplitude{-120.0};    // Spur genliği (dBm)
    double relativePower{0.0};   // Bağıl güç (dB)
};

class Analyzer : public QObject {
    Q_OBJECT
public:
    explicit Analyzer(QObject *parent = nullptr);
    ~Analyzer() override;

    // Temel ölçümler
    double measureChannelPower(const QVector<double>& frequencies,
                             const QVector<double>& amplitudes,
                             double startFreq,
                             double stopFreq);

    double measureOBW(const QVector<double>& frequencies,
                     const QVector<double>& amplitudes,
                     double percentPower);

    ACPRResult measureACPR(const QVector<double>& frequencies,
                          const QVector<double>& amplitudes,
                          double channelBW,
                          double channelSpacing);

    QVector<SpurResult> findSpurs(const QVector<double>& frequencies,
                                 const QVector<double>& amplitudes,
                                 double threshold);

    QVector<double> measurePhaseNoise(const QVector<std::complex<float>>& iqData,
                                    double sampleRate,
                                    const QVector<double>& offsets);

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;

    // Yardımcı fonksiyonlar
    double calculatePower(const QVector<double>& amplitudes,
                         int startIndex,
                         int stopIndex);

    int findFrequencyIndex(const QVector<double>& frequencies,
                          double frequency);

    QVector<double> calculatePSD(const QVector<std::complex<float>>& iqData,
                               double sampleRate);
};

#endif // ANALYZER_H
