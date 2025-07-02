#include "mainwindow.h"
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QAction>
#include <QKeySequence>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QPageSetupDialog>
#include <QPrintDialog>
#include <QTextCursor>
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QRadioButton>
#include <QTextCursor>
#include <QChar>
#include <QInputDialog>
#include <QTextBlock>
#include <QDateTime>
#include <QLocale>
#include <QFont>
#include <QFontDialog>
#include <QSettings>
#include <QStatusBar>
#include <QDesktopServices>
#include <QUrl>
#include <QPixmap>
#include <QFrame>
#include <QDialogButtonBox>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Untitled - %1").arg(QCoreApplication::applicationName()));

    //'this' sets MainWindow as the parent
    // 'this' ensures the editor is deleted with the window
    //QPlainTextEdit (unformatted text, efficient for large files)
    m_mainTextEdit = new QPlainTextEdit(this);

    //Tell QMainWindow to use it as main content
    setCentralWidget(m_mainTextEdit);

    setupMenus();

    m_editUndoAction->setEnabled(false);
    m_editCutAction->setEnabled(false);
    m_editCopyAction->setEnabled(false);
    m_editDeleteAction->setEnabled(false);
    m_editFindAction->setEnabled(false);
    m_editFindNextAction->setEnabled(false);

    m_formatWordWrapAction->setCheckable(true);
    m_formatWordWrapAction->setChecked(false);
    m_mainTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);

    restoreStatusBarSetting();

    connect(m_mainTextEdit->document(),&QTextDocument::undoAvailable, m_editUndoAction, &QAction::setEnabled);
    connect(m_mainTextEdit, &QPlainTextEdit::copyAvailable, m_editCutAction, &QAction::setEnabled);
    connect(m_mainTextEdit, &QPlainTextEdit::copyAvailable, m_editCopyAction, &QAction::setEnabled);
    connect(m_mainTextEdit, &QPlainTextEdit::copyAvailable, m_editDeleteAction, &QAction::setEnabled);
    connect(m_mainTextEdit, &QPlainTextEdit::textChanged, this, [this](){
        QString content = m_mainTextEdit->toPlainText().trimmed();
        m_editFindAction->setEnabled(!content.isEmpty());
        m_editFindNextAction->setEnabled(!content.isEmpty());
    });
    connect(m_mainTextEdit, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::updateLnColLabel);


    connect(m_fileNewAction, &QAction::triggered, this, &MainWindow::onFileNew);
    connect(m_fileOpenAction, &QAction::triggered, this, &MainWindow::onFileOpen);
    connect(m_fileSaveAction, &QAction::triggered, this, [this](){saveFile();});
    connect(m_fileSaveAsAction, &QAction::triggered, this, &MainWindow::onFileSaveAs);
    connect(m_filePageSetupAction, &QAction::triggered, this, &MainWindow::onFilePageSetup);
    connect(m_filePrintAction, &QAction::triggered, this, &MainWindow::onFilePrint);
    connect(m_fileExitAction, &QAction::triggered, this, &MainWindow::onFileExit);
    connect(m_editUndoAction, &QAction::triggered, this, [this](){m_mainTextEdit->undo();});
    connect(m_editCutAction, &QAction::triggered, this, [this](){m_mainTextEdit->cut();});
    connect(m_editCopyAction, &QAction::triggered, this, [this](){m_mainTextEdit->copy();});
    connect(m_editPasteAction, &QAction::triggered, this, [this](){m_mainTextEdit->paste();});
    connect(m_editDeleteAction, &QAction::triggered, this, &MainWindow::onEditDelete);
    connect(m_editFindAction, &QAction::triggered, this, &MainWindow::onEditFind);
    connect(m_editFindNextAction, &QAction::triggered, this, &MainWindow::onEditFindNext);
    connect(m_editReplaceAction, &QAction::triggered, this, &MainWindow::onEditReplace);
    connect(m_editGoToAction, &QAction::triggered, this, &MainWindow::onGoTo);
    connect(m_editSelectAllAction, &QAction::triggered, this, [this](){if(m_mainTextEdit) m_mainTextEdit->selectAll();});
    connect(m_editTimeDateAction, &QAction::triggered, this, &MainWindow::onTimeDate);
    connect(m_formatWordWrapAction, &QAction::toggled, this, &MainWindow::onWordWrap);
    connect(m_formatFontAction, &QAction::triggered, this, &MainWindow::onFont);
    connect(m_viewStatusBarAction, &QAction::toggled, this, &MainWindow::onStatusBar);
    connect(m_helpViewHelpAction, &QAction::triggered, this, [](){
        QDesktopServices::openUrl(QUrl("https://www.bing.com/search?q=get+help+with+notepad+in+windows"));
    });
    connect(m_helpAboutNotepadAction, &QAction::triggered, this, &MainWindow::onAboutNotepad);

}

