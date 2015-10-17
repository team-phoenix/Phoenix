from sqldatabase import SqlDatabase
from retrieve_core_info import retrieveCoreInfo
from collections import OrderedDict

from updateSystemMap import SystemMapUpdater
from updateFirmware import FirmwareUpdater
from updateCoreMetadata import CoreMetadataUpdater
from updateSystemCoreMap import SystemCoreMapUpdater
from updateRomHeaderOffsets import RomHeaderOffsetUpdater
from updateExtensions import ExtensionUpdater
from updateDefaultCores import DefaultCoresUpdater

import os

if __name__ == "__main__":

    info = retrieveCoreInfo()

    romHeaders = RomHeaderOffsetUpdater(tableName="systemHeaderOffsets", coreInfo=info)
    romHeaders.updateTable()

    systemCoreMap = SystemCoreMapUpdater(tableName="systemCoreMap", coreInfo=info)
    systemCoreMap.updateTable()

    firmware = FirmwareUpdater(tableName="firmware", coreInfo=info)
    firmware.updateTable()

    systemMap = SystemMapUpdater(tableName="systemMap", coreInfo=info)
    systemMap.updateTable()

    coreMetadata = CoreMetadataUpdater(tableName="coreMetadata", coreInfo=info)
    coreMetadata.updateTable()

    extensions = ExtensionUpdater(tableName="extensions", coreInfo=info)
    extensions.updateTable()

    defaultCores = DefaultCoresUpdater(tableName="defaultCoresMap")
    defaultCores.updateTable()

    #db.createTable( "schema_version", {"version": "INTEGER NOT NULL"} )


