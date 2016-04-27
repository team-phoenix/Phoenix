#include "databasesettings.h"

#include <QSqlDatabase>
#include <QDebugStateSaver>

DatabaseSettings::DatabaseSettings(QObject *parent)
    : QObject( parent ),
      mDriverType( QStringLiteral( "QSQLITE" ) ),
      mPort( INT_MIN ),
      mNumericalPrecisionPolicy( NumericalPrecisionPolicy::Default )
{

}

QString DatabaseSettings::password() const
{
    return mPassword;
}

QString DatabaseSettings::userName() const
{
    return mUserName;
}

QString DatabaseSettings::connectionOptions() const
{
    return mConnectionOptions;
}

QString DatabaseSettings::databaseName() const
{
    return mDatabaseName;
}

QString DatabaseSettings::driverType() const
{
    return mDriverType;
}

QString DatabaseSettings::hostName() const
{
    return mHostName;
}

QString DatabaseSettings::connectionName() const
{
    return mConnectionName;
}

int DatabaseSettings::port() const
{
    return mPort;
}

DatabaseSettings::NumericalPrecisionPolicy DatabaseSettings::numericalPrecisionPolicy() const
{
    return mNumericalPrecisionPolicy;
}

void DatabaseSettings::setPassword(const QString password) {
    mPassword = password;
}

void DatabaseSettings::setUserName(const QString userName) {
    mUserName = userName;
}

void DatabaseSettings::setConnectionOptions(const QString options) {
    mConnectionOptions = options;
}

void DatabaseSettings::setDatabaseName(const QString name) {
    mDatabaseName = name;
}

void DatabaseSettings::setDriverType(const QString type) {
    mDriverType = type;
}

void DatabaseSettings::setHostName(const QString name) {
    mHostName = name;
}

void DatabaseSettings::setConnectionName(const QString name) {
    mConnectionName = name;
}

void DatabaseSettings::setPort(const int port) {
    mPort = port;
}

void DatabaseSettings::setNumericalPrecisionPolicy(const DatabaseSettings::NumericalPrecisionPolicy policy) {
    mNumericalPrecisionPolicy = policy;
}

bool DatabaseSettings::isDriverAvailable(const QString name) {
    return QSqlDatabase::isDriverAvailable( name );
}

QDebug operator <<(QDebug d, const DatabaseSettings &db) {
    QDebugStateSaver saver( d );
    Q_UNUSED( saver );
    d << "DatabaseSettings(";
    d << "\n  userName: " << db.userName()
      << "\n  password: " << db.password()
      << "\n  databaseName: " << db.databaseName()
      << "\n  driverType: " << db.driverType()
      << "\n  hostName:" << db.hostName()
      << "\n  port:" << ( db.port() == INT_MIN ? QStringLiteral( "undefined" ) : QString::number( static_cast<int>( db.port() ) ) )
      << "\n  numericalPrecisionPolicy:" << static_cast<int>( db.numericalPrecisionPolicy() )
      << "\n  connectionOptions: " << db.connectionOptions()
      << "\n  policy: " << static_cast<int>( db.numericalPrecisionPolicy() )
      << "\n)";
    return d;
}
