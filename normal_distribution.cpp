#include "normal_distribution.h"
#include "ui_normal_distribution.h"
#pragma execution_character_set("utf-8")

const int g_nBorder = 6;
static double maxValue = 0.0001;
static double minValue = 0.0001;
static QImage* img=new QImage,* wrong_img=new QImage,* correct_img = new QImage;
static bool cDrag;
static double average;
static double deviation;
normal_distribution::normal_distribution(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::normal_distribution)
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

    //设置合格率字号
    QFont ft;
    ft.setPointSize(30);
    ui->Correct_percent->setFont(ft);
    //设置合格率颜色并初始化0
    UpdateCorrect(0);

    ui->Constant_error->setFont(ft);
    UpdateConstantError(0);

//设置背景颜色
    setStyleSheet("QMainWindow{color:#E8E8E8;}");

//输入限制（-1000~1000,小数点后4位纯数字）
    QDoubleValidator *pDfValidator = new QDoubleValidator(-1000.0, 1000.0 , 4, ui->maxLine);
    pDfValidator->setNotation(QDoubleValidator::StandardNotation);
    ui->maxLine->setValidator(pDfValidator);
    ui->minLine->setValidator(pDfValidator);
//图片初始化重新定义大小
    img->load(":/image/wrong.png");
    *wrong_img=img->scaled(15,15);
    img->load(":/image/correct.png");
    *correct_img=img->scaled(15,15);
//label对错框架
    ui->maxCorrect->setPixmap(QPixmap::fromImage(*wrong_img));
    ui->minCorrect->setPixmap(QPixmap::fromImage(*wrong_img));


//图表内容
    average = 0;deviation = 1;
    UpdateChart(deviation,average);


    //容器连接
    connect(ui->resetBtn,SIGNAL(clicked()),this,SLOT(ResetAll()));
    connect(ui->btnBack,SIGNAL(clicked()),this,SLOT(login_mainwindow()));
    connect(ui->addBtn, SIGNAL(clicked()), this, SLOT(AddItem()));
    connect(ui->addLine, SIGNAL(returnPressed()), this, SLOT(AddItem()));
    connect(ui->maxLine, SIGNAL(returnPressed()), this, SLOT(UpdateMax()));
    connect(ui->maxLine, SIGNAL(editingFinished()), this, SLOT(UpdateMax()));
    connect(ui->minLine, SIGNAL(returnPressed()), this, SLOT(UpdateMin()));
    connect(ui->minLine, SIGNAL(editingFinished()), this, SLOT(UpdateMin()));
    connect(ui->calcBtn,SIGNAL(clicked()),this,SLOT(CalcNumber()));

    //列表容器连接
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ItemMenu(const QPoint&)));
    connect(ui->actionAdd, SIGNAL(triggered()), this, SLOT(OnMenuAdd()));
    connect(ui->actionRemove, SIGNAL(triggered()), this, SLOT(OnMenuRemove()));
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(OnItemDoubleClicked(QListWidgetItem*)));
}

normal_distribution::~normal_distribution()
{
    delete ui;
}
//返回主页面
void normal_distribution::login_mainwindow(){
    qDebug()<<"mainwindow";
    MainWindow *win = new MainWindow;
    ResetAll();
    win->show();
    this->close();
}
//添加项目
void normal_distribution::AddItem() {
    QString text = ui->addLine->text();

    if (text != nullptr) {
        QStringList list = text.split(" ");
        QStringList listtrue;
        bool listcorrect = true;
        for(int i =0;i<list.count();i++){
            QString temp = list[i];
            if(IsNumber(temp)&&(temp !=" ")){
                listtrue.append(temp);
            }
            else {
                QMessageBox::about(this,tr("warning"),tr("输入有误"));
                listtrue.clear();
                listcorrect = false;
                break;
            }
        }

        if(listcorrect){
            for(int i =0;i<list.count();i++){
                if(list[i] !=nullptr){
                    QListWidgetItem *item = new QListWidgetItem;
                    item->setText(list[i]);
                    ui->listWidget->addItem(item);
                    sortItemsSelf();
                }
            }
            ui->listWidget->scrollToBottom();
            ui->addLine->clear();
        }
    }else {
        QMessageBox::about(this,tr("warning"),tr("输入为空"));
    }
    sortItemsSelf();
}
//右键菜单
void normal_distribution::ItemMenu(const QPoint& pt) {
    QListWidgetItem *selectedItem = ui->listWidget->itemAt(pt);
    QMenu menu;
    menu.addAction(ui->actionAdd);
    if (selectedItem) {
        menu.addAction(ui->actionRemove);
    }
    menu.exec(ui->listWidget->mapToGlobal(pt));
}

