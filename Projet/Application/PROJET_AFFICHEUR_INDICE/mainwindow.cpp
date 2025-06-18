#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iostream"
#include "windows.h"
#include <shellapi.h>
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

        QString requete("SELECT nom as \"Nom de l'indice\", etape as \"Étape N°\" FROM indice ");

        if(ui->trierEnvoiBox->currentIndex() != 0 || (ui->gestionRecherche->text().isEmpty() == false)){
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

        std::cout << ui->reponseTable->size().width() << std::endl;
        std::cout << ui->reponseTable->columnWidth(1) << std::endl;
        std::cout << ui->reponseTable->verticalHeader()->width() << std::endl;

        //Change la taille des colonnes (bug? obligé de double recherche car la taille horizontal ne s'actualise pas directement)

        ui->reponseTable->setColumnWidth(0, ui->reponseTable->size().width() - 17 - ui->reponseTable->columnWidth(1));

        db.close();
    }
}

void MainWindow::afficherResultat( QSqlQuery query ) // Affiche la BDD sur une table et un volet déroulant de l'affichage graphique
{
    QSqlRecord record = query.record();

    ui->indiceBox->clear();
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
            if( colonne == 0 && ui->stackedWidget->currentIndex() == 2) // Seul les noms des indices sont ajoutés à la boîte déroulante (à fixer)
                ui->indiceBox->addItem(valeur);
        }

        ligne++;
    }

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

        QString requete("INSERT INTO indice (nom, etape) VALUES (\"");

        requete.push_back(ui->nouvelIndice->text());
        requete.push_back("\", \"");
        requete.push_back(ui->etapeNouvelIndice->text());
        requete.push_back("\")");

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

void MainWindow::modifierIndice() // Requête SQL de modification d'un indice
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

//------------------------------------------------
// Méthodes concernant le tri des indices