MainWindow::~MainWindow(){}

void MainWindow::setupMenus()
{
    //Access the built-in menu bar
    m_menuBar = menuBar();
    //Instantiate File menu
    m_fileMenu = m_menuBar->addMenu(tr("&File"));
    //Instantiate File menu actions and shortcuts
    m_fileNewAction = new QAction(tr("New"), this);
    m_fileNewAction->setShortcut(QKeySequence::New);
    m_fileOpenAction = new QAction(tr("Open..."), this);
    m_fileOpenAction->setShortcut(QKeySequence::Open);
    m_fileSaveAction = new QAction(tr("Save"), this);
    m_fileSaveAction->setShortcut(QKeySequence::Save);
    m_fileSaveAsAction = new QAction(tr("Save As..."), this);
    m_fileSaveAsAction->setShortcut(QKeySequence::SaveAs);
    m_filePageSetupAction = new QAction(tr("Page Setup..."), this);
    m_filePrintAction = new QAction(tr("Print..."), this);
    m_filePrintAction->setShortcut(QKeySequence::Print);
    m_fileExitAction = new QAction(tr("Exit"), this);
    //Add File menu actions and separators
    m_fileMenu->addActions({m_fileNewAction, m_fileOpenAction,
                            m_fileSaveAction, m_fileSaveAsAction});
    m_fileMenu->addSeparator();
    m_fileMenu->addActions({m_filePageSetupAction, m_filePrintAction});
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_fileExitAction);
    //Instantiate Edit menu
    m_editMenu = m_menuBar->addMenu(tr("&Edit"));
    //Instantiate Edit menu actions and shortcuts
    m_editUndoAction = new QAction(tr("Undo"), this);
    m_editUndoAction->setShortcut(QKeySequence::Undo);
    m_editCutAction = new QAction(tr("Cut"), this);
    m_editCutAction->setShortcut(QKeySequence::Cut);
    m_editCopyAction = new QAction(tr("Copy"), this);
    m_editCopyAction->setShortcut(QKeySequence::Copy);
    m_editPasteAction = new QAction(tr("Paste"), this);
    m_editPasteAction->setShortcut(QKeySequence::Paste);
    m_editDeleteAction = new QAction(tr("Delete"), this);
    m_editDeleteAction->setShortcut(QKeySequence::Delete);
    m_editFindAction = new QAction(tr("Find..."), this);
    m_editFindAction->setShortcut(QKeySequence::Find);
    m_editFindNextAction = new QAction(tr("Find Next"), this);
    m_editFindNextAction->setShortcut(QKeySequence::FindNext);
    m_editReplaceAction = new QAction(tr("Replace..."), this);
    m_editReplaceAction->setShortcut(QKeySequence::Replace);
    m_editGoToAction = new QAction(tr("Go To..."), this);
    m_editGoToAction->setShortcut(QKeySequence("Ctrl+G"));
    m_editSelectAllAction = new QAction(tr("SelectAll"), this);
    m_editSelectAllAction->setShortcut(QKeySequence::SelectAll);
    m_editTimeDateAction = new QAction(tr("Time/Date"), this);
    m_editTimeDateAction->setShortcut(QKeySequence("F5"));
    //Edit menu actions and separators
    m_editMenu->addAction(m_editUndoAction);
    m_editMenu->addSeparator();
    m_editMenu->addActions({m_editCutAction, m_editCopyAction,
                            m_editPasteAction, m_editDeleteAction});
    m_editMenu->addSeparator();
    m_editMenu->addActions({m_editFindAction, m_editFindNextAction,
                            m_editReplaceAction, m_editGoToAction});
    m_editMenu->addSeparator();
    m_editMenu->addActions({m_editSelectAllAction, m_editTimeDateAction});
    //Instantiate Format menu
    m_formatMenu = m_menuBar->addMenu(tr("&Format"));
    //Instantiate Format menu actions
    m_formatWordWrapAction = new QAction(tr("Word Wrap"), this);
    m_formatFontAction = new QAction(tr("Font..."), this);
    m_formatMenu->addActions({m_formatWordWrapAction, m_formatFontAction});
    //Instantiate View menu
    m_viewMenu = m_menuBar->addMenu(tr("&View"));
    //Instantiate View menu action
    m_viewStatusBarAction = new QAction(tr("Status Bar"), this);
    m_viewStatusBarAction->setCheckable(true);

    m_lnColLabel = new QLabel(this);
    m_lnColLabel->setText("Ln 1, Col 1");

    statusBar()->addPermanentWidget(m_lnColLabel);

    m_viewMenu->addAction(m_viewStatusBarAction);
    //Instantiate Help menu
    m_helpMenu = m_menuBar->addMenu(tr("&Help"));
    //Instantiate Help menu actions
    m_helpViewHelpAction = new QAction(tr("View Help"), this);
    m_helpAboutNotepadAction = new QAction(tr("About Notepad"), this);
    m_helpMenu->addAction(m_helpViewHelpAction);
    m_helpMenu->addSeparator();
    m_helpMenu->addAction(m_helpAboutNotepadAction);
}

