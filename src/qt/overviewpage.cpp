#include "overviewpage.h"
#include "ui_overviewpage.h"

#include "walletmodel.h"
#include "bitcoinunits.h"
#include "optionsmodel.h"
#include "transactiontablemodel.h"
#include "transactionfilterproxy.h"
#include "guiutil.h"
#include "guiconstants.h"
#include "askpassphrasedialog.h"
#include "PopMessageBox.h"

#include <QAbstractItemDelegate>
#include <QPainter>
#include <QTextStream>
#include <QFile>
#include <QDesktopServices>
#include <QMessageBox>
//#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QByteArray>
#include <QTextCodec>

#define DECORATION_SIZE 30
#define NUM_ITEMS 6

const QString btnstyle = "QPushButton{background:url(:res/skin/%1.png);background-repeat:no-repeat;"
        "border:none;background-color: rgb(88,96,98);border-radius:5px;}"
        "QPushButton::hover{background-color: rgb(121,121,121);}";

class TxViewDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    TxViewDelegate(): QAbstractItemDelegate(), unit(BitcoinUnits::BTC)
    {

    }

    inline void paint(QPainter *painter, const QStyleOptionViewItem &option,
                      const QModelIndex &index ) const
    {
        painter->save();

        QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
        QRect mainRect = option.rect;
        QRect decorationRect(mainRect.topLeft(), QSize(DECORATION_SIZE, DECORATION_SIZE));
        int xspace = DECORATION_SIZE + 8;
        int ypad = 6;
        int halfheight = (mainRect.height() - 2*ypad)/2;
        QRect amountRect(mainRect.left() + xspace, mainRect.top()+ypad, mainRect.width() - xspace - 10, mainRect.height());
        QRect addressRect(mainRect.left() + xspace, mainRect.top()+ypad+halfheight, mainRect.width() - xspace, halfheight);
        icon.paint(painter, decorationRect);

        QDateTime date = index.data(TransactionTableModel::DateRole).toDateTime();
        QString address = index.data(Qt::DisplayRole).toString();
        qint64 amount = index.data(TransactionTableModel::AmountRole).toLongLong();
        bool confirmed = index.data(TransactionTableModel::ConfirmedRole).toBool();
        QVariant value = index.data(Qt::ForegroundRole);
        QColor foreground = option.palette.color(QPalette::Text);
        if(qVariantCanConvert<QColor>(value))
        {
            foreground = qvariant_cast<QColor>(value);
        }

        painter->setPen(foreground);
        //painter->drawText(addressRect, Qt::AlignLeft|Qt::AlignVCenter, address);
        painter->drawText(amountRect, Qt::AlignHCenter |Qt::AlignVCenter, address);
        if(amount < 0)
        {
            foreground = COLOR_NEGATIVE;
        }
        else if(!confirmed)
        {
            foreground = COLOR_UNCONFIRMED;
        }
        else
        {
            foreground = option.palette.color(QPalette::Text);
        }
        painter->setPen(foreground);
        QString amountText = BitcoinUnits::formatWithUnit(unit, amount, true);
        if(!confirmed)
        {
            amountText = QString("[") + amountText + QString("]");
        }
        painter->drawText(amountRect, Qt::AlignRight|Qt::AlignVCenter, amountText);

        painter->setPen(option.palette.color(QPalette::Text));
        painter->drawText(amountRect, Qt::AlignLeft|Qt::AlignVCenter, GUIUtil::dateTimeStr(date));

        painter->restore();
    }

    inline QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        return QSize(DECORATION_SIZE, DECORATION_SIZE);
    }

    int unit;

};
#include "overviewpage.moc"