void normal_distribution::OnMenuAdd() {
    QListWidgetItem *newitem = new QListWidgetItem("0");
    ui->listWidget->addItem(newitem);
    sortItemsSelf();
}

void normal_distribution::OnMenuRemove() {
    QListWidgetItem *item = ui->listWidget->currentItem();
    if (item) {
        delete item;
    }
}
//双击列表修改
void normal_distribution::OnItemDoubleClicked(QListWidgetItem *item) {
    item->setFlags(item->flags() | Qt::ItemIsEditable);
}
//数字判断
bool normal_distribution::IsNumber(QString &qstrSrc)
{
    QByteArray ba = qstrSrc.toLatin1();
    const char *s = ba.data();
    bool bret = true;
    while(*s)
    {
        if((*s>='0' && *s<='9') || *s=='.')
        {

        }
        else
        {
            bret = false;
            break;
        }
        s++;
    }
    return bret;
}
//最大值更新
void normal_distribution::UpdateMax(){
    QString text = ui->maxLine->text();
    if (text != nullptr && IsNumber(text)) {
        maxValue = text.toDouble();
        ui->maxCorrect->setPixmap(QPixmap::fromImage(*correct_img));
//        focusNextChild();
    }
    else {
        ui->maxCorrect->setPixmap(QPixmap::fromImage(*wrong_img));
        QMessageBox::about(this,tr("warning"),tr("输入错误"));
    }
}
//最小值更新
void normal_distribution::UpdateMin(){
    QString text = ui->minLine->text();
    if (text != nullptr && IsNumber(text)) {
        minValue = text.toDouble();
        ui->minCorrect->setPixmap(QPixmap::fromImage(*correct_img));
//        focusNextChild();
        if(!minValue){
            QMessageBox::about(this,tr("warning"),tr("输入错误"));
            ui->minCorrect->setPixmap(QPixmap::fromImage(*wrong_img));
        }
    }
    else {
        QMessageBox::about(this,tr("warning"),tr("输入错误"));
        ui->minCorrect->setPixmap(QPixmap::fromImage(*wrong_img));
    }
}
//数据清空
void normal_distribution::ResetAll(){
    ui->addLine->clear();
    ui->maxLine->clear();
    ui->minLine->clear();
    ui->listWidget->clear();
    maxValue = 0.0001;
    minValue = 0.0001;
    UpdateCorrect(0);
    UpdateConstantError(0);
    ui->minCorrect->setPixmap(QPixmap::fromImage(*wrong_img));
    ui->maxCorrect->setPixmap(QPixmap::fromImage(*wrong_img));
    UpdateChart(1,0);
}
//更新合格率
void normal_distribution::UpdateCorrect(double percent){
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
}
//更新常值误差
void normal_distribution::UpdateConstantError(double error){
     ui->Constant_error->setText(QString::number(error));
}
//列表排序
void normal_distribution::sortItemsSelf()
{
    ui->listWidget->sortItems(Qt::AscendingOrder);
}

