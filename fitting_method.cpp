#include "fitting_method.h"
#include "ui_fitting_method.h"
#include "mainwindow.h"

#pragma execution_character_set("utf-8")
const int g_nBorder = 6;
static Value Closed_ring;
static Value Fix_ring;
static int Closed_row = -1;
static int Fix_row = -1;
static int Basic_deviation = 7;

fitting_method::fitting_method(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::fitting_method)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);

    mLocation = this->geometry();
    mIsMax = false;
    mDrag = false;

    ui->widgetTitle->installEventFilter(this);
    ui->btnMin->setIcon(QIcon(":/image/min.png"));
    ui->btnMax->setIcon(QIcon(":/image/max1.png"));
    ui->btnExit->setIcon(QIcon(":/image/exit.png"));
    ui->btnBack->setIcon(QIcon(":/image/back.png"));

    setMouseTracking(true);
    setStyleSheet("QMainWindow{color:#E8E8E8;}");

    connect(ui->btnBack,SIGNAL(clicked(bool)),this,SLOT(login_mainwindow()));
    ui->tabWidget->setTabEnabled(2, false);

    ui->tableWidget->setRowCount(3);
    ui->tableWidget->setColumnCount(7);
    QStringList header;
    header<<"封闭环"<<"修配环"<<"轴/孔"<<"基本尺寸"<<"公差等级"<<"上偏差"<<"下偏差";
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tableWidget->setColumnWidth(0,70);
    ui->tableWidget->setColumnWidth(1,70);
    ui->tableWidget->setColumnWidth(2,70);
    for(int i = 0;i < 3;i++)
    {
        QTableWidgetItem *check=new QTableWidgetItem;
        check->setCheckState (Qt::Unchecked);
        ui->tableWidget->setItem(i,0,check);
        check->setFlags(check->flags() & (~Qt::ItemIsEditable));

        QTableWidgetItem *check2=new QTableWidgetItem;
        check2->setCheckState (Qt::Unchecked);
        ui->tableWidget->setItem(i,1,check2);
        check2->setFlags(check2->flags() & (~Qt::ItemIsEditable));

        QTableWidgetItem *check3=new QTableWidgetItem;
        check3->setCheckState (Qt::Unchecked);
        ui->tableWidget->setItem(i,2,check3);
        check3->setFlags(check3->flags() & (~Qt::ItemIsEditable));

        ui->tableWidget->setItem(i,3,new QTableWidgetItem);
        ui->tableWidget->setItem(i,4,new QTableWidgetItem);
        ui->tableWidget->setItem(i,5,new QTableWidgetItem);
        ui->tableWidget->setItem(i,6,new QTableWidgetItem);
        ui->tableWidget->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,6)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,4)->setText(QString::number(Basic_deviation));
        ui->tableWidget->item(i,5)->setText(QString::number(0));
        ui->tableWidget->item(i,6)->setText(QString::number(0));

    }

    ui->spinBox->setValue(Basic_deviation);

    connect(ui->tableWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ItemMenu(const QPoint&)));
    connect(ui->actionAdd, SIGNAL(triggered()), this, SLOT(OnMenuAdd()));
    connect(ui->actionRemove, SIGNAL(triggered()), this, SLOT(OnMenuRemove()));
    connect(ui->AddButton,SIGNAL(clicked()),this,SLOT(OnMenuAdd()));
    connect(ui->DeleteButton,SIGNAL(clicked()),this,SLOT(OnMenuRemove()));
    connect(ui->tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(SettingTableChanged(int, int)));
    connect(ui->tableWidget, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(EditChanged(QTableWidgetItem *)));
    connect(ui->CalcButton,SIGNAL(clicked()),this,SLOT(CalcNumber()));
    connect(ui->spinBox,SIGNAL(valueChanged(int)),this,SLOT(Basic_changed(int)));
    connect(ui->ResetButton,SIGNAL(clicked()),this,SLOT(ResetAll()));

}

fitting_method::~fitting_method()
{
    delete ui;
}


