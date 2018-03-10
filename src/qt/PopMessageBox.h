#ifndef POPMESSAGEBOX_H_
#define POPMESSAGEBOX_H_

#include <QDialog>
#include <CSkinSet.h>

class QVBoxLayout;
class QLabel;

enum {
    Icon_Warn,
    Icon_Alert,
    Icon_Ques
};

class PopMessageBox : public QDialog, public CSkinSet
{
    Q_OBJECT

public:
    PopMessageBox(int iconType,
                  const QString &title,
                  const QString &text,
                  int perLineLength = 20,
                  const QString &skinDir = "",
                  QWidget       *parent = 0);
    ~PopMessageBox();

    static int information(int iconType,
                           const QString &title,
                           const QString &text,
                           QWidget *parent = 0,
                           int perLineLength = 20,
                           const QString &skinDir = "");

private:
    void createUI(QWidget *widget);

    void addLine(const QString &text);

private:
    QVBoxLayout *infoLayout;

    QLabel *labelicon;

};

#endif /*POPMESSAGEBOX_H_*/