void MainWindow::onFileNew()
{
    if(!m_mainTextEdit->document()->isModified())
    {
        m_mainTextEdit->clear();
        setWindowTitle(tr("Untitled - %1").arg(QCoreApplication::applicationName()));
        return;
    }

        QMessageBox::StandardButton reply = confirmDiscardChanges();

        if (reply == QMessageBox::Yes)
        {
            saveFile();
        }
        else if (reply == QMessageBox::No)
        {
            m_mainTextEdit->clear();
            updateWindowTitle();
        }
}

void MainWindow::onFileOpen()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), "",
                                    tr("Text Files (*.txt);;All Files (*)"));
    if(fileName.isEmpty()) return;

    QFile file(fileName);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("Cannot open file: \n%1").arg(file.errorString()));
        return;
    }

    QTextStream in(&file);

    m_mainTextEdit->setPlainText(in.readAll());
    file.close();

    m_currentFile = fileName;

    updateWindowTitle();
}

bool MainWindow::saveFile()
{
    if(m_currentFile.isEmpty())
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save as"), "",
                            tr("Text Files (*.txt);;All Files (*)"));
        if(fileName.isEmpty()) return false;
        m_currentFile = fileName;
    }

    QFile file(m_currentFile);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("Cannot save file:\n%1").arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out << m_mainTextEdit->toPlainText();
    file.close();
    m_mainTextEdit->document()->setModified(false);

    updateWindowTitle();

    return true;
}

