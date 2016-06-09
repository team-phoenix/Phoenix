#ifndef CUEFILE_H
#define CUEFILE_H

#include <QStringList>
#include <QString>

class CueFile {
    public:
        CueFile();

        static QStringList parse( const QString &filePath );

    private:
        static const QString delimiter();
        static const QString prefix();

};

#endif // CUEFILE_H
