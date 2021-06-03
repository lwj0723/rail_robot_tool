#include "GcRailRobot.h"
#include <QMessageBox>

GcRailRobot::GcRailRobot(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	init_conn();
	//
	init();
}

GcRailRobot::~GcRailRobot()
{
	if (m_socket) delete m_socket;
}

void GcRailRobot::init_conn()
{
	m_socket = new QTcpSocket;
	connect(m_socket, &QTcpSocket::connected, this, [this]() {
		auto ip = ui.lineEdit_ip->text();
		ui.textEdit_recv->append(QString::fromLocal8Bit("IP:%1 连接成功!!!")
			.arg(ip));
		//
		ui.common_widget->setEnabled(true);
		m_bConnected = true;
	});
	connect(m_socket, &QTcpSocket::readyRead, this, [this]() {
		auto bytes = m_socket->readAll();
		recvData(bytes);
	});
	connect(m_socket, &QTcpSocket::disconnected, [this]() {
		ui.textEdit_recv->append(QString::fromLocal8Bit("连接断开!!!"));

		m_bConnected = false;
	});
	connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
		this, [this]() {
		QTcpSocket::SocketError err = m_socket->error();
		ui.textEdit_recv->append(QString::fromLocal8Bit("tcp err: %1").arg(m_socket->errorString()));
	});

	//business
	connect(ui.conn_btn, &QPushButton::clicked, this, [this]() {
		if (ui.lineEdit_ip->text().isEmpty() || 
			ui.lineEdit_port->text().isEmpty()) {
			QMessageBox box;
			box.setText(QString::fromLocal8Bit("请输入ip or 端口!!!"));
			return box.exec();
		}

		QString ip = ui.lineEdit_ip->text();
		QString port = ui.lineEdit_port->text();
		m_socket->connectToHost(ip, port.toInt());
		debug_output(QString::fromLocal8Bit(" connect ip:%1 port:%2").arg(ip).arg(port));
	});
	connect(ui.send_mode_btn, &QPushButton::clicked, this, [this]() {
		//m_bConnected = true;
		if (m_bConnected) {
			int index = ui.comboBox_mode->currentIndex();
			auto mode = ui.comboBox_mode->itemData(index).toInt();
			QByteArray bytes = creayeBytes(mode, OperationEnum::Mode_operator);
			int len = m_socket->write(bytes);
			debug_output(QString::fromLocal8Bit("send mode:%1 bytes:%2 len:%3")
			.arg(mode)
			.arg(QString(bytes))
			.arg(len));
		}
	});
	connect(ui.send_speed_btn, &QPushButton::clicked, this, [this]() {
		if (m_bConnected) {
			auto speed = ui.lineEdit->text().toInt();
			QByteArray bytes = creayeBytes(speed, OperationEnum::speed_operator);
			int len =  m_socket->write(bytes);
			debug_output(QString::fromLocal8Bit("send speed:%1 bytes:%2 len:%3")
				.arg(speed)
				.arg(QString(bytes))
				.arg(len));
		}
	});
}

void GcRailRobot::init()
{
	//combox;
	for (const auto &ele : m_modeCtl)
	{
		ui.comboBox_mode->addItem(ele.first, ele.second);
	}

	ui.common_widget->setEnabled(false);
}

QByteArray GcRailRobot::creayeBytes(QVariant data, OperationEnum type)
{
	QByteArray ba;
	ba.resize(12);//12 byte
	int order = 0;
	if (type == OperationEnum::Mode_operator) {
		order = 41;
	}
	else {
		order = 40;
	}

	int val = data.toInt();
	//
	static int n = 0; n++;
	if (n >= 255) {
		n = 0;
	}
	ba[0] = n;
	ba[1] = 0x0;
	ba[2] = 0x0;
	ba[3] = 0x0;
	ba[4] = 0x0;

	ba[5] = 6;
	ba[6] = 0x01;
	ba[7] = 0x06;

	ba[8] = order/256;
	ba[9] = order % 256;
	ba[10] = val / 256;
	ba[11] = val % 256;
	//
	return ba;
}

void GcRailRobot::recvData(QByteArray &data)
{
	ui.textEdit_recv->append(QString::fromLocal8Bit("recv data bytes: %1")
		.arg(QString(data)));
}

void GcRailRobot::debug_output(QString &text)
{
	ui.textEdit_debug->append(text);
}


