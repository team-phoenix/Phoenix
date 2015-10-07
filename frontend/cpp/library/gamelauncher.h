#ifndef GAMELAUNCHER_H
#define GAMELAUNCHER_H

#include <QObject>

class GameLauncher : public QObject
{
    Q_OBJECT
public:
    explicit GameLauncher(QObject *parent = 0);

public slots:
    const QString getDefaultCore( const QString system );

    bool verifyGame( const QString system, const QString rom );

};

#endif // GAMELAUNCHER_H
