#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <mutex>
#include <thread>
#include <QMainWindow>

#include "ecu_obd2.hpp"
#include "socket_can.hpp"

QT_FORWARD_DECLARE_CLASS(QTableWidget)

class MainWindow : public QMainWindow {
    Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr,
                        Qt::WindowFlags flags = 0);

  ~MainWindow();

 public slots:
  void cellChanged(int nRow, int nCol);

 private:
  enum Rows {
    kRowEnable = 0,
    kRowPid,
    kRowName,
    kRowValue,
    kNumRows
  };
  void update_value(int nRow, int nCol);

  std::atomic<bool> keep_running_{true};
  std::unique_ptr<SocketCan> can_;
  std::unique_ptr<EcuObd2> ecu_;
  std::mutex mutex_;
  std::thread thread_;

  QTableWidget *tableWidget_=nullptr;
  QStringList tableHeader_;

};

#endif // MAINWINDOW_H
