#include "widget.h"
#include "ui_widget.h"

#include<QFileDialog>
#include<QMessageBox>
#include<QDebug>
#include<QFileInfo>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //初始化
    ctrlInit();
    process = new QProcess(this);
    process->setProcessChannelMode(QProcess::MergedChannels);

    //连接信号和动作
    /*命令行处理结束*/
    connect(process,SIGNAL(readyReadStandardOutput()),this,SLOT(readCmd()));

    /*输入浏览按下*/
    connect(ui->PBT_input, &QPushButton::clicked,[=](){
        QFileInfo fileinfo;
        QString fileNameIn = QFileDialog::getOpenFileName(this, tr("图片文件"),
                "/home", tr("Images (*.png *.bmp *.jpg)") );
        ui->textEdit_in->setText(fileNameIn);
        fileinfo = QFileInfo(fileNameIn);
        ui->textEdit_out->setText(fileinfo.absolutePath() + "/HD_" + fileinfo.baseName() + ".png");
    });

    /*输出浏览按下*/
    connect(ui->PBT_output, &QPushButton::clicked,[=](){
        QString fileNameOut = QFileDialog::getExistingDirectory(this, tr("打开文件夹"),
                "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        QString fileNameIn = ui->textEdit_in->toPlainText();
        if(fileNameIn != "")
        {
            QFileInfo fileinfo = QFileInfo(fileNameIn);
            ui->textEdit_out->setText(fileNameOut + "/HD_" + fileinfo.baseName() + ".png" );
        }
        else if(fileNameIn == "")
        {
            ui->textEdit_out->setText(fileNameOut + "/HD_Pic.png");
        }

    });

    /*单选框按钮按下*/
    connect(ui->radioButton, &QRadioButton::clicked, [=](){
        ui->comboBox->clear();
        ui->comboBox->addItem("realesr-animevideov3-x2");
    });
    connect(ui->radioButton_2, &QRadioButton::clicked, [=](){
        ui->comboBox->clear();
        ui->comboBox->addItem("realesr-animevideov3-x3");
    });
    connect(ui->radioButton_3, &QRadioButton::clicked, [=](){
        ui->comboBox->clear();
        ui->comboBox->addItems(QStringList()<<"realesr-animevideov3-x4"
                                            <<"realesrgan-x4plus"
                                            <<"realesrgan-x4plus-anime");
    });

    /*开始处理按钮按下*/
    connect(ui->PBT_start, &QPushButton::clicked,[&](){
        QString scale = "4";
        if (true == ui->radioButton->isChecked())
        {
            scale = "2";
        }
        else if (true == ui->radioButton_2->isChecked())
        {
            scale = "3";
        }
        else if (true == ui->radioButton_3->isChecked())
        {
            scale = "4";
        }

        //获取文件路径
        QString fileNameIn, fileNameOut;
        currentPath = QCoreApplication::applicationDirPath();
        fileNameIn = ui->textEdit_in->toPlainText();
        fileNameOut = ui->textEdit_out->toPlainText();

        //输入判断，不正确时程序提示
        if(fileNameIn == "" || fileNameOut == "")
        {
            QMessageBox::warning(this,"警告","请选择输入文件和输出路径");
            return ;
        }
        QString modelName = ui->comboBox->currentText();

        //命令生成
        QString order =  currentPath + "/ESRGAN/" +
                            QString("realesrgan-ncnn-vulkan.exe")
                            + " -i " + fileNameIn      //输入文件路径
                            + " -o " + fileNameOut     //输出文件路径
                            + " -s " + scale           //放大倍率
                            + " -n " + modelName       //模型名字
                            +"\r\n";

        //命令执行
        process->start("cmd.exe");
        //order = "ping 192.168.1.1\r\n";
        QByteArray qbarr = order.toLatin1();
        char *ch = qbarr.data();
        qint64 len = order.length();
        qDebug()<<order;
        order.replace("/","\\");
        process->write(ch, len);
    });

}
Widget::~Widget()
{
    delete ui;
    delete process;
}

//控件初始化
void Widget::ctrlInit()
{
    ui->radioButton_3->setChecked(true);
    ui->comboBox->addItems(QStringList()<<"realesr-animevideov3-x4"
                                        <<"realesrgan-x4plus"
                                        <<"realesrgan-x4plus-anime");
    ui->progressBar->setValue(0);
}

//读取命令行消息
void Widget::readCmd()
{
        QByteArray bytes = process->readAll();
        /* 显示 */
        QString msg = QString::fromLocal8Bit(bytes);
        int len = msg.length();
        int enLen = currentPath.length();
        if (msg.length()<10)
        {
            ui->progressBar->setValue(msg.replace("%\r\n"," ").toDouble());
        }
        else if (len < (enLen + 10) && len > (enLen -10 ))
        {
            ui->progressBar->setValue(100);
            QMessageBox::information(this,"提示","修复成功");
        }

}

