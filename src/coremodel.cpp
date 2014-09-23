#include "coremodel.h"

CoreModel::CoreModel(QObject *parent, QString coreName, QString corePath)
    : QObject(parent)
{
    m_core_name = coreName;
    m_core_path = corePath;
}

CoreModel::~CoreModel()
{

}

void CoreModel::setCoreName(QString coreName)
{
    m_core_name = coreName;
    emit coreNameChanged();
}

void CoreModel::setCorePath(QString corePath)
{
    m_core_path = corePath;
    emit corePathChanged();
}
