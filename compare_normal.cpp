#include "compare_normal.h"
#include "ui_compare_normal.h"
#pragma execution_character_set("utf-8")
const int g_nBorder = 6;

compare_normal::compare_normal(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::compare_normal)
{
    ui->setupUi(this);
    //窗口无边框化
    setWindowFlags(Qt::FramelessWindowHint);
    //鼠标事件跟踪
    setMouseTracking(true);
    mLocation = this->geometry();
    mIsMax = false;
    mDrag = false;
    //标题栏基本图标设置
    ui->widgetTitle->installEventFilter(this);
    ui->btnMin->setIcon(QIcon(":/image/min.png"));
    ui->btnMax->setIcon(QIcon(":/image/max1.png"));
    ui->btnExit->setIcon(QIcon(":/image/exit.png"));
    ui->btnBack->setIcon(QIcon(":/image/back.png"));
//设置背景颜色
    setStyleSheet("QMainWindow{color:#E8E8E8;}");
    UpdateChart(normal_distribution::deviation,normal_distribution::average);
    UpdateChart_2(normal_distribution::deviation_2,normal_distribution::average_2);
    UpdateCorrect(normal_distribution::correct_percent);
    UpdateCorrect_2(normal_distribution::correct_percent_2);
    UpdateConstantError(normal_distribution::constant_error);
    UpdateConstantError_2(normal_distribution::constant_error_2);
    connect(ui->btnBack,SIGNAL(clicked()),this,SLOT(close_windows()));
}

compare_normal::~compare_normal()
{
    delete ui;
}


void compare_normal::UpdateChart(double deviation,double average){
    //图表内容
        QList<QLineSeries *> m_series;
        QLineSeries *series0 = new QLineSeries();
        QLineSeries *series1 = new QLineSeries();//实例化一个QLineSeries对象
        QLineSeries *series2 = new QLineSeries();//实例化一个QLineSeries对象
        QLineSeries *series3 = new QLineSeries();//实例化一个QLineSeries对象
        QLineSeries *series4 = new QLineSeries();
        m_series.append(series1);
        m_series.append(series2);
        m_series.append(series3);
        //设置线条名称
        series1->setName(QString("line"));
        series2->setName(QString("maxline"));
        series3->setName(QString("minline"));


        //设置是否线条可视
        series1->setVisible(true);
        series2->setVisible(true);
        series3->setVisible(true);
        double maxnum = 0;
        //正态分布曲线绘制
        for(double j = -500;j < 501;j++){
            double step = 20*deviation/1000;
            double x = average + j * step;
            double y = 0.39894228/deviation*qExp(-(x-average)*(x-average)/2/deviation/deviation);
            series1->append(x, y);
            if(x>=normal_distribution::minValue&&x<=normal_distribution::maxValue)
                series4->append(x,y);
            if(y > maxnum)
                maxnum = y;
        }
        series2->append(normal_distribution::maxValue, - maxnum * 20);
        series2->append(normal_distribution::maxValue, maxnum * 20);
        series3->append(normal_distribution::minValue, - maxnum * 20);
        series3->append(normal_distribution::minValue, maxnum * 20);
        *series0 << QPointF(normal_distribution::minValue, 0) << QPointF(normal_distribution::maxValue, 0);

        QAreaSeries *series = new QAreaSeries(series0, series4);
        series->setName("area");
        QPen pen(0x059605);
        pen.setWidth(2);
        series->setPen(pen);
        QLinearGradient gradient(QPointF(0, 0), QPointF(0, 1));
        gradient.setColorAt(0.0, 0x3cc63c);
        gradient.setColorAt(1.0, 0x26f626);
        gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
        series->setBrush(gradient);

        QChart *chart = new QChart();
        chart->setTheme(QChart::ChartThemeBlueCerulean);//设置系统主题
        chart->setAnimationOptions(QChart::AllAnimations);//设置启用或禁用动画
        chart->setDropShadowEnabled(true);
        chart->setLocalizeNumbers(true);//数字是否本地化
        chart->addSeries(series1);//添加系列到QChart上
        chart->addSeries(series2);//添加系列到QChart上
        chart->addSeries(series3);//添加系列到QChart上
        chart->addSeries(series);
        chart->createDefaultAxes();//创建默认轴

        QValueAxis *axisX = new QValueAxis();//轴变量、数据系列变量，都不能声明为局部临时变量
        QValueAxis *axisY = new QValueAxis();//创建X/Y轴
        axisX->setRange(-3 * deviation + average, 3 * deviation + average);
        axisY->setRange(maxnum * - 0.03, maxnum * 1.2);//设置X/Y显示的区间
        chart->setAxisX(axisX);
        chart->setAxisY(axisY);//设置chart的坐标轴
        series1->attachAxis(axisX);
        series1->attachAxis(axisY);
        series2->attachAxis(axisX);
        series2->attachAxis(axisY);
        series3->attachAxis(axisX);
        series3->attachAxis(axisY);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
        chart->setTitle("正态分布曲线");//设置标题
        chart->setTitleBrush(QBrush(QColor(255,170,255)));//设置标题Brush
        chart->setTitleFont(QFont("微软雅黑"));//设置标题字体
        ui->graphicsView->setChart(chart);
        ui->graphicsView->setRenderHint(QPainter::Antialiasing);
}

