#ifndef FAVORITEDIALOG_H
#define FAVORITEDIALOG_H

#include <QDialog>
#include <QVector>
#include "merchantmanager.h"

namespace Ui
{
    class FavoriteDialog;
}

class FavoriteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FavoriteDialog(QWidget *parent = nullptr);
    ~FavoriteDialog();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::FavoriteDialog *ui;
    QVector<int> favoriteIds; // 存储收藏的商家ID
    
    void setupUI();
    void loadFavorites();
    void updateFavorites();
    void createMerchantWidget(const MerchantInfo& info, int index);

signals:
    void SigMerchantSelected(int merchant_id);
};

#endif // FAVORITEDIALOG_H
