#include "serialhandler.h"

SerialWorker::SerialWorker(CSerialHandler* handler) : m_handler(handler) {}

void SerialWorker::processData() {
  while (m_handler->m_serial.bytesAvailable()) {
    QByteArray data = m_handler->m_serial.readAll();
    m_handler->handleData(data);
    emit coordinateUpdated(m_handler->currentCoordinate());
    emit dateTimeUpdated(m_handler->currentDateTime());

  }
}


CSerialHandler::CSerialHandler(QObject* parent)
  : QObject{parent} {

  m_loggingTimer = new QTimer(this);
  connect(m_loggingTimer, &QTimer::timeout, this, &CSerialHandler::logData);
  m_loggingTimer->start(3000);


  moveToThread(QGuiApplication::instance()->thread());
  QList<QSerialPortInfo> availablePorts = QSerialPortInfo::availablePorts();
  for (const QSerialPortInfo& portInfo : availablePorts) {
    qDebug() << "Port Name: " << portInfo.portName();
    qDebug() << "Description: " << portInfo.description();
    qDebug() << "Manufacturer: " << portInfo.manufacturer();
    qDebug() << "System Location: " << portInfo.systemLocation();
    qDebug() << "-------------------------";
    if (portInfo.description().contains("USB")) {
      openPort(portInfo.portName());
    }
  }
  SerialWorker* worker = new SerialWorker(this);
  worker->moveToThread(&m_workerThread);
  connect(&m_serial, &QSerialPort::readyRead, worker, &SerialWorker::processData);
  connect(worker, &SerialWorker::coordinateUpdated, this, &CSerialHandler::currentCoordinateChanged);
  connect(worker, &SerialWorker::dateTimeUpdated, this, &CSerialHandler::currentDateTimeChanged);

  // Start thread
  m_workerThread.start();
  m_currentCoordinate = QGeoCoordinate(0.0, 0.0);
  m_serial.setBaudRate(QSerialPort::Baud9600);
  m_serial.setDataBits(QSerialPort::Data8);
  m_serial.setParity(QSerialPort::NoParity);
  m_serial.setStopBits(QSerialPort::OneStop);
  emit currentCoordinateChanged(m_currentCoordinate);
}

CSerialHandler::~CSerialHandler() {
  m_workerThread.quit();
  m_workerThread.wait();
}

QGeoCoordinate CSerialHandler::currentCoordinate() const {
  return m_currentCoordinate;
}

void CSerialHandler::handleData(QByteArray& newData) {
  m_buffer += QString::fromLatin1(newData);

  // Process all complete sentences
  int endIndex;
  while ((endIndex = m_buffer.indexOf("\r\n")) != -1) {
    QString fullSentence = m_buffer.left(endIndex).trimmed();
    m_buffer = m_buffer.mid(endIndex + 2);
    if (!fullSentence.isEmpty()) {
      parseNMEASentence(fullSentence);
      addNMEASentence(fullSentence);
    }
  }
}

void CSerialHandler::openPort(const QString& portName) {
  m_serial.setPortName(portName);
  if (m_serial.open(QIODevice::ReadOnly)) {
    connect(&m_serial, &QSerialPort::readyRead, this, &CSerialHandler::readData);
  } else {
    qDebug() << portName << " is unavailabel.";
  }
}

void CSerialHandler::readData() {
  while (m_serial.bytesAvailable()) {
    QByteArray data = m_serial.readAll();
    handleData(data);
  }
}

void CSerialHandler::logData() {
  QGeoCoordinate coord = m_currentCoordinate;
  QDateTime dateTime = m_currentDateTime;
  logToCSV(coord, dateTime);

}

