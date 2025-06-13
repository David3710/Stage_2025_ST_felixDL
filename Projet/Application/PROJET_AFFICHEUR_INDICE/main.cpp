#include "mainwindow.h"
#include "client.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // gestion de la feuille de style
    QFile File("../PROJET_AFFICHEUR_INDICE/stylesheet.qss");
    File.open(QFile::ReadOnly);
    QString StyleSheet = QLatin1String(File.readAll());

    QApplication a(argc, argv);

    a.setStyleSheet(StyleSheet);

    MainWindow w;
    w.show();
    return a.exec();
}
