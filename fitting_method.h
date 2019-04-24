#ifndef FITTING_METHOD_H
#define FITTING_METHOD_H

#include <QMainWindow>
#include <QMouseEvent>

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
