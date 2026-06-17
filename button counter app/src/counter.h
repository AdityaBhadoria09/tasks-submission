#ifndef COUNTER_H
#define COUNTER_H

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QList>

class Counter : public QObject {
    Q_OBJECT
    Q_PROPERTY(int value READ getValue NOTIFY valueChanged)

public:
    explicit Counter(QObject *parent = nullptr);
    ~Counter(); // Cleanup function
    int getValue() const;

public slots:
    void increment();
    void reset();

signals:
    void valueChanged();

private slots:
    // Network functions
    void onNewConnection();
    void processTextMessage(QString message);
    void socketDisconnected();

private:
    void broadcastValue(); // Broadcasts the number to the web

    int m_count;
    QWebSocketServer *m_server;
    QList<QWebSocket *> m_clients; // Keeps track of connected web pages
};

#endif // COUNTER_H