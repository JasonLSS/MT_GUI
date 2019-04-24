#ifndef COMPARE_NORMAL_H
#define COMPARE_NORMAL_H
#include <QMainWindow>
#include <normal_distribution.h>
namespace Ui {
class compare_normal;
}

class compare_normal : public QMainWindow
{
    Q_OBJECT
protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);

public:
    explicit compare_normal(QWidget *parent = nullptr);
    ~compare_normal();
    bool mDrag;

private slots:
    void on_btnMin_clicked();
    void on_btnMax_clicked();
    void on_btnExit_clicked();
    void UpdateChart(double deviation,double average);
    void UpdateChart_2(double deviation,double average);
    void close_windows();
    void UpdateCorrect(double percent);
    void UpdateConstantError(double error);
    void UpdateCorrect_2(double percent);
    void UpdateConstantError_2(double error);

private:
    Ui::compare_normal *ui;
    QPoint mDragPos;
    bool mIsMax;
    QRect mLocation;
    QFont mIconFont;
};

#endif // COMPARE_NORMAL_H
