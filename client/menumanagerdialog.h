#ifndef MENUMANAGERDIALOG_H
#define MENUMANAGERDIALOG_H

#include <QWidget>
#include <QMap>

namespace Ui {
class MenuManagerDialog;
}

class MenuManagerDialog : public QWidget
{
    Q_OBJECT

public:
    explicit MenuManagerDialog(QWidget *parent = nullptr);
    ~MenuManagerDialog();
    void setMerchantId(int id) { merchantId = id; }

private slots:
    void onAddProductButtonClicked();
    void onDeleteProductButtonClicked();
    void onEditProductButtonClicked();
    void onSaveButtonClicked();
    void onItemSelectionChanged();
    void updateMenuList();

private:
    void setupUI();
    void loadMenuItems();
    bool validateInput();
    void clearInputs();

private:
    Ui::MenuManagerDialog *ui;
    QMap<QString, double> menuItems;  // 存储菜品名称和价格
    int merchantId = 0;
};

#endif // MENUMANAGERDIALOG_H 