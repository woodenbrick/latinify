#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"


class MainWindow : public QMainWindow, public Ui::MainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_nativeText_textChanged();
    void on_langSelect_currentIndexChanged(QString);

private:
    QMap<QString, QString> langMap;
};

#endif // MAINWINDOW_H
