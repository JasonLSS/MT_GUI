#include "normal_distribution.h"
#include "ui_normal_distribution.h"
#include <compare_normal.h>
#pragma execution_character_set("utf-8")
const int g_nBorder = 6;
double normal_distribution::maxValue = 0.0001;
double normal_distribution::minValue = 0.0001;
double normal_distribution::maxValue_2 = 0.0001;
double normal_distribution::minValue_2 = 0.0001;
double normal_distribution::average = 0;
double normal_distribution::average_2 = 0;
double normal_distribution::deviation = 1;
double normal_distribution::deviation_2 = 1;
double normal_distribution::correct_percent;
double normal_distribution::correct_percent_2;
double normal_distribution::constant_error;
double normal_distribution::constant_error_2;
static QImage* img=new QImage,* wrong_img=new QImage,* correct_img = new QImage;
//static bool cDrag;

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

//设置背景颜色
    setStyleSheet("QMainWindow{color:#E8E8E8;}");

//输入限制（-1000~1000,小数点后4位纯数字）
    QDoubleValidator *pDfValidator = new QDoubleValidator(-10000.0, 10000.0 , 4, ui->maxLine);
    pDfValidator->setNotation(QDoubleValidator::StandardNotation);
    ui->maxLine->setValidator(pDfValidator);
    ui->minLine->setValidator(pDfValidator);
    ui->maxLine_2->setValidator(pDfValidator);
    ui->minLine_2->setValidator(pDfValidator);
//图片初始化重新定义大小
    img->load(":/image/wrong.png");
    *wrong_img=img->scaled(15,15);
    img->load(":/image/correct.png");
    *correct_img=img->scaled(15,15);
//label对错框架
    ui->maxCorrect->setPixmap(QPixmap::fromImage(*wrong_img));
    ui->minCorrect->setPixmap(QPixmap::fromImage(*wrong_img));
    ui->maxCorrect_2->setPixmap(QPixmap::fromImage(*wrong_img));
    ui->minCorrect_2->setPixmap(QPixmap::fromImage(*wrong_img));


//图表内容
    normal_distribution::average = 0;normal_distribution::deviation = 1;
    UpdateChart(normal_distribution::deviation,normal_distribution::average);

    normal_distribution::average_2 = 0;normal_distribution::deviation_2 = 1;
    UpdateChart_2(normal_distribution::deviation_2,normal_distribution::average_2);

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

    connect(ui->resetBtn_2,SIGNAL(clicked()),this,SLOT(ResetAll_2()));
    connect(ui->addBtn_2, SIGNAL(clicked()), this, SLOT(AddItem_2()));
    connect(ui->addLine_2, SIGNAL(returnPressed()), this, SLOT(AddItem_2()));
    connect(ui->maxLine_2, SIGNAL(returnPressed()), this, SLOT(UpdateMax_2()));
    connect(ui->maxLine_2, SIGNAL(editingFinished()), this, SLOT(UpdateMax_2()));
    connect(ui->minLine_2, SIGNAL(returnPressed()), this, SLOT(UpdateMin_2()));
    connect(ui->minLine_2, SIGNAL(editingFinished()), this, SLOT(UpdateMin_2()));
    connect(ui->calcBtn_2,SIGNAL(clicked()),this,SLOT(CalcNumber_2()));

    //列表容器连接
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ItemMenu(const QPoint&)));
    connect(ui->actionAdd, SIGNAL(triggered()), this, SLOT(OnMenuAdd()));
    connect(ui->actionRemove, SIGNAL(triggered()), this, SLOT(OnMenuRemove()));
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(OnItemDoubleClicked(QListWidgetItem*)));

    ui->listWidget_2->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget_2, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ItemMenu_2(const QPoint&)));
    connect(ui->actionAdd_2, SIGNAL(triggered()), this, SLOT(OnMenuAdd_2()));
    connect(ui->actionRemove_2, SIGNAL(triggered()), this, SLOT(OnMenuRemove_2()));
    connect(ui->listWidget_2, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(OnItemDoubleClicked(QListWidgetItem*)));

    QPushButton *b2 = new QPushButton(QString::fromStdWString(L"比较"));
    b2->setObjectName("CompareButton");
    ui->tabWidget->setTabEnabled(2, false);
    ui->tabWidget->tabBar()->setTabButton(2, QTabBar::RightSide, b2);
    connect(b2,SIGNAL(clicked()),this,SLOT(CompareChartWindows()));
    //设置合格率字号
    QFont ft;
    ft.setPointSize(30);
    ui->Correct_percent->setFont(ft);
    //设置合格率颜色并初始化0
    UpdateCorrect(0);

    ui->Correct_percent_2->setFont(ft);
    //设置合格率颜色并初始化0
    UpdateCorrect_2(0);

    ui->Constant_error->setFont(ft);
    UpdateConstantError(0);

    ui->Constant_error_2->setFont(ft);
    UpdateConstantError_2(0);

    normal_distribution::constant_error = 0;
    normal_distribution::constant_error_2 = 0;
    normal_distribution::correct_percent = 0;
    normal_distribution::correct_percent_2 = 0;
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
    ResetAll_2();
    win->show();
    this->close();
}
void normal_distribution::CompareChartWindows(){
    qDebug()<<"CompareChartWindows";
    compare_normal *win = new compare_normal;
    win->show();
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

void normal_distribution::AddItem_2() {
    QString text = ui->addLine_2->text();

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
                    ui->listWidget_2->addItem(item);
                    sortItemsSelf();
                }
            }
            ui->listWidget_2->scrollToBottom();
            ui->addLine_2->clear();
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