void normal_distribution::CalcNumber(){
    QVector<double> number;
    sortItemsSelf();
    if(maxValue <= minValue || maxValue == 0.0001 || minValue == 0.0001){
        QMessageBox::about(this, tr("error"), tr("检查最小最大尺寸"));
        return;
    }
    if(ui->listWidget->count() == 0){
        QMessageBox::about(this, tr("error"), tr("数据表为空"));
        return;
    }
    double sum = 0;
    for(int i = 0;i < ui->listWidget->count();i++){
        QString itemText = ui->listWidget->item(i)->text();
        number.append(itemText.toDouble());
        sum += itemText.toDouble();
    }
    average = sum/number.count();
    deviation = 0;
    for(int i = 0;i < number.count();i++){
        deviation += (average - number[i])*(average - number[i]);
    }
    deviation = sqrt(deviation/number.count());

    UpdateCorrect(average);
    if((maxValue < (average - 3 * deviation))||(minValue > (average + 3 * deviation))){
        QMessageBox::about(this,tr("about"),tr("全不合格"));
        return;
    }
    double temp = 0;
    double sumcorrect = 0;
    double correct = 0;
    temp = maxValue-minValue;
    double step = temp/1000;
    for(int j = 0;j<1000;j++){
        double x = minValue + j*step;
        sumcorrect += 0.39894228/deviation*qExp(-(x-average)*(x-average)/2/deviation/deviation)*step;
    }
    correct += abs(sumcorrect);
    UpdateCorrect(correct);
    UpdateChart(deviation,average);
    UpdateConstantError((maxValue + minValue)/2 - average);
}

void normal_distribution::UpdateChart(double deviation,double average){
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
            if(x>=minValue&&x<=maxValue)
                series4->append(x,y);
            if(y > maxnum)
                maxnum = y;
        }
        series2->append(maxValue, - maxnum * 20);
        series2->append(maxValue, maxnum * 20);
        series3->append(minValue, - maxnum * 20);
        series3->append(minValue, maxnum * 20);
        *series0 << QPointF(minValue, 0) << QPointF(maxValue, 0);

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
//        chart->setAnimationOptions(QChart::AllAnimations);//设置启用或禁用动画
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



















void normal_distribution::wheelEvent(QWheelEvent *event)
{
    if (event->delta() > 0) {
        ui->graphicsView->chart()->zoom(1.1);
    } else {
        ui->graphicsView->chart()->zoom(10.0/11);
    }
    QWidget::wheelEvent(event);
}

//双击放大缩小
bool normal_distribution::eventFilter(QObject *obj, QEvent *e)
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
bool normal_distribution::nativeEvent(const QByteArray &eventType, void *message, long *result)
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


void normal_distribution::mousePressEvent(QMouseEvent *e)//鼠标按下事件
{
    if (e->button() == Qt::LeftButton)
    {
        mDrag = true;
        mDragPos = e->globalPos() - pos();
        e->accept();
    }
    else if (e->button() & Qt::RightButton) {
            ui->graphicsView->chart()->zoomReset();
            UpdateChart(deviation,average);
    }
}

void normal_distribution::mouseMoveEvent(QMouseEvent *e)//鼠标移动事件
{
    if (mDrag && (e->buttons() & Qt::LeftButton))
    {
        move(e->globalPos() - mDragPos);
        e->accept();
    }
    QWidget::mouseMoveEvent(e);
}

void normal_distribution::mouseReleaseEvent(QMouseEvent *e)//鼠标释放事件
{
    mDrag = false;

}

void QtCharts::QChartView::mousePressEvent(QMouseEvent *e){
    if (e->button() == Qt::LeftButton)
    {
        cDrag = true;
    }
}

void QtCharts::QChartView::mouseMoveEvent(QMouseEvent *event)//鼠标移动事件
{
    int x,y;
    static int xOld = 0,yOld = 0;
    if (cDrag) {
        if (xOld == 0 && yOld == 0) {

        } else {
            x = event->x() - xOld;
            y = event->y() - yOld;
            chart()->scroll(- x, y);
        }
        xOld = event->x();
        yOld = event->y();
    }
    else{
        xOld = 0;
        yOld = 0;
    }
}

void normal_distribution::ChartMove(int x,int y){
    ui->graphicsView->chart()->scroll(x, y);
}

void QtCharts::QChartView::mouseReleaseEvent(QMouseEvent *event){
    cDrag = false;
}
//最小化
void normal_distribution::on_btnMin_clicked()
{
    showMinimized();
}
//最大化
void normal_distribution::on_btnMax_clicked()
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
void normal_distribution::on_btnExit_clicked()
{
    qApp->exit();
}