QMessageBox::StandardButton MainWindow::confirmDiscardChanges()
{
    QMessageBox messageBox(this);

    messageBox.setWindowTitle(tr("Notepad"));

    //Main message
    messageBox.setText(tr("Do you want to save changes to Untitled?"));

    messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    messageBox.setDefaultButton(QMessageBox::Yes);

    //Customize the text labels of the buttons
    QPushButton* yesButton = qobject_cast<QPushButton*>(messageBox.button(QMessageBox::Yes));

    QPushButton* noButton = qobject_cast<QPushButton*>(messageBox.button(QMessageBox::No));

    QPushButton* cancelButton = qobject_cast<QPushButton*>(messageBox.button(QMessageBox::Cancel));


    if(yesButton) yesButton->setText(tr("Save"));
    if(noButton) noButton->setText(tr("Don't Save"));
    if(cancelButton) cancelButton->setText(tr("Cancel"));

    //exec() return int and we convert it to enum
    return static_cast<QMessageBox::StandardButton>(messageBox.exec());
}

void MainWindow::updateWindowTitle()
{
    QString fileName = m_currentFile.isEmpty() ? tr("Untitled") : QFileInfo(m_currentFile).completeBaseName();
    //if(m_mainTextEdit->document()->isModified()) fileName += "*";

    setWindowTitle(fileName + " - " + QCoreApplication::applicationName());
}

void MainWindow::findNext()
{
    if(!m_mainTextEdit || !m_findLineEdit)
    { return; }

    QString searchText = m_findLineEdit->text().trimmed();

    if(searchText.isEmpty()) {return;}

    QTextDocument::FindFlags findFlags{};

    if(m_findMatchCaseCheck && m_findMatchCaseCheck->isChecked())
    {
        findFlags |= QTextDocument::FindCaseSensitively;
    }
    if(m_findDownRadio && !m_findDownRadio->isChecked())
    {
        findFlags |= QTextDocument::FindBackward;
    }

    bool foundMatch = m_mainTextEdit->find(searchText, findFlags);

    if(foundMatch)
    {
        m_findLastSearchText = searchText;

        m_mainTextEdit->setStyleSheet("QPlainTextEdit{selection-background-color: yellow; color: black}");
    }
    else
    {
        QMessageBox::information(this, QCoreApplication::applicationName(),
                                 tr("Cannot find *%1*").arg(searchText));
    }

}

void MainWindow::restoreStatusBarSetting()
{
    QSettings settings{};
    if(settings.contains("ShowStatusBar"))
    {
        bool showStatusBar{settings.value("ShowStatusBar").toBool()};

        statusBar()->setVisible(showStatusBar);
        m_viewStatusBarAction->setChecked(showStatusBar);
    }
    else
    {
        statusBar()->setVisible(false);
        m_viewStatusBarAction->setChecked(false);
    }
}

void MainWindow::updateLnColLabel()
{
    QTextCursor cursor{m_mainTextEdit->textCursor()};
    int line{cursor.blockNumber() + 1};
    int column{cursor.positionInBlock() + 1};

    m_lnColLabel->setText(QString("Ln %1, Col %2").arg(line).arg(column));
}

