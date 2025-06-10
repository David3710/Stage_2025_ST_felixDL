#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iostream"
#include "windows.h"
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    rechercher();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//------------------------------------------------
// Personnalisation et envoi de la trame à l'afficheur

void MainWindow::ajoutVitesseDefilement(QString &trame) // Ajoute à la trame la balise de défilement
{
    if(ui->vitesseDefilementBox->currentText() == "Lent")
        trame.push_back("<Mq><WC><FA>");
    if(ui->vitesseDefilementBox->currentText() == "Normal")
        trame.push_back("<Ma><WC><FA>");
    if(ui->vitesseDefilementBox->currentText() == "Rapide")
        trame.push_back("<MQ><WC><FA>");
}

void MainWindow::ajoutPolice(QString &trame) // Ajoute à la trame la balise de police d'écriture
{
    if(ui->policeBox->currentText() == "Petit")
        trame.push_back("<AC>");
    if(ui->policeBox->currentText() == "Normal")
        trame.push_back("<AA>");
    if(ui->policeBox->currentText() == "Gras")
        trame.push_back("<AB>");
}

void MainWindow::ajoutCouleur(QString &trame) // Ajoute à la trame la balise de couleur
{
    if(ui->couleurBox->currentText() == "Rouge")
        trame.push_back("<CB>");
    if(ui->couleurBox->currentText() == "Jaune")
        trame.push_back("<CJ>");
    if(ui->couleurBox->currentText() == "Vert")
        trame.push_back("<CE>");
    if(ui->couleurBox->currentText() == "Orange")
        trame.push_back("<CH>");
    if(ui->couleurBox->currentText() == "Tricolor")
        trame.push_back("<CR>");
    if(ui->couleurBox->currentText() == "Multicolor")
        trame.push_back("<CS>");
}


void MainWindow::on_envoiTrame_clicked() // Envoi la trame à la variable client qui s'occupe de l'envoi à l'afficheur (convertisseur)
{
    QString trame = "<L1><PA><FE>";

    ajoutVitesseDefilement(trame);
    ajoutPolice(trame);
    ajoutCouleur(trame);

    trame.push_back(ui->indiceBox->currentText());

    std::cout << trame.toStdString() << std::endl;

    m_client.envoiTexte(trame.toStdString());
}

//------------------------------------------------
// Méthodes qui gèrent la BDD

void MainWindow::rechercher() // Cherche et ouvre la BDD
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("../PROJET_AFFICHEUR_INDICE/indice.db");

    if( ! db.open() )
        std::cout << "La base de données n'a pas été ouverte" << std::endl;
    else
    {
        QSqlQuery query;

        QString requete("SELECT nom, etape FROM indice ");

        if(ui->trierEnvoiBox->currentIndex() != 0){
            trierEnvoi(requete);
        }
        else {
            ui->selectionTriage->hide();
            ui->numeroEtape->hide();
            requete.push_back("WHERE 1");
        }

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

void MainWindow::afficherResultat( QSqlQuery query ) // Affiche la BDD sur une table de l'affichage graphique
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
            if(valeur.toInt() % 2 == 0 || valeur == 0) // Seul les noms des indices sont ajoutés à la boîte déroulante
                ui->indiceBox->addItem(valeur);
        }
        ui->reponseTable->horizontalHeader()->setOffset(10);
        ligne++;
    }
}

void MainWindow::listerResultat() // Enregistre la BDD dans une variable pour l'afficher dans une boîte déroulante dans l'écran d'envoi d'indice
{

    ui->indiceBox->clear();
    rechercher();

}

