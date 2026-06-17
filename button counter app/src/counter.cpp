#include "counter.h"
#include <QDebug>

Counter::Counter(QObject *parent) : QObject(parent), m_count(0) {
    // 1. Boot up the server on port 12345
    m_server = new QWebSocketServer(QStringLiteral("Counter Server"), QWebSocketServer::NonSecureMode, this);
    if (m_server->listen(QHostAddress::Any, 12345)) {
        qDebug() << "Server successfully started on port 12345!";
        connect(m_server, &QWebSocketServer::newConnection, this, &Counter::onNewConnection);
    }
}

Counter::~Counter() {
    m_server->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}

int Counter::getValue() const {
    return m_count;
}

void Counter::increment() {
    m_count++;
    emit valueChanged(); // Update Desktop UI
    broadcastValue();    // Update Web UI
}

void Counter::reset() {
    m_count = 0;
    emit valueChanged(); // Update Desktop UI
    broadcastValue();    // Update Web UI
}

// --- NETWORK LOGIC ---

void Counter::onNewConnection() {
    QWebSocket *socket = m_server->nextPendingConnection();
    connect(socket, &QWebSocket::textMessageReceived, this, &Counter::processTextMessage);
    connect(socket, &QWebSocket::disconnected, this, &Counter::socketDisconnected);
    
    m_clients << socket; // Add new web client to our list
    
    // Immediately send them the current count so they are in sync
    socket->sendTextMessage(QString::number(m_count));
}

void Counter::processTextMessage(QString message) {
    // If the website sends a command, trigger our C++ logic
    if (message == "INCREMENT") {
        increment();
    } else if (message == "RESET") {
        reset();
    }
}

void Counter::socketDisconnected() {
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if (client) {
        m_clients.removeAll(client);
        client->deleteLater();
    }
}

void Counter::broadcastValue() {
    QString message = QString::number(m_count);
    // Loop through every connected website and send the new number
    for (QWebSocket *client : std::as_const(m_clients)) {
        client->sendTextMessage(message);
    }
}