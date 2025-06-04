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

    //void rechercher();
    //void afficherResultat( QSqlQuery query );

private slots:
    void on_envoiTrame_clicked();

    void on_versEnvoi_clicked();
    void on_backEnvoi_clicked();

    void on_versGestion_clicked();

    void on_versAide_clicked();

    void on_backGestion_clicked();

    void on_backAide_clicked();

private:
    Client m_client;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
