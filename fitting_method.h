#ifndef FITTING_METHOD_H
#define FITTING_METHOD_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QDebug>
#include <QDesktopWidget>
#include <qt_windows.h>
#include <QMenu>
#include <QCheckBox>
#include <QTableWidget>
#include <QMessageBox>
#include <QtCore/qmath.h>

enum Type{
    Hold = 0,
    Shaft = 1,
};

struct Value{
    double basic;
    double Up_deviation;
    double Low_deviation;
    Type type;
    int tolerance;

};
namespace Ui {
class fitting_method;
}

class fitting_method : public QMainWindow
{
    Q_OBJECT

public:
    explicit fitting_method(QWidget *parent = nullptr);
    ~fitting_method();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);

private slots:
    void on_btnMin_clicked();
    void on_btnMax_clicked();
    void on_btnExit_clicked();
    void login_mainwindow();
    void OnMenuAdd();
    void OnMenuRemove();
    void ItemMenu(const QPoint& pt);
    void SettingTableChanged (int row, int col);
    void CalcNumber();
    void EditChanged(QTableWidgetItem *);
    bool IsNumber(QString &qstrSrc);
    void Basic_changed(int basic);
    void Calc_Ring(Value *ring,int row);
    double Low_Sheet(double basic,int tolerance,Type type);
    double Up_Sheet(double basic,int tolerance,Type type);
    void ResetAll();

private:
    Ui::fitting_method *ui;
    //标题拖动、双击最大化
    bool mDrag;
    QPoint mDragPos;
    bool mIsMax;
    QRect mLocation;
    QFont mIconFont;

};



#endif // FITTING_METHOD_H
