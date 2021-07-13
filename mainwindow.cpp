#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QString>
#include <QDebug>
#include <QMessageBox>
#include <QColor>

#include <list>
#include <string>

#include "controller.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_title("XML editor")
{
    ui->setupUi(this);

    m_controller = new Controller(-1);
    m_text = nullptr;
    m_parsed = nullptr;
    m_textState = TextState::original;

    connect(this, undoChanged, updateUndoAction);
    connect(this, redoChanged, updateRedoAction);
    updateUndoAction();
    updateRedoAction();

    connect(ui->actionOpen, QAction::triggered, this, openXML);
    connect(ui->actionSave, QAction::triggered, this, saveText);
    connect(ui->actionMinify, QAction::triggered, this, minifyText);
    connect(ui->actionBeautify_2, QAction::triggered, this, beautifyText);
    connect(ui->actionColorize, QAction::triggered, this, colorizeText);
    connect(ui->actionConvert_to_JSON, QAction::triggered, this, Convert_to_JSONText);
    connect(ui->actionClose, QAction::triggered, this, closeXML);
    connect(ui->actionExit, QAction::triggered, this, close);
    connect(ui->actionUndo, QAction::triggered, this, undo);
    connect(ui->actionRedo, QAction::triggered, this, redo);
    connect(ui->actionSolve_Inconsistency, QAction::triggered, this, solveIncon);

    connect(this, MainWindow::stateChanged, this, MainWindow::updateState);
    setState(State::idle);
    updateState();

    ui->statusBar->showMessage("Ready!");
    setWindowTitle(m_title);


//    openXML();
}

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow::State MainWindow::state() const
{
    return m_state;
}

void MainWindow::setState(const State &state)
{
    if (state != m_state) {
        m_state = state;
        emit stateChanged();
    }
}

QString MainWindow::path() const
{
    return m_path;
}

void MainWindow::setPath(const QString &path)
{
    m_path = path;
}

std::list<std::string> *MainWindow::text() const
{
    return m_text;
}

void MainWindow::setText(std::list<std::string> *text, bool doDel)
{
    if (text != m_text && m_text != nullptr && doDel)
        delete m_text;
    m_text = text;
}

std::list<Token> *MainWindow::parsed() const
{
    return m_parsed;
}

void MainWindow::setParsed(std::list<Token> *parsed)
{
    if (m_parsed != nullptr && m_parsed != parsed)
        delete m_parsed;
    m_parsed = parsed;
}

void MainWindow::setActionsEnabled(bool enabled, bool isCon)
{
    this->ui->actionBeautify_2->setEnabled(enabled);
    this->ui->actionConvert_to_JSON->setEnabled(enabled);
    this->ui->actionMinify->setEnabled(enabled);
    this->ui->actionSave->setEnabled(enabled);
    this->ui->actionOptimize->setEnabled(enabled);
    this->ui->actionClose->setEnabled(enabled);
    this->ui->actionColorize->setEnabled(enabled);
    this->ui->actionSolve_Inconsistency->setEnabled(isCon);
}

void MainWindow::displayText(unsigned int bufferSize)
{
    std::string *str = m_controller->listToString(text(), bufferSize,
                                                  m_textState != TextState::minified);
    QString qstr = QString::fromStdString(*str);
    if ((unsigned int) qstr.size() > (bufferSize * 100u))
        qstr.resize(bufferSize * 100u);

    ui->textEdit->setTextColor(QColor(0,0,0));
    ui->textEdit->setPlainText(qstr);

    delete str;
}

void MainWindow::openingFailed()
{
    QMessageBox::information(this, "Error", "Correcting file inconsistencies failed");
    setState(State::xmlOpenedFailed);
}

QString MainWindow::getExtension(const QString &str)
{
    QString ext;
    unsigned int i = str.size()-1;
    while (str[i] != '.')
        ext = str[i--] + ext;
    return ext;
}

