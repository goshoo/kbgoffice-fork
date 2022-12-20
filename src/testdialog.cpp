#include "testdialog.h"
#include "ui_testdialog.h"
#include "ui_testoptions.h"
#include "encodechar.h"
#include <time.h>
#include <QStyleFactory>

/*
* This is main test/exam window
* Written by Radostin Radnev - radnev@yahoo.com
*/

// Some constants
static const int  MAX_ANSWER_LEN    = 100;
static const int  MIN_LEFT_WIDTH    = 100;
static const int  SPACE_IN_DIALOGS  = 10;

// Welcome, help and other messages
static const char CAPTION[]         = QT_TRANSLATE_NOOP("TestDialog", "bgOffice Test Suite");
static const char FOUND[]           = QT_TRANSLATE_NOOP("TestDialog", ":-)");
static const char NOT_FOUND[]       = QT_TRANSLATE_NOOP("TestDialog", ":-(");
static const char SIMILAR[]         = QT_TRANSLATE_NOOP("TestDialog", ":-|");
static const char END_OF_TEST[]     = QT_TRANSLATE_NOOP("TestDialog", "End of session. Start again or exit.");
static const char WELCOME_TEXT[]    = QT_TRANSLATE_NOOP("TestDialog", "To begin new test press button \"New Test\".");
static const char HELP_TEXT[]       = QT_TRANSLATE_NOOP("TestDialog",
        "Enter answer that you think is correct.<br>"
        "Use <b>comma</b> (<b>,</b>) to separate two or more answers that you may have.<br>");

// Status Info and word format message
static const char STATUS_INFO[] = QT_TRANSLATE_NOOP("TestDialog", " Correct  %1%  ( %2 / %3 )     Asked  %4%  ( %5 / %6 )");
static const char ASKED[]       = QT_TRANSLATE_NOOP("TestDialog", "%1.  %2");

// Color objects for found and not found
//static const QColor COLOR_FOUND(0, 255, 0);
//static const QColor COLOR_NOT_FOUND(255, 0, 0);
static const char COLOR_FOUND[]          = "background-color: #00FF00"; // green
static const char COLOR_NOT_FOUND[]      = "background-color: #FF0000"; // red

// Default options
static const char DEFAULT_GEOMETRY[]              = "50, 50, 350, 250";
static const int  DEFAULT_TRANSLATE               = 1;
static const int  DEFAULT_SHOW_ANSWER             = 1;
static const int  DEFAULT_SESSION                 = 0;
static const int  DEFAULT_QUESTIONS               = 100;
static const int  DEFAULT_LEVEL                   = 3;

// Min and max allowed numbers
static const int  MIN_SHOW_ANSWER                 = 1;
static const int  MAX_SHOW_ANSWER                 = 5;
static const int  MIN_SESSION                     = 0;
static const int  MAX_SESSION                     = 1000000;
static const int  STEP_SESSION                    = 1;
static const int  MIN_QUESTIONS                   = 10;
static const int  MAX_QUESTIONS                   = 10000;
static const int  STEP_QUESTIONS                  = 10;
static const int  MIN_LEVEL                       = 1;
static const int  MAX_LEVEL                       = 5;

// Properties keys
static const char OPTION_GEOMETRY[]               = "Test/Geometry";
static const char OPTION_TRANSLATE[]              = "Test/Translate";
static const char OPTION_SHOW_ANSWER[]            = "Test/ShowAnswer";
static const char OPTION_SESSION[]                = "Test/Session";
static const char OPTION_QUESTIONS[]              = "Test/Questions";
static const char OPTION_LEVEL[]                  = "Test/Level";

TestDialog::TestDialog(QSettings *pProp, TranslatorManager *pDictionary, const QFont *pFont, const int pBorderWidth, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestDialog)
{
    ui->setupUi(this);
/*
    // Moved to caller (bclose, bhelp)- seems on show resets this!
    QPushButton *bcloze= ui->bclose->button( QDialogButtonBox::Close );
    Q_CHECK_PTR(bcloze);
    if(bcloze) bcloze->setAutoDefault(false);
*/
    prop = pProp;
    trManager = pDictionary;
    dictionary = NULL;
    borderWidth = pBorderWidth;
    font = QFont(*pFont);
    setFont(font);

    int X = 0, Y = 0, W = 0, H = 0;
    sscanf(DEFAULT_GEOMETRY, "%d, %d, %d, %d", &X, &Y, &W, &H);
    sscanf(prop->value(OPTION_GEOMETRY, DEFAULT_GEOMETRY).toString().toLatin1(), "%d, %d, %d, %d", &X, &Y, &W, &H);
    resize(QSize(W, H));
    move(QPoint(X, Y));

    translate = prop->value(OPTION_TRANSLATE, DEFAULT_TRANSLATE).toInt();

    showAnswer = prop->value(OPTION_SHOW_ANSWER, DEFAULT_SHOW_ANSWER).toInt();
    if(showAnswer < MIN_SHOW_ANSWER)        showAnswer = MIN_SHOW_ANSWER;
    else if(showAnswer > MAX_SHOW_ANSWER)   showAnswer = MAX_SHOW_ANSWER;

    session = prop->value(OPTION_SESSION, DEFAULT_SESSION).toInt();
    if(session < MIN_SESSION)        session = MIN_SESSION;
    else if(session > MAX_SESSION)   session = MAX_SESSION;

    questions = prop->value(OPTION_QUESTIONS, DEFAULT_QUESTIONS).toInt();
    if(questions < MIN_QUESTIONS)        questions = MIN_QUESTIONS;
    else if(questions > MAX_QUESTIONS)   questions = MAX_QUESTIONS;

    level = prop->value(OPTION_LEVEL, DEFAULT_LEVEL).toInt();
    if(level < MIN_LEVEL)        level = MIN_LEVEL;
    else if(level > MAX_LEVEL)   level = MAX_LEVEL;

    current = questions + 1;
    correct = 0;

    ui->answer->setMaxLength(MAX_ANSWER_LEN);
    ui->answer->adjustSize();

    ui->question->setMaximumHeight(ui->answer->height());
    ui->found->setMaximumWidth(ui->answer->height());
    ui->found->setMinimumWidth(ui->answer->height());
    ui->found->setMaximumHeight(ui->answer->height());
    ui->found->setMinimumHeight(ui->answer->height());
    ui->word->setMaximumHeight(ui->answer->height());

    ui->answer->setFocus();
    ui->question->setText(tr(WELCOME_TEXT));
}

