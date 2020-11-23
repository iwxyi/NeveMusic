#ifndef ORDERPLAYERWINDOW_H
#define ORDERPLAYERWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class OrderPlayerWindow; }
QT_END_NAMESPACE

class OrderPlayerWindow : public QMainWindow
{
    Q_OBJECT

public:
    OrderPlayerWindow(QWidget *parent = nullptr);
    ~OrderPlayerWindow();

private:
    Ui::OrderPlayerWindow *ui;
};
#endif // ORDERPLAYERWINDOW_H
