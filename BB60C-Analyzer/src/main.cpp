#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Uygulama bilgileri
    QCoreApplication::setOrganizationName("BB60C");
    QCoreApplication::setApplicationName("Spectrum Analyzer");
    QCoreApplication::setApplicationVersion("1.0.0");
    
    // Koyu tema ayarları
    app.setStyle(QStyleFactory::create("Fusion"));
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    app.setPalette(darkPalette);
    
    // Ayarları yükle
    QSettings settings;
    
    // Ana pencereyi oluştur
    MainWindow mainWindow;
    
    // Son pencere durumunu yükle
    if (settings.contains("geometry")) {
        mainWindow.restoreGeometry(settings.value("geometry").toByteArray());
    }
    if (settings.contains("windowState")) {
        mainWindow.restoreState(settings.value("windowState").toByteArray());
    }
    
    mainWindow.show();
    
    // Uygulamayı çalıştır
    int result = app.exec();
    
    // Pencere durumunu kaydet
    settings.setValue("geometry", mainWindow.saveGeometry());
    settings.setValue("windowState", mainWindow.saveState());
    
    return result;
} 