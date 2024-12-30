#include "givemoneydialog.h"
#include "ui_givemoneydialog.h"
#include "scancodedialog.h"

#include <QDebug>
#include <QRegExpValidator>
#include <QMessageBox>

GiveMoneyDialog::GiveMoneyDialog(QWidget *parent) : QDialog(parent),
                                                    ui(new Ui::GiveMoneyDialog),
                                                    currentAmount(0.0),
                                                    totalBalance(0.0)
{
    ui->setupUi(this);
    initUI();

    // 更新余额显示
    ui->balanceLabel->setText(QString("当前余额：¥ %1").arg(totalBalance, 0, 'f', 2));

    // 连接信号槽
    connect(ui->customAmountEdit, &QLineEdit::textChanged, this, &GiveMoneyDialog::onCustomAmountChanged);
    connect(ui->wechatPayBtn, &QPushButton::clicked, this, &GiveMoneyDialog::onWechatPayClicked);
    connect(ui->alipayBtn, &QPushButton::clicked, this, &GiveMoneyDialog::onAlipayClicked);
}

GiveMoneyDialog::~GiveMoneyDialog()
{
    delete ui;
}

void GiveMoneyDialog::initUI()
{
    // 设置金额按钮组
    amountGroup = new QButtonGroup(this);
    amountGroup->addButton(ui->amount_10, 10);
    amountGroup->addButton(ui->amount_20, 20);
    amountGroup->addButton(ui->amount_50, 50);
    amountGroup->addButton(ui->amount_100, 100);
    amountGroup->addButton(ui->amount_200, 200);
    amountGroup->addButton(ui->amount_500, 500);

    connect(amountGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),
            this, &GiveMoneyDialog::onAmountButtonClicked);

    // 设置自定义金额输入框验证器
    QRegExp rx("^\\d{1,5}(\\.\\d{0,2})?$");
    ui->customAmountEdit->setValidator(new QRegExpValidator(rx, this));

    // 初始化支付按钮状态
    updatePaymentButtons(false);
}

void GiveMoneyDialog::onAmountButtonClicked(int id)
{
    ui->customAmountEdit->clear();
    currentAmount = id;
    updatePaymentButtons(true);
}

void GiveMoneyDialog::onCustomAmountChanged(const QString &text)
{
    // 取消金额按钮的选中状态
    if (amountGroup->checkedButton())
    {
        amountGroup->setExclusive(false);
        amountGroup->checkedButton()->setChecked(false);
        amountGroup->setExclusive(true);
    }

    bool ok;
    double amount = text.toDouble(&ok);
    currentAmount = ok ? amount : 0.0;
    updatePaymentButtons(ok && amount > 0);
}

void GiveMoneyDialog::updatePaymentButtons(bool enabled)
{
    ui->wechatPayBtn->setEnabled(enabled);
    ui->alipayBtn->setEnabled(enabled);

    QString amountText = QString("¥ %1").arg(currentAmount, 0, 'f', 2);
    if (enabled)
    {
        ui->wechatPayBtn->setText(QString("微信支付 (%1)").arg(amountText));
        ui->alipayBtn->setText(QString("支付宝 (%1)").arg(amountText));
    }
    else
    {
        ui->wechatPayBtn->setText("微信支付");
        ui->alipayBtn->setText("支付宝");
    }
}

void GiveMoneyDialog::onWechatPayClicked()
{
    processPayment("wechat");
}

void GiveMoneyDialog::onAlipayClicked()
{
    processPayment("alipay");
}

void GiveMoneyDialog::processPayment(const QString &method)
{
    QString message = QString("确认使用%1支付 ¥%2 ?")
                          .arg(method == "wechat" ? "微信" : "支付宝")
                          .arg(currentAmount, 0, 'f', 2);

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "确认支付",
        message,
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        // 更新总余额
        totalBalance += currentAmount;
        ui->balanceLabel->setText(QString("当前余额：¥ %1").arg(totalBalance, 0, 'f', 2));

        QMessageBox::information(this, "支付成功", 
            QString("充值 ¥%1 成功！").arg(currentAmount, 0, 'f', 2));
        
        if (amountGroup->checkedButton()) 
        {
            amountGroup->setExclusive(false);
            amountGroup->checkedButton()->setChecked(false);
            amountGroup->setExclusive(true);
        }
        ui->customAmountEdit->clear();
        currentAmount = 0.0;
        updatePaymentButtons(false);
    }
}

double GiveMoneyDialog::getCurrentAmount() const
{
    return currentAmount;
}
