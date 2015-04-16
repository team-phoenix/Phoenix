#include "phoenixlibraryhelper.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QMap>
#include <QStringList>
#include <QDebug>

#include "phoenixglobals.h"
#include "usernotifications.h"

QMap<QString, QString> playstationBios {

    // MD5 checksums are used for psx.
    {"8dd7d5296a650fac7319bce665a6a53c", "scph5500"},
    {"490f666e1afb15b7362b406ed1cea246", "scph5501"},
    {"32736f17079d0b2b7024407c39bd3050", "scph5502"},

};

PhoenixLibraryHelper::PhoenixLibraryHelper(QObject *parent) : QObject(parent)
{

}

PhoenixLibraryHelper::~PhoenixLibraryHelper()
{

}

bool PhoenixLibraryHelper::needsBios(const QString core)
{
    if (core.contains("mednafen_psx")) {
        bool result = !foundBiosFiles(3, PhoenixLibraryHelper::PlayStation);
        if (result)
            userNotifications.setBiosNotification("Bios files are missing!");
        qDebug() << "result from bios: " << result;
        return result;
    }

    return false;
}

bool PhoenixLibraryHelper::foundBiosFiles(unsigned bios_count, PhoenixLibraryHelper::BiosType type)
{
    unsigned result = 0;
    QDirIterator iter(phxGlobals.biosPath(), QStringList() << "*.bin", QDir::Files, QDirIterator::NoIteratorFlags);

    while (iter.hasNext()) {
        QFileInfo bios_file = QFileInfo(iter.next());
        QString hash = QString(getCheckSum(bios_file.canonicalFilePath(), QCryptographicHash::Md5));

        switch(type) {
            case PhoenixLibraryHelper::PlayStation:
                if (playstationBios.contains(hash)) {
                    qDebug() << "found: " << bios_file.baseName() << result << bios_count;
                    result++;
                }
                break;
            default:
                break;
        }
    }

    return (result == bios_count);
}

QByteArray PhoenixLibraryHelper::getCheckSum(const QString &file, QCryptographicHash::Algorithm algorithm)
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

bool PhoenixLibraryHelper::checkForBios(const QFileInfo &info)
{
    QString suffix = info.suffix().toLower();
    qDebug() << "Checking for bios";
    QString hash = QString(getCheckSum(info.canonicalFilePath(), QCryptographicHash::Md5));
    QString val = playstationBios.value(hash, "");
    qDebug() << "bios value: " << val << hash;
    if (val != "")
        return QFile::copy(info.canonicalFilePath(), phxGlobals.biosPath() + val + "." + suffix);

    return false;
}

