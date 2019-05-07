#include "fitting_method.h"
#include "ui_fitting_method.h"
#include "mainwindow.h"

#pragma execution_character_set("utf-8")
const int g_nBorder = 6;
static Value Closed_ring;
static Value Fix_ring;
static int Closed_row = -1;
static int Fix_row = -1;

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
    ui->tableWidget->setColumnCount(5);
    QStringList header;
    header<<"封闭环"<<"修配环"<<"基本尺寸"<<"上偏差"<<"下偏差";
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tableWidget->setColumnWidth(0,70);
    ui->tableWidget->setColumnWidth(1,70);
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

        ui->tableWidget->setItem(i,2,new QTableWidgetItem);
        ui->tableWidget->setItem(i,3,new QTableWidgetItem);
        ui->tableWidget->setItem(i,4,new QTableWidgetItem);
        ui->tableWidget->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    }

    connect(ui->tableWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ItemMenu(const QPoint&)));
    connect(ui->actionAdd, SIGNAL(triggered()), this, SLOT(OnMenuAdd()));
    connect(ui->actionRemove, SIGNAL(triggered()), this, SLOT(OnMenuRemove()));
    connect(ui->AddButton,SIGNAL(clicked()),this,SLOT(OnMenuAdd()));
    connect(ui->DeleteButton,SIGNAL(clicked()),this,SLOT(OnMenuRemove()));
    connect(ui->tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(SettingTableChanged(int, int)));
    connect(ui->tableWidget, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(EditChanged(QTableWidgetItem *)));
    connect(ui->CalcButton,SIGNAL(clicked()),this,SLOT(CalcNumber()));


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

    ui->tableWidget->setItem(row,2,new QTableWidgetItem);
    ui->tableWidget->setItem(row,3,new QTableWidgetItem);
    ui->tableWidget->setItem(row,4,new QTableWidgetItem);
    ui->tableWidget->item(row,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->item(row,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->item(row,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

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
    else if((ui->tableWidget->item(row_length - 1,2)->text() == "") && (ui->tableWidget->item(row_length - 1,3)->text() == "")
            && (ui->tableWidget->item(row_length - 1,4)->text() == "")){ //判断后三格是否为空
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
    if(ui->tableWidget->item (row,col)->checkState ()==Qt::Checked){
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
}

void fitting_method::EditChanged(QTableWidgetItem * item){
    QString word;
    word = item->text();
    if(IsNumber(word)){
        ui->tableWidget->item(item->row(),item->column())->setBackground(QBrush(QColor(255, 255, 255)));
    }
}

void fitting_method::CalcNumber(){
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
        for(int j = 2; j < 5; j++){
            if(ui->tableWidget->item(i,j)->text()==""){
                ui->tableWidget->item(i,j)->setBackground(QBrush(QColor(255, 0, 0)));
                error_num++;
            }else{
                ui->tableWidget->item(i,j)->setBackground(QBrush(QColor(255, 255, 255)));
            }
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

    QVector<Value> ring_list;
    bool error_word = false;
    for(int i = 0;i<ui->tableWidget->rowCount();i++){
        Value temp;
        QString word;
        word = ui->tableWidget->item(i,2)->text();
        if(IsNumber(word)){
            temp.basic = word.toFloat();
        }else{
            error_word = true;
            ui->tableWidget->item(i,2)->setBackground(QBrush(QColor(255, 0, 0)));
        }

        word = ui->tableWidget->item(i,3)->text();
        if(IsNumber(word)){
            temp.Up_deviation = word.toFloat();
        }else{
            error_word = true;
            ui->tableWidget->item(i,3)->setBackground(QBrush(QColor(255, 0, 0)));
        }

        word = ui->tableWidget->item(i,4)->text();
        if(IsNumber(word)){
            temp.Low_deviation = word.toFloat();
        }else{
            error_word = true;
            ui->tableWidget->item(i,4)->setBackground(QBrush(QColor(255, 0, 0)));
        }

        if(!error_word)
            ring_list.append(temp);
    }
    if(error_word){
        QMessageBox::about(this,tr("warning"),tr("请检查数据！"));
        return;
    }

    Closed_ring.basic = ui->tableWidget->item(Closed_row,2)->text().toFloat();
    Closed_ring.Up_deviation = ui->tableWidget->item(Closed_row,3)->text().toFloat();
    Closed_ring.Low_deviation = ui->tableWidget->item(Closed_row,4)->text().toFloat();

    Fix_ring.basic = ui->tableWidget->item(Fix_row,2)->text().toFloat();
    Fix_ring.Up_deviation = ui->tableWidget->item(Fix_row,3)->text().toFloat();
    Fix_ring.Low_deviation = ui->tableWidget->item(Fix_row,4)->text().toFloat();



}

bool fitting_method::IsNumber(QString &qstrSrc)
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
