#include "mappingmodel.h"

#include <QtConcurrent>
#include <QDebug>

MappingModel::MappingModel(QObject *parent)
 : QObject(parent)
{
    m_qml_context = nullptr;

    connect(this, &MappingModel::itemAdded, this, &MappingModel::update);
    connect(this, &MappingModel::itemDeleted, this, &MappingModel::update);

}

MappingModel::~MappingModel()
{
    deleteAll();
}


void MappingModel::setDeviceMap(InputDeviceMapping *map)
{
    if (map == nullptr) {
        qDebug() << "InputDebviceMapping was null";
        return;
    }

    m_list.clear();
    populateModel(map->mappings());
    update();

    //QFuture<void> fut = QtConcurrent::run(this, &MappingModel::populateModel);
}

void MappingModel::populateModel(std::unordered_map<InputDeviceEvent *, retro_device_id> *map)
{
    qDebug() << "Adding mapping with count: " << map->size();
    for( auto m = map->begin(); m != map->end(); ++m ) {
        QString event = QString( *( m->first ) );
        QString id = QString::number(m->second);
        addMapping(new ExposedMapping(event, id));
    }

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

void MappingModel::update()
{
    m_qml_context->setContextProperty("mappingmodel", this);
}

void MappingModel::saveModel(unsigned port)
{
    QSettings settings;
    settings.beginGroup("input");
    settings.beginGroup( QString( "port%1" ).arg( port ) );
    for (QObject *obj : m_list) {
        auto *map = static_cast<ExposedMapping *>(obj);
        settings.setValue( id_to_qstring.value(map->retroID().toUInt()), map->deviceEvent());
    }
}

bool MappingModel::collisionDetected(QString event, unsigned port)
{
    unsigned int i = 0;
    bool result = false;
    for (QObject *obj :  m_list) {
        auto *map = static_cast<ExposedMapping *>(obj);

        result = map->deviceEvent() == event;

        if (result) {
            if (i == port) {
                result = false;
            }
            break;
        }
        ++i;
    }

    return result;
}

void MappingModel::setContext(QQmlContext *context)
{
    m_qml_context = context;
    update();
}

void MappingModel::deleteAll()
{
    m_list.clear();
}
