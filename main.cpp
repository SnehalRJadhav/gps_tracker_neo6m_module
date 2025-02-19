#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtPositioning/QGeoCoordinate>
#include <QQmlContext>
#include "serialhandler.h"


int main(int argc, char* argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
  QGuiApplication app(argc, argv);

  CSerialHandler serialHandler;

  QQmlApplicationEngine engine;

  engine.rootContext()->setContextProperty("serialHandler", &serialHandler);

  qRegisterMetaType<QGeoCoordinate>("QGeoCoordinate");
  qRegisterMetaType<QDateTime>("QDateTime");

  const QUrl url(QStringLiteral("qrc:/main.qml"));
  QObject::connect(
      &engine,
      &QQmlApplicationEngine::objectCreated,
      &app,
      [url](QObject * obj, const QUrl & objUrl) {
          if (!obj && url == objUrl)
          QCoreApplication::exit(-1);
      },
      Qt::QueuedConnection);

  engine.load(url);

  QObject* qmlRootObject =  engine.rootObjects().first();
  if (!qmlRootObject) {
    qCritical() << "qmlRootObject is NULL!";
    return -1;
  }

  QObject::connect(
      &serialHandler, &CSerialHandler::currentCoordinateChanged,
      qmlRootObject, [](const QGeoCoordinate & coord) {
          qDebug() << "Received new coordinate in QML: " << coord;
      },
      Qt::QueuedConnection // Critical for thread safety
  );


  return app.exec();
}
