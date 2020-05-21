
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
  QApplication app(argc, argv);
  MainWindow mainWin;
  mainWin.resize(800, 600);
  mainWin.show();
  return app.exec();
}
