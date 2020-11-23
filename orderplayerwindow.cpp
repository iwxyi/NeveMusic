#include "orderplayerwindow.h"
#include "ui_orderplayerwindow.h"

OrderPlayerWindow::OrderPlayerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::OrderPlayerWindow)
{
    ui->setupUi(this);
}

OrderPlayerWindow::~OrderPlayerWindow()
{
    delete ui;
}

