#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "client.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void ajoutVitesseDefilement(QString &trame);
    void ajoutPolice(QString &trame);
    void ajoutCouleur(QString &trame);

private slots:
    void on_envoiTrame_clicked();

private:
    Client m_client;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
