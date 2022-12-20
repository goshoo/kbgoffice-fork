#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

#include <QClipboard>
#include <QtWidgets/QListWidget>
#include <QSettings>
#include <QtWidgets/QSystemTrayIcon>
#include <QTranslator>
#include <QUrl>

#include "history.h"
#include "translator_manager.h"

enum TRANSLATIONS_TYPE {
    // Lycos
    LE2F, LE2G, LE2I, LE2P, LE2S,   // En2: Fr, De, It, Pt, Sp
    LF2E, LF2G, LF2I, LF2P, LF2S,   // Fr2: Fr, De, It, Pt, Sp
    LG2E, LG2F,                     // De2: En, Fr
    LI2E, LI2F,                     // It2: En, Fr
    LP2E,                           // Pt2: En
    LS2E, LS2F,                     // Sp2: En, Fr
    // Google
    GA2B, GA2E, // Auto2: Bg, En
    GE2B, GF2B, GG2B, GI2B, GP2B, GR2B, GS2B, // 2Bg: En, Fr, De, It, Pt, Ru, Sp
    GB2E, GB2F, GB2G, GB2I, GB2P, GB2R, GB2S, // Bg2: En, Fr, De, It, Pt, Ru, Sp
    // Multitran
    ME2G, ME2J, ME2R, ME2Z, // En2: De, Jp, Ru, Est
    MR2A, MR2D, MR2E, MR2F, MR2G, MR2I, MR2J, MR2K, MR2L, MR2S, MR2X, MR2Z, // Ru2: Afr, Dut, En, Fr, De, It, Jp, Km, Lat, Sp, Esp, Est
    // Wiktionary
    W2B, W2E, W2F, W2G, W2I, W2P, W2R, W2S, // in: Bg, En, Fr, De, It, Pt, Ru, Sp
    // Merriam-Webster
    MW,
    // Longman Dictionary of Contemporary English
    LDOCE
};

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QSettings *pProp, QString sDataDir, QWidget *parent = 0);
    ~MainWindow();
    bool isDataOk();
    QTranslator appTranslator;
    QTranslator qtTranslator;

protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *ev);
    void changeEvent(QEvent *e);
    void setView(QString style);
    void addWordToHistory(const char *word);
    void Translations(TRANSLATIONS_TYPE tType);
    void launchExternalBrowser(QString qstrFileName);
    void word_UpDownPressed(int keyCode);
    void createLanguageMenu();
protected slots:
//    void slotSelectionChanged();
    void slotClipboardChanged(QClipboard::Mode);
    void slotClipboardChanged(QString);
    void slotClipboardChanged();
    void slotWatchClipboardOptionChanged();
    void slotSearchInfoDone(bool bError);
    void menu_Dict();
    void switchLanguage(QAction *action);

private:
    Ui::MainWindow *ui;

    QAction *titleAction;
    QAction *watchClipboardAction;
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QClipboard *systemClipboard;
    QFont font;

    bool fromSystemHighlighted;
    bool fromListHighlighted;
    bool fromHistoryBuffer;
    bool fromListSelected;
    bool fromClipboard;
    bool fromSystem;
    bool fromTextChanged;
    bool fromAdvancedSearch;

    int inputFieldWidth;
    int leftPanelWidth;
    int wordsInList;
    int borderWidth;
    int wordsInHistory;

    bool translateOnEveryKey;
    bool watchClipboard;
    bool advancedClipboard;
    bool advancedOptions;

    bool showToolBar;
    bool dockToolBar;
    bool advSrchIgnoreKey;
    bool advSrchIgnoreHistory;
    bool advSrchEnterSearch;
    bool advSrchDoNotReplacePhrase;
    bool useExternalBrowser;
    bool sysTrayIcon;
    bool closeToSysTray;
    bool minimizeToSysTray;
    QString browserName;
    QString systemStyleName;
    QString strLink;

    bool useSplashScreen;
    bool dataOk;
    TRANSLATIONS_TYPE tType;

    QSettings *prop;
    char dataDir[1024];
    TranslatorManager *dictionary;
    History *history;

