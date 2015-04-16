#include "usernotifications.h"

UserNotifications::UserNotifications( QObject *parent ) : QObject( parent ) {

}

UserNotifications::~UserNotifications() {

}

QString UserNotifications::biosNotification() const {
    return m_bios_notification;
}

QString UserNotifications::coreNotification() const {
    return m_core_notification;
}

QString UserNotifications::gameNotification() const {
    return m_game_notification;
}

QString UserNotifications::inputNotification() const {
    return m_input_notification;
}

void UserNotifications::setBiosNotification( const QString note ) {
    m_bios_notification = note;
    emit biosNotificationChanged();
}

void UserNotifications::setCoreNotification( const QString note ) {
    m_core_notification = note;
    emit coreNotificationChanged();
}

void UserNotifications::setGameNotification( const QString note ) {
    m_game_notification = note;
    emit gameNotificationChanged();
}

void UserNotifications::setInputNotification( const QString note ) {
    m_input_notification = note;
    emit inputNotificationChanged();
}

