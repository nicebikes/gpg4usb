/*
 *      gpgwin.cpp
 *
 *      Copyright 2008 gpg4usb-team <gpg4usb@cpunk.de>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */
#include <QApplication>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <iostream>
#include <QtGui>
#include <QString>
#include <QFileDialog>
#include <QStringList>
#include <QIcon>
#include <QMessageBox>
#include <QVBoxLayout>

#include "context.h"
#include "gpgwin.h"


GpgWin::GpgWin()
{
    myCtx = new GpgME::Context();

    /* get path were app was started */
    QString appPath = qApp->applicationDirPath();
    iconPath = appPath + "/icons/";

    edit = new QPlainTextEdit();
    setCentralWidget(edit);

    /* the list of Keys available*/
    m_keyList = new KeyList();
    m_keyList->setIconPath(iconPath);
    m_keyList->setContext(myCtx);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createDockWindows();

    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    setIconSize(QSize(32, 32));
    setCurrentFile("");

}

void GpgWin::createActions()
{
    /** Main Menu
      */
    openAct = new QAction(tr("&Open..."), this);
    openAct->setIcon(QIcon(iconPath + "fileopen.png"));
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setIcon(QIcon(iconPath + "filesave.png"));
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save the current File"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As"), this);
    saveAsAct->setIcon(QIcon(iconPath + "filesaveas.png"));
    saveAsAct->setStatusTip(tr("Save the current File as..."));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));
    
    encryptFileAct = new QAction(tr("&Encrypt File"), this);
    //encryptFileAct->setIcon(QIcon(iconPath + "filesave.png"));
    //encryptFileAct->setShortcut(tr("Ctrl+S"));
    //encryptFileAct->setStatusTip(tr("Save the current File"));
    connect(encryptFileAct, SIGNAL(triggered()), this, SLOT(encryptFile()));

    decryptFileAct = new QAction(tr("&Decrypt File"), this);
    //decryptFileAct->setIcon(QIcon(iconPath + "filesaveas.png"));
    //decryptFileAct->setStatusTip(tr("Save the current File as..."));
    connect(decryptFileAct, SIGNAL(triggered()), this, SLOT(decryptFile()));   

    printAct = new QAction(tr("&Print"), this);
    printAct->setIcon(QIcon(iconPath + "fileprint.png"));
    printAct->setShortcut(tr("Ctrl+P"));
    printAct->setStatusTip(tr("Print Document"));
    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

    quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcut(tr("Ctrl+Q"));
    quitAct->setIcon(QIcon(iconPath + "exit.png"));
    quitAct->setStatusTip(tr("Quit Program"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    /** Edit Menu
     */
    pasteAct = new QAction(tr("&Paste"), this);
    pasteAct->setIcon(QIcon(iconPath + "button_paste.png"));
    pasteAct->setShortcut(tr("Ctrl+V"));
    pasteAct->setStatusTip(tr("Paste Text From Clipboard"));
    connect(pasteAct, SIGNAL(triggered()), edit, SLOT(paste()));

    cutAct = new QAction(tr("Cu&t"), this);
    cutAct->setIcon(QIcon(iconPath + "button_cut.png"));
    cutAct->setShortcut(tr("Ctrl+X"));
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), edit, SLOT(cut()));

    copyAct = new QAction(tr("&Copy"), this);
    copyAct->setIcon(QIcon(iconPath + "button_copy.png"));
    copyAct->setShortcut(tr("Ctrl+C"));
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), edit, SLOT(copy()));

    selectallAct = new QAction(tr("Select &All"), this);
    selectallAct->setIcon(QIcon(iconPath + "edit.png"));
    selectallAct->setShortcut(tr("Ctrl+A"));
    selectallAct->setStatusTip(tr("Select the whole text"));
    connect(selectallAct, SIGNAL(triggered()), edit, SLOT(selectAll()));

    /** Crypt Menu
     */
    encryptAct = new QAction(tr("&Encrypt"), this);
    encryptAct->setIcon(QIcon(iconPath + "encrypted.png"));
    encryptAct->setShortcut(tr("Ctrl+E"));
    encryptAct->setStatusTip(tr("Encrypt Message"));
    connect(encryptAct, SIGNAL(triggered()), this, SLOT(encrypt()));

    decryptAct = new QAction(tr("&Decrypt"), this);
    decryptAct->setIcon(QIcon(iconPath + "decrypted.png"));
    decryptAct->setShortcut(tr("Ctrl+D"));
    decryptAct->setStatusTip(tr("Decrypt Message"));
    connect(decryptAct, SIGNAL(triggered()), this, SLOT(decrypt()));
    
    importKeyFromFileAct = new QAction(tr("&File"), this);
    importKeyFromFileAct->setIcon(QIcon(iconPath + "kgpg_import.png"));
    importKeyFromFileAct->setStatusTip(tr("Import New Key From File"));
    connect(importKeyFromFileAct, SIGNAL(triggered()), this, SLOT(importKeyFromFile()));

    importKeyFromEditAct = new QAction(tr("&Editor"), this);
    importKeyFromEditAct->setIcon(QIcon(iconPath + "importkey_editor.png"));
    importKeyFromEditAct->setStatusTip(tr("Import New Key From Editor"));
    connect(importKeyFromEditAct, SIGNAL(triggered()), this, SLOT(importKeyFromEdit()));

    importKeyFromClipboardAct = new QAction(tr("&Clipboard"), this);
    importKeyFromClipboardAct->setIcon(QIcon(iconPath + "importkey_editor.png"));
    importKeyFromClipboardAct->setStatusTip(tr("Import New Key From Clipboard"));
    connect(importKeyFromClipboardAct, SIGNAL(triggered()), this, SLOT(importKeyFromClipboard()));

    importKeyDialogAct = new QAction(tr("Import Key"), this);
    importKeyDialogAct->setIcon(QIcon(iconPath + "importkey_editor.png"));
    importKeyDialogAct->setStatusTip(tr("Import New Key"));
    connect(importKeyDialogAct, SIGNAL(triggered()), this, SLOT(importKeyDialog()));
    /** About Menu
     */
    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setIcon(QIcon(iconPath + "help.png"));
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void GpgWin::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(decryptFileAct);
    fileMenu->addAction(encryptFileAct);    
    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(copyAct);
    editMenu->addAction(cutAct);
    editMenu->addAction(pasteAct);
    editMenu->addAction(selectallAct);

    cryptMenu = menuBar()->addMenu(tr("&Crypt"));
    cryptMenu->addAction(encryptAct);
    cryptMenu->addAction(decryptAct);
    cryptMenu->addSeparator();

    importKeyMenu = cryptMenu->addMenu(tr("&Import key from..."));
    importKeyMenu->addAction(importKeyFromFileAct);
    importKeyMenu->addAction(importKeyFromEditAct);
    importKeyMenu->addAction(importKeyFromClipboardAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

void GpgWin::createToolBars()
{
    cryptToolBar = addToolBar(tr("Crypt"));
    cryptToolBar->addAction(encryptAct);
    cryptToolBar->addAction(decryptAct);
    cryptToolBar->addAction(importKeyDialogAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
    editToolBar->addAction(selectallAct);
}

void GpgWin::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void GpgWin::createDockWindows()
{
    QDockWidget *dock = new QDockWidget(tr("Encrypt for:"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    dock->setWidget(m_keyList);
}

void GpgWin::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void GpgWin::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool GpgWin::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool GpgWin::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

void GpgWin::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    edit->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

void GpgWin::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    edit->document()->setModified(false);
    setWindowModified(false);

    QString shownName;
    if (curFile.isEmpty())
        shownName = tr("untitled.txt");
    else
        shownName = strippedName(curFile);

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(qApp->applicationName()));
}

QString GpgWin::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

bool GpgWin::maybeSave()
{
    if (edit->document()->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Application"),
                                   tr("The document has been modified.\nDo you want to save your changes?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

bool GpgWin::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("File"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << edit->toPlainText();
    QApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("Saved '%1'").arg(fileName), 2000);
    return true;
}

void GpgWin::print()
{
#ifndef QT_NO_PRINTER
    QTextDocument *document = edit->document();
    QPrinter printer;

    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (dlg->exec() != QDialog::Accepted)
        return;

    document->print(&printer);

    statusBar()->showMessage(tr("Ready"), 2000);
#endif
}

void GpgWin::about()
{
    QMessageBox::about(this, tr("About ") + qApp->applicationName(),
                       "<center><h2>" + qApp->applicationName() + " "
                       + qApp->applicationVersion() +"</h2></center>"
                       +tr("<center>This Application allows you to do simple<br>"
                           "encryption/decryption of your text-files.<br>"
                           "It's licensed under the GPL v2.0<br><br>"
                           "<b>Developer:</b><br>"
                           "Bene, Heimer, Juergen, Nils, Ubbo<br><br>"
                           "If you have any questions and/or<br>"
                           "suggestions, contact us at<br>"
                           "gpg4usb at cpunk.de</a><br><br>"
                           "or feel free to meet us in our channel at<br>"
                           "gpg4usb at conference.jabber.ccc.de<br><br>"
                           "and always remember:<br>"
                           "cpunk is NOT a bot...</center>"));
}

void GpgWin::encrypt()
{
    QList<QString> *uidList = m_keyList->getChecked();

    QByteArray *tmp = new QByteArray();
    if (myCtx->encrypt(uidList, edit->toPlainText().toAscii(), tmp)) {
        QString *tmp2 = new QString(*tmp);
        edit->setPlainText(*tmp2);
    }

}

void GpgWin::decrypt()
{
    QByteArray *tmp = new QByteArray();
    QByteArray text = edit->toPlainText().toAscii();
    preventNoDataErr(&text);
    myCtx->decrypt(text, tmp);
    if (!tmp->isEmpty()) {
        QString *tmp2 = new QString(*tmp);
        edit->setPlainText(*tmp2);
    }
}

/**
 * if there is no '\n' before the PGP-Begin-Block, but for example a whitespace,
 * GPGME doesn't recognise the Message as encrypted. This function adds '\n'
 * before the PGP-Begin-Block, if missing.
 */
void GpgWin::preventNoDataErr(QByteArray *in)
{
    int block_start = in->indexOf("-----BEGIN PGP MESSAGE-----");
    if(block_start > 0 && in->at(block_start - 1) != '\n') {
        in->insert(block_start, '\n');
    }
}

void GpgWin::importKeyFromEdit()
{
    myCtx->importKey(edit->toPlainText().toAscii());
    m_keyList->refresh();
}

void GpgWin::importKeyFromClipboard()
{
    QClipboard *cb = QApplication::clipboard();
    myCtx->importKey(cb->text(QClipboard::Clipboard).toAscii());
    m_keyList->refresh();
}

void GpgWin::importKeyFromFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Key"), "", tr("Key Files") + " (*.asc *.txt);;All Files (*.*)");
    if (! fileName.isNull()) {
        QFile file;
        file.setFileName(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << tr("couldn't open file: ") + fileName;
        }
        QByteArray inBuffer = file.readAll();

        myCtx->importKey(inBuffer);
        m_keyList->refresh();
    }
}

void GpgWin::importKeyDialog() {

        QDialog *dialog = new QDialog();

        dialog->setWindowTitle(tr("Import Key"));
        dialog->setModal(true);

        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        connect(buttonBox,SIGNAL(accepted()),dialog, SLOT(accept()));
        connect(buttonBox,SIGNAL(rejected()), dialog, SLOT(reject()));

        QGroupBox *groupBox = new QGroupBox(tr("Import Key From..."));
        QRadioButton *radio1 = new QRadioButton(tr("&File"));
        QRadioButton *radio2 = new QRadioButton(tr("&Editor"));
        QRadioButton *radio3 = new QRadioButton(tr("&Clipboard"));
        radio1->setChecked(true);

        QVBoxLayout *vbox1 = new QVBoxLayout();
        vbox1->addWidget(radio1);
        vbox1->addWidget(radio2);
        vbox1->addWidget(radio3);

        groupBox->setLayout(vbox1);

        QVBoxLayout *vbox2 = new QVBoxLayout();
        vbox2->addWidget(groupBox);
        vbox2->addWidget(buttonBox);

        dialog->setLayout(vbox2);

        if(dialog->exec() == QDialog::Accepted ) {
            if (radio1->isChecked()) importKeyFromFile();
            if (radio2->isChecked()) importKeyFromEdit();
            if (radio3->isChecked()) importKeyFromClipboard();
        }

}

void GpgWin::encryptFile(){

    QString infileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Files") + "All Files (*.*)");
    if (infileName.isNull()) return;
    
    QFile infile;
    infile.setFileName(infileName);
    if (!infile.open(QIODevice::ReadOnly)) {
        qDebug() << tr("couldn't open file: ") + infileName;
    }
    QByteArray inBuffer = infile.readAll();
        
    QList<QString> *uidList = m_keyList->getChecked();

    QByteArray *outBuffer = new QByteArray();
    if (!myCtx->encrypt(uidList, inBuffer, outBuffer)) return;
        
    QString outfileName = QFileDialog::getSaveFileName(this);
    if (outfileName.isEmpty())
        return;    
        
    QFile outfile(outfileName);
    if (!outfile.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("File"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(outfileName)
                             .arg(outfile.errorString()));
        return;
    }
    
    QTextStream out(&outfile);
    out << outBuffer->data();
}

void GpgWin::decryptFile(){

    QString infileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Files") + "All Files (*.*)");
    if (infileName.isNull()) return;
    
    QFile infile;
    infile.setFileName(infileName);
    if (!infile.open(QIODevice::ReadOnly)) {
        qDebug() << tr("couldn't open file: ") + infileName;
    }
    QByteArray inBuffer = infile.readAll();

    QByteArray *outBuffer = new QByteArray();
    if (!myCtx->decrypt(inBuffer, outBuffer)) return;
        
    QString outfileName = QFileDialog::getSaveFileName(this);
    if (outfileName.isEmpty())
        return;    
        
    QFile outfile(outfileName);
    if (!outfile.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("File"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(outfileName)
                             .arg(outfile.errorString()));
        return;
    }
    
    QDataStream out(&outfile);
    out.writeRawData(outBuffer->data(), outBuffer->length());
  
}
