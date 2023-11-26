#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Adding title for widget
    QWidget::setWindowTitle("Serial Port Connection");

    //Set font colors
    QPalette sample_palette;
    sample_palette.setColor(QPalette::WindowText, Qt::red);

    ui->SendMessageLabel->setPalette(sample_palette);
    ui->ReceivedMessageLabel->setPalette(sample_palette);
    ui->StatusLabel->setPalette(sample_palette);
    ui->SettingsLabel->setPalette(sample_palette);
    ui->ControlLabel->setPalette(sample_palette);
    ui->PortLabel->setPalette(sample_palette);
    ui->BudrateLabel->setPalette(sample_palette);

    //Set background
    QPixmap bkgnd("/home/mati/Desktop/QtProjects/HelloUart/Graphics/Background2.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, bkgnd);
    this->setPalette(palette);

    //Create ports list
    QList<QSerialPortInfo> ports = Info.availablePorts();
    QList<QString> StringPorts;
    for(int i = 0 ; i < ports.size() ; i++){
        StringPorts.append(ports.at(i).portName());
    }
    ui->PortsList->addItems(StringPorts);

    //Creat BaudRate list
    QList<qint32> BaudRates = Info.standardBaudRates();
    QList<QString> StringBaudRates;
    for(int i = 0 ; i < BaudRates.size() ; i++){
        StringBaudRates.append(QString::number(BaudRates.at(i)));
    }
    ui->BaudRateList->addItems(StringBaudRates);
    //Set default value of baudrate
    ui->BaudRateList->setCurrentText("115200");


    //Set fixed settings to serialport object

    //DataBits
    SerialPort.setDataBits(QSerialPort::Data8);
    //StopBits
    SerialPort.setStopBits(QSerialPort::OneStop);
    //Parity
    SerialPort.setParity(QSerialPort::NoParity);
    //FLow control
    SerialPort.setFlowControl(QSerialPort::NoFlowControl);

    //Clean recived buffer
    ReceivedBuffer.clear();

    connect(&SerialPort,SIGNAL(readyRead()),this,SLOT(ReceiveMessage()));
}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::ReceiveMessage()
{
    //Debug messages
    qDebug() << "ReciveMessage slot started: " << Qt::endl;
    //qDebug() << "Bytes available: " <<SerialPort.bytesAvailable() << Qt::endl;

    while(SerialPort.bytesAvailable() > 0)   
    {
        ReceivedCharacter = SerialPort.read(1);

        if(ReceivedCharacter == code)
        {
            qDebug() << "Received message: " << ReceivedBuffer << Qt::endl;
            //Clean recived buffer
            ReceivedBuffer.clear();
            SerialPort.clear(QSerialPort::AllDirections);
        }
        else
        {
            ReceivedBuffer.push_back(ReceivedCharacter);
        }

    }

}


//Send comment
void MainWindow::on_SendPushButton_clicked()
{
    QString MessageToSend = ui->InputMessage->text(); //Read message to write
    SerialPort.write(MessageToSend.toUtf8()); //Send message
}

//Disconnet button
void MainWindow::on_DisconnectPushButton_clicked()
{
    SerialPort.close();
}

//Connect Button
void MainWindow::on_ConnectPushButton_clicked()
{
    //Set connection port
    QString PortName = ui->PortsList->currentText();
    SerialPort.setPortName(PortName);

    //Open connection port
    SerialPort.open(QIODevice::ReadWrite);

    if(!SerialPort.isOpen()){
        ui->StatusMessageOutput->setTextColor(Qt::red);
        ui->StatusMessageOutput->append("Error: Cannot open port");
    }
    else {

        ui->StatusMessageOutput->setTextColor(Qt::green);
        ui->StatusMessageOutput->append("Message: Connection success");

        //Set BaudRate
        QString StringBaudRate = ui->BaudRateList->currentText();
        int IntBaudRate = StringBaudRate.toInt();
        SerialPort.setBaudRate(IntBaudRate);

        //Set fixed settings to serialport object

        //DataBits
        SerialPort.setDataBits(QSerialPort::Data8);
        //StopBits
        SerialPort.setStopBits(QSerialPort::OneStop);
        //Parity
        SerialPort.setParity(QSerialPort::NoParity);
        //FLow control
        SerialPort.setFlowControl(QSerialPort::NoFlowControl);

        //Connet to specified port
        connect(&SerialPort,SIGNAL(readyRead()),this,SLOT(ReceiveMessage()));

    }
}

void MainWindow::on_RefreshPushButton_clicked()
{
    //Create ports list
    QList<QSerialPortInfo> ports = Info.availablePorts();
    QList<QString> StringPorts;
    for(int i = 0 ; i < ports.size() ; i++){
        StringPorts.append(ports.at(i).portName());
    }
    ui->PortsList->addItems(StringPorts);
}

void MainWindow::on_ClearPushButton_clicked()
{
    ui->ReceivedMessageOutput->clear();
}