void compare_normal::UpdateChart_2(double deviation,double average){
    //图表内容
        QList<QLineSeries *> m_series;
        QLineSeries *series0 = new QLineSeries();
        QLineSeries *series1 = new QLineSeries();//实例化一个QLineSeries对象
        QLineSeries *series2 = new QLineSeries();//实例化一个QLineSeries对象
        QLineSeries *series3 = new QLineSeries();//实例化一个QLineSeries对象
        QLineSeries *series4 = new QLineSeries();
        m_series.append(series1);
        m_series.append(series2);
        m_series.append(series3);
        //设置线条名称
        series1->setName(QString("line"));
        series2->setName(QString("maxline"));
        series3->setName(QString("minline"));


        //设置是否线条可视
        series1->setVisible(true);
        series2->setVisible(true);
        series3->setVisible(true);
        double maxnum = 0;
        //正态分布曲线绘制
        for(double j = -500;j < 501;j++){
            double step = 20*deviation/1000;
            double x = average + j * step;
            double y = 0.39894228/deviation*qExp(-(x-average)*(x-average)/2/deviation/deviation);
            series1->append(x, y);
            if(x>=normal_distribution::minValue_2&&x<=normal_distribution::maxValue_2)
                series4->append(x,y);
            if(y > maxnum)
                maxnum = y;
        }
        series2->append(normal_distribution::maxValue_2, - maxnum * 20);
        series2->append(normal_distribution::maxValue_2, maxnum * 20);
        series3->append(normal_distribution::minValue_2, - maxnum * 20);
        series3->append(normal_distribution::minValue_2, maxnum * 20);
        *series0 << QPointF(normal_distribution::minValue_2, 0) << QPointF(normal_distribution::maxValue_2, 0);

        QAreaSeries *series = new QAreaSeries(series0, series4);
        series->setName("area");
        QPen pen(0x059605);
        pen.setWidth(2);
        series->setPen(pen);
        QLinearGradient gradient(QPointF(0, 0), QPointF(0, 1));
        gradient.setColorAt(0.0, 0x3cc63c);
        gradient.setColorAt(1.0, 0x26f626);
        gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
        series->setBrush(gradient);

        QChart *chart = new QChart();
        chart->setTheme(QChart::ChartThemeBlueCerulean);//设置系统主题
        chart->setAnimationOptions(QChart::AllAnimations);//设置启用或禁用动画
        chart->setDropShadowEnabled(true);
        chart->setLocalizeNumbers(true);//数字是否本地化
        chart->addSeries(series1);//添加系列到QChart上
        chart->addSeries(series2);//添加系列到QChart上
        chart->addSeries(series3);//添加系列到QChart上
        chart->addSeries(series);
        chart->createDefaultAxes();//创建默认轴


        QValueAxis *axisX = new QValueAxis();//轴变量、数据系列变量，都不能声明为局部临时变量
        QValueAxis *axisY = new QValueAxis();//创建X/Y轴
        axisX->setRange(-3 * deviation + average, 3 * deviation + average);
        axisY->setRange(maxnum * - 0.03, maxnum * 1.2);//设置X/Y显示的区间
        chart->setAxisX(axisX);
        chart->setAxisY(axisY);//设置chart的坐标轴
        series1->attachAxis(axisX);
        series1->attachAxis(axisY);
        series2->attachAxis(axisX);
        series2->attachAxis(axisY);
        series3->attachAxis(axisX);
        series3->attachAxis(axisY);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
        chart->setTitle("正态分布曲线");//设置标题
        chart->setTitleBrush(QBrush(QColor(255,170,255)));//设置标题Brush
        chart->setTitleFont(QFont("微软雅黑"));//设置标题字体
        ui->graphicsView_2->setChart(chart);
        ui->graphicsView_2->setRenderHint(QPainter::Antialiasing);
}

