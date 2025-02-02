#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QVector>
#include <QString>
#include <memory>

class DataManager : public QObject
{
    Q_OBJECT
public:
    explicit DataManager(QObject *parent = nullptr);
    ~DataManager() override;

    // Trace kaydetme/yükleme
    bool saveTrace(const QString& filename,
                   const QVector<double>& frequencies,
                   const QVector<double>& amplitudes);

    bool loadTrace(const QString& filename,
                   QVector<double>& frequencies,
                   QVector<double>& amplitudes);

    // CSV dışa aktarma
    bool exportToCSV(const QString& filename,
                     const QVector<double>& frequencies,
                     const QVector<double>& amplitudes);

    // Durum kaydetme/yükleme
    bool saveState(const QString& filename,
                   const QByteArray& state);

    bool loadState(const QString& filename,
                   QByteArray& state);

    // Hata mesajı
    QString getLastError() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;
    QString lastError;

    // Yardımcı fonksiyonlar
    void setError(const QString& error);
    bool writeFile(const QString& filename, const QByteArray& data);
    bool readFile(const QString& filename, QByteArray& data);
};

#endif // DATAMANAGER_H
