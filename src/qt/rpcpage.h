#ifndef RPCPAGE_H
#define RPCPAGE_H

#include <QWidget>
#include <CSkinSet.h>

class RPCPage : public QWidget, public CSkinSet
{
    Q_OBJECT
public:
    explicit RPCPage(QWidget *parent = 0);

private:
    void CreateUI(QWidget *w);
};

#endif // RPCPAGE_H
