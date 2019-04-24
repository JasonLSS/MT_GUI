#ifndef NORMAL_DISTRIBUTION_H
#define NORMAL_DISTRIBUTION_H

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QValueAxis>
#include "mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <qt_windows.h>
#include <QListWidgetItem>
#include <QMenu>
#include <QScrollBar>
#include <QtCore/qmath.h>
#include <QMessageBox>

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class normal_distribution;
}


#if !defined(POINT_QUANTITY)
#define POINT_QUANTITY 192
#endif

#if !defined(MAX_AMP_VALUE)
#define MAX_AMP_VALUE 255
#endif

#if !defined(MIN_AMP_VALUE)
#define MIN_AMP_VALUE 0
#endif

class normal_distribution : public QMainWindow
{
    Q_OBJECT

public:
    explicit normal_distribution(QWidget *parent = nullptr);
    ~normal_distribution();
    void ChartMove(int x,int y);
    QValueAxis      *       m_pAxsX;
    QValueAxis      *       m_pAxsY;
    QLineSeries     *       m_pLineData;
    QChartView      *       m_pChartView;
    QChart          *       m_pChart;

    float                   m_fMaxY;
    float                   m_fMinY;

    bool mDrag;

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);

private slots:
    void on_btnMin_clicked();
    void on_btnMax_clicked();
    void on_btnExit_clicked();
    void login_mainwindow();
    void AddItem();
    void ItemMenu(const QPoint& pt);
    void OnMenuAdd();
    void OnMenuRemove();
    void OnItemDoubleClicked(QListWidgetItem *item);
    void UpdateMax();
    void UpdateMin();
    bool IsNumber(QString &qstrSrc);
    void ResetAll();
    void UpdateCorrect(double percent);
    void sortItemsSelf();
    void CalcNumber();
    void UpdateConstantError(double error);
    void UpdateChart(double deviation,double average);


private:
    Ui::normal_distribution *ui;

    QPoint mDragPos;
    bool mIsMax;
    QRect mLocation;
    QFont mIconFont;

};

#endif // NORMAL_DISTRIBUTION_H