void fitting_method::OnMenuAdd() {
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);//添加新行

    QTableWidgetItem *check=new QTableWidgetItem;
    check->setCheckState (Qt::Unchecked);
    ui->tableWidget->setItem(row,0,check);
    check->setFlags(check->flags() & (~Qt::ItemIsEditable));

    QTableWidgetItem *check2=new QTableWidgetItem;
    check2->setCheckState (Qt::Unchecked);
    ui->tableWidget->setItem(row,1,check2);
    check2->setFlags(check2->flags() & (~Qt::ItemIsEditable));

    QTableWidgetItem *check3=new QTableWidgetItem;
    check3->setCheckState (Qt::Unchecked);
    ui->tableWidget->setItem(row,2,check3);
    check3->setFlags(check3->flags() & (~Qt::ItemIsEditable));

    ui->tableWidget->setItem(row,3,new QTableWidgetItem);
    ui->tableWidget->setItem(row,4,new QTableWidgetItem);
    ui->tableWidget->setItem(row,5,new QTableWidgetItem);
    ui->tableWidget->setItem(row,6,new QTableWidgetItem);
    ui->tableWidget->item(row,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->item(row,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->item(row,5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->item(row,6)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->item(row,4)->setText(QString::number(Basic_deviation));
    ui->tableWidget->item(row,5)->setText(QString::number(0));
    ui->tableWidget->item(row,6)->setText(QString::number(0));

}

void fitting_method::OnMenuRemove() {
    QTableWidgetItem *item = ui->tableWidget->currentItem();
    int row_length = ui->tableWidget->rowCount();
    if(row_length == 0){
        return;
    }
    if (item) {
        ui->tableWidget->removeRow(item->row());
    }
    else if(ui->tableWidget->item(row_length - 1,3)->text() == ""){ //判断数据是否为空
        ui->tableWidget->removeRow(row_length - 1);
    }

    bool flag = false;
    bool flag2 = false;
    for(int i = 0;i<ui->tableWidget->rowCount();i++){
        if(ui->tableWidget->item (i,0)->checkState ()==Qt::Checked){
            Closed_row = i;
            flag = true;
        }
        if(ui->tableWidget->item (i,1)->checkState ()==Qt::Checked){
            Fix_row = i;
            flag2 = true;
        }
    }
    if(!flag){
        Closed_row = -1;
        //        for(int i = 0;i<ui->tableWidget->rowCount();i++){
        //            ui->tableWidget->item(i,0)->setBackground(QBrush(QColor(255, 0, 0)));
        //        }
    }
    if(!flag2){
        Fix_row = -1;
        //        for(int i = 0;i<ui->tableWidget->rowCount();i++){
        //            ui->tableWidget->item(i,1)->setBackground(QBrush(QColor(255, 0, 0)));
        //        }
    }
}

void fitting_method::ItemMenu(const QPoint& pt) {
    QTableWidgetItem *selectedItem = ui->tableWidget->currentItem();
    QMenu menu;
    menu.addAction(ui->actionAdd);
    if (selectedItem) {
        menu.addAction(ui->actionRemove);
    }
    menu.exec(ui->tableWidget->mapToGlobal(pt));
}

void fitting_method::SettingTableChanged (int row, int col){
    if(ui->tableWidget->item (row,col)->checkState ()==Qt::Checked &&(col == 0 ||col == 1)){
        for(int j=0;j<ui->tableWidget->rowCount();j++)
        {
            if(j!=row){
                ui->tableWidget->item(j,col)->setCheckState (Qt::Unchecked); //遍历不选择
            }
        }
        if(col == 0){
            Closed_row = row;
            for(int i = 0;i<ui->tableWidget->rowCount();i++){
                ui->tableWidget->item(i,0)->setBackground(QBrush(QColor(255, 255, 255)));
            }
        }
        else if(col == 1){
            Fix_row = row;
            for(int i = 0;i<ui->tableWidget->rowCount();i++){
                ui->tableWidget->item(i,1)->setBackground(QBrush(QColor(255, 255, 255)));
            }
        }
    }
    if(col == 4){
        if(ui->tableWidget->item (row,col)->text() != ""){
            if(ui->tableWidget->item (row,col)->text().toDouble() < 5){
                ui->tableWidget->item (row,col)->setBackground(QBrush(QColor(255, 0, 0)));
                QMessageBox::about(this,tr("warning"),tr("请输入数字5~14！"));
                return;
            }
            else if(ui->tableWidget->item (row,col)->text().toDouble() > 14){
                ui->tableWidget->item (row,col)->setBackground(QBrush(QColor(255, 0, 0)));
                QMessageBox::about(this,tr("warning"),tr("请输入数字5~14！"));
                return;
            }
        }
    }
    if(col == 3){
        ui->tableWidget->item (row,5)->setText(QString::number(0));
        ui->tableWidget->item (row,6)->setText(QString::number(0));
    }
}

void fitting_method::EditChanged(QTableWidgetItem * item){
    QString word;
    word = item->text();
    if(IsNumber(word) && word != "" && item->column() != 4){
        ui->tableWidget->item(item->row(),item->column())->setBackground(QBrush(QColor(255, 255, 255)));
    }
    if(IsNumber(word) && word != "" && item->column() == 4 && word.toDouble()<=14 && word.toDouble()>=5){
        ui->tableWidget->item(item->row(),item->column())->setBackground(QBrush(QColor(255, 255, 255)));
    }
}

void fitting_method::CalcNumber(){
    QVector<Value> ring_list;
    if(Closed_row == -1){
        for(int i = 0;i<ui->tableWidget->rowCount();i++){
            ui->tableWidget->item(i,0)->setBackground(QBrush(QColor(255, 0, 0)));
        }
    }else{
        for(int i = 0;i<ui->tableWidget->rowCount();i++){
            ui->tableWidget->item(i,0)->setBackground(QBrush(QColor(255, 255, 255)));
        }
    }

    if(Fix_row == -1){
        for(int i = 0;i<ui->tableWidget->rowCount();i++){
            ui->tableWidget->item(i,1)->setBackground(QBrush(QColor(255, 0, 0)));
        }
    }else{
        for(int i = 0;i<ui->tableWidget->rowCount();i++){
            ui->tableWidget->item(i,1)->setBackground(QBrush(QColor(255, 255, 255)));
        }
    }

    int error_num = 0;
    for(int i = 0;i<ui->tableWidget->rowCount();i++){
        for(int j = 3; j < 4; j++){
            if(ui->tableWidget->item(i,j)->text()==""){
                ui->tableWidget->item(i,j)->setBackground(QBrush(QColor(255, 0, 0)));
                error_num++;
            }else{
                ui->tableWidget->item(i,j)->setBackground(QBrush(QColor(255, 255, 255)));
            }
        }
    }
    bool error_word = false;
    for(int i = 0;i<ui->tableWidget->rowCount();i++){
        QString word;
        word = ui->tableWidget->item(i,3)->text();
        if(!IsNumber(word)){
            error_word = true;
            ui->tableWidget->item(i,3)->setBackground(QBrush(QColor(255, 0, 0)));
        }
        word = ui->tableWidget->item(i,5)->text();
        if(!IsNumber(word)){
            error_word = true;
            ui->tableWidget->item(i,5)->setBackground(QBrush(QColor(255, 0, 0)));
        }
        word = ui->tableWidget->item(i,6)->text();
        if(!IsNumber(word)){
            error_word = true;
            ui->tableWidget->item(i,6)->setBackground(QBrush(QColor(255, 0, 0)));
        }
    }

    if(Closed_row == -1){
        QMessageBox::about(this,tr("warning"),tr("请勾选封闭环！"));
        return;
    }
    if(Fix_row == -1){
        QMessageBox::about(this,tr("warning"),tr("请勾选修配环!"));
        return;
    }
    if(error_num > 0){
        QMessageBox::about(this,tr("warning"),tr("请输入数据！"));
        return;
    }
    if(error_word){
        QMessageBox::about(this,tr("warning"),tr("请检查数据！"));
        return;
    }

    //读取所有列表数据并计算上下偏差
    Calc_Ring(&Closed_ring,Closed_row);
    Calc_Ring(&Fix_ring,Fix_row);
    for(int i = 0;i<ui->tableWidget->rowCount();i++){
        Value temp;
        Calc_Ring(&temp,i);
        ring_list.append(temp);
    }

    //校验基本尺寸和是否为0
    double ring_length = 0;
    for(int i = 0;i<ring_list.length();i++){
        ring_length += ring_list[i].basic;
    }
    if(qAbs(ring_length) >= 0.000001){
        QMessageBox::about(this,tr("warning"),tr("基本尺寸和不为0，请检查基本尺寸！"));
        return;
    }

    double T0 = Closed_ring.Up_deviation - Closed_ring.Low_deviation;
    double Tf = Fix_ring.Up_deviation - Fix_ring.Low_deviation;
    double T01 = 0;
    double delt_fix = 0;
    double delt_close = T0/2;
    for(int i = 0;i<ring_list.length();i++){
        if(i != Closed_row){
            T01 += ring_list[i].Up_deviation - ring_list[i].Low_deviation;
        }
        if(i != Closed_row && i != Fix_row){
            if(fabs(ring_list[i].basic) <= 0.00000001){
                delt_fix += 0;
            }else {
                delt_fix += fabs(ring_list[i].basic)/ring_list[i].basic*(ring_list[i].Up_deviation - ring_list[i].Low_deviation)/2;
            }
        }
    }
    delt_fix = delt_fix - delt_close;
    double Fbasic = 0;
    double ESf = delt_fix + Tf/2;
    double EIf = delt_fix - Tf/2;
    double ESc = delt_close + T01/2;
//    double EIc = delt_close - T01/2;
//    if(fabs(ESc + EIc - Closed_ring.Up_deviation - Closed_ring.Low_deviation)<=0.00000001){
//        QMessageBox::about(this,tr("warning"),tr("无法满足要求，请检查数据！"));
//        return;
//    }

    if(Fix_ring.basic > 0){
        Fbasic = Fix_ring.basic + ESc - Closed_ring.Up_deviation;
    }else{
        Fbasic = Fix_ring.basic - ESc + Closed_ring.Up_deviation;
    }

    ui->Fix_basic->setText(QString::number(Fbasic));
    if(ESf >= 0){
        ui->Fix_up->setText("+" + QString::number(ESf));
    }else{
        ui->Fix_up->setText(QString::number(ESf));
    }
    if(EIf >= 0){
         ui->Fix_low->setText("+" + QString::number(EIf));
    }else{
        ui->Fix_low->setText(QString::number(EIf));
    }
}

bool fitting_method::IsNumber(QString &qstrSrc)
{
    QByteArray ba = qstrSrc.toLatin1();
    const char *s = ba.data();
    bool bret = true;
    while(*s)
    {
        if((*s>='0' && *s<='9') || *s=='.'||*s=='-')
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

void fitting_method::Basic_changed(int basic){
    Basic_deviation = basic;
}

void fitting_method::Calc_Ring(Value *ring,int row){
    ring->basic = ui->tableWidget->item(row,3)->text().toDouble();
    ring->tolerance = ui->tableWidget->item(row,4)->text().toInt();
    ring->Up_deviation = ui->tableWidget->item(row,5)->text().toDouble();
    ring->Low_deviation = ui->tableWidget->item(row,6)->text().toDouble();
    if(ui->tableWidget->item (row,2)->checkState () == Qt::Checked){
        ring->type = Shaft;
        ring->Low_deviation = 0;
        ring->Up_deviation = 0;
    }
    else {
        ring->type = Hold;
    }
    if(fabs(ring->Low_deviation) == 0 && fabs(ring->Up_deviation) == 0){
        ring->Low_deviation = Low_Sheet(fabs(ring->basic),ring->tolerance,ring->type)/1000;
        ring->Up_deviation = Up_Sheet(fabs(ring->basic),ring->tolerance,ring->type)/1000;
        ui->tableWidget->item(row,5)->setText(QString::number(ring->Up_deviation));
        ui->tableWidget->item(row,6)->setText(QString::number(ring->Low_deviation));
    }
}

double fitting_method::Low_Sheet(double basic,int tolerance,Type type){
    //轴类下偏差
    if(type == Shaft){
        double seperate[14] = {0,3,6,10,18,30,50,80,120,180,250,315,400,500};
        double div[14][10] = {{4,6,10,14,25,40,60,100,140,250},
                              {5,8,12,18,30,48,75,120,180,300},
                              {6,9,15,22,36,58,90,150,220,360},
                              {8,11,18,27,43,70,110,180,270,430},
                              {9,13,21,33,52,84,130,220,330,520},
                              {11,16,25,39,62,100,160,250,390,620},
                              {13,19,30,46,74,120,190,300,460,740},
                              {15,22,35,54,87,140,220,350,540,870},
                              {18,25,40,63,100,160,250,400,630,1000},
                              {20,29,46,72,115,185,290,460,720,1150},
                              {23,32,52,81,130,210,320,520,810,1300},
                              {25,36,57,89,140,230,360,570,890,1400},
                              {27,40,63,97,155,250,400,630,970,1550}};
        for(int i = 1;i<14;i++){
            if(basic < seperate[i]){
                return div[i][tolerance - 5];
            }
        }
    }
    //孔类下偏差
    return 0;
}

double fitting_method::Up_Sheet(double basic,int tolerance,Type type){
    //轴类上偏差
    if(type == Shaft){
        return 0;
    }
    //孔类上偏差
    else{
        double seperate[14] = {0,3,6,10,18,30,50,80,120,180,250,315,400,500};
        double div[14][10] = {{4,6,10,14,25,40,60,100,140,250},
                              {5,8,12,18,30,48,75,120,180,300},
                              {6,9,15,22,36,58,90,150,220,360},
                              {8,11,18,27,43,70,110,180,270,430},
                              {9,13,21,33,52,84,130,220,330,520},
                              {11,16,25,39,62,100,160,250,390,620},
                              {13,19,30,46,74,120,190,300,460,740},
                              {15,22,35,54,87,140,220,350,540,870},
                              {18,25,40,63,100,160,250,400,630,1000},
                              {20,29,46,72,115,185,290,460,720,1150},
                              {23,32,52,81,130,210,320,520,810,1300},
                              {25,36,57,89,140,230,360,570,890,1400},
                              {27,40,63,97,155,250,400,630,970,1550}};
        for(int i = 1;i<14;i++){
            if(basic < seperate[i]){
                return div[i][tolerance - 5];
            }
        }
    }
    return 0;
}

void fitting_method::ResetAll(){
    ui->Fix_basic->setText(QString::number(0));
    ui->Fix_up->setText(QString::number(0));
    ui->Fix_low->setText(QString::number(0));
    Basic_deviation = 7;
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(3);
    ui->tableWidget->setColumnCount(7);
    QStringList header;
    header<<"封闭环"<<"修配环"<<"轴/孔"<<"基本尺寸"<<"公差等级"<<"上偏差"<<"下偏差";
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tableWidget->setColumnWidth(0,70);
    ui->tableWidget->setColumnWidth(1,70);
    ui->tableWidget->setColumnWidth(2,70);
    for(int i = 0;i < 3;i++)
    {
        QTableWidgetItem *check=new QTableWidgetItem;
        check->setCheckState (Qt::Unchecked);
        ui->tableWidget->setItem(i,0,check);
        check->setFlags(check->flags() & (~Qt::ItemIsEditable));

        QTableWidgetItem *check2=new QTableWidgetItem;
        check2->setCheckState (Qt::Unchecked);
        ui->tableWidget->setItem(i,1,check2);
        check2->setFlags(check2->flags() & (~Qt::ItemIsEditable));

        QTableWidgetItem *check3=new QTableWidgetItem;
        check3->setCheckState (Qt::Unchecked);
        ui->tableWidget->setItem(i,2,check3);
        check3->setFlags(check3->flags() & (~Qt::ItemIsEditable));

        ui->tableWidget->setItem(i,3,new QTableWidgetItem);
        ui->tableWidget->setItem(i,4,new QTableWidgetItem);
        ui->tableWidget->setItem(i,5,new QTableWidgetItem);
        ui->tableWidget->setItem(i,6,new QTableWidgetItem);
        ui->tableWidget->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,6)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,4)->setText(QString::number(Basic_deviation));
        ui->tableWidget->item(i,5)->setText(QString::number(0));
        ui->tableWidget->item(i,6)->setText(QString::number(0));

    }

    ui->spinBox->setValue(Basic_deviation);

    Closed_ring = *new Value;
    Fix_ring = *new Value;
    Closed_row = -1;
    Fix_row = -1;

}








void fitting_method::login_mainwindow(){
    qDebug()<<"mainwindow";
    MainWindow *win = new MainWindow;
    win->show();
    this->close();
}

bool fitting_method::eventFilter(QObject *obj, QEvent *e)
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

bool fitting_method::nativeEvent(const QByteArray &eventType, void *message, long *result)
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

void fitting_method::mousePressEvent(QMouseEvent *e)//鼠标按下事件
{
    if (e->button() == Qt::LeftButton)
    {
        mDrag = true;
        mDragPos = e->globalPos() - pos();
        e->accept();
    }
}

void fitting_method::mouseMoveEvent(QMouseEvent *e)//鼠标移动事件
{
    if (mDrag && (e->buttons() & Qt::LeftButton))
    {
        move(e->globalPos() - mDragPos);
        e->accept();
    }
}


void fitting_method::mouseReleaseEvent(QMouseEvent *e)//鼠标释放事件
{
    mDrag = false;
}

void fitting_method::on_btnMin_clicked()
{
    showMinimized();
}

void fitting_method::on_btnMax_clicked()
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

void fitting_method::on_btnExit_clicked()
{
    qApp->exit();
}
