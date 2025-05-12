#include "database.h"
#include "qdatetime.h"

CDatabase::CDatabase(QObject* parent)
  : QObject{parent}
{}

void CDatabase::initDatabase() {
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName("track_log.db");
  if (!db.open()) {
    qCritical() << "Can not open Database!";
    return;
  }
  QSqlQuery query;
  QString createTable = R"(
        CREATE TABLE IF NOT EXISTS track_log(id INTEGER PRIMARY KEY AUTOINCREMENT,
        latitude DOUBLE PRECISION NOT NULL,
        longitude DOUBLE PRECISION NOT NULL,
        speed DOUBLE PRECISION ,
        heading DOUBLE PRECISION,
        time TEXT,
        raw TEXT
        )
    )";

  if (!query.exec(createTable)) {
    qCritical() << "Create table failed!";
  }
}

void CDatabase::insertData(const double& lat, const double& lon, const double& speed, const double& heading, const QDateTime time, const QString& rawSentence) {

  QString dateTime = time.toString("yyyy-MM-dd HH:mm:ss");
  QSqlQuery query;
  query.prepare("INSERT INTO track_log (latitude, longitude, speed, heading, time, raw) VALUES (:lat, :lon, :speed, :heading, :dateTime, :rawSentence)");
  query.bindValue(":lat", lat);
  query.bindValue(":lon", lon);
  query.bindValue(":speed", speed);
  query.bindValue(":heading", heading);
  query.bindValue(":dateTime", dateTime);
  query.bindValue(":rawSentence", rawSentence);

  if (!query.exec()) {
    qWarning() << "Insert failed:" << query.lastError().text();
  }
}
