#ifndef PHOENIXGLOBALS_H
#define PHOENIXGLOBALS_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QDebug>
#include <QCryptographicHash>
#include <QMap>


class PhoenixGlobals : public QObject {
    Q_OBJECT
public:
    explicit PhoenixGlobals(QObject *parent=0) : QObject(parent)
    {
    }

    enum Bios {
        PSX = 0,
        SNES,
    };

    ~PhoenixGlobals()
    {

    }

    void createFolder(const QString &path)
    {
        QDir dir(path);
        if (!dir.exists())
            dir.mkpath(".");
    }

    bool isBios(const QString &file)
    {
        return (
                    // Check for psx bios files

                    file == "scph5500"
                 || file == "scph5501"
                 || file == "scph5502"

                    // Other bios files go here.
               );
    }


    void setOfflineStoragePath(const QString &path)
    {
        m_offline_storage_path = path;
        createFolder(path + "/Bios");
        createFolder(path + "/Artwork");
    }

    QByteArray getCheckSum(const QString &file, QCryptographicHash::Algorithm algorithm)
    {
        QFile game_file( file );

        if(!game_file.open(QIODevice::ReadOnly))
            return QByteArray("");

        QCryptographicHash sha1_hash(algorithm);
        sha1_hash.addData( &game_file );
        QByteArray result = sha1_hash.result().toHex();

        game_file.close();
        return result;
    }


public slots:
    QString offlineStoragePath()
    {
        return m_offline_storage_path;
    }

    bool checkPSXBios()
    {
        if (QDir(offlineStoragePath() + "Bios").exists()) {
            int result = 0;
            QDirIterator iter(offlineStoragePath() + "/Bios", QDir::Files, QDirIterator::NoIteratorFlags);
            while (iter.hasNext()) {
                QFileInfo bios_file = QFileInfo(iter.next());
                if (isBios(bios_file.baseName()))
                    result++;
                qDebug() << "found: " << bios_file.baseName();
            }

            return (result == 3);
        }

    }

private:
    QString m_offline_storage_path;
    QMap<PhoenixGlobals::Bios, QStringList> bios_map;

};

#endif // PHOENIXGLOBALS_H