void MainWindow::onAboutNotepad()
{
    QDialog aboutDialog{this};
    aboutDialog.setWindowTitle(tr("About Notepad"));
    aboutDialog.setFixedSize(460, 370);

    QVBoxLayout* mainLayout{new QVBoxLayout(&aboutDialog)};
    QHBoxLayout* topLayout{new QHBoxLayout()};
    topLayout->setAlignment(Qt::AlignCenter);

    QLabel* windowsLogo{new QLabel()};
    windowsLogo->setPixmap(QPixmap(":/icons/windows.png")
            .scaled(70, 70, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    windowsLogo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QLabel* windowsText{new QLabel("Windows 10")};
    windowsText->setStyleSheet("font-size: 50px; color: rgb(80, 160, 255)");
    windowsText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    topLayout->addWidget(windowsLogo);
    topLayout->addWidget(windowsText);
    mainLayout->addLayout(topLayout);

    //Line separator
    QFrame* line{new QFrame()};
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("color: gray");

    mainLayout->addWidget(line);

    //Notepad section
    QHBoxLayout* notepadLayout{new QHBoxLayout()};

    QLabel* notepadLogo{new QLabel()};

    notepadLogo->setPixmap(QPixmap(":/icons/notepad.png")
        .scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    notepadLogo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QLabel* notepadText{new QLabel("Microsoft Windows\n"
                "Version 1803 (OS Build 17134.165)\n"
                "2018 Microsoft Corporation. All rights reserved.\n"
                "The Windows 10 Pro operating system and its user "
                "interface are protected \nby trademark and other "
                "pending or existing intellectual property rights in\n"
                "the United States and other countries/regions.")};

    notepadText->setStyleSheet("font-size: 11px; color: black");

    notepadLayout->addWidget(notepadLogo, 0, Qt::AlignTop);
    notepadLayout->addWidget(notepadText, 0, Qt::AlignTop);

    mainLayout->addLayout(notepadLayout);

    //License text with username
    QString userName{QString::fromLocal8Bit(getenv("USERNAME"))};
    if(userName.isEmpty())
    {
        userName = QString::fromLocal8Bit(getenv("USER"));
    }
    QLabel* licenseLabel{new QLabel()};

    licenseLabel->setTextFormat(Qt::RichText);
    licenseLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    licenseLabel->setOpenExternalLinks(false);
    licenseLabel->setText(QString("This product is licensed under the "
        "<a href='license'>Microsoft Software License terms</a> to: "
        "<p style='margin-left: 0 0 0 20px;'>%1</p>").arg(userName));
    licenseLabel->setStyleSheet("font-size: 11px; color: black");
    licenseLabel->setContentsMargins(35, 0, 0, 0);

    mainLayout->addWidget(licenseLabel);
    //Connect ON LICENSE CLICKED
    connect(licenseLabel, &QLabel::linkActivated, this, &MainWindow::onLicenseLinkClicked);

    QDialogButtonBox* buttonBox{new QDialogButtonBox(QDialogButtonBox::Ok)};

    mainLayout->addWidget(buttonBox);

    connect(buttonBox, QDialogButtonBox::accepted, &aboutDialog, &QDialog::accept);
    //Show the modal dialog
    aboutDialog.exec();
}

void MainWindow::onLicenseLinkClicked(const QString& link)
{
    if(link == "license")
    {
        QDialog licenseDialog(this);

        licenseDialog.setWindowTitle(tr("Microsoft Software License Term"));
        licenseDialog.setFixedSize(630, 400);

        QVBoxLayout* licenseLayout{new QVBoxLayout(&licenseDialog)};
        QTextBrowser* licenseView{new QTextBrowser()};
        licenseView->setReadOnly(true);

        //Load HTML from embeded resources
        QFile licenseFile(":/licenseTerm/licenseTerm.html");

        if(licenseFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&licenseFile);
            licenseView->setHtml(in.readAll());
        }
        else
        {
            licenseView->setText("Failed to load license file");
        }

        licenseLayout->addWidget(licenseView);

        QDialogButtonBox* buttonBox{new QDialogButtonBox(QDialogButtonBox::Ok)};

        licenseLayout->addWidget(buttonBox, 0, Qt::AlignRight);

        connect(buttonBox, &QDialogButtonBox::accepted, &licenseDialog, &QDialog::accept);

        licenseDialog.exec();
    }
}

void MainWindow::onFileSaveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), "",
                                      tr("Text Files (*.txt);;All Files(*)"));

    if(fileName.isEmpty()) return;

    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("Error"), tr("Cannot save file:\n%1").arg(file.errorString()));
        return;
    }

    QTextStream out(&file);
    out << m_mainTextEdit->toPlainText();
    file.close();

    m_mainTextEdit->document()->setModified(false);

    m_currentFile = fileName;

    updateWindowTitle();
}

