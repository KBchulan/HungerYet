#ifndef FAVORITEDIALOG_H
#define FAVORITEDIALOG_H

#include <QDialog>

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

private:
    Ui::FavoriteDialog *ui;
};

#endif // FAVORITEDIALOG_H
