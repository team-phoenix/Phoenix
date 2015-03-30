#ifndef PHOENIXLIBRARYHELPER_H
#define PHOENIXLIBRARYHELPER_H

#include <QObject>
#include <QCryptographicHash>
#include <QFileInfo>

/* The PhoenixLibraryHelper class is a global singleton class.

 * The purpose of this class is to provide necessary library operations on files, folders, and other types of I/O,
 * without needing to reside inside of the PhoenixLibrary class. This is because we may eventually want to add
 * in support for multiple libraries.
 *
 * Keeping these operations separate will decrease size of the PhoenixLibrary.
 */

class PhoenixLibraryHelper : public QObject
{
    Q_OBJECT
public:
    explicit PhoenixLibraryHelper(QObject *parent = 0);
    ~PhoenixLibraryHelper();

    enum BiosType {
        PlayStation = 0,
    };

    QByteArray getCheckSum(const QString &file, QCryptographicHash::Algorithm algorithm);
    bool checkForBios(const QFileInfo &file);
    bool foundBiosFiles(unsigned bios_count, PhoenixLibraryHelper::BiosType type);

signals:

public slots:
    bool needsBios(const QString core);
};

#endif // PHOENIXLIBRARYHELPER_H
