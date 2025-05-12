#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <QObject>
#include <QSerialPort>
#include <QDebug>
#include <QGeoCoordinate>
#include <QDateTime>
#include <QTimeZone>
#include <QSerialPortInfo>
#include <QThread>
#include <QGuiApplication>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include "database.h"

class CSerialHandler : public QObject {
  Q_OBJECT
  Q_PROPERTY(QGeoCoordinate currentCoordinate READ currentCoordinate NOTIFY currentCoordinateChanged)
  Q_PROPERTY(QDateTime currentDateTime READ currentDateTime NOTIFY currentDateTimeChanged)
  Q_PROPERTY(QStringList sentence READ sentence NOTIFY sentenceChanged)
  Q_PROPERTY(double compassHeading READ compassHeading NOTIFY compassHeadingChanged)
  Q_PROPERTY(double speed READ speed NOTIFY speedChanged)
  Q_PROPERTY(int satelliteCount READ satelliteCount NOTIFY satelliteCountChanged)

 public:
  explicit CSerialHandler(QObject* parent = nullptr);
  ~CSerialHandler();
  void openPort(const QString& portName);
  QGeoCoordinate currentCoordinate() const;
  void handleData(QByteArray& newData);
  QDateTime currentDateTime() const;
  QStringList sentence() const;
  CDatabase database;

  Q_INVOKABLE void addNMEASentence(const QString& sentence);

 signals:

  void currentCoordinateChanged(const QGeoCoordinate& coord);
  void currentDateTimeChanged(const QDateTime& cuurentDateTime);
  void sentenceChanged();
  void compassHeadingChanged(const double& heading);
  void speedChanged(const double& speedVal);
  void satelliteCountChanged(const int& satCount);

 private slots:
  void readData();

 private:
  QString m_buffer;
  QGeoCoordinate m_currentCoordinate;
  QDateTime m_currentDateTime = QDateTime::currentDateTime();
  QStringList m_sentence;
  QThread m_workerThread;
  double m_compassHeading = 0;
  double m_speed = 0;
  int m_satelliteCount = 0;


  void parseNMEASentence(const QString& sentence);
  QDateTime parseDateTime(const QString& sentence);
  double convertToDecimal(const QString& coord, const QString& dir);
  double convertToKiloMeterPerHour(double& speed);

 public:
  QSerialPort m_serial;

  double compassHeading() const;
  double speed() const;
  int satelliteCount() const;
};

class SerialWorker : public QObject {
  Q_OBJECT
 public:
  explicit SerialWorker(CSerialHandler* handler);

 signals:
  void coordinateUpdated(QGeoCoordinate coord);
  void dateTimeUpdated(QDateTime dt);

 public slots:
  void processData();

 private:
  CSerialHandler* m_handler;
};

#endif // SERIALHANDLER_H
