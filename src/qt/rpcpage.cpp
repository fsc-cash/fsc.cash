#include "rpcpage.h"

RPCPage::RPCPage(QWidget *parent) :
    QWidget(parent), CSkinSet()
{
    this->setObjectName(QString("RPCPage"));
    this->CreateUI(this);
    this->loadQssConfig(this);
}

void RPCPage::CreateUI(QWidget *w)
{

}
