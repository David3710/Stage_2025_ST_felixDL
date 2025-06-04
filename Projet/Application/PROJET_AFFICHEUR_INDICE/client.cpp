#include <QtNetwork>
#include <QThread>
#include <iostream>
#include <sstream>
#include "client.h"

Client::Client()
:   m_networkSession(0)
{
    std::cout << "Application Client" << std::endl;
    m_tcpSocket = new QTcpSocket(this);

    // La méthode lireTexte sera appelée sur le signal readyRead
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(lireTexte()));

    // La méthode afficherErreur sera appelée sur le signal error
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(afficherErreur(QAbstractSocket::SocketError)));

    QNetworkConfigurationManager manager;
    QNetworkConfiguration config;

    std::cout << "Ouverture session" << std::endl;
    m_networkSession = new QNetworkSession(config, this);

    m_networkSession->open();

    m_blockSize = 0;
    m_tcpSocket->abort();

    // connexion au serveur sur le port 53000
    m_tcpSocket->connectToHost( QHostAddress("10.16.25.211").toString(),1470 );
    //QString trame = "<ID01><L1><PA><FE><MA><WC><FE>messageeea7E<E>";
}

//###############################################################################################################
// Méthode appelée lors de la réception d'un texte
void Client::lireTexte()
{
    QDataStream in(m_tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);

    if (m_blockSize == 0) {
        if (m_tcpSocket->bytesAvailable() < (int)sizeof(quint16))
            return;

        in >> m_blockSize;
    }

    if (m_tcpSocket->bytesAvailable() < m_blockSize)
        return;

    QString texte;
    in >> texte;

    std::cout << texte.toStdString() << std::endl;
    m_blockSize = 0;
}

//###############################################################################################################
// Méthode appelée lors d'un déclenchement d'une excepetion sur un socket
void Client::afficherErreur(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        std::cout << "Le serveur n'a pas ete trouve. Verifiez le parametrage du serveur et du port." << std::endl;
        break;
    case QAbstractSocket::ConnectionRefusedError:
        std::cout << "La communication a ete refusee. Verifiez que le serveur est pret, ainsi que le parametrage du serveur et du port." << std::endl;
        break;
    default:
        std::cout << "L'erreur suivante s'est produite : " << m_tcpSocket->errorString().toStdString() << std::endl;
    }
}

QString Client::constructionTrame(std::string t)
{

    QString trame = "<ID01>";
    trame.push_back(t.c_str());
    std::cout << trame.toStdString() << std::endl;
    QString checksum = calculChecksum(t);
    trame.push_back(checksum);

    std::cout << trame.toStdString() << std::endl;

    return trame;

}

QString Client::calculChecksum(std::string t)
{

    uint8_t checksum = 0;

        // Boucle sur chaque caractère de la chaîne
        for (char c : t) {
            checksum ^= static_cast<uint8_t>(c);
        }

        // Convertir le résultat en QString hexadécimal, sur 2 chiffres
        QString result = QString("%1").arg(checksum, 2, 16, QChar('0')).toUpper();
        result.push_back("<E>");

        return result;

}

void Client::envoiTexte(const std::string &s)
{
    std::cout << "Envoi de : " << s << std::endl;
    //QString texte = tr(s.c_str());
    //QString texte = QByteArrayLiteral();
    std::string trame = constructionTrame(s).toStdString();
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << trame.c_str();
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    block.remove(0, 6);
    std::cout << "|" << block.size() << "|" << std::endl;
    for ( int i = 0; i != block.size(); ++i )
        std::cout << block[i];
    std::cout << std::endl;

    m_tcpSocket->write(block);
}