void CSerialHandler::parseNMEASentence(const QString& sentence) {
  qDebug() << sentence;
  QStringList parts = sentence.split(",");
  if (sentence.startsWith("$GPRMC")) {
    if (parts.size() > 9 && parts[2] == "A") {
      QString latStr = parts[3];
      QString lonStr = parts[5];

      double lat = convertToDecimal(latStr, parts[4]);
      double lon = convertToDecimal(lonStr, parts[6]);

      m_currentCoordinate = QGeoCoordinate(lat, lon);
      qDebug() << "m_currentCoordinate : " << m_currentCoordinate;
      QDateTime gpsDateTime = parseDateTime(sentence);
      QTimeZone localTimeZone = QTimeZone::systemTimeZone();

      qDebug() << "gpsDateTime : " << gpsDateTime.toTimeZone(localTimeZone);
      m_currentDateTime = gpsDateTime;
      QMetaObject::invokeMethod(
          this, [this, gpsDateTime]() {
              emit currentCoordinateChanged(m_currentCoordinate);
              emit currentDateTimeChanged(gpsDateTime);
          }, Qt::QueuedConnection
      );
    } else {
      qWarning() << "Invalid GPRMC sentence or insufficient fields:" << sentence;
    }
  } else if (sentence.startsWith("$GPGGA")) {
    if (parts.size() > 10 && parts[6] != '0') {

      QString ggaLatStr = parts[2];
      QString ggaLonStr = parts[4];

      double ggaLat = convertToDecimal(ggaLatStr, parts[3]);
      double ggaLon = convertToDecimal(ggaLonStr, parts[5]);

      QString satelliteCount = parts[7];
      QString altitude = parts[9];
      qDebug() << "Lat : " << ggaLat << "\nLon : " << ggaLon << "\nsatellite count : " << satelliteCount << "\n Altitude : " << altitude;

    } else {
      qDebug() << "Invalid GPGGA sentence or insufficient fields:" << sentence;
    }
  }
}

QDateTime CSerialHandler::parseDateTime(const QString& sentence) {
  QStringList parts = sentence.split(",");

  QString timeStr = parts[1];
  QString dateStr = parts[9];

  QTime time;
  if (!timeStr.isEmpty() && timeStr.length() >= 6) {
    bool ok;
    QString cleanTime = timeStr.left(6);
    int hours = cleanTime.mid(0, 2).toInt(&ok);
    int minute = cleanTime.mid(2, 2).toInt(&ok);
    int seconds = cleanTime.mid(4, 2).toInt(&ok);

    if (ok && QTime::isValid(hours, minute, seconds)) {
      time = QTime(hours, minute, seconds);
    }
  }

  QDate date;
  if (!dateStr.isEmpty() && dateStr.length() == 6) {
    bool ok;
    int day = dateStr.mid(0, 2).toInt(&ok);
    int month = dateStr.mid(2, 2).toInt(&ok);
    int year = 2000 + dateStr.mid(4, 2).toInt(&ok);

    if (ok && QDate::isValid(year, month, day)) {
      date = QDate(year, month, day);
    }
  }
  if (date.isValid() && time.isValid()) {
    return QDateTime(date, time, Qt::UTC);
  }
  return QDateTime();
}

double CSerialHandler::convertToDecimal(const QString& coord, const QString& dir) {
  int degreeLength = (dir == "N" || dir == "S") ? 2 : 3;
  int degrees = coord.left(degreeLength).toInt();
  double minutes = coord.mid(degreeLength).toDouble();
  double decimal = degrees + (minutes / 60.0);

  if (dir == "S" || dir == "W")
    decimal *= -1;
  qDebug() << "Converted" << coord << dir << "to" << decimal;
  return decimal;
}

void CSerialHandler::logToCSV(const QGeoCoordinate& coord, const QDateTime& dateTime) {

  QFile file("gps_data.csv");

  if (file.open(QIODevice::Append | QIODevice::Text)) {
    QTextStream out(&file);
    out << coord.latitude() << "," << coord.longitude() << "," << dateTime.toString(Qt::ISODate) << "\n";
    file.close();
  } else {
    qWarning() << "Failed to open CSV file for logging";
  }
}


QDateTime CSerialHandler::currentDateTime() const {
  return m_currentDateTime;
}

QStringList CSerialHandler::sentence() const {

  QStringList list;
  for (const QString& str : m_sentence) {
    list.append(str);
  }
  return m_sentence;
}

void CSerialHandler::addNMEASentence(const QString& sentence) {
  m_sentence.append(sentence);
  emit sentenceChanged();
}
