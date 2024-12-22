/******************************************************************************
 *
 * @file       favoritedialog.h
 * @brief      收藏页面
 *
 * @author     KBchulan
 * @date       2024/12/22
 * @history
 *****************************************************************************/
#ifndef FAVORITEDIALOG_H
#define FAVORITEDIALOG_H

#include "merchantmanager.h"

#include <QDialog>
#include <QVector>

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
    QVector<int> favoriteIds;
    
    void setupUI();
    void loadFavorites();
    void updateFavorites();
    void createMerchantWidget(const MerchantInfo& info, int index);

signals:
    void SigMerchantSelected(int merchant_id);
};

#endif // FAVORITEDIALOG_H