void MainWindow::onFilePageSetup()
{
    QPageSetupDialog pageSetupDialog(&m_printer, this);

    if(pageSetupDialog.exec() == QDialog::Accepted){}
}

void MainWindow::onFilePrint()
{
    QPrintDialog printDialog(&m_printer, this);

    if(printDialog .exec() == QDialog::Accepted)
    {
        m_mainTextEdit->print(&m_printer);
    }
}

void MainWindow::onFileExit()
{
    if(!m_mainTextEdit->document()->isModified())
    {
        close();
        return;
    }

    QMessageBox::StandardButton reply = confirmDiscardChanges();

    if(reply == QMessageBox::Yes)
    {
        if(saveFile())
        {
            updateWindowTitle();
        }
    }
    else if(reply == QMessageBox::No)
    {
        close();
    }
}

void MainWindow::onEditDelete()
{
    QTextCursor cursor = m_mainTextEdit->textCursor();

    cursor.removeSelectedText();

    m_mainTextEdit->setTextCursor(cursor);
}

void MainWindow::onEditFind()
{
    if(!m_findDialog)
    {
        m_findDialog = new QDialog(this);

        m_findDialog->setWindowTitle(tr("Find"));
        m_findDialog->setFixedSize(330, 130);

        QGridLayout* findGridLayout{new QGridLayout(m_findDialog)};

        QLabel* findLabel{new QLabel(tr("Find what:"), m_findDialog)};

        m_findLineEdit = new QLineEdit(m_findDialog);

        m_findNextButton = new QPushButton(tr("Find Next"), m_findDialog);

        m_findNextButton->setEnabled(false);

        findGridLayout->addWidget(findLabel, 0, 0);
        findGridLayout->addWidget(m_findLineEdit, 0, 1);
        findGridLayout->addWidget(m_findNextButton, 0, 2);

        m_findMatchCaseCheck = new QCheckBox(tr("Match case"), m_findDialog);
        QPushButton* findCancelButton{new QPushButton(tr("Cancel"), m_findDialog)};

        findGridLayout->addWidget(m_findMatchCaseCheck, 1, 0);
        findGridLayout->addWidget(findCancelButton, 1, 2);

        QGroupBox* directionGroup{new QGroupBox(tr("Direction"), m_findDialog)};

        m_findUpRadio = new QRadioButton(tr("Up"), directionGroup);
        m_findDownRadio = new QRadioButton(tr("Down"), directionGroup);

        m_findDownRadio->setChecked(true);

        QHBoxLayout* directionLayout{new QHBoxLayout(directionGroup)};

        directionLayout->addWidget(m_findUpRadio);
        directionLayout->addWidget(m_findDownRadio);

        findGridLayout->addWidget(directionGroup, 1, 1);

        connect(m_findLineEdit, &QLineEdit::textChanged, this, [this](const QString& text){
            m_findNextButton->setEnabled(!text.trimmed().isEmpty());
        });

        connect(m_findNextButton, &QPushButton::clicked, this, [this](){
            m_findLastSearchText = m_findLineEdit->text();

            findNext();
        });

        connect(findCancelButton, &QPushButton::clicked, m_findDialog, &QDialog::hide);

        m_findDialog->setLayout(findGridLayout);
    }

    m_findDialog->show();
    m_findDialog->raise();
    m_findDialog->activateWindow();
}

void MainWindow::onEditFindNext()
{
    if(m_findLastSearchText.isEmpty())
    {
        onEditFind();
        return;
    }
    else
    {
        findNext();
    }
}

