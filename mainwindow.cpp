#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDirIterator>
#include <QDebug>
#include <QTextStream>
#include <QTimer>
#include <QScriptValue>
#include <QScriptEngine>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);
    charCount = 0;
    //set combo box from tables available in language db
    QDir langDir("phonetics");
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
    originalText->selectAll();
    originalText->setFocus();
    checkCharCount.setInterval(1000);
    QObject::connect(&checkCharCount, SIGNAL(timeout()), this, SLOT(hasTextChanged()));
    QObject::connect(&connection, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(translateReady(QNetworkReply*)));
    fromEnglish = false;
    updateLanguageStatus();

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
    QFile langFile(lang.prepend("phonetics/"));
    if(!langFile.open(QFile::ReadOnly))
    {
        statusMsg->setText(langFile.errorString());
    }
    else
    {
        QTextStream stream(&langFile);
        statusMsg->clear();
        QString line;

        foreignLanguage = stream.readLine().remove(0, 1).trimmed();
        updateLanguageStatus();
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

void MainWindow::on_reverseTranslation_clicked()
{
    if(!fromEnglish)
        fromEnglish = true;
    else
        fromEnglish = false;
    updateLanguageStatus();
}

void MainWindow::updateLanguageStatus()
{
    QString order;
    if(fromEnglish)
        order = QString("English to "+ langSelect->currentText());
    else
        order = QString(langSelect->currentText() +  " to English");
    translatingStatus->setText(order);
}

void MainWindow::on_originalText_textChanged()
{
    statusMsg->clear();
    if(!checkCharCount.isActive())
        checkCharCount.start();
    //do a phonetic replacement if this is not english
    if(!fromEnglish)
    {
        latinifyText(originalText->toPlainText());
    }
}

void MainWindow::on_translation_textChanged()
{
    //if current language is not english and do a phonetic replacement
    if(fromEnglish)
    {
        latinifyText(translation->toPlainText());
    }
}

void MainWindow::latinifyText(QString foreignString)
{

    QString latin;
    for(int i=0; i<foreignString.length(); i++)
    {
        latin.append(langMap.value(foreignString.at(i), foreignString.at(i)));
    }
    latinText->setText(latin);

}

bool MainWindow::hasTextChanged()
{
    //returns true if the character count has not changed in the last 2 seconds
        if(originalText->toPlainText().isEmpty())
            return false;

    if(oldCharCount == charCount)
    {
        checkCharCount.stop();
        requestTranslation();
        return true;
    }
    oldCharCount = charCount;
    return false;
}

void MainWindow::requestTranslation()
{
    QByteArray langpair;
    if(fromEnglish)
        langpair = "en|" + foreignLanguage.toUtf8();
    else
        langpair = foreignLanguage.toUtf8() + "|en";

    if(originalText->toPlainText().isEmpty())
            return;

    translation->setText(translation->toPlainText() + "...");
    QUrl url(QString("http://ajax.googleapis.com/ajax/services/language/translate"));
    url.addQueryItem("v", "1.0");
    url.addQueryItem("q", originalText->toPlainText());
    url.addQueryItem("langpair", langpair);
    request.setUrl(url);
    connection.get(request);
}


void MainWindow::translateReady(QNetworkReply* reply)
{
    QScriptValue sc;
    QScriptEngine engine;
    sc = engine.evaluate(QString(reply->readAll()));
    qDebug() << sc.property("responseStatus").toString();
    QString trans = sc.property("responseData").toObject().property("translatedText").toString();
    translation->setText(trans);
}

