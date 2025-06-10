#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "client.h"
#include <QtSql/QtSql>

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

    void rechercher();
    void afficherResultat( QSqlQuery query );
    void listerResultat();
    void wantCreerIndice();
    void creerIndice();
    void supprimerIndice();
    void modifierIndice();
    void trierEnvoi(QString &requete);

private slots:
    void on_envoiTrame_clicked();

    void on_versEnvoi_clicked();

    void on_backEnvoi_clicked();

    void on_versGestion_clicked();

    void on_versAide_clicked();

    void on_backGestion_clicked();

    void on_backAide_clicked();

    void on_ajouterIndice_clicked();

    void on_creerIndice_clicked();

    void on_supprimerIndice_clicked();

    void on_reponseTable_cellClicked(int row, int column);

    void on_nouvelIndice_returnPressed();

    void on_trierEnvoiBox_activated(const QString &arg1);

    void on_selectionTriage_textChanged(const QString &arg1);

    void on_reponseTable_cellDoubleClicked(int row, int column);

    void on_resetOption_clicked();

    void on_numeroEtape_textChanged(const QString &arg1);

    void on_modifierIndice_clicked();

    void on_annulerCreer_clicked();

    void on_annulerModifier_clicked();

    void on_modificationIndice_clicked();

private:
    QString m_nom_indice;
    Client m_client;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
