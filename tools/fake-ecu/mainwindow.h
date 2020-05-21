#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_FORWARD_DECLARE_CLASS(QTableWidget)

class MainWindow : public QMainWindow {
    Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr,
                        Qt::WindowFlags flags = 0);

  ~MainWindow();

 public slots:

 private:
  QTableWidget *tableWidget_=nullptr;
  QStringList tableHeader_;

};

#endif // MAINWINDOW_H
