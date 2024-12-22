#include "favoritedialog.h"
#include "ui_favoritedialog.h"

FavoriteDialog::FavoriteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FavoriteDialog)
{
    ui->setupUi(this);
}

FavoriteDialog::~FavoriteDialog()
{
    delete ui;
}
