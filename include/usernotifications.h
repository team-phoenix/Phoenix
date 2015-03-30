#ifndef USERNOTIFICATIONS_H
#define USERNOTIFICATIONS_H

#include <QObject>

/* The UserNotifications is a global singleton class
 * is, in effect, a way for a developer relay information from c++ to QML in a graphical manner.
 *
 * Each Q_PROPERTY will generate a different popup when called from QML.
 *
 * You want to tell the user that they are missing 3 bios files, use this class.
 * You want to tell the user that they have disconnected their controller, use this class,
 * and so on.
 */

class UserNotifications : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString biosNotification READ biosNotification WRITE setBiosNotification NOTIFY biosNotificationChanged)
    Q_PROPERTY(QString coreNotification READ coreNotification WRITE setCoreNotification NOTIFY coreNotificationChanged)
    Q_PROPERTY(QString gameNotification READ gameNotification WRITE setGameNotification NOTIFY gameNotificationChanged)
    Q_PROPERTY(QString inputNotification READ inputNotification WRITE setInputNotification NOTIFY inputNotificationChanged)

public:
    explicit UserNotifications(QObject *parent = 0);
    ~UserNotifications();

    QString biosNotification() const;
    QString coreNotification() const;
    QString gameNotification() const;
    QString inputNotification() const;

    void setBiosNotification(const QString note);
    void setCoreNotification(const QString note);
    void setGameNotification(const QString note);
    void setInputNotification(const QString note);

signals:
    void biosNotificationChanged();
    void coreNotificationChanged();
    void gameNotificationChanged();
    void inputNotificationChanged();

public slots:

private:
    QString m_bios_notification;
    QString m_game_notification;
    QString m_core_notification;
    QString m_input_notification;
};

#endif // USERNOTIFICATIONS_H
