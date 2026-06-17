#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "counter.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    Counter myCounter;

    // Connects the C++ counter to the QML frontend
    engine.rootContext()->setContextProperty("backendCounter", &myCounter);

    // Bulletproof standard string to avoid C++20 operator errors
   const QUrl url("qrc:/qt/qml/Main/src/main.qml");
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    
    engine.load(url);

    return app.exec();
}