void MainWindow::creerIndice() // Permet de créer un indice en choisissant son nom
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("../PROJET_AFFICHEUR_INDICE/indice.db");

    if( ! db.open() )
        std::cout << "La base de données n'a pas été ouverte" << std::endl;
    else
    {
        QSqlQuery query;

        QString requete("INSERT INTO indice (nom, etape) VALUES ('");

        requete.push_back(ui->nouvelIndice->text());
        requete.push_back("', '");
        requete.push_back(ui->etapeNouvelIndice->text());
        requete.push_back("')");

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

void MainWindow::supprimerIndice() // Permet de supprimer un indice en le sélectionnant dans la table
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("../PROJET_AFFICHEUR_INDICE/indice.db");

    if( ! db.open() )
        std::cout << "La base de données n'a pas été ouverte" << std::endl;
    else
    {
        QSqlQuery query;

        QString requete("DELETE FROM indice WHERE nom = \"");

        std::cout << m_nom_indice.toStdString() << std::endl;

        requete.push_back(ui->reponseTable->currentItem()->text());
        requete.push_back("\"");

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

void MainWindow::modifierIndice()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("../PROJET_AFFICHEUR_INDICE/indice.db");

    if( ! db.open() )
        std::cout << "La base de données n'a pas été ouverte" << std::endl;
    else
    {
        QSqlQuery query;

        QString requete("UPDATE indice SET nom = \"");
        requete.push_back(ui->indiceModifiable->text());
        requete.push_back("\", etape = '");
        requete.push_back(ui->etapeModifiable->text());
        requete.push_back("' ");

        requete.push_back("WHERE nom = \"");
        requete.push_back(ui->reponseTable->item(ui->reponseTable->currentRow(), 0)->text());
        requete.push_back("\"");

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

void MainWindow::trierEnvoi(QString &requete) // Permet le tri des indices listés dans la boîte déroulante dans l'écran d'envoi d'indice
{

    if(ui->trierEnvoiBox->currentIndex() == 1) { // Par ordre alphabétique
        ui->selectionTriage->hide();
        ui->numeroEtape->hide();
        requete.push_back("ORDER BY lower(nom)");
    }
    if(ui->trierEnvoiBox->currentIndex() == 2) { // Contient un mot
        ui->selectionTriage->show();
        ui->numeroEtape->hide();
        requete.push_back("WHERE nom LIKE \"%");
        requete.push_back(ui->selectionTriage->text());
        requete.push_back("%\"");
    }
    if(ui->trierEnvoiBox->currentIndex() == 3) { // Commençant par...
        ui->selectionTriage->show();
        ui->numeroEtape->hide();
        requete.push_back("WHERE nom LIKE \"");
        requete.push_back(ui->selectionTriage->text());
        requete.push_back("%\"");
    }
    if(ui->trierEnvoiBox->currentIndex() == 4) { // Utilisé à l'étape...
        ui->selectionTriage->hide();
        ui->numeroEtape->show();
        requete.push_back("WHERE etape = '");
        requete.push_back(ui->numeroEtape->text());
        requete.push_back("'");
    }



}

//------------------------------------------------
// Méthodes des boutons du menu principal

void MainWindow::on_versGestion_clicked() // Cliquer sur le bouton "Gérer les indices"
{
    ui->index->hide();
    ui->gestion->show();
    ui->advertisementSuppression->hide();
    ui->advertisementModification->hide();
}

void MainWindow::on_versEnvoi_clicked() // Cliquer sur le bouton "Envoyer un indice"
{
    ui->index->hide();
    remettreDefaut();
    ui->envoi->show();
    listerResultat();
}

void MainWindow::on_versAide_clicked() // Cliquer sur le bouton "Aide"
{
    ui->index->hide();
    ui->aide->show();
}

//------------------------------------------------
// Méthodes des boutons de retour en arrière

void MainWindow::on_backGestion_clicked() // Cliquer sur le bouton retour en arrière de l'écran de gestion
{
    ui->gestion->hide();
    ui->index->show();
}

void MainWindow::on_backEnvoi_clicked() // Cliquer sur le bouton retour en arrière de l'écran d'envoi
{
    ui->envoi->hide();
    ui->index->show();
}

void MainWindow::on_backAide_clicked() // Cliquer sur le bouton retour en arrière de l'écran d'aide
{
    ui->aide->hide();
    ui->index->show();
}

//------------------------------------------------
// Méthodes gérants l'ajout d'indices à la BDD

void MainWindow::on_creerIndice_clicked()
{
    ui->gestion->hide();
    ui->creer->show();
}

void MainWindow::wantCreerIndice() {
    creerIndice();
    ui->creer->hide();
    ui->nouvelIndice->setText("");
    ui->gestion->show();
    rechercher();
}

void MainWindow::on_ajouterIndice_clicked()
{
    wantCreerIndice();
}

void MainWindow::on_nouvelIndice_returnPressed()
{
    wantCreerIndice();
}

void MainWindow::on_annulerCreer_clicked()
{
    ui->creer->hide();
    ui->gestion->show();
}

//------------------------------------------------
// Méthodes gérants la suppression d'un indice

void MainWindow::on_supprimerIndice_clicked()
{

    if(ui->reponseTable->currentItem() != NULL) {
        supprimerIndice();
        rechercher();
    }
    else {
        ui->advertisementModification->hide();
        ui->advertisementSuppression->show();
    }
}

//------------------------------------------------
// Méthodes gérants la modification d'un indice

void MainWindow::on_modifierIndice_clicked()
{

    if(ui->reponseTable->currentItem() != NULL) {
        ui->gestion->hide();
        ui->modifier->show();

        ui->indiceModifiable->setText(ui->reponseTable->item(ui->reponseTable->currentRow(), 0)->text());
        ui->etapeModifiable->setText(ui->reponseTable->item(ui->reponseTable->currentRow(), 1)->text());
    }
    else {
        ui->advertisementSuppression->hide();
        ui->advertisementModification->show();
    }
}

void MainWindow::on_modificationIndice_clicked()
{
    modifierIndice();
    rechercher();

    if ( ui->etapeModifiable->text().data()->isDigit() != true ) {

    }

    ui->modifier->hide();
    ui->gestion->show();
}

void MainWindow::on_annulerModifier_clicked()
{
    ui->modifier->hide();
    ui->gestion->show();
}

//------------------------------------------------
// Méthode de la table d'affichage

void MainWindow::on_reponseTable_cellClicked(int row, int column)
{
    std::cout << row << std::endl;
    std::cout << column << std::endl;
}

void MainWindow::on_reponseTable_cellDoubleClicked(int row, int column)
{
    rechercher();
}

void MainWindow::on_selectionTriage_textChanged(const QString &arg1)
{
    listerResultat();
}


void MainWindow::on_trierEnvoiBox_activated(const QString &arg1)
{
    listerResultat();
}

void MainWindow::remettreDefaut() {
    ui->trierEnvoiBox->setCurrentIndex(0);
    ui->selectionTriage->hide();
    ui->numeroEtape->hide();
    listerResultat();
    ui->vitesseDefilementBox->setCurrentIndex(1);
    ui->policeBox->setCurrentIndex(1);
    ui->couleurBox->setCurrentIndex(0);
}

void MainWindow::on_resetOption_clicked()
{
    remettreDefaut();
}

void MainWindow::on_numeroEtape_textChanged(const QString &arg1)
{
    listerResultat();
}
