#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QString>
#include <QVector>

class DataManager : public QObject {
    Q_OBJECT
public:
    explicit DataManager(QObject *parent = nullptr);
    
    // Veri kaydetme
    bool saveTrace(const QString& filename,
                  const QVector<double>& freqs,
                  const QVector<double>& amplitudes);
                  
    bool loadTrace(const QString& filename,
                  QVector<double>& freqs,
                  QVector<double>& amplitudes);
                  
    bool exportCSV(const QString& filename,
                  const QVector<double>& freqs,
                  const QVector<double>& amplitudes);
                  
    bool saveState(const QString& filename);
    bool loadState(const QString& filename);
    
    // IQ veri kaydetme
    bool saveIQData(const QString& filename,
                   const QVector<std::complex<float>>& iqData,
                   double sampleRate);
                   
    bool loadIQData(const QString& filename,
                   QVector<std::complex<float>>& iqData,
                   double& sampleRate);
};

#endif // DATAMANAGER_H 