//更新合格率
void compare_normal::UpdateCorrect(double percent){
    if(percent > 0.7){
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::yellow);
        ui->Correct_percent->setPalette(pa);
    }
    else if(percent >0.9){
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::green);
        ui->Correct_percent->setPalette(pa);
    }else{
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::red);
        ui->Correct_percent->setPalette(pa);
    }
    ui->Correct_percent->setText(QString::number(percent*100) + "%");
    normal_distribution::correct_percent = percent;
}

void compare_normal::UpdateCorrect_2(double percent){
    if(percent > 0.7){
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::yellow);
        ui->Correct_percent_2->setPalette(pa);
    }
    else if(percent >0.9){
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::green);
        ui->Correct_percent_2->setPalette(pa);
    }else{
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::red);
        ui->Correct_percent_2->setPalette(pa);
    }
    ui->Correct_percent_2->setText(QString::number(percent*100) + "%");
    normal_distribution::correct_percent_2 = percent;
}

//更新常值误差
void compare_normal::UpdateConstantError(double error){
     ui->Constant_error->setText(QString::number(error));
     normal_distribution::constant_error = error;
}

void compare_normal::UpdateConstantError_2(double error){
     ui->Constant_error_2->setText(QString::number(error));
     normal_distribution::constant_error_2 = error;
}

void compare_normal::close_windows(){
    this->close();
}
//最小化
void compare_normal::on_btnMin_clicked()
{
    showMinimized();
}
//最大化
void compare_normal::on_btnMax_clicked()
{
    if (mIsMax)
    {
        setGeometry(mLocation);
        ui->btnMax->setIcon(QIcon(":/image/max1.png"));
        ui->btnMax->setToolTip(QStringLiteral("最大化"));
    }
    else
    {
        mLocation = geometry();
        setGeometry(qApp->desktop()->availableGeometry());
        ui->btnMax->setIcon(QIcon(":/image/max2.png"));
        ui->btnMax->setToolTip(QStringLiteral("还原"));
    }
    mIsMax = !mIsMax;
}
//关闭
void compare_normal::on_btnExit_clicked()
{
   this->close();
}


//双击放大缩小
bool compare_normal::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == ui->widgetTitle)
    {
        if(e->type() == QEvent::MouseButtonDblClick)
        {
            on_btnMax_clicked();
            return true;
        }
    }
    return QObject::eventFilter(obj, e);
}
//拖拉缩放窗口
bool compare_normal::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG* pMsg = (MSG*)message;
    switch (pMsg->message)
    {
        case WM_NCHITTEST:
        {
            QPoint pos = mapFromGlobal(QPoint(LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)));
            bool bHorLeft = pos.x() < g_nBorder;
            bool bHorRight = pos.x() > width() - g_nBorder;
            bool bVertTop = pos.y() < g_nBorder;
            bool bVertBottom = pos.y() > height() - g_nBorder;
            if (bHorLeft && bVertTop)
            {
                *result = HTTOPLEFT;
            }
            else if (bHorLeft && bVertBottom)
            {
                *result = HTBOTTOMLEFT;
            }
            else if (bHorRight && bVertTop)
            {
                *result = HTTOPRIGHT;
            }
            else if (bHorRight && bVertBottom)
            {
                *result = HTBOTTOMRIGHT;
            }
            else if (bHorLeft)
            {
                *result = HTLEFT;
            }
            else if (bHorRight)
            {
                *result = HTRIGHT;
            }
            else if (bVertTop)
            {
                *result = HTTOP;
            }
            else if (bVertBottom)
            {
                *result = HTBOTTOM;
            }
            else
            {
                return false;
            }
            return true;
        }
    default:
        break;
    }
    return QWidget::nativeEvent(eventType, message, result);
}


void compare_normal::mousePressEvent(QMouseEvent *e)//鼠标按下事件
{
    if (e->button() == Qt::LeftButton)
    {
        mDrag = true;
        mDragPos = e->globalPos() - pos();
        e->accept();
    }
}

void compare_normal::mouseMoveEvent(QMouseEvent *e)//鼠标移动事件
{
    if (mDrag && (e->buttons() & Qt::LeftButton))
    {
        move(e->globalPos() - mDragPos);
        e->accept();
    }
    QWidget::mouseMoveEvent(e);
}

void compare_normal::mouseReleaseEvent(QMouseEvent *e)//鼠标释放事件
{
    mDrag = false;

}


