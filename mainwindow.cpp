#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDirIterator>
#include <QDebug>
#include <QTextStream>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);

    //set combo box from tables available in language db
    QDir langDir("lang");
    langDir.setFilter(QDir::Files);
    QStringList languages = langDir.entryList();
    langSelect->addItems(languages);
    //set default if there is one
    QFile lastLang("lastLang");
    if(lastLang.exists())
    {
        lastLang.open(QFile::ReadOnly);
        int index = langSelect->findText(lastLang.readAll());
        langSelect->setCurrentIndex(index);
        lastLang.close();
    }
    nativeText->selectAll();
    nativeText->setFocus();


}

MainWindow::~MainWindow()
{
    //save last language as default
    QFile lastLang("lastLang");
    lastLang.open(QFile::WriteOnly);
    lastLang.write(langSelect->currentText().toUtf8());
    lastLang.close();
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_langSelect_currentIndexChanged(QString lang)
{
    //build mapping
    QFile langFile(lang.prepend("lang/"));
    if(!langFile.open(QFile::ReadOnly))
    {
        error->setText(langFile.errorString());
    }
    else
    {
        QTextStream stream(&langFile);
        error->clear();
        QString line;
        while(true)
        {
            line = stream.readLine().trimmed();
            if(!line.contains("|"))
                break;
            QStringList chars = line.split("|");
            langMap.insert(chars.at(0), chars.at(1));
        }
            langFile.close();
    }

}

void MainWindow::on_nativeText_textChanged()
{
    if(nativeText->toPlainText().isEmpty())
    {
        latinText->clear();
        return;
    }
    QString text = nativeText->toPlainText();
    QString latin;
    for(int i=0; i<text.length(); i++)
    {
        latin.append(langMap.value(text.at(i), text.at(i)));
    }
    latinText->setText(latin);


}
