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

private slots:
    void onAddButtonClicked();
    void onDeleteButtonClicked();
    void onEditButtonClicked();
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
};

#endif // MENUMANAGERDIALOG_H 