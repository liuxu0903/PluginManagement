#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "linkbutton.h"
#include "addfileform.h"
#include "editfileform.h"
#include "cmdstart.h"
#include <QDebug>
#include <QFileDialog>
#include <QCoreApplication>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QFile>
#include <QTableWidgetItem>
#include <string>
#include <QProcess>
#include <QMessageBox>
#include <QTextCodec>


//json文件地址
//QString json_python ="../DEMO/Json/python.json";
QString json_python =QDir::currentPath() + QDir::separator() + "data" + QDir::separator() +"Json/python.json";
QString json_cplus =QDir::currentPath() + QDir::separator() + "data" + QDir::separator() +"Json/c++.json";
QString json_mlb =QDir::currentPath() + QDir::separator() + "data" + QDir::separator() +"Json/matlab.json";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_Process{nullptr}
{
    ui->setupUi(this);
    //默认打开python界面
    on_table_clicked();


}
MainWindow::~MainWindow()
{
    delete ui;
}

//写入json文件
void MainWindow::writeJson(QString filePath,ST_STRUCT st)
{

    QJsonObject obj;
    obj.insert("Name", st.name);
    obj.insert("Type", st.type);
    obj.insert("Path", st.path);
    obj.insert("config", st.config);

   // qDebug() << "开始" ;
    //打开文件
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "can't open error!";
        return;
    }

    // 读取文件的全部内容
    QTextStream stream(&file);
    stream.setCodec("UTF-8");		// 设置读取编码是UTF8
    QString str = stream.readAll();
    file.close();
    // QJsonParseError类用于在JSON解析期间报告错误。
    QJsonParseError jsonError;
    // 将json解析为UTF-8编码的json文档，并从中创建一个QJsonDocument。
    // 如果解析成功，返回QJsonDocument对象，否则返回null
    QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toUtf8(), &jsonError);
    // 判断是否解析失败
    if (jsonError.error != QJsonParseError::NoError && !jsonDoc.isNull()) {
        qDebug() << "Json格式错误！" << jsonError.error;
        return;
    }

    QJsonObject rootObj=jsonDoc.object();

    QJsonValue ListValue = rootObj.value("List");
    if (ListValue.type() == QJsonValue::Array) {
        // 转换成数组类型
        QJsonArray ListArray = ListValue.toArray();
        ListArray.append(obj);
        rootObj["List"] = ListArray;
    }


    // 将object设置为本文档的主要对象
    jsonDoc.setObject(rootObj);

    // 重写打开文件，覆盖原有文件，达到删除文件全部内容的效果
    QFile writeFile(filePath);
    if (!writeFile.open(QFile::WriteOnly | QFile::Truncate)) {
        qDebug() << "can't open error!";
        return;
    }

    // 将修改后的内容写入文件
    QTextStream wirteStream(&writeFile);
    wirteStream.setCodec("UTF-8");		// 设置读取编码是UTF8
    wirteStream << jsonDoc.toJson();		// 写入文件
    writeFile.close();					// 关闭文件


}



//python插件按钮
void MainWindow::on_table_clicked()
{
    QString filePath=json_python;
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "can't open error!";
        return;
    }
    // 读取文件的全部内容
    QTextStream stream(&file);
    stream.setCodec("UTF-8");		// 设置读取编码是UTF8
    QString str = stream.readAll();
    file.close();
    // QJsonParseError类用于在JSON解析期间报告错误。
    QJsonParseError jsonError;
    // 将json解析为UTF-8编码的json文档，并从中创建一个QJsonDocument。
    // 如果解析成功，返回QJsonDocument对象，否则返回null
    QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toUtf8(), &jsonError);
    // 判断是否解析失败
    if (jsonError.error != QJsonParseError::NoError && !jsonDoc.isNull()) {
        qDebug() << "Json格式错误！" << jsonError.error;
        return;
    }
    QJsonObject rootObj=jsonDoc.object();
    QJsonValue ListValue = rootObj.value("List");
    QJsonArray ListArray = ListValue.toArray();
    //禁止编辑
    ui->jsontable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //行数和列数
    ui->jsontable->setRowCount(ListArray.size());
    ui->jsontable->setColumnCount(4);
    ui->jsontable->horizontalHeader()->setStyleSheet("color: rgb(0, 83, 128);border:1px solid rgb(210, 210, 210);");
    ui->jsontable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
//    ui->tableWidget->verticalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    ui->jsontable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    for(int i = 0; i < ListArray.size(); i++) {
        QJsonObject obj = ListArray[i].toObject();
        QTableWidgetItem *item1 = new QTableWidgetItem(obj["Name"].toString());
        QTableWidgetItem *item2 = new QTableWidgetItem(obj["Type"].toString());
        QTableWidgetItem *item3 = new QTableWidgetItem(obj["Path"].toString());

        ui->jsontable->setItem(i, 0, item1);
        ui->jsontable->setItem(i, 1, item2);
        ui->jsontable->setItem(i, 2, item3);

        //删除按钮
        LinkButton *del = new LinkButton(i,filePath);
        del->setText("删除");
        connect(del, SIGNAL(getPointer(int, QString)), this, SLOT(delete_clicked(int, QString)));
        //修改按钮
        LinkButton *edit = new LinkButton(i,filePath);
        edit->setText("修改");
        connect(edit, SIGNAL(getPointer(int, QString)), this, SLOT(edit_clicked(int, QString)));
        //启动按钮
        LinkButton *start = new LinkButton(i,filePath);
        start->setText("启动");
        connect(start, SIGNAL(getPointer(int, QString)), this, SLOT(start_clicked(int, QString)));
        QWidget* pWidget = new QWidget(ui->jsontable);
        QHBoxLayout *hLayout = new QHBoxLayout(pWidget); //创建布局
        SetBtnStyle(edit, "220,220,220", "修改");
        SetBtnStyle_2(start, "220,220,220", "启动");
        SetBtnStyle_3(del, "220,220,220", "删除");
        hLayout->addWidget(edit);
        hLayout->addWidget(start);
        hLayout->addWidget(del);
        hLayout->setContentsMargins(0, 0, 0, 0);
        ui->jsontable->setCellWidget(i,3,pWidget);
    }
}

//按钮背景设置
//修改按钮
void MainWindow::SetBtnStyle(QPushButton *pBtn, QString clr, QString btnTxt)//注意qstring clr的格式写法
{
    pBtn->setText(btnTxt);
    QString style = QString("QPushButton{min-height:40px;font-size:26px;color:rgb(255,255,255);background:rgb(255, 165, 79);border-radius:10px;padding:2px 2px;font-family:Microsoft YaHei;}"
    "QPushButton:hover{color:rgb(79,79,79);border-style:solid;background: rgba(%2,0.6);border-radius:10px;font-family:Microsoft YaHei;}"
    "QPushButton:pressed{color:rgb(79,79,79);border-style:solid;background: rgba(%3,0.4);border-radius:10px;font-family:Microsoft YaHei;}").arg(clr).arg(clr).arg(clr);
    pBtn->setStyleSheet(style);
}
//启动按钮
void MainWindow::SetBtnStyle_2(QPushButton *pBtn, QString clr, QString btnTxt)//注意qstring clr的格式写法
{
    pBtn->setText(btnTxt);
    QString style = QString("QPushButton{min-height:40px;font-size:26px;color:rgb(255,255,255);background:rgb(71,209,109);border-radius:10px;padding:2px 2px;font-family:Microsoft YaHei;}"
    "QPushButton:hover{color:rgb(79,79,79);border-style:solid;background: rgba(%2,0.6);border-radius:10px;font-family:Microsoft YaHei;}"
    "QPushButton:pressed{color:rgb(79,79,79);border-style:solid;background: rgba(%3,0.4);border-radius:10px;font-family:Microsoft YaHei;}").arg(clr).arg(clr).arg(clr);
    pBtn->setStyleSheet(style);
}
//删除按钮
void MainWindow::SetBtnStyle_3(QPushButton *pBtn, QString clr, QString btnTxt)//注意qstring clr的格式写法
{
    pBtn->setText(btnTxt);
    QString style = QString("QPushButton{min-height:40px;font-size:26px;color:rgb(255,255,255);background:rgb(237,69,85);border-radius:10px;padding:2px 2px;font-family:Microsoft YaHei;}"
    "QPushButton:hover{color:rgb(79,79,79);border-style:solid;background: rgba(%2,0.6);border-radius:10px;font-family:Microsoft YaHei;}"
    "QPushButton:pressed{color:rgb(79,79,79);border-style:solid;background: rgba(%3,0.4);border-radius:10px;font-family:Microsoft YaHei;}").arg(clr).arg(clr).arg(clr);
    pBtn->setStyleSheet(style);
}

//添加按钮
void MainWindow::on_Add_clicked()
{

    addfileform *configWinow = new addfileform;
    configWinow->setModal(true);
    configWinow->show();
    configWinow->setWindowTitle("添加插件");

    connect(this,SIGNAL(sendFilePath(QString)),configWinow,SLOT(getFilePath(QString)));
    emit sendFilePath(json_python);

    //刷新界面
    connect(configWinow,SIGNAL(ExitWin()),this,SLOT(on_table_clicked()));

}

//删除按钮
void MainWindow::delete_clicked(int i,QString filePath)
{
    //打开文件
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "can't open error!";
        return;
    }

    // 读取文件的全部内容
    QTextStream stream(&file);
    stream.setCodec("UTF-8");		// 设置读取编码是UTF8
    QString str = stream.readAll();
    file.close();
    // QJsonParseError类用于在JSON解析期间报告错误。
    QJsonParseError jsonError;
    // 将json解析为UTF-8编码的json文档，并从中创建一个QJsonDocument。
    // 如果解析成功，返回QJsonDocument对象，否则返回null
    QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toUtf8(), &jsonError);
    // 判断是否解析失败
    if (jsonError.error != QJsonParseError::NoError && !jsonDoc.isNull()) {
        qDebug() << "Json格式错误！" << jsonError.error;
        return;
    }

    //删除对象
    QJsonObject rootObj=jsonDoc.object();
    QJsonValue ListValue = rootObj.value("List");
    if (ListValue.type() == QJsonValue::Array) {
        // 转换成数组类型
        QJsonArray ListArray = ListValue.toArray();
        ListArray.removeAt(i);
        rootObj["List"] = ListArray;
    }

    // 将object设置为本文档的主要对象
    jsonDoc.setObject(rootObj);

    // 重写打开文件，覆盖原有文件，达到删除文件全部内容的效果
    QFile writeFile(filePath);
    if (!writeFile.open(QFile::WriteOnly | QFile::Truncate)) {
        qDebug() << "can't open error!";
        return;
    }

    // 将修改后的内容写入文件
    QTextStream wirteStream(&writeFile);
    wirteStream.setCodec("UTF-8");		// 设置读取编码是UTF8
    wirteStream << jsonDoc.toJson();	// 写入文件
    writeFile.close();					// 关闭文件

    //刷新界面
    if(QString::compare(filePath,json_python) == 0){
         on_table_clicked();
    }
}

//修改按钮
void MainWindow::edit_clicked(int i, QString filePath)
{
    editfileform *configWinow = new editfileform;
    configWinow->show();
    configWinow->setWindowTitle("修改插件");

    connect(this,SIGNAL(sendData(QString,ST_STRUCT,int)),configWinow,SLOT(getData(QString ,ST_STRUCT ,int)));


    //打开文件
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "can't open error!";
        return;
    }

    // 读取文件的全部内容
    QTextStream stream(&file);
    stream.setCodec("UTF-8");		// 设置读取编码是UTF8
    QString str = stream.readAll();
    file.close();
    // QJsonParseError类用于在JSON解析期间报告错误。
    QJsonParseError jsonError;
    // 将json解析为UTF-8编码的json文档，并从中创建一个QJsonDocument。
    // 如果解析成功，返回QJsonDocument对象，否则返回null
    QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toUtf8(), &jsonError);
    // 判断是否解析失败
    if (jsonError.error != QJsonParseError::NoError && !jsonDoc.isNull()) {
        qDebug() << "Json格式错误！" << jsonError.error;
        return;
    }

    QJsonObject rootObj=jsonDoc.object();
    QJsonValue ListValue = rootObj.value("List");
    QJsonArray ListArray = ListValue.toArray();
    QJsonValue ListValueChild = ListArray.at(i);
    QJsonObject ListObj = ListValueChild.toObject();
    ST_STRUCT st;
    st.name = ListObj.value("Name").toString();
    st.path = ListObj.value("Path").toString();
    st.type = ListObj.value("Type").toString();
    st.config = ListObj.value("Config").toString();

    //刷新界面
   emit sendData(filePath, st, i);

    connect(configWinow,SIGNAL(ExitWin()),this,SLOT(on_table_clicked()));
}


//启动按钮
void MainWindow::start_clicked(int i, QString filePath)
{
    //打开文件
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "can't open error!";
        return;
    }

    // 读取文件的全部内容
    QTextStream stream(&file);
    stream.setCodec("UTF-8");		// 设置读取编码是UTF8
    QString str = stream.readAll();
    file.close();
    // QJsonParseError类用于在JSON解析期间报告错误。
    QJsonParseError jsonError;
    // 将json解析为UTF-8编码的json文档，并从中创建一个QJsonDocument。
    // 如果解析成功，返回QJsonDocument对象，否则返回null
    QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toUtf8(), &jsonError);
    // 判断是否解析失败
    if (jsonError.error != QJsonParseError::NoError && !jsonDoc.isNull()) {
        qDebug() << "Json格式错误！" << jsonError.error;
        return;
    }

    QJsonObject rootObj=jsonDoc.object();
    QJsonValue ListValue = rootObj.value("List");
    QJsonArray ListArray = ListValue.toArray();
    QJsonValue ListValueChild = ListArray.at(i);
    QJsonObject ListObj = ListValueChild.toObject();
    ST_STRUCT st;
    st.name = ListObj.value("Name").toString();
    st.path = ListObj.value("Path").toString();
    st.type = ListObj.value("Type").toString();
    st.config = ListObj.value("Config").toString();

    if(nullptr != m_Process)
    {
        m_Process->close();
        delete m_Process;
        m_Process = nullptr;
//        QMessageBox::warning(this,tr("提示"),tr("杀死已存在的进程"));

    }

    if(st.type == ".py")
    {
        cmdstart *configWinow = new cmdstart;
        configWinow->show();
        configWinow->setWindowTitle("修改插件");
        connect(this,SIGNAL(sendData(QString ,int)),configWinow,SLOT(getData(QString ,int)));
        emit sendData(filePath, i);

    }
    else{
        m_Process = new QProcess(this);
        QString fileName = st.path;
        QString command = "cmd";
        QStringList args;
        args.append("/c");
        args.append(st.config.split("\t"));
        //qDebug() << "Arguments:" << args;
        m_Process->start(command,args);

        if(m_Process->waitForStarted(3000))
        {
            //QMessageBox::warning(this,tr("提示"),tr("进程拉起成功！"));
        }
        else
        {
            QMessageBox::warning(this,tr("提示"),tr("错误: 进程拉起失败！！"));
        }
    }
}