void MainWindow::updateState() {
    switch (m_state) {
    case State::idle:
        m_textState = TextState::original;
        setPath("");
        setText(nullptr);
        setParsed(nullptr);
        ui->textEdit->setPlainText("");
        setActionsEnabled(false, false);

        m_undo.clear();
        emit undoChanged();
        m_redo.clear();
        emit redoChanged();

        break;

    case State::xmlOpenedSuccessfully:
        setActionsEnabled(true, false);
        break;

    case State::xmlOpenedFailed:
        setActionsEnabled(false, true);
        break;
    }
}

void MainWindow::openXML()
{
    QString newPath = QFileDialog::getOpenFileName(this, tr("Open XML"), QDir::currentPath(), "XML files (*.xml)");

//    QString newPath = "E:\\Projects\\qt c++\\build-XMLEditor-Desktop_Qt_5_6_0_MinGW_32bit-Debug\\testCorr.xml";
    if (newPath != "") {
        setState(State::idle);
        setPath(newPath);
        setText(m_controller->getText(path().toStdString()));
        setParsed(m_controller->parsing(text()));

        displayText();
        qDebug() << "number of lines =" << text()->size();
        setWindowTitle(m_title + " - " + path());

        if (m_controller->checkBalance(parsed())) {
            setState(State::xmlOpenedSuccessfully);
            ui->statusBar->showMessage("File opened successfully. " + QString::number(text()->size()) + " line(s).");
        }
        else {
            setState(State::xmlOpenedFailed);
            ui->statusBar->showMessage("File is not consistent");
        }
    }
}

void MainWindow::closeXML()
{
    setState(State::idle);
    setWindowTitle(m_title);
    ui->statusBar->showMessage("File closed.");
}

void MainWindow::saveText()
{
    QString savePath = QFileDialog::getSaveFileName(this, tr("save XML"), QDir::currentPath(), "XML files (*.xml);;JSON files(*.json)");
    if (savePath.endsWith("xml")) {
        m_controller->saveText(savePath.toStdString(), text(),
                               m_textState != TextState::minified);
        ui->statusBar->showMessage("File saved.");
    } else if (savePath.endsWith("json") && m_textState != TextState::JSON) {
        list<string> *jsonText = m_controller->json(parsed());
        m_controller->saveText(savePath.toStdString(), jsonText,
                               true);
        ui->statusBar->showMessage("JSON file saved.");
        delete jsonText;
    } else if (savePath.endsWith("json")) {
        m_controller->saveText(savePath.toStdString(), text(),
                               true);
        ui->statusBar->showMessage("JSON file saved.");
    }
}

void MainWindow::minifyText()
{
    if (m_textState != TextState::minified) {
        m_undo.push(text(), m_textState);
        emit undoChanged();

        setText(m_controller->minifyText(parsed()), false);
        m_textState = TextState::minified;
        displayText();
        ui->statusBar->showMessage("File minified");

        m_redo.clear();
        emit redoChanged();
    }
}

void MainWindow::beautifyText()
{
    if (m_textState != TextState::beautified) {
        m_undo.push(text(), m_textState);
        emit undoChanged();

        setText(m_controller->beautifyXML(parsed()), false);
        m_textState = TextState::beautified;
        displayText();
        ui->statusBar->showMessage("File prettified!");

        m_redo.clear();
        emit redoChanged();
    }
}

void MainWindow::Convert_to_JSONText()
{
    if (m_textState != TextState::JSON) {
        m_undo.push(text(), m_textState);
        emit undoChanged();

        setText(m_controller->json(parsed()), false);
        m_textState = TextState::JSON;
        displayText();
        ui->statusBar->showMessage("File converted to JSON format!");

        m_redo.clear();
        emit redoChanged();
    }
}

void MainWindow::solveIncon()
{
    list<int> *corrected = nullptr;
    if (m_controller->solveIncon(parsed(), corrected)) {
        setState(State::xmlOpenedSuccessfully);
        beautifyText();
        ui->statusBar->showMessage("Correcting the file succeeded.");
    }
    else
        openingFailed();

//    int i = 0;
//    for (Token &t : *parsed()) {
//        if (!corrected->empty() && i == corrected->front()) {
//            qDebug() << QString::fromStdString(t.get_name()) << t.get_type();
//            corrected->pop_front();
//        }
//        ++i;
//    }
    delete corrected;
}

