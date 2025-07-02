#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPrinter>
#include <QString>
#include <QMessageBox>

class QMenuBar;
class QPlainTextEdit;
class QMenu;
class QAction;
class QDialog;
class QLineEdit;
class QPushButton;
class QCheckBox;
class QRadioButton;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void onFileNew();
    void onFileOpen();
    void onFileSaveAs();
    void onFilePageSetup();
    void onFilePrint();
    void onFileExit();
    void onEditDelete();
    void onEditFind();
    void onEditFindNext();
    void onEditReplace();
    void replaceOne();
    void replaceAll();
    void replaceFindNext();
    void onGoTo();
    void onTimeDate();
    void onWordWrap(bool enabled);
    void onFont();
    void onStatusBar(bool checked);
    void updateLnColLabel();
    void onAboutNotepad();
    void onLicenseLinkClicked(const QString& link);

private:
    void setupMenus();
    bool saveFile();
    QMessageBox::StandardButton confirmDiscardChanges();
    void updateWindowTitle();
    void findNext();
    void restoreStatusBarSetting();

    QPlainTextEdit* m_mainTextEdit{};
    QMenuBar* m_menuBar{};
    //File menu
    QMenu* m_fileMenu{};
    //File menu actions
    QAction* m_fileNewAction{};
    QAction* m_fileOpenAction{};
    QAction* m_fileSaveAction{};
    QAction* m_fileSaveAsAction{};
    QAction* m_filePageSetupAction{};
    QAction* m_filePrintAction{};
    QAction* m_fileExitAction{};
    //Edit menu
    QMenu* m_editMenu{};
    //Edit menu actions
    QAction* m_editUndoAction{};
    QAction* m_editCutAction{};
    QAction* m_editCopyAction{};
    QAction* m_editPasteAction{};
    QAction* m_editDeleteAction{};
    QAction* m_editFindAction{};
    QAction* m_editFindNextAction{};
    QAction* m_editReplaceAction{};
    QAction* m_editGoToAction{};
    QAction* m_editSelectAllAction{};
    QAction* m_editTimeDateAction{};
    //Format menu
    QMenu* m_formatMenu{};
    //Format menu actions
    QAction* m_formatWordWrapAction{};
    QAction* m_formatFontAction{};
    //View menu
    QMenu* m_viewMenu{};
    //View menu actions
    QAction* m_viewStatusBarAction{};
    QLabel* m_lnColLabel{};
    //Help menu
    QMenu* m_helpMenu{};
    //Help menu actions
    QAction* m_helpViewHelpAction{};
    QAction* m_helpAboutNotepadAction{};
    //Holds the filename or path of the open file
    QString m_currentFile{};
    //For storing printer setup
    QPrinter m_printer{};
    //Find Dialog member variables for Edit menu
    QDialog* m_findDialog{};
    QLineEdit* m_findLineEdit{};
    QPushButton* m_findNextButton{};
    QCheckBox* m_findMatchCaseCheck{};
    QRadioButton* m_findUpRadio{};
    QRadioButton* m_findDownRadio{};

    QString m_findLastSearchText{};
    //Replace Dialog member variables for Edit menu
    QDialog* m_replaceDialog{};
    QLineEdit* m_replaceFindLineEdit{};
    QPushButton* m_replaceFindNextButton{};
    QLineEdit* m_replaceLineEdit{};
    QPushButton* m_replaceButton{};
    QPushButton* m_replaceAllButton{};
    QPushButton* m_replaceCancelButton{};
    QCheckBox* m_replaceMatchCaseCheckBox{};

    QString m_replaceLastSearchText{};
};
#endif // MAINWINDOW_H
