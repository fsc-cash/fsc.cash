#include "PopMessageBox.h"

#include <QToolButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

enum {
    IconSize = 50,
    TextLineHeight = 15
};

PopMessageBox::PopMessageBox(int iconType, const QString &title, const QString &text, int perLineLength, const QString &skinDir, QWidget *parent)
    : QDialog(parent), CSkinSet()
{
    setObjectName("PopMessageBox");

    setFixedWidth(329);
    setMinimumHeight(156);
    createUI(this);

    QString info(text), tmpstr;

    while (info.length() >= perLineLength) {
        tmpstr = info.left(perLineLength);
        info = info.remove(0, perLineLength);
        addLine(tmpstr);
    }
    addLine(info);

    switch (iconType) {
    case Icon_Alert:
        labelicon->setPixmap(QPixmap(":/resource/img2.png"));
        break;
    case Icon_Warn:
        labelicon->setPixmap(QPixmap(":/resource/img1.png"));
        break;
    case Icon_Ques:
        labelicon->setPixmap(QPixmap(":/resource/img3.png"));
        break;
    default:
        break;
    }

    loadQssConfig(this);
}

PopMessageBox::~PopMessageBox()
{
}

void PopMessageBox::createUI(QWidget *widget)
{
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout;
    horizontalLayout_2 = new QHBoxLayout(widget);
    horizontalLayout_2->setSpacing(0);
    horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
    horizontalLayout_2->setContentsMargins(25, 10, 16, 10);
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(15);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    labelicon = new QLabel(widget);
    labelicon->setScaledContents(true);
    labelicon->setObjectName(QString::fromUtf8("labelicon"));
    labelicon->setMinimumSize(QSize(IconSize, IconSize));
    labelicon->setMaximumSize(QSize(IconSize, IconSize));

    horizontalLayout->addWidget(labelicon);

    infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(0);
    infoLayout->setObjectName(QString::fromUtf8("infoLayout"));

    horizontalLayout->addLayout(infoLayout);


    horizontalLayout_2->addLayout(horizontalLayout);
}

void PopMessageBox::addLine(const QString &text)
{
    QLabel *label = new QLabel(this);
    label->setText(text);
    label->setObjectName(QString::fromUtf8("label"));
    label->setFixedHeight(TextLineHeight);
    infoLayout->addWidget(label);
}

int PopMessageBox::information(int iconType, const QString &title, const QString &text, QWidget *parent, int perLineLength, const QString &skinDir)
{
    PopMessageBox dlg(iconType, title, text, perLineLength, skinDir, parent);
    return dlg.exec();
}