OverviewPage::OverviewPage(QWidget *parent) :
    QWidget(parent),CSkinSet(),
    ui(new Ui::OverviewPage),
    currentBalance(-1),
    currentStake(0),
    currentUnconfirmedBalance(-1),
    currentImmatureBalance(-1),
    txdelegate(new TxViewDelegate()),
    filter(0)
{
    ui->setupUi(this);
    // Recent transactions
    ui->listTransactions->setItemDelegate(txdelegate);
    ui->listTransactions->setIconSize(QSize(DECORATION_SIZE, DECORATION_SIZE));
    //ui->listTransactions->setMinimumHeight(NUM_ITEMS * (DECORATION_SIZE + 2));
    ui->listTransactions->setAttribute(Qt::WA_MacShowFocusRect, false);

    connect(ui->listTransactions, SIGNAL(clicked(QModelIndex)), this, SLOT(handleTransactionClicked(QModelIndex)));
    connect(ui->btn_setpwd, SIGNAL(clicked()), this, SLOT(onBtnSetpwd()));
    connect(ui->btn_opendir, SIGNAL(clicked()), this, SLOT(onBtnOpenDir()));

    // init "out of sync" warning labels
    //ui->labelWalletStatus->setText("(" + tr("out of sync") + ")");
    //ui->labelTransactionsStatus->setText("(" + tr("out of sync") + ")");

    // start with displaying the "out of sync" warnings

    showOutOfSyncWarning(true);

    m_nm = new QNetworkAccessManager(this);
    QObject::connect(m_nm, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(finishedSlot(QNetworkReply*)));

    QNetworkRequest request;
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    //request.setHeader(QNetworkRequest::UserAgentHeader,"Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/33.0.1750.152 Safari/537.36");
    //request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QString getstr = QString("http://122.114.243.188:8099/news.html");
    request.setUrl(QUrl(getstr));
    QNetworkReply* reply = m_nm->get(request);

    ui->tb_webinfo->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tb_webinfo->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tb_webinfo->setOpenLinks(false);
    connect(ui->tb_webinfo, SIGNAL(anchorClicked(const QUrl&)),this, SLOT(anchorClickedSlot(const QUrl&)));

}

void OverviewPage::handleTransactionClicked(const QModelIndex &index)
{
    if(filter)
        emit transactionClicked(filter->mapToSource(index));
}

void OverviewPage::onBtnSetpwd()
{
    switch (model->getEncryptionStatus())
    {
    case WalletModel::Unencrypted:
    {
        AskPassphraseDialog dlg(AskPassphraseDialog::Encrypt, this);
        dlg.loadQssConfig(&dlg);
        dlg.setModel(model);
        dlg.exec();
    }
        break;
    case WalletModel::Unlocked:
    case WalletModel::Locked:
    {
        AskPassphraseDialog dlg(AskPassphraseDialog::ChangePass, this);
        dlg.loadQssConfig(&dlg);
        dlg.setModel(model);
        dlg.exec();
    }
        break;
    }
}

void OverviewPage::onBtnOpenDir()
{
    QString path = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
    path += "/AppData/Roaming/FoodSafetyCoin";
    QDesktopServices::openUrl(QUrl("file:///" + path, QUrl::TolerantMode));
}

void OverviewPage::finishedSlot(QNetworkReply *reply)
{
    //QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    QVariant redirectionTargetUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    //QString jmpurl = redirectionTargetUrl.toString();

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = reply->readAll();
        QString string = QString::fromUtf8(bytes);
        ui->tb_webinfo->clear();
        ui->tb_webinfo->insertHtml(string);
    }
    reply->deleteLater();
}

void OverviewPage::anchorClickedSlot(const QUrl &url)
{
    QDesktopServices::openUrl(url);
}

OverviewPage::~OverviewPage()
{
    delete ui;
}

