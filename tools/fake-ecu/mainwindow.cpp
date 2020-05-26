#include "mainwindow.h"

#include <thread>

#include <QHeaderView>
#include <QItemDelegate>
#include <QLineEdit>
#include <QTableWidget>

#include "obd2/obd2.hpp"

class Delegate : public QItemDelegate {
  public:
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option,
        const QModelIndex &index) const {
      QLineEdit *lineEdit = new QLineEdit(parent);
      QDoubleValidator *valid = new QDoubleValidator(lineEdit);
      lineEdit->setValidator(valid);
      return lineEdit;
    }
};


MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
  setObjectName("MainWindow");
  setWindowTitle("Fake OBD2 ECU");
  tableWidget_ = new QTableWidget(Obd2::kNumService01Pids, kNumRows, this);
  tableWidget_->verticalHeader()->setVisible(false);
  tableWidget_->setSelectionMode(QAbstractItemView::SingleSelection);
  tableWidget_->setItemDelegate(new Delegate);

  tableHeader_<<"Enable"<<"PID"<<"Name"<<"Value";
  tableWidget_->setHorizontalHeaderLabels(tableHeader_);
  tableWidget_->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  tableWidget_->horizontalHeader()->setStretchLastSection(true);

  can_ = std::make_unique<SocketCan>("can0"); // TODO: Make this configurable
  ecu_ = std::make_unique<EcuObd2>(Obd2::kEcuStartAddress);

  QTableWidgetItem *item;
  char buffer[100];
  for(int i = 0; i < Obd2::kNumService01Pids; i++) {
    item = new QTableWidgetItem();
    item->setCheckState(Qt::Unchecked);
    tableWidget_->setItem(i, kRowEnable, item);

    item = new QTableWidgetItem(Obd2::service_01_to_string(i).c_str());
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    tableWidget_->setItem(i, kRowName, item);

    snprintf(buffer, sizeof(buffer), "0x%02X", i);
    item = new QTableWidgetItem(buffer);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    tableWidget_->setItem(i, kRowPid, item);

    ecu_->set_pid_value(Obd2::kService01CurrentData, i, 0.0);
    snprintf(buffer, sizeof(buffer), "%lf", 0.0);
    item = new QTableWidgetItem(buffer);
    tableWidget_->setItem(i, kRowValue, item);

    if(i == Obd2::kPidSupported01To20) {
      item = tableWidget_->item(i, kRowEnable);
      item->setFlags(0);
      item->setCheckState(Qt::Checked);
    }
    if(i == Obd2::kPidSupported01To20 ||
       i == Obd2::kPidSupported21To40 ||
       i == Obd2::kPidSupported41To60 ||
       i == Obd2::kPidSupported61To80) {
      item = tableWidget_->item(i, kRowValue);
      item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    }
  }
  connect(tableWidget_, SIGNAL(cellChanged (int, int)),
                  this, SLOT(cellChanged( int, int)));
  setCentralWidget(tableWidget_);

  thread_ = std::thread([this](){
    ecu_->set_send_function([&](const CanMsg &msg){
      LOG_DEBUG(*UtilManager::logger(), "Tx: %s", msg.to_string().c_str());
      can_->write_can(msg);
    });

    CanMsg msg;
    while(keep_running_) {
      if(!can_->read_can(msg, std::chrono::milliseconds(800))) {
        continue;
      }

      std::unique_lock<std::mutex> lk(mutex_);
      if(!ecu_->process_rx_packet(msg)) {
        LOG_DEBUG(*UtilManager::logger(), "ECU uninteresting packet: %s", msg.to_string().c_str());
      }
    }
  });
}

MainWindow::~MainWindow() {
  keep_running_ = false;
  if(thread_.joinable()) {
    thread_.join();
  }
}

void MainWindow::cellChanged(int nRow, int nCol) {
  printf("Cell changed: %d %d\n", nRow, nCol);
  std::lock_guard<std::mutex> lk(mutex_);
  switch(nCol) {
    case kRowEnable:
      // TODO: Update supportedPid row
      ecu_->set_pid_supported(Obd2::kService01CurrentData, nRow, tableWidget_->item(nRow, nCol)->checkState());
      break;
    case kRowName:
      // fallthrough
    case kRowPid:
      // this is a read only field
      break;
    case kRowValue:
      update_value(nRow, nCol);
      break;
    default:
      printf("Unknown column changed. %d\n", nCol);
  }
}

void MainWindow::update_value(int nRow, int nCol) {
  QTableWidgetItem *item = tableWidget_->item(nRow, nCol);
  const std::string val = item->text().toStdString();
  switch(nRow) {
    case Obd2::kPidVin:
      printf("New VIN: %s\n", val.c_str());
      ecu_->set_vin(val);
      break;
    default:
      printf("NEW VAL: %s\n", val.c_str());
      ecu_->set_pid_value(Obd2::kService01CurrentData, nRow, std::stod(val));
      break;
  }
}