void normal_distribution::ItemMenu_2(const QPoint& pt) {
    QListWidgetItem *selectedItem = ui->listWidget_2->itemAt(pt);
    QMenu menu;
    menu.addAction(ui->actionAdd_2);
    if (selectedItem) {
        menu.addAction(ui->actionRemove_2);
    }
    menu.exec(ui->listWidget_2->mapToGlobal(pt));
}

void normal_distribution::OnMenuAdd() {
    QListWidgetItem *newitem = new QListWidgetItem("0");
    ui->listWidget->addItem(newitem);
    sortItemsSelf();
}

void normal_distribution::OnMenuAdd_2() {
    QListWidgetItem *newitem = new QListWidgetItem("0");
    ui->listWidget_2->addItem(newitem);
    sortItemsSelf_2();
}

void normal_distribution::OnMenuRemove() {
    QListWidgetItem *item = ui->listWidget->currentItem();
    if (item) {
        delete item;
    }
}

void normal_distribution::OnMenuRemove_2() {
    QListWidgetItem *item = ui->listWidget_2->currentItem();
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

void normal_distribution::UpdateMax_2(){
    QString text = ui->maxLine_2->text();
    if (text != nullptr && IsNumber(text)) {
        normal_distribution::maxValue_2 = text.toDouble();
        ui->maxCorrect_2->setPixmap(QPixmap::fromImage(*correct_img));
//        focusNextChild();
    }
    else {
        ui->maxCorrect_2->setPixmap(QPixmap::fromImage(*wrong_img));
        QMessageBox::about(this,tr("warning"),tr("输入错误"));
    }
}

//最小值更新
void normal_distribution::UpdateMin(){
    QString text = ui->minLine->text();
    if (text != nullptr && IsNumber(text)) {
        normal_distribution::minValue = text.toDouble();
        ui->minCorrect->setPixmap(QPixmap::fromImage(*correct_img));
//        focusNextChild();
        if(!normal_distribution::minValue){
            QMessageBox::about(this,tr("warning"),tr("输入错误"));
            ui->minCorrect->setPixmap(QPixmap::fromImage(*wrong_img));
        }
    }
    else {
        QMessageBox::about(this,tr("warning"),tr("输入错误"));
        ui->minCorrect->setPixmap(QPixmap::fromImage(*wrong_img));
    }
}

void normal_distribution::UpdateMin_2(){
    QString text = ui->minLine_2->text();
    if (text != nullptr && IsNumber(text)) {
        normal_distribution::minValue_2 = text.toDouble();
        ui->minCorrect_2->setPixmap(QPixmap::fromImage(*correct_img));
//        focusNextChild();
        if(!normal_distribution::minValue_2){
            QMessageBox::about(this,tr("warning"),tr("输入错误"));
            ui->minCorrect_2->setPixmap(QPixmap::fromImage(*wrong_img));
        }
    }
    else {
        QMessageBox::about(this,tr("warning"),tr("输入错误"));
        ui->minCorrect_2->setPixmap(QPixmap::fromImage(*wrong_img));
    }
}

//数据清空
void normal_distribution::ResetAll(){
    ui->addLine->clear();
    ui->maxLine->clear();
    ui->minLine->clear();
    ui->listWidget->clear();
    normal_distribution::maxValue = 0.0001;
    normal_distribution::minValue = 0.0001;
    UpdateCorrect(0);
    UpdateConstantError(0);
    ui->minCorrect->setPixmap(QPixmap::fromImage(*wrong_img));
    ui->maxCorrect->setPixmap(QPixmap::fromImage(*wrong_img));
    UpdateChart(1,0);
}

void normal_distribution::ResetAll_2(){
    ui->addLine_2->clear();
    ui->maxLine_2->clear();
    ui->minLine_2->clear();
    ui->listWidget_2->clear();
    normal_distribution::maxValue_2 = 0.0001;
    normal_distribution::minValue_2 = 0.0001;
    UpdateCorrect_2(0);
    UpdateConstantError_2(0);
    ui->minCorrect_2->setPixmap(QPixmap::fromImage(*wrong_img));
    ui->maxCorrect_2->setPixmap(QPixmap::fromImage(*wrong_img));
    UpdateChart_2(1,0);
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
    normal_distribution::correct_percent = percent;
}

void normal_distribution::UpdateCorrect_2(double percent){
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
void normal_distribution::UpdateConstantError(double error){
     ui->Constant_error->setText(QString::number(error));
     normal_distribution::constant_error = error;
}

void normal_distribution::UpdateConstantError_2(double error){
     ui->Constant_error_2->setText(QString::number(error));
     normal_distribution::constant_error_2 = error;
}

//列表排序
void normal_distribution::sortItemsSelf()
{
    ui->listWidget->sortItems(Qt::AscendingOrder);
}

void normal_distribution::sortItemsSelf_2()
{
    ui->listWidget_2->sortItems(Qt::AscendingOrder);
}

void normal_distribution::CalcNumber(){
    QVector<double> number;
    sortItemsSelf();
    if(normal_distribution::maxValue <= normal_distribution::minValue || normal_distribution::maxValue == 0.0001 || normal_distribution::minValue == 0.0001){
        QMessageBox::about(this, tr("error"), tr("检查最小最大尺寸"));
        return;
    }
    if(ui->listWidget->count() == 0){
        QMessageBox::about(this, tr("error"), tr("数据表为空"));
        return;
    }
    if(ui->listWidget->count() == 1){
        QMessageBox::about(this, tr("error"), tr("数据表内容过少"));
        return;
    }
    double sum = 0;
    for(int i = 0;i < ui->listWidget->count();i++){
        QString itemText = ui->listWidget->item(i)->text();
        number.append(itemText.toDouble());
        sum += itemText.toDouble();
    }
    normal_distribution::average = sum/number.count();
    normal_distribution::deviation = 0;
    for(int i = 0;i < number.count();i++){
        normal_distribution::deviation += (normal_distribution::average - number[i])*(normal_distribution::average - number[i]);
    }
    normal_distribution::deviation = sqrt(normal_distribution::deviation/number.count());
    if(abs(normal_distribution::deviation) <= 0.0000001){
        QMessageBox::about(this, tr("error"), tr("数据表内容有误"));
        return;
    }
    if((normal_distribution::maxValue < (normal_distribution::average - 3 * normal_distribution::deviation))||
            (normal_distribution::minValue > (normal_distribution::average + 3 * normal_distribution::deviation))){
        QMessageBox::about(this,tr("about"),tr("全不合格"));
        return;
    }
    double temp = 0;
    double sumcorrect = 0;
    double correct = 0;
    temp = normal_distribution::maxValue-normal_distribution::minValue;
    double step = temp/1000;
    for(int j = 0;j<1000;j++){
        double x = normal_distribution::minValue + j*step;
        sumcorrect += 0.39894228/normal_distribution::deviation*qExp(-(x-normal_distribution::average)*
                    (x-normal_distribution::average)/2/normal_distribution::deviation/normal_distribution::deviation)*step;
    }
    correct += abs(sumcorrect);
    UpdateCorrect(correct);
    UpdateChart(normal_distribution::deviation,normal_distribution::average);
    UpdateConstantError((normal_distribution::maxValue + normal_distribution::minValue)/2 - normal_distribution::average);
}

void normal_distribution::CalcNumber_2(){
    QVector<double> number;
    sortItemsSelf_2();
    if(normal_distribution::maxValue_2 <= normal_distribution::minValue_2 || normal_distribution::maxValue_2 == 0.0001 || normal_distribution::minValue_2 == 0.0001){
        QMessageBox::about(this, tr("error"), tr("检查最小最大尺寸"));
        return;
    }
    if(ui->listWidget_2->count() == 0){
        QMessageBox::about(this, tr("error"), tr("数据表为空"));
        return;
    }
    if(ui->listWidget_2->count() == 1){
        QMessageBox::about(this, tr("error"), tr("数据表内容过少"));
        return;
    }
    double sum = 0;
    for(int i = 0;i < ui->listWidget_2->count();i++){
        QString itemText = ui->listWidget_2->item(i)->text();
        number.append(itemText.toDouble());
        sum += itemText.toDouble();
    }
    normal_distribution::average_2 = sum/number.count();
    normal_distribution::deviation_2 = 0;
    for(int i = 0;i < number.count();i++){
        normal_distribution::deviation_2 += (normal_distribution::average_2 - number[i])*(normal_distribution::average_2 - number[i]);
    }
    normal_distribution::deviation_2 = sqrt(normal_distribution::deviation_2/number.count());
    if(abs(normal_distribution::deviation_2) <= 0.0000001){
        QMessageBox::about(this, tr("error"), tr("数据表内容有误"));
        return;
    }
    if((normal_distribution::maxValue_2 < (normal_distribution::average_2 - 3 * normal_distribution::deviation_2))||(normal_distribution::minValue_2 > (normal_distribution::average_2 + 3 * normal_distribution::deviation_2))){
        QMessageBox::about(this,tr("about"),tr("全不合格"));
        return;
    }
    double temp = 0;
    double sumcorrect = 0;
    double correct = 0;
    temp = normal_distribution::maxValue_2-normal_distribution::minValue_2;
    double step = temp/1000;
    for(int j = 0;j<1000;j++){
        double x = normal_distribution::minValue_2 + j*step;
        sumcorrect += 0.39894228/normal_distribution::deviation_2*qExp(-(x-normal_distribution::average_2)*(x-normal_distribution::average_2)/2/normal_distribution::deviation_2/normal_distribution::deviation_2)*step;
    }
    correct += abs(sumcorrect);
    UpdateCorrect_2(correct);
    UpdateChart_2(normal_distribution::deviation_2,normal_distribution::average_2);
    UpdateConstantError_2((normal_distribution::maxValue_2 + normal_distribution::minValue_2)/2 - normal_distribution::average_2);
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

void normal_distribution::UpdateChart_2(double deviation,double average){
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

//void QtCharts::QChartView::mousePressEvent(QMouseEvent *e){
//    if (e->button() == Qt::LeftButton)
//    {
//        cDrag = true;
//    }
//}

//void QtCharts::QChartView::mouseMoveEvent(QMouseEvent *event)//鼠标移动事件
//{
//    int x,y;
//    static int xOld = 0,yOld = 0;
//    if (cDrag) {
//        if (xOld == 0 && yOld == 0) {

//        } else {
//            x = event->x() - xOld;
//            y = event->y() - yOld;
//            chart()->scroll(- x, y);
//        }
//        xOld = event->x();
//        yOld = event->y();
//    }
//    else{
//        xOld = 0;
//        yOld = 0;
//    }
//}

//void normal_distribution::ChartMove(int x,int y){
//    ui->graphicsView->chart()->scroll(x, y);
//}

//void QtCharts::QChartView::mouseReleaseEvent(QMouseEvent *event){
//    cDrag = false;
//}
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

