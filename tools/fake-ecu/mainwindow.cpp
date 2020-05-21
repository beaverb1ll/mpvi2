#include "mainwindow.h"

#include <QTableWidget>
#include <QHeaderView>

#include "obd2/obd2.hpp"


MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
  setObjectName("MainWindow");
  setWindowTitle("Fake OBD2 ECU");
  tableWidget_ = new QTableWidget(Obd2::kNumService01Pids, 3, this);
  tableWidget_->verticalHeader()->setVisible(false);
  //tableWidget_->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableWidget_->setSelectionMode(QAbstractItemView::SingleSelection);

  tableHeader_<<"Enable"<<"Name"<<"Value";
  tableWidget_->setHorizontalHeaderLabels(tableHeader_);

  for(int i = 0; i < Obd2::kNumService01Pids; i++) {
    tableWidget_->setItem(i, 0, new QTableWidgetItem());
    tableWidget_->item(i, 0)->setCheckState(Qt::Checked);

    tableWidget_->setItem(i, 1, new QTableWidgetItem(Obd2::service_01_to_string(i).c_str()));
    if(i == 0) {
      tableWidget_->item(i, 0)->setFlags(0);
    }
  }
  setCentralWidget(tableWidget_);
}

MainWindow::~MainWindow()
{
}
