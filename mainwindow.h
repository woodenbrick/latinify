#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include "ui_mainwindow.h"


class MainWindow : public QMainWindow, public Ui::MainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_originalText_textChanged();
    void on_translation_textChanged();
    void on_langSelect_currentIndexChanged(QString);
    void on_reverseTranslation_clicked();
    void translateReady(QNetworkReply*);
    bool hasTextChanged();
    void requestTranslation();

private:
    QMap<QString, QString> langMap;
    QNetworkAccessManager connection;
    QNetworkRequest request;
    QTimer checkCharCount;
    int charCount;
    int oldCharCount;
    QString foreignLanguage;
    bool fromEnglish;
    void latinifyText(QString);
    void  updateLanguageStatus();

};

#endif // MAINWINDOW_H
