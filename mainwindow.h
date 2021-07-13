#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <string>
#include <list>
#include <stack>

#include "controller.h"
#include "stack.h"
#include "Tokens.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum class State{idle, xmlOpenedSuccessfully, xmlOpenedFailed};
    enum TextState{original, minified, beautified, JSON};
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    State state() const;
    void setState(const State &state);

    QString path() const;
    void setPath(const QString &path);

    std::list<std::string> *text() const;
    void setText(std::list<std::string> *text, bool doDel = true);

    std::list<Token> *parsed() const;
    void setParsed(std::list<Token> *parsed);

private:
    Ui::MainWindow *ui;
    State m_state;
    QString m_path;

    const QString m_title;

    std::list<std::string> *m_text;
    std::list<Token> *m_parsed;
    TextState m_textState;

    Stack<std::list<std::string>> m_undo, m_redo;
    Controller *m_controller;

    void setActionsEnabled(bool enabled, bool isCon);
    void displayText(unsigned int bufferSize = 128);
    void openingFailed();
    QString getExtension(const QString &str);

signals:
    void stateChanged();
    void undoChanged();
    void redoChanged();

private slots:
    void updateState();
    void openXML();
    void closeXML();
    void saveText();
    void minifyText();
    void beautifyText();
    void Convert_to_JSONText();
    void colorizeText(unsigned int bufferSize = 64);

    void solveIncon();

    void undo();
    void redo();

    void updateUndoAction();
    void updateRedoAction();
};

#endif // MAINWINDOW_H
