#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iostream"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ajoutVitesseDefilement(QString &trame)
{
    if(ui->vitesseDefilementBox->currentText() == "Lent")
        trame.push_back("<MD><WC><FA>");
    if(ui->vitesseDefilementBox->currentText() == "Normal")
        trame.push_back("<MC><WC><FA>");
    if(ui->vitesseDefilementBox->currentText() == "Rapide")
        trame.push_back("<MB><WC><FA>");
}

void MainWindow::ajoutPolice(QString &trame)
{
    if(ui->policeBox->currentText() == "Petit")
        trame.push_back("<AC>");
    if(ui->policeBox->currentText() == "Normal")
        trame.push_back("<AA>");
    if(ui->policeBox->currentText() == "Gras")
        trame.push_back("<AB>");
}

void MainWindow::ajoutCouleur(QString &trame)
{
    if(ui->couleurBox->currentText() == "Rouge")
        trame.push_back("<CB>");
}


void MainWindow::on_envoiTrame_clicked()
{
    QString trame = "<L1><PA><FE>";

    ajoutVitesseDefilement(trame);
    ajoutPolice(trame);
    ajoutCouleur(trame);

    trame.push_back(ui->texteMessage->text());

    std::cout << trame.toStdString() << std::endl;

    m_client.envoiTexte(trame.toStdString());
    //ui->texteMessage->textEdited("");
}
/*
void MainWindow::rechercher()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("../PROJET_AFFICHEUR_INDICE/indice.db");

    if( ! db.open() )
        std::cout << "La base de données n'a pas été ouverte" << std::endl;
    else
    {
        QSqlQuery query;

        QString requete("SELECT id, nom FROM indice WHERE 1");

        std::cout << requete.toStdString() << std::endl;
        query.prepare( requete );

        if( !query.exec() )
        {
            // Error Handling, check query.lastError(), probably return
        }

        afficherResultat(query);

        db.close();
    }
}

void MainWindow::afficherResultat( QSqlQuery query )
{
    QSqlRecord record = query.record();

    ui->reponseTable->clear(); // on efface toute la table
    ui->reponseTable->setRowCount(0); // on enlève toutes les lignes
    ui->reponseTable->setColumnCount( record.count() ); // on initialise le nombre de colonne de la table de réponse

    // affichage des titres de colonnes
    for ( int colonne = 0; colonne != record.count(); ++colonne )
        ui->reponseTable->setHorizontalHeaderItem(colonne, new QTableWidgetItem(record.fieldName(colonne)) );

    // ajout des lignes
    int ligne = 0;
    while( query.next() )
    {
        ui->reponseTable->insertRow(ligne); // on ajoute une ligne

        // pour chaque colonne, on ajoute la valeur
        for ( int colonne = 0; colonne != record.count(); ++colonne )
        {
            QString valeur = query.value( colonne ).toString(); // récupération de la valeur
            ui->reponseTable->setItem(ligne, colonne, new QTableWidgetItem(valeur) ); // ajout de la valeur dans la table
        }

        ligne++;
    }
}
*/
//------------------------------------------------
// Méthodes des boutons du menu principal

void MainWindow::on_versGestion_clicked()
{
    ui->index->hide();
    ui->gestion->show();
}

void MainWindow::on_versEnvoi_clicked()
{
    ui->index->hide();
    ui->envoi->show();
}

void MainWindow::on_versAide_clicked()
{
    ui->index->hide();
    ui->aide->show();
}

//------------------------------------------------
// Méthodes des boutons de retour en arrière

void MainWindow::on_backGestion_clicked()
{
    ui->gestion->hide();
    ui->index->show();
}

void MainWindow::on_backEnvoi_clicked()
{
    ui->envoi->hide();
    ui->index->show();
}

void MainWindow::on_backAide_clicked()
{
    ui->aide->hide();
    ui->index->show();
}