void MainWindow::undo()
{
    if (m_undo.size() > 0) {
        m_redo.push(text(), m_textState);

        setText(m_undo.top().first, false);
        m_textState = static_cast<TextState>(m_undo.top().second);

        m_undo.pop(false);
        displayText();

        emit undoChanged();
        emit redoChanged();
    }
}

void MainWindow::redo()
{
    if (m_redo.size() > 0) {
        m_undo.push(text(), m_textState);

        setText(m_redo.top().first, false);
        m_textState = static_cast<TextState>(m_redo.top().second);

        m_redo.pop(false);
        displayText();

        emit undoChanged();
        emit redoChanged();
    }
}

void MainWindow::updateUndoAction()
{
    ui->actionUndo->setEnabled(!m_undo.empty());
}

void MainWindow::updateRedoAction()
{
    ui->actionRedo->setEnabled(!m_redo.empty());
}

void MainWindow::colorizeText(unsigned int bufferSize) {
    bufferSize = 64u;
    std::string *str = m_controller->listToString(text(), bufferSize,
                                                  m_textState != TextState::minified);
    QString qstr = QString::fromStdString(*str);
//    if ((unsigned int) qstr.size() > (bufferSize * 10u))
//        qstr.resize(bufferSize * 10u);

    ui->textEdit->setPlainText("");

    int index =0;

    /*Colorize JSON*/
    if(m_textState == TextState::JSON)
    {
        int qoutes =0;
        while(qstr[index] != '\0')
        {
            if(qstr[index] == '"')
            {
                qoutes++;
                switch(qoutes)
                {
                    case 1:
                        ui->textEdit->setTextColor(QColor(20,80,250));
                    break;

                    case 2:
                        ui->textEdit->insertPlainText(QString(qstr[index]));
                        ui->textEdit->moveCursor(QTextCursor::End);
                        index++;
                        ui->textEdit->setTextColor(QColor(0,0,0));
                    break;

                    case 3:
                        ui->textEdit->setTextColor(QColor(0,120,0));
                    break;

                    case 4:
                        ui->textEdit->insertPlainText(QString(qstr[index]));
                        ui->textEdit->moveCursor(QTextCursor::End);
                        index++;
                        qoutes = 0;
                        ui->textEdit->setTextColor(QColor(0,0,0));
                    break;
                }
            }
            else if(qstr[index] == '{' && qoutes == 2)
                qoutes = 0;

            ui->textEdit->insertPlainText(QString(qstr[index]));
            ui->textEdit->moveCursor(QTextCursor::End);
            index++;
        }
    }

    /*Colorize XML*/
    else
    {
        while(qstr[index] != '\0')
        {
            ui->textEdit->setTextColor(QColor(0,0,0));
            if(qstr[index] == '<')
            {
                if(qstr[index+1] == '!' && qstr[index+2] == '-' && qstr[index+3] == '-')
                {
                    ui->textEdit->setTextColor(QColor(0,180,0));
                    while(qstr[index] != '>')
                    {
                        ui->textEdit->insertPlainText(QString(qstr[index]));
                        ui->textEdit->moveCursor(QTextCursor::End);
                        index++;
                    }
                    ui->textEdit->insertPlainText(QString(qstr[index]));
                    ui->textEdit->moveCursor(QTextCursor::End);
                    index++;
                }
                else if(qstr[index+1] == '!' && qstr[index+2] == 'D' && qstr[index+3] == 'O' && qstr[index+4] == 'C')
                {
                    while(qstr[index] != '>')
                    {
                        ui->textEdit->insertPlainText(QString(qstr[index]));
                        ui->textEdit->moveCursor(QTextCursor::End);
                        index++;
                    }
                    ui->textEdit->insertPlainText(QString(qstr[index]));
                    ui->textEdit->moveCursor(QTextCursor::End);
                    index++;
                }
                else
                {
                    ui->textEdit->setTextColor(QColor(20,80,250));
                    ui->textEdit->insertPlainText(QString(qstr[index]));
                    ui->textEdit->moveCursor(QTextCursor::End);
                    index++;
                    ui->textEdit->insertPlainText(QString(qstr[index]));
                    ui->textEdit->moveCursor(QTextCursor::End);
                    index++;
                    while(qstr[index] != ' ' && qstr[index] != '\t' && qstr[index] != '>' && qstr[index] != '?' && qstr[index] != '/')
                    {
                        ui->textEdit->insertPlainText(QString(qstr[index]));
                        ui->textEdit->moveCursor(QTextCursor::End);
                        index++;
                    }
                    if(qstr[index] == '/' || qstr[index] == '?')
                    {
                        ui->textEdit->setTextColor(QColor(20,80,250));
                        ui->textEdit->insertPlainText(QString(qstr[index]));
                        ui->textEdit->moveCursor(QTextCursor::End);
                        index++;
                    }
                    if(qstr[index] == '>')
                    {
                        ui->textEdit->setTextColor(QColor(20,80,250));
                        ui->textEdit->insertPlainText(QString(qstr[index]));
                        ui->textEdit->moveCursor(QTextCursor::End);
                        index++;
                    }
                    else
                    {
                        while(1)
                        {
                            while(qstr[index] == ' ' || qstr[index] == '\t')
                            {
                                ui->textEdit->insertPlainText(QString(qstr[index]));
                                ui->textEdit->moveCursor(QTextCursor::End);
                                index++;
                            }
                            if(qstr[index] == '?' || qstr[index] == '/')
                            {
                                ui->textEdit->setTextColor(QColor(20,80,250));
                                ui->textEdit->insertPlainText(QString(qstr[index]));
                                ui->textEdit->moveCursor(QTextCursor::End);
                                index++;
                            }
                            if(qstr[index] == '>')
                            {
                                ui->textEdit->setTextColor(QColor(20,80,250));
                                ui->textEdit->insertPlainText(QString(qstr[index]));
                                ui->textEdit->moveCursor(QTextCursor::End);
                                index++;
                                break;
                            }
                            ui->textEdit->setTextColor(QColor(255,0,0));
                            while(qstr[index] != ' ' && qstr[index] != '\t' && qstr[index] != '=')
                            {
                                ui->textEdit->insertPlainText(QString(qstr[index]));
                                ui->textEdit->moveCursor(QTextCursor::End);
                                index++;
                            }
                            ui->textEdit->setTextColor(QColor(82,58,0));
                            ui->textEdit->insertPlainText(QString(qstr[index]));
                            ui->textEdit->moveCursor(QTextCursor::End);
                            index++;
                            ui->textEdit->setTextColor(QColor(100,0,200));
                            ui->textEdit->insertPlainText(QString(qstr[index]));
                            ui->textEdit->moveCursor(QTextCursor::End);
                            index++;
                            ui->textEdit->setTextColor(QColor(100,0,200));
                            while(qstr[index] != '"')
                            {
                                ui->textEdit->insertPlainText(QString(qstr[index]));
                                ui->textEdit->moveCursor(QTextCursor::End);
                                index++;
                            }
                            ui->textEdit->insertPlainText(QString(qstr[index]));
                            ui->textEdit->moveCursor(QTextCursor::End);
                            index++;
                        }
                    }
                }
            }
            else
            {
                while(qstr[index] != '<' && qstr[index] != '\n')
                {
                    ui->textEdit->insertPlainText(QString(qstr[index]));
                    ui->textEdit->moveCursor(QTextCursor::End);
                    index++;
                }
                if(qstr[index] == '\n')
                {
                    ui->textEdit->insertPlainText(QString(qstr[index]));
                    ui->textEdit->moveCursor(QTextCursor::End);
                    index++;
                }
            }
        }
    }

    ui->textEdit->moveCursor(QTextCursor::Start);
    delete str;
}
