#ifndef TESTDIALOG_H
#define TESTDIALOG_H

#include <QDialog>
#include <QSettings>

#include "translator_manager.h"
#include "properties.h"

namespace Ui {
    class TestDialog;
}

class TestDialog : public QDialog {
    Q_OBJECT
public:
    TestDialog(QSettings *pProp, TranslatorManager *pDictionary, const QFont *pFont, const int pBorderWidth, QWidget *parent = 0);
    ~TestDialog();
    Ui::TestDialog *ui;

protected:
    void changeEvent(QEvent *e);

private:
//    Ui::TestDialog *ui;

    QFont font;
    QSettings *prop;

    TranslatorManager *trManager;
    Translator *dictionary;
    int borderWidth;

    int translate;
    int showAnswer;
    int session;
    int questions;
    int level;

    int current;
    int correct;

private slots:
    void on_answer_returnPressed();
    void on_bnew_clicked();
    void on_bclose_helpRequested();
};

#endif // TESTDIALOG_H
