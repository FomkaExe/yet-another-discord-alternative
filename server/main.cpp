#include <QCoreApplication>
#include "server.h"

int main(int argc, char *argv[]) {
    Server* server = argc <= 1 ?
                     Server::instance(23012) :
                     Server::instance(std::stoi(argv[1]));

    while (true) {
        QTextStream qtin(stdin);
        QString cmd = qtin.readLine();
        server->sendToAllClients(cmd);
    }
    return 0;
}
