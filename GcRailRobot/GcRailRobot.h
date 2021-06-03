#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_GcRailRobot.h"
#include <QtNetwork/QTcpSocket>
#include <map>
using namespace std;

class GcRailRobot : public QMainWindow
{
    Q_OBJECT

	typedef enum OperationEnum
	{
		Mode_operator = 0,
		speed_operator,
	};

public:
    GcRailRobot(QWidget *parent = Q_NULLPTR);
	~GcRailRobot();

	QByteArray creayeBytes(QVariant data, OperationEnum type);

private:
	void init();
	void init_conn();
	void debug_output(QString &text);

public Q_SLOTS:
	void recvData(QByteArray &data);

private:
    Ui::GcRailRobotClass ui;
	QTcpSocket *m_socket;
	const std::map<QString, int> m_modeCtl{
		{ QString::fromLocal8Bit("����"), 0 },
		{ QString::fromLocal8Bit("�ֶ�"), 1 },
		{ QString::fromLocal8Bit("�Զ�"), 2 },
		{ QString::fromLocal8Bit("һ������"), 3 },
	};

	bool m_bConnected = false;
};
