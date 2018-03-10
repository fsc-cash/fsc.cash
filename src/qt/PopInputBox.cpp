#include "PopInputBox.h"

enum {
    IconSize = 50,
    TextLineHeight = 24
};

PopInputBox::PopInputBox(const QString &title, const QString &text, int inputMaxLength, int perLineLength, const QString &skinDir, QWidget *parent, const long &exStyle)
    : BaseDialog(skinDir, parent, exStyle)
{
    setWindowTitle(tr("信息提示"));
    setObjectName("PopInputBox");
    setWindowBG(QPixmap(":/resource/popFrameBg.jpg"));
    setIcon(QPixmap(":/resource/tctIcon.ico"));
    setTitle(title);
    createUI(getContentWidget());
    labelicon->setPixmap(QPixmap(":/resource/img2.png"));
    lineEdit->setMaxLength(inputMaxLength);
    setMinimumSize(329, 156);
    QString info(text), tmpstr;

    while (info.length() >= perLineLength) {
        tmpstr = info.left(perLineLength);
        info = info.remove(0, perLineLength);
        addLine(tmpstr);
    }
    addLine(info);

    loadQssConfig(this, objectName());
}

PopInputBox::~PopInputBox()
{

}

void PopInputBox::createUI(QWidget *widget)
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
    labelicon->setObjectName(QString::fromUtf8("labelicon"));
    labelicon->setFixedSize(IconSize, IconSize);

    horizontalLayout->addWidget(labelicon);

    infoLayout = new QVBoxLayout();
    infoLayout->setObjectName(QString::fromUtf8("infoLayout"));
    infoLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
//     label = new QLabel(widget);
//     label->setObjectName(QString::fromUtf8("label"));
//     label->setMinimumSize(QSize(0, 24));
//     label->setMaximumSize(QSize(16777215, 24));
// 
//     infoLayout->addWidget(label);

    lineEdit = new QLineEdit(widget);
    lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
    lineEdit->setFixedHeight(TextLineHeight);
    infoLayout->addWidget(lineEdit);

    horizontalLayout->addLayout(infoLayout);
    horizontalLayout_2->addLayout(horizontalLayout);
}

void PopInputBox::addLine(const QString &text)
{
    QLabel *label = new QLabel(getContentWidget());
    label->setText(text);
    label->setObjectName(QString::fromUtf8("label"));
    label->setFixedHeight(TextLineHeight);
    infoLayout->insertWidget(infoLayout->count() - 1, label);
}

QString PopInputBox::inputValue()
{
    return lineEdit->text();
}

QString PopInputBox::getInput(const QString &title, const QString &text, int inputMaxLength, QWidget *parent /* = 0 */, int perLineLength /* = 20 */, const QString &skinDir /* = EmptyQString */, const long &exStyle /* = DIALOG_DEFAULTSTYLE */)
{
    PopInputBox dlg(title, text, inputMaxLength, perLineLength, skinDir, parent, exStyle);
    if (DIALOG_YES == dlg.exec())
        return dlg.inputValue();
    else
        return EmptyQString;
}
