#include "merchantdialog.h"
#include "favoritedialog.h"
#include "ui_favoritedialog.h"

#include <QLabel>
#include <QDebug>
#include <QAction>
#include <QPushButton>
#include <QVBoxLayout>

FavoriteDialog::FavoriteDialog(QWidget *parent) 
    : QDialog(parent),
      ui(new Ui::FavoriteDialog)
{
    ui->setupUi(this);
    setupUI();
    loadFavorites();
}

FavoriteDialog::~FavoriteDialog()
{
    delete ui;
}

void FavoriteDialog::setupUI()
{
    setWindowTitle("我的收藏");

    // 设置固定大小
    setMinimumWidth(480);
    setMinimumHeight(680);

    // 设置列表widget的样式
    ui->listWidget->setSpacing(10);
    ui->listWidget->setViewMode(QListWidget::ListMode);
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 搜索框样式设置
    QAction *searchAction = new QAction(ui->search_edit);
    searchAction->setIcon(QIcon(":/resources/Application/btn/search.png"));
    ui->search_edit->addAction(searchAction, QLineEdit::LeadingPosition);
    ui->search_edit->setPlaceholderText("搜索收藏的商家");

    // 清除按钮
    QAction *clearAction = new QAction(ui->search_edit);
    clearAction->setIcon(QIcon(":/resources/Application/btn/close_transparent.png"));
    ui->search_edit->addAction(clearAction, QLineEdit::TrailingPosition);

    connect(ui->search_edit, &QLineEdit::textChanged, [clearAction](const QString &text)
            { clearAction->setIcon(QIcon(text.isEmpty() ? ":/resources/Application/btn/close_transparent.png" : ":/resources/Application/btn/close_search.png")); });

    connect(clearAction, &QAction::triggered, [this, clearAction]()
            {
        ui->search_edit->clear();
        clearAction->setIcon(QIcon(":/resources/Application/btn/close_transparent.png"));
        ui->search_edit->clearFocus(); });
}

void FavoriteDialog::loadFavorites()
{
    // 这里模拟3个收藏的商家
    favoriteIds = {0, 1, 2};

    for (int i = 0; i < favoriteIds.size(); i++)
    {
        MerchantInfo info = MerchantManager::GetInstance()->GetMerchantInfo(favoriteIds[i]);
        createMerchantWidget(info, i);
    }
}

void FavoriteDialog::createMerchantWidget(const MerchantInfo &info, int index)
{
    QWidget *widget = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout(widget);

    // 商家图片
    QLabel *imageLabel = new QLabel;
    imageLabel->setObjectName("merchantImage");
    imageLabel->setFixedSize(80, 80);
    QPixmap pixmap(std::get<2>(info));
    imageLabel->setPixmap(pixmap.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageLabel->setScaledContents(true);

    // 商家信息容器
    QWidget *infoWidget = new QWidget;
    QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);

    // 商家名称
    QLabel *nameLabel = new QLabel(std::get<0>(info));
    QFont nameFont = nameLabel->font();
    nameFont.setPointSize(12);
    nameFont.setBold(true);
    nameLabel->setFont(nameFont);
    nameLabel->setObjectName("merchantName");

    // 商家地址
    QLabel *locationLabel = new QLabel(std::get<1>(info));
    locationLabel->setObjectName("merchantLocation");

    // 添加菜品预览
    QString menuPreview;
    const auto &menu = std::get<3>(info);
    int count = 0;
    for (const auto &item : menu)
    {
        if (count++ > 2)
            break;
        menuPreview += item.first + "、";
    }
    if (!menuPreview.isEmpty())
    {
        menuPreview.chop(1);
    }
    QLabel *menuLabel = new QLabel(menuPreview);
    menuLabel->setObjectName("menuPreview");

    // 将信息添加到布局中
    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(locationLabel);
    infoLayout->addWidget(menuLabel);
    infoLayout->setSpacing(5);

    // 设置主布局
    layout->addWidget(imageLabel);
    layout->addWidget(infoWidget, 1);
    layout->setContentsMargins(10, 10, 10, 10);

    // 创建列表项
    QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
    item->setSizeHint(widget->sizeHint());
    ui->listWidget->setItemWidget(item, widget);

    // 存储商家ID
    widget->setProperty("merchant_id", favoriteIds[index]);
    widget->setCursor(Qt::PointingHandCursor);
    widget->installEventFilter(this);
}

bool FavoriteDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        int merchantId = obj->property("merchant_id").toInt();
        emit SigMerchantSelected(merchantId);
        return true;
    }
    return QDialog::eventFilter(obj, event);
}

void FavoriteDialog::updateFavorites()
{
    ui->listWidget->clear();
    loadFavorites();
}
