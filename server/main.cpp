#include <QCoreApplication>
#include "server.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    Server* server = argc <= 1 ?
                         Server::instance(23012) :
                         Server::instance(std::stoi(argv[1]));
    return a.exec();
}