void OverviewPage::setBalance(qint64 balance, qint64 stake, qint64 unconfirmedBalance, qint64 immatureBalance)
{
    int unit = model->getOptionsModel()->getDisplayUnit();
    currentBalance = balance;
    currentStake = stake;
    currentUnconfirmedBalance = unconfirmedBalance;
    currentImmatureBalance = immatureBalance;
    //    ui->labelBalance->setText(BitcoinUnits::formatWithUnit(unit, balance));
    //    ui->labelStake->setText(BitcoinUnits::formatWithUnit(unit, stake));
    //    ui->labelUnconfirmed->setText(BitcoinUnits::formatWithUnit(unit, unconfirmedBalance));
    //    ui->labelImmature->setText(BitcoinUnits::formatWithUnit(unit, immatureBalance));

    // only show immature (newly mined) balance if it's non-zero, so as not to complicate things
    // for the non-mining users
    //bool showImmature = immatureBalance != 0;
    //ui->labelImmature->setVisible(showImmature);
    //ui->labelImmatureText->setVisible(showImmature);
}

void OverviewPage::setNumTransactions(int count)
{
    ui->labelNumTransactions->setText(QLocale::system().toString(count));
}

void OverviewPage::unlockWallet()
{
    if(model->getEncryptionStatus() == WalletModel::Locked)
    {
        AskPassphraseDialog dlg(AskPassphraseDialog::Unlock, this);
        dlg.loadQssConfig(&dlg);
        dlg.setModel(model);
        if(dlg.exec() == QDialog::Accepted)
        {
            ui->unlockWalletButton->setStyleSheet(btnstyle.arg("lock"));
            //ui->unlockWalletButton->setText(QString("Lock Wallet"));
        }
    }
    else
    {
        model->setWalletLocked(true);
        ui->unlockWalletButton->setStyleSheet(btnstyle.arg("unlock"));
        //ui->unlockWalletButton->setText(QString("Unlock Wallet"));
    }
}

void OverviewPage::setModel(WalletModel *model)
{
    this->model = model;
    if(model && model->getOptionsModel())
    {
        // Set up transaction list
        filter = new TransactionFilterProxy();
        filter->setSourceModel(model->getTransactionTableModel());
        filter->setLimit(NUM_ITEMS);
        filter->setDynamicSortFilter(true);
        filter->setSortRole(Qt::EditRole);
        filter->sort(TransactionTableModel::Status, Qt::DescendingOrder);

        ui->listTransactions->setModel(filter);
        ui->listTransactions->setModelColumn(TransactionTableModel::ToAddress);

        // Keep up to date with wallet
        //setBalance(model->getBalance(), model->getStake(), model->getUnconfirmedBalance(), model->getImmatureBalance());
        //connect(model, SIGNAL(balanceChanged(qint64, qint64, qint64, qint64)), this, SLOT(setBalance(qint64, qint64, qint64, qint64)));

        setNumTransactions(model->getNumTransactions());
        connect(model, SIGNAL(numTransactionsChanged(int)), this, SLOT(setNumTransactions(int)));

        connect(model->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));

        // Unlock wallet button
        WalletModel::EncryptionStatus status = model->getEncryptionStatus();
        switch (status)
        {
        case WalletModel::Unencrypted:
            ui->unlockWalletButton->setDisabled(true);
            break;
        case WalletModel::Locked:
            ui->unlockWalletButton->setStyleSheet(btnstyle.arg("unlock"));
            break;
        case WalletModel::Unlocked:
            ui->unlockWalletButton->setStyleSheet(btnstyle.arg("lock"));
            break;
        default:
            break;
        }
        connect(ui->unlockWalletButton, SIGNAL(clicked()), this, SLOT(unlockWallet()));
    }

    // update the display unit, to not use the default ("ECC")
    updateDisplayUnit();
}

void OverviewPage::updateDisplayUnit()
{
    if(model && model->getOptionsModel())
    {
        if(currentBalance != -1)
            setBalance(currentBalance, model->getStake(), currentUnconfirmedBalance, currentImmatureBalance);

        // Update txdelegate->unit with the current unit
        txdelegate->unit = model->getOptionsModel()->getDisplayUnit();

        ui->listTransactions->update();
    }
}

void OverviewPage::showOutOfSyncWarning(bool fShow)
{
    //ui->labelWalletStatus->setVisible(fShow);
    ui->labelTransactionsStatus->setVisible(fShow);
}