private slots:
    void on_actionAbout_Qt_triggered();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showTrayMessage();
    void messageClicked();
    void on_actionWhat_s_This_triggered();
    void on_actionHistory_Forward_changed();
    void on_actionHistory_Back_changed();
    void on_action_Test_triggered();
    void on_action_Configure_triggered();
    void on_actionHistory_Forward_triggered();
    void on_actionHistory_Back_triggered();
    void on_bsearchAll_clicked();
    void on_bsearchEncycl_clicked();
    void on_actionAdvanced_Search_triggered();
    void on_bclear_clicked();
    void on_actionSearch_in_Dic_tionary_triggered();
    void on_result_anchorClicked(QUrl qUrl);
    void on_actionSearch_in_A_ll_triggered();
    void on_actionSearch_in_Encyclopedia_triggered();
    void on_bsearchDict_clicked();
    void on_actionSelect_Font_triggered();
    void on_list_itemDoubleClicked(QListWidgetItem* item);
    void on_list_currentRowChanged(int currentRow);
    void on_word_textChanged(const QString &newText);
    void on_bsearch_clicked();
    void on_word_returnPressed();
    void on_action_Info_triggered();
    void on_action_About_triggered();
    void on_actionA_bbreviations_triggered();
    void on_action_Quit_triggered();
    void on_action_Show_ToolBar_triggered();
    void on_actionSelect_All_triggered();
    void on_actionC_lear_triggered();
    void on_action_Paste_triggered();
    void on_action_Copy_triggered();
    void on_actionSpanish_to_French_triggered();
    void on_actionSpanish_to_English_triggered();
    void on_actionPortuguese_to_English_triggered();
    void on_actionItalian_to_French_triggered();
    void on_actionItalian_to_English_triggered();
    void on_actionGerman_to_French_triggered();
    void on_actionGerman_to_English_triggered();
    void on_actionFrench_to_Spanish_triggered();
    void on_actionFrench_to_Portuguese_triggered();
    void on_actionFrench_to_Italian_triggered();
    void on_actionFrench_to_German_triggered();
    void on_actionFrench_to_English_triggered();
    void on_actionEnglish_to_Spanish_triggered();
    void on_actionEnglish_to_Portuguese_triggered();
    void on_actionEnglish_to_Italian_triggered();
    void on_actionEnglish_to_German_triggered();
    void on_actionEnglish_to_French_triggered();
    void on_actionAuto_to_Bulgarian_triggered();
    void on_actionAuto_to_English_triggered();
    void on_actionBulgarian_to_English_triggered();
    void on_actionBulgarian_to_French_triggered();
    void on_actionBulgarian_to_German_triggered();
    void on_actionBulgarian_to_Italian_triggered();
    void on_actionBulgarian_to_Portuguesse_triggered();
    void on_actionBulgarian_to_Spanish_triggered();
    void on_actionBulgarian_to_Russian_triggered();
    void on_actionEnglish_to_Bulgarian_triggered();
    void on_actionFrench_to_Bulgarian_triggered();
    void on_actionGerman_to_Bulgarian_triggered();
    void on_actionItalian_to_Bulgarian_triggered();
    void on_actionPortuguese_to_Bulgarian_triggered();
    void on_actionRussian_to_Bulgarian_triggered();
    void on_actionSpanish_to_Bulgarian_triggered();
    void on_actionEnglish_to_GermanMT_triggered();
    void on_actionEnglish_to_Japanese_triggered();
    void on_actionEnglish_to_Russian_triggered();
    void on_actionEnglish_to_Estonian_triggered();
    void on_actionRussian_to_Afrikaans_triggered();
    void on_actionRussian_to_Dutch_triggered();
    void on_actionRussian_to_English_triggered();
    void on_actionRussian_to_French_triggered();
    void on_actionRussian_to_German_triggered();
    void on_actionRussian_to_Italian_triggered();
    void on_actionRussian_to_Japanese_triggered();
    void on_actionRussian_to_Kalmik_triggered();
    void on_actionRussian_to_Latvian_triggered();
    void on_actionRussian_to_Spanish_triggered();
    void on_actionRussian_to_Estonian_triggered();
    void on_actionRussian_to_Experanto_triggered();
    void on_actionMeriam_Webster_triggered();
    void on_actionLongman_triggered();
    void on_actionIn_Bulgarian_triggered();
    void on_actionIn_English_triggered();
    void on_actionIn_French_triggered();
    void on_actionIn_German_triggered();
    void on_actionIn_Italian_triggered();
    void on_actionIn_Portuguesse_triggered();
    void on_actionIn_Russian_triggered();
    void on_actionIn_Spanish_triggered();
};

#endif // MAINWINDOW_H
