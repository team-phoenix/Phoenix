#include "sqlthreadedmodel.h"

#include <QDebug>

SqlThreadedModel::SqlThreadedModel(QObject *parent)
    : QAbstractTableModel( parent ),
      mModelThread( this )
{
    connect( &mModel, SIGNAL(modelAboutToBeReset()), this, SLOT(aboutToResetModel()) );
    connect( &mModel, SIGNAL(modelReset()), this, SLOT(resetModel()) );

    mModel.moveToThread( &mModelThread );
    mModelThread.start();

}

SqlThreadedModel::~SqlThreadedModel() {
    // Make sure the internal model's thread shuts down properly.
    mModelThread.quit();
    mModelThread.wait();
}

QHash<int, QByteArray> SqlThreadedModel::roleNames() const
{
    return mModel.roleNames();
}

int SqlThreadedModel::rowCount(const QModelIndex &parent) const {
    return mModel.rowCount( parent );
}

int SqlThreadedModel::columnCount(const QModelIndex &parent) const
{
    return mModel.columnCount( parent );
}

QVariant SqlThreadedModel::data(const QModelIndex &index, int role) const
{
    return mModel.data( index, role );
}

bool SqlThreadedModel::cacheModel() const
{
    return mModel.cacheModel();
}

QString SqlThreadedModel::tableName() const
{
    return mModel.tableName();
}

QUrl SqlThreadedModel::fileLocation() const
{
    return mModel.fileLocation();
}

bool SqlThreadedModel::autoCreate() const
{
    return mModel.autoCreate();
}

DatabaseSettings *SqlThreadedModel::databaseSettings() {
    return mModel.databaseSettings();
}

QQmlListProperty<SqlColumn> SqlThreadedModel::tableColumns()
{
    return mModel.tableColumns();
}

void SqlThreadedModel::appendTableRow(QQmlListProperty<SqlColumn> *list, SqlColumn *row) {
    SqlModel::appendTableRow( list, row );
}

void SqlThreadedModel::aboutToResetModel() {
    beginResetModel();
}

void SqlThreadedModel::resetModel() {
    endResetModel();
}

void SqlThreadedModel::setFilter(const QString column, const QVariant value, const SqlModel::FilterType type)
{
    QMetaObject::invokeMethod( &mModel, "setFilter", Q_ARG( QString, column )
                               , Q_ARG( QVariant, value )
                               , Q_ARG( SqlModel::FilterType, type ) );
}

void SqlThreadedModel::clearFilter(const QString column) {
    QMetaObject::invokeMethod( &mModel, "clearFilter", Q_ARG( QString, column  ) );
}

void SqlThreadedModel::finishModelConstruction() {
    QMetaObject::invokeMethod( &mModel, "finishModelConstruction" );
}

bool SqlThreadedModel::addRow(const QVariantMap rowData) {
    QMetaObject::invokeMethod( &mModel, "addRow", Q_ARG( QVariantMap, rowData ) );
    return true;
}

bool SqlThreadedModel::deleteRow(int index, const QString column, const QVariant absFilePath) {
    QMetaObject::invokeMethod( &mModel, "deleteRow", Q_ARG( int, index )
                               , Q_ARG( QString, column )
                               , Q_ARG( QVariant, absFilePath ) );
    return true;

}

bool SqlThreadedModel::updateRow(int index, const QString column, const QVariant oldData, const QVariant newData) {
    QMetaObject::invokeMethod( &mModel, "updateRow", Q_ARG( int, index  )
                               , Q_ARG( QString, column )
                               , Q_ARG( QVariant, oldData )
                               , Q_ARG( QVariant, newData ) );
    return true;
}

void SqlThreadedModel::clearDatabase() {
    QMetaObject::invokeMethod( &mModel, "clearDatabase" );
}

void SqlThreadedModel::setCacheModel( const bool cache ) {
    mModel.setCacheModel( cache );
    emit cacheModelChanged();
}

void SqlThreadedModel::setTableName( const QString tableName ) {
    mModel.setTableName( tableName );
    emit tableNameChanged();
}

void SqlThreadedModel::setFileLocation( const QUrl location ) {
    mModel.setFileLocation( location );
    emit fileLocationChanged();
}

void SqlThreadedModel::setAutoCreate(const bool create) {
    mModel.setAutoCreate( create );
    emit autoCreateChanged();
}
