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

class CSerialHandler : public QObject {
  Q_OBJECT
  Q_PROPERTY(QGeoCoordinate currentCoordinate READ currentCoordinate NOTIFY currentCoordinateChanged)
  Q_PROPERTY(QDateTime currentDateTime READ currentDateTime NOTIFY currentDateTimeChanged)
  Q_PROPERTY(QStringList sentence READ sentence NOTIFY sentenceChanged)

 public:
  explicit CSerialHandler(QObject* parent = nullptr);
  ~CSerialHandler();
  void openPort(const QString& portName);
  QGeoCoordinate currentCoordinate() const;
  void handleData(QByteArray& newData);
  QDateTime currentDateTime() const;
  QStringList sentence() const;

  Q_INVOKABLE void addNMEASentence(const QString& sentence);

 signals:

  void currentCoordinateChanged(const QGeoCoordinate& coord);
  void currentDateTimeChanged(const QDateTime& cuurentDateTime);

  void sentenceChanged();

 private slots:
  void readData();
  void logData();

 private:
  QString m_buffer;
  QGeoCoordinate m_currentCoordinate;
  QDateTime m_currentDateTime;
  QStringList m_sentence;
  QThread m_workerThread;
  QTimer* m_loggingTimer;

  void parseNMEASentence(const QString& sentence);
  QDateTime parseDateTime(const QString& sentence);
  double convertToDecimal(const QString& coord, const QString& dir);
  void logToCSV(const QGeoCoordinate& coord, const QDateTime& dateTime);

 public:
  QSerialPort m_serial;

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
