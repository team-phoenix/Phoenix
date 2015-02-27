#include "mappingmodel.h"

#include <QtConcurrent>
#include <QDebug>

MappingModel::MappingModel(QObject *parent)
 : QObject(parent)
{
    m_qml_context = nullptr;
}

MappingModel::~MappingModel()
{
    deleteAll();
}

void MappingModel::insertMapping(unsigned index, QObject *mapping)

{
    emit beforeItemAdded();
    m_list.insert(index, mapping);
    emit itemAdded();
}

void MappingModel::addMapping(QObject *mapping)
{
    emit beforeItemAdded();
    m_list.append(mapping);
    emit itemAdded();
}

void MappingModel::removeMapping(int index)
{
    emit beforeItemDeleted();
    m_list.removeAt(index);
    emit itemDeleted();
}



void MappingModel::setContext(QQmlContext *context)
{
    m_qml_context = context;
}

void MappingModel::deleteAll()
{
    m_list.clear();
}
