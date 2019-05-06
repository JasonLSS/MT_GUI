#include "fitting_method.h"
#include "ui_fitting_method.h"
#include "mainwindow.h"

#pragma execution_character_set("utf-8")
const int g_nBorder = 6;

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

    for(int i = 0;i < 3;i++)
   {
        QWidget *widget;
        QHBoxLayout *hLayout;
        QCheckBox *ckb;
        ckb = new QCheckBox();
        hLayout = new QHBoxLayout();
        widget = new QWidget(ui->tableWidget);
        hLayout->addWidget(ckb);
        hLayout->setMargin(0);                          // 必须添加, 否则CheckBox不能正常显示
        hLayout->setAlignment(ckb, Qt::AlignCenter);
        widget->setLayout(hLayout);
        ui->tableWidget->setCellWidget(i, 0, widget);
        QWidget *widget2;
        QHBoxLayout *hLayout2;
        QCheckBox *ckb2;
        ckb2 = new QCheckBox();
        hLayout2 = new QHBoxLayout();
        widget2 = new QWidget(ui->tableWidget);
        hLayout2->addWidget(ckb2);
        hLayout2->setMargin(0);                          // 必须添加, 否则CheckBox不能正常显示
        hLayout2->setAlignment(ckb2, Qt::AlignCenter);
        widget2->setLayout(hLayout2);
        ui->tableWidget->setCellWidget(i, 1, widget2);
   }

    connect(ui->tableWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ItemMenu(const QPoint&)));
    connect(ui->actionAdd, SIGNAL(triggered()), this, SLOT(OnMenuAdd()));
    connect(ui->actionRemove, SIGNAL(triggered()), this, SLOT(OnMenuRemove()));
    connect(ui->AddButton,SIGNAL(clicked()),this,SLOT(OnMenuAdd()));
    connect(ui->DeleteButton,SIGNAL(clicked()),this,SLOT(OnMenuRemove()));
    connect(ui->tableWidget, SIGNAL(cellChanged(int row,int col)), this, SLOT(settingTableChanged(int row, int col)));


}

fitting_method::~fitting_method()
{
    delete ui;
}


void fitting_method::OnMenuAdd() {
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);
    QWidget *widget;
    QHBoxLayout *hLayout;
    QCheckBox *ckb;
    ckb = new QCheckBox();
    hLayout = new QHBoxLayout();
    widget = new QWidget(ui->tableWidget);
    hLayout->addWidget(ckb);
    hLayout->setMargin(0);                          // 必须添加, 否则CheckBox不能正常显示
    hLayout->setAlignment(ckb, Qt::AlignCenter);
    widget->setLayout(hLayout);
    ui->tableWidget->setCellWidget(row, 0, widget);
    QWidget *widget2;
    QHBoxLayout *hLayout2;
    QCheckBox *ckb2;
    ckb2 = new QCheckBox();
    hLayout2 = new QHBoxLayout();
    widget2 = new QWidget(ui->tableWidget);
    hLayout2->addWidget(ckb2);
    hLayout2->setMargin(0);                          // 必须添加, 否则CheckBox不能正常显示
    hLayout2->setAlignment(ckb2, Qt::AlignCenter);
    widget2->setLayout(hLayout2);
    ui->tableWidget->setCellWidget(row, 1, widget2);
}

void fitting_method::OnMenuRemove() {
    QTableWidgetItem *item = ui->tableWidget->currentItem();
    int row_length = ui->tableWidget->rowCount();
    if (item) {
        ui->tableWidget->removeRow(item->row());
    }
    else if(ui->tableWidget->item(row_length - 1,0) == nullptr & ui->tableWidget->item(row_length - 1,1) == nullptr & ui->tableWidget->item(row_length - 1,2) == nullptr & ui->tableWidget->item(row_length - 1,3) == nullptr & ui->tableWidget->item(row_length - 1,4) == nullptr){
        ui->tableWidget->removeRow(row_length - 1);
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

void fitting_method::settingTableChanged (int row, int col){
    QWidget *widget = ui->tableWidget->cellWidget(row, col);
    QCheckBox *ckb = (QCheckBox *)widget->children().at(0);
    if(ckb->checkState() == Qt::Checked){
        ckb->setChecked(true);
        for(int j=1;j<ui->tableWidget->rowCount();j++){
            if(j!=row){
                QWidget *widget1 = ui->tableWidget->cellWidget(j, col);
                QCheckBox *ckb1 = (QCheckBox *)widget1->children().at(0);
                ckb1->setChecked(false);
            }
        }
    }
//    if(ui->tableWidget->item (row,col)->checkState ()==Qt::Checked){
//        ui->tableWidget->item(row,col)->setTextColor (Qt::green);
//        for(int j=1;j<16;j++)
//        {
//            if(j!=col){
//                ui->tableWidget->item(row,j)->setCheckState (Qt::Unchecked); //插入复选框
//            }
//        }
//    }

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