//列表筛选
void MainWindow::on_Select_clicked()
{
    QString input_name=ui->lineEdit->text();
        int row_num=ui->jsontable->rowCount();
        if (input_name==""){          //判断是否是空，如果是空就显示所有行
            for(int i=0;i<row_num;i++){
                ui->jsontable->setRowHidden(i,false);//为false就是显示
            }
        }
        else{          //找到符合条件的索引 是通过你输入的和表格里面所有内容进行比对
            QList <QTableWidgetItem *> item = ui->jsontable->findItems(ui->lineEdit->text(), Qt::MatchContains);
            //然后把所有行都隐藏
            for(int i=0;i<row_num;i++){
                ui->jsontable->setRowHidden(i,true);//隐藏
            }
            if(!item.empty()){  //判断符合条件索引是不是空
                for(int i=0;i<item.count();i++){     //恢复对应的行
                    ui->jsontable->setRowHidden(item.at(i)->row(),false);//恢复对应的行，也可以恢复列
                }
           }
     }
}

void MainWindow::on_python_clicked()
{
    QString input_name = ".py";
        int row_num=ui->jsontable->rowCount();
        QList <QTableWidgetItem *> item = ui->jsontable->findItems(".py", Qt::MatchContains);
            //然后把所有行都隐藏
        for(int i=0;i<row_num;i++){
                ui->jsontable->setRowHidden(i,true);//隐藏
            }
        if(!item.empty()){  //判断符合条件索引是不是空
                for(int i=0;i<item.count();i++){     //恢复对应的行
                    ui->jsontable->setRowHidden(item.at(i)->row(),false);//恢复对应的行，也可以恢复列
          }
     }
}
void MainWindow::on_exe_clicked()
{
    QString input_name = ".exe";
        int row_num=ui->jsontable->rowCount();
        QList <QTableWidgetItem *> item = ui->jsontable->findItems(".exe", Qt::MatchContains);
            //然后把所有行都隐藏
        for(int i=0;i<row_num;i++){
                ui->jsontable->setRowHidden(i,true);//隐藏
            }
        if(!item.empty()){  //判断符合条件索引是不是空
                for(int i=0;i<item.count();i++){     //恢复对应的行
                    ui->jsontable->setRowHidden(item.at(i)->row(),false);//恢复对应的行，也可以恢复列
          }
     }
}
void MainWindow::on_mlb_clicked()
{
    QString input_name = ".m";
        int row_num=ui->jsontable->rowCount();
        QList <QTableWidgetItem *> item = ui->jsontable->findItems(".m", Qt::MatchContains);
            //然后把所有行都隐藏
        for(int i=0;i<row_num;i++){
                ui->jsontable->setRowHidden(i,true);//隐藏
            }
        if(!item.empty()){  //判断符合条件索引是不是空
                for(int i=0;i<item.count();i++){     //恢复对应的行
                    ui->jsontable->setRowHidden(item.at(i)->row(),false);//恢复对应的行，也可以恢复列
          }
     }
}
void MainWindow::on_cplus_clicked()
{
    QString input_name = ".exe";
        int row_num=ui->jsontable->rowCount();
        QList <QTableWidgetItem *> item = ui->jsontable->findItems(".exe", Qt::MatchContains);
            //然后把所有行都隐藏
        for(int i=0;i<row_num;i++){
                ui->jsontable->setRowHidden(i,true);//隐藏
            }
        if(!item.empty()){  //判断符合条件索引是不是空
                for(int i=0;i<item.count();i++){     //恢复对应的行
                    ui->jsontable->setRowHidden(item.at(i)->row(),false);//恢复对应的行，也可以恢复列
          }
     }
}
