#pragma once

#include <QObject>
#include <QVariantMap>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

QVariantMap parseCommandLine( QCoreApplication &app );
