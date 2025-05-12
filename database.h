#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

class CDatabase : public QObject {
  Q_OBJECT
 public:
  explicit CDatabase(QObject* parent = nullptr);
  void initDatabase();
  void insertData(const double& lat, const double& lon, const double& speed,
                  const double& heading, const QDateTime time, const QString& rawSentence);

 signals:
};

#endif // DATABASE_H