void MainWindow::onEditReplace()
{
    if(!m_replaceDialog)
    {
        m_replaceDialog = new QDialog(this);
        m_replaceDialog->setWindowTitle(tr("Replace"));
        m_replaceDialog->setFixedSize(350, 150);
        m_replaceDialog->setWindowModality(Qt::NonModal);
        m_replaceDialog->setAttribute(Qt::WA_DeleteOnClose, false);

        QGridLayout* replaceLayout{new QGridLayout(m_replaceDialog)};

        QLabel* findLabel{new QLabel(tr("Find What:"), m_replaceDialog)};

        m_replaceFindLineEdit = new QLineEdit(m_replaceDialog);
        m_replaceFindNextButton = new QPushButton(tr("Find Next"), m_replaceDialog);

        QLabel* replaceLabel{new QLabel(tr("Replace with:"), m_replaceDialog)};

        m_replaceLineEdit = new QLineEdit(m_replaceDialog);
        m_replaceButton = new QPushButton(tr("Replace"), m_replaceDialog);
        m_replaceAllButton = new QPushButton(tr("Replace All"), m_replaceDialog);
        m_replaceCancelButton = new QPushButton(tr("Cancel"), m_replaceDialog);
        m_replaceMatchCaseCheckBox = new QCheckBox(tr("Match case"), m_replaceDialog);

        m_replaceFindNextButton->setEnabled(false);
        m_replaceButton->setEnabled(false);
        m_replaceAllButton->setEnabled(false);

        replaceLayout->addWidget(findLabel, 0, 0);
        replaceLayout->addWidget(m_replaceFindLineEdit, 0, 1);
        replaceLayout->addWidget(m_replaceFindNextButton, 0, 2);
        replaceLayout->addWidget(replaceLabel, 1, 0);
        replaceLayout->addWidget(m_replaceLineEdit, 1, 1);
        replaceLayout->addWidget(m_replaceButton, 1, 2);
        replaceLayout->addWidget(m_replaceMatchCaseCheckBox, 2, 0);
        replaceLayout->addWidget(m_replaceCancelButton, 2, 1);
        replaceLayout->addWidget(m_replaceAllButton, 2, 2);

        connect(m_replaceFindLineEdit, &QLineEdit::textChanged, this, [this](){
            bool hasText = !m_replaceFindLineEdit->text().isEmpty();
            m_replaceFindNextButton->setEnabled(hasText);
            m_replaceButton->setEnabled(hasText);
            m_replaceAllButton->setEnabled(hasText);
        });
        connect(m_replaceFindNextButton, &QPushButton::clicked, this, &MainWindow::replaceFindNext);
        connect(m_replaceButton, &QPushButton::clicked, this, &MainWindow::replaceOne);
        connect(m_replaceAllButton, &QPushButton::clicked, this, &MainWindow::replaceAll);
        connect(m_replaceCancelButton, &QPushButton::clicked, m_replaceDialog, &QDialog::hide);
    }
        m_replaceDialog->show();
        m_replaceDialog->raise();
        m_replaceDialog->activateWindow();
}

void MainWindow::replaceOne()
{
    if(!m_mainTextEdit || !m_replaceFindLineEdit || !m_replaceLineEdit) {return;}

    QString replaceText{m_replaceLineEdit->text()};
    QTextCursor cursor{m_mainTextEdit->textCursor()};

    if(cursor.hasSelection())
    {
        QString selected = cursor.selectedText().replace(QChar(0x2029), "\n");
        QString lastSearch = m_replaceLastSearchText.replace(QChar(0x2029), "\n");

        Qt::CaseSensitivity cs = m_replaceMatchCaseCheckBox && m_replaceMatchCaseCheckBox->isChecked() ?
                                     Qt::CaseSensitive : Qt::CaseInsensitive;
        if(QString::compare(selected, lastSearch, cs) == 0)
        {
            cursor.insertText(replaceText);
            m_mainTextEdit->setTextCursor(cursor);

            m_mainTextEdit->setStyleSheet("QPlainTextEdit{selection-background-color:yellow; color:black;}");
            return;
        }
    }

    QMessageBox::information(this, tr("Replace"), tr("Cannot find *%1*").arg(m_replaceLastSearchText));

}