void MainWindow::trierEnvoi(QString &requete) // Permet le tri des indices listés dans la boîte déroulante dans l'écran d'envoi d'indice
{

    if(ui->trierEnvoiBox->currentIndex() == 1) { // Par ordre alphabétique
        ui->selectionTriage->hide();
        ui->numeroEtape->hide();
        requete.push_back("ORDER BY lower(nom)");
    }
    if(ui->trierEnvoiBox->currentIndex() == 2 || ui->gestionRecherche->text().isEmpty() == false) { // Contient un mot
        ui->selectionTriage->show();
        ui->numeroEtape->hide();
        requete.push_back("WHERE nom LIKE \"%");
        requete.push_back(ui->selectionTriage->text());
        requete.push_back(ui->gestionRecherche->text());
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

void MainWindow::on_selectionTriage_textChanged(const QString &arg1) // Méthode qui met à jour la BDD à chaque fois que la barre de recherche du tri change
{
    rechercher();
}

void MainWindow::on_trierEnvoiBox_activated(const QString &arg1) // Quand l'index de la boîte déroulante de tri change
{
    rechercher();
}

void MainWindow::on_numeroEtape_textChanged(const QString &arg1) // // Méthode qui met à jour la BDD à chaque fois que la barre de recherche du tri change
{
    rechercher();
}

//------------------------------------------------
// Méthodes des boutons du menu principal

void MainWindow::on_versGestion_clicked() // Cliquer sur le bouton "Gérer les indices"
{
    rechercher();
    ui->stackedWidget->setCurrentIndex(1);
    rechercher();
    ui->advertisementSuppression->hide();
    ui->advertisementModification->hide();
}

void MainWindow::on_versEnvoi_clicked() // Cliquer sur le bouton "Envoyer un indice"
{
    remettreDefaut();
    ui->stackedWidget->setCurrentIndex(2);
    rechercher();
}

void MainWindow::on_versAide_clicked() // Cliquer sur le bouton "Aide"
{
    ui->stackedWidget->setCurrentIndex(3);
}

//------------------------------------------------
// Méthodes des boutons de retour en arrière

void MainWindow::on_backGestion_clicked() // Cliquer sur le bouton retour en arrière de l'écran de gestion
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_backEnvoi_clicked() // Cliquer sur le bouton retour en arrière de l'écran d'envoi
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_backAide_clicked() // Cliquer sur le bouton retour en arrière de l'écran d'aide
{
    ui->stackedWidget->setCurrentIndex(0);
}

//------------------------------------------------
// Méthodes gérants l'ajout d'indices à la BDD

void MainWindow::on_creerIndice_clicked() // Quand on appui sur le bouton "Créer un indice" dans l'écran de gestion des indices
{
    ui->stackedWidget->setCurrentIndex(4);
}

void MainWindow::on_ajouterIndice_clicked() // Quand on appui sur le bouton "Créer"
{
    creerIndice();
    ui->nouvelIndice->setText("");
    ui->etapeNouvelIndice->setText("");
    rechercher();
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_nouvelIndice_returnPressed() // On appui sur "Entrée" dans la barre du nom du nouvel indice
{
    on_ajouterIndice_clicked();
}

void MainWindow::on_etapeNouvelIndice_returnPressed() // On appui sur "Entrée" dans la barre du numéro de l'étape
{
    on_ajouterIndice_clicked();
}

void MainWindow::on_annulerCreer_clicked() // Quand on appui sur le bouton "Annuler"
{
    ui->stackedWidget->setCurrentIndex(1);
}

//------------------------------------------------
// Méthodes gérants la suppression d'un indice

void MainWindow::on_supprimerIndice_clicked() // Quand on appui sur le bouton "Supprimer un indice" dans l'écran de gestion des indices
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

void MainWindow::on_modifierIndice_clicked() // Quand on appui sur le bouton "Modifier un indice" dans l'écran de gestion des indices
{

    if(ui->reponseTable->currentItem() != NULL) {
        ui->stackedWidget->setCurrentIndex(5);

        ui->indiceModifiable->setText(ui->reponseTable->item(ui->reponseTable->currentRow(), 0)->text());
        ui->etapeModifiable->setText(ui->reponseTable->item(ui->reponseTable->currentRow(), 1)->text());
    }
    else {
        ui->advertisementSuppression->hide();
        ui->advertisementModification->show();
    }
}

void MainWindow::on_modificationIndice_clicked() //On clique sur le bouton "Modifier"
{
    modifierIndice();

    rechercher();

    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_annulerModifier_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_indiceModifiable_returnPressed()
{
    on_modificationIndice_clicked();
}

void MainWindow::on_etapeModifiable_returnPressed()
{
    on_modificationIndice_clicked();
}

//------------------------------------------------
// Méthode de la table d'affichage

void MainWindow::on_reponseTable_cellClicked(int row, int column) // On affiche dans la console les données de la cellule sélectionnée
{
    std::cout << row << std::endl;
    std::cout << column << std::endl;
    std::cout << ui->reponseTable->currentItem()->text().toStdString() << std::endl;
}

void MainWindow::on_reponseTable_cellDoubleClicked(int row, int column) // On met à jour la table quand on double clique sur la table (pour éviter l'édition manuelle des cellules)
{
    rechercher();
}

void MainWindow::on_gestionRecherche_textChanged(const QString &arg1) // Barre sur la table permettant de rechercher un indice
{
    rechercher();
}

//------------------------------------------------
// Méthode de remise à défaut de l'écran d'envoi

void MainWindow::remettreDefaut() {
    ui->trierEnvoiBox->setCurrentIndex(0);
    ui->selectionTriage->hide();
    ui->numeroEtape->hide();
    rechercher();
    ui->vitesseDefilementBox->setCurrentIndex(1);
    ui->policeBox->setCurrentIndex(1);
    ui->couleurBox->setCurrentIndex(0);
    ui->advertisementSuppression->hide();
    ui->advertisementModification->hide();
}

void MainWindow::on_resetOption_clicked()
{
    remettreDefaut();
}

//------------------------------------------------
// Méthode de l'écran d'aide

void MainWindow::on_aideGestion_clicked() // Charge le PDF à une certaine page (à faire)
{
    QDesktopServices::openUrl(QUrl(QUrl::fromLocalFile("../PROJET_AFFICHEUR_INDICE/manuel.pdf")));
}

void MainWindow::on_aideEnvoi_clicked() // Charge le PDF à une certaine page (à faire)
{
    QDesktopServices::openUrl(QUrl(QUrl::fromLocalFile("../PROJET_AFFICHEUR_INDICE/manuel.pdf")));
}




