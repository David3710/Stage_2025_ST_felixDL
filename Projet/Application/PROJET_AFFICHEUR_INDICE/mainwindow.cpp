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