void MainWindow::replaceAll()
{
    if(!m_mainTextEdit || !m_replaceFindLineEdit || !m_replaceLineEdit || !m_replaceMatchCaseCheckBox){return;}

    const QString findText{m_replaceFindLineEdit->text()};
    const QString replaceText{m_replaceLineEdit->text()};

    if(findText.isEmpty()) {return;}

    QTextDocument::FindFlags flags{};

    if(m_replaceMatchCaseCheckBox && m_replaceMatchCaseCheckBox->isChecked())
    {
        flags |= QTextDocument::FindCaseSensitively;
    }

    QTextDocument* doc{m_mainTextEdit->document()};
    if(!doc){return;}

    QTextCursor cursor(doc);

    cursor.beginEditBlock();

    int count{0};

    while(true)
    {
        QTextCursor found{doc->find(findText, cursor, flags)};

        if(found.isNull()) break;

        found.insertText(replaceText);

        cursor.setPosition(found.position(), QTextCursor::MoveAnchor);
        count++;
    }
    cursor.endEditBlock();

    m_replaceLastSearchText = findText;

    if(count == 0)
    {
    QMessageBox::information(this, tr("Replace"), tr("Cannot find *%1*").arg(findText));
    }
}

void MainWindow::replaceFindNext()
{
    if(!m_mainTextEdit || !m_replaceFindLineEdit) {return;}

    QString searchText{m_replaceFindLineEdit->text()};

    if(searchText.isEmpty()) {return;}

    QTextDocument::FindFlags replaceFlags{};

    if(m_replaceMatchCaseCheckBox && m_replaceMatchCaseCheckBox->isChecked())
    {
        replaceFlags |= QTextDocument::FindCaseSensitively;
    }

    bool found = m_mainTextEdit->find(searchText, replaceFlags);

    if(!found)
    {
        m_mainTextEdit->moveCursor(QTextCursor::Start);

        if(!m_mainTextEdit->find(searchText, replaceFlags))
        {
            QMessageBox::information(this, tr("Replace"), tr("Cannot find *%1*").arg(searchText));
        }
    }
    else
    {
        m_mainTextEdit->setStyleSheet("QPlainTextEdit{selection-background-color: yellow; color: black}");
        m_replaceLastSearchText = searchText;
    }
}

void MainWindow::onGoTo()
{
    if(!m_mainTextEdit){return;}

    bool ok{};
    int maxLine{m_mainTextEdit->document()->blockCount()};
    int lineNumber{QInputDialog::getInt(this, tr("Go to line"),
                tr("Line number 1 - %1:").arg(maxLine), 1, 1, maxLine, 1, &ok)};

    if(ok)
    {
        QTextBlock textBlock{m_mainTextEdit->document()->
                             findBlockByLineNumber(lineNumber - 1)};
        if(textBlock.isValid())
        {
            QTextCursor cursor{textBlock};
            m_mainTextEdit->setTextCursor(cursor);
            m_mainTextEdit->centerCursor();
        }
    }
}

void MainWindow::onTimeDate()
{
    if(!m_mainTextEdit) {return;}

    QDateTime now{QDateTime::currentDateTime()};

    QString timestamp{QLocale::system().toString(now, QLocale::ShortFormat)};

    m_mainTextEdit->insertPlainText(timestamp);
}

void MainWindow::onWordWrap(bool enabled)
{
    if(!m_mainTextEdit) {return;}

    m_mainTextEdit->setLineWrapMode(enabled ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
}

void MainWindow::onFont()
{
    if(!m_mainTextEdit){return;}

    bool ok{false};

    QFont selectedFont{QFontDialog::getFont(&ok, m_mainTextEdit->font(), this,
                                            tr("Font"))};

    if(ok){m_mainTextEdit->setFont(selectedFont);}
}

void MainWindow::onStatusBar(bool checked)
{
    statusBar()->setVisible(checked);
    QSettings settings{};

    settings.setValue("ShowStatusBar", checked);
}
