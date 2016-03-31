#pragma once

#include <QObject>

// SqlColumn in used from QML. It represents an SQL column entry that is used
// during an SQL CREATE TABLE statement and for referrencing the column in
// any SELECT, INSERT, or other query.

// Usage is defined like this:
/*
 *          SqlModel {
 *              id: sqlModel;
 *              cacheModel: false;
 *              tableName: "Movies";
 *              autoCreate: true;
 *              connectionName: "LIBRARY";
 *              fileLocation: "/home/username/Desktop/db.sqlite";
 *
 *              SqlColumn { name: "Title"; type: "TEXT"; }
 *              SqlColumn { name: "Poster"; type: "TEXT"; }
 *              SqlColumn { name: "Description"; type: "TEXT"; }
 *              SqlColumn { name: "AbsoluteFilePath"; type: "TEXT NOT NULL UNIQUE"; }
 *
 *              Component.onCompleted: {
 *                  sqlModel.finishModelConstruction();
 *              }
 *          }
 * 'name' corresponds to the actual SQL column value in a SQL CREATE TABLE statement. 'name' also
 * defaults to the roleName for the SqlModel, so in order to use the data from a View's delegate
 * the usage is as follows:
 *
 *          ListView {
 *
 *          model: SqlModel {
 *              //...
 *          }
 *
 *          delegate: Item {
 *              //...
 *              Text {
 *                  anchors.verticalCenter: parent.verticalCenter;
 *                  text: AbsoluteFilePath // This is without quotes! It is an actual hashed role.
 *              }
 *              //...
 *          }
 *
 *
 *
 *          }
 *
 * 'type' corresponds to the Type of the SQL column, it's value can be any value supported by
 *      your SQL driver. All types of qualifiers are supported, like PRIMARY KEY, etc...
 *
 * A CREATE statement constructed from these SqlColumns, look like:
 *
 *          CREATE TABLE 'Movies' (
 *              'Title' TEXT,
 *              'Poster' TEXT,
 *              'Description': TEXT,
 *              'AbsoluteFilePath': TEXT NOT NULL UNIQUE
 *          )
 */

class SqlColumn : public QObject {
        Q_OBJECT
        Q_PROPERTY( QString name READ name WRITE setName )
        Q_PROPERTY( QString type READ type WRITE setType )

    public:
        SqlColumn( QObject *parent = nullptr );

        QString name() const;
        QString type() const;

        void setName( const QString name );
        void setType( const QString type );

    private:
        QString mName;
        QString mType;
};

Q_DECLARE_METATYPE( SqlColumn * )
