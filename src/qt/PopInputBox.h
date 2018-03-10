#ifndef POPINPUTBOX_H_
#define POPINPUTBOX_H_

#include "./../BaseDialog/BaseDialog.h"
#include "TCTglobal.h"

class PopInputBox : public BaseDialog
{
    Q_OBJECT

public:
    PopInputBox(const QString &title,
                const QString &text,
                int inputMaxLength,
                int perLineLength = 20,
                const QString &skinDir = EmptyQString,
                QWidget       *parent = 0,
                const long    &exStyle = DIALOG_DEFAULTSTYLE | DIALOG_CANCEL);

    ~PopInputBox();

    static QString getInput(const QString &title,
                            const QString &text,
                            int inputMaxLength,
                            QWidget *parent = 0,
                            int perLineLength = 20,
                            const QString &skinDir = EmptyQString,
                            const long &exStyle = DIALOG_DEFAULTSTYLE | DIALOG_CANCEL);

private:
    void createUI(QWidget *widget);

    QString inputValue();

    void addLine(const QString &text);

private:
    QVBoxLayout *infoLayout;
    QLabel    *labelicon;
    QLineEdit *lineEdit;
};

#endif /*POPINPUTBOX_H_*/