TestDialog::~TestDialog()
{
    prop->setValue(OPTION_TRANSLATE, translate);
    prop->setValue(OPTION_SHOW_ANSWER, showAnswer);
    prop->setValue(OPTION_SESSION, session);
    prop->setValue(OPTION_QUESTIONS, questions);
    prop->setValue(OPTION_LEVEL, level);
    if (!isMinimized()) {
            QString val = QString("%1, %2, %3, %4").arg(pos().x()).arg(pos().y()).arg(size().width()).arg(size().height());
            prop->setValue(OPTION_GEOMETRY, val);
    }
    prop->sync();

    if (dictionary != NULL)
        delete dictionary;

    delete ui;
}

void TestDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void TestDialog::on_answer_returnPressed()
{
    if (current > questions) {
            // Do nothing
            ui->answer->clear();
            return;
    }
    // Clear fields
    ui->result->clear();
    // Check for new sessoin
    if (current == 0) {
            // Start new session
            if (session == 0) {
                    srand(time(NULL));
            } else {
                    srand(session);
            }
            ui->answer->clear();
            ui->word->clear();
            ui->found->clear();
            ui->found->setStyleSheet(ui->word->styleSheet());
    } else {
            // Check result
            if (dictionary->testWord(U2W(ui->answer->text()))) {
                    correct++;
                    ui->found->setStyleSheet(COLOR_FOUND);
                    ui->found->setText(tr(FOUND));
            } else {
                    ui->found->setStyleSheet(COLOR_NOT_FOUND);
                    ui->found->setText(tr(NOT_FOUND));
                    if (((showAnswer == 2) && ((ui->answer->text()).length() > 0))  ||
                        ((showAnswer == 3) && ((ui->answer->text()).length() == 0)) || (showAnswer == 4)) {
                            ui->result->setText(W2U(dictionary->getResult()));
                            ui->result->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
                    }
            }
            ui->word->setText(ui->answer->text());
            ui->answer->clear();
            if (showAnswer == 1) {
                    ui->result->setText(W2U(dictionary->getResult()));
                    ui->result->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
            }
    }
    // Show status
    ui->info->setText(tr(STATUS_INFO)
                    .arg((current == 0 ? 0 : (correct*100)/current))
                    .arg(correct)
                    .arg(current)
                    .arg((questions == 0 ? 0 : (current*100)/questions))
                    .arg(current)
                    .arg(questions));
    // Increase current ponter
    current++;
    // Generata next random word or end of test
    if (current > questions) {
            ui->question->setText(tr(END_OF_TEST));
    } else {
            ui->question->setText(tr(ASKED)
                            .arg(current)
                            .arg(W2U(dictionary->getNextRandomWord())));
    }
}

void TestDialog::on_bnew_clicked()
{
    Ui::NewTest tui;
    QDialog options(this);
    tui.setupUi(&options);

    for(int i = 0; i < trManager->sizeOfTestDictionaries(); i++)
        tui.tf->addItem(W2U(trManager->getTestDictionary(i).name.c_str()));
    if (translate - 1 < trManager->sizeOfTestDictionaries())
        tui.tf->setCurrentIndex(translate - 1);
    else
        tui.tf->setCurrentIndex(0);

    tui.sa->setCurrentIndex(showAnswer - 1);

    tui.sn->setMinimum(MIN_SESSION);
    tui.sn->setMaximum(MAX_SESSION);
    tui.sn->setSingleStep(STEP_SESSION);
    tui.sn->setValue(session);

    tui.qn->setMinimum(MIN_QUESTIONS);
    tui.qn->setMaximum(MAX_QUESTIONS);
    tui.qn->setSingleStep(STEP_QUESTIONS);
    tui.qn->setValue(questions);

    tui.lv->setCurrentIndex(level - 1);

    // Execute dialog and check for OK button
    if (options.exec() == QDialog::Accepted) {
        translate = tui.tf->currentIndex() + 1;
        showAnswer = tui.sa->currentIndex() + 1;
        level = tui.lv->currentIndex() + 1;
        session = tui.sn->value();
        questions = tui.qn->value();
        current = 0;
        correct = 0;
        if (dictionary != NULL)
            delete dictionary;

        dictionary = trManager->getTestDictionaryObject(translate - 1, level);
        ui->answer->setFocus();
        on_answer_returnPressed();
    }
}

void TestDialog::on_bclose_helpRequested()
{
    ui->result->setText(tr(HELP_TEXT));
    ui->result->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
}
