from sqldatabase import SqlDatabase
from retrieve_core_info import retrieveCoreInfo
from collections import OrderedDict

from updaters.updateFirmware import FirmwareUpdater
from updaters.coresUpdater import CoresUpdater
from updaters.updateSystemCoreMap import SystemCoreMapUpdater
from updaters.updateRomHeaderOffsets import RomHeaderOffsetUpdater
from updaters.updateExtensions import ExtensionUpdater
from updaters.updateDefaultCores import DefaultCoresUpdater
from updaters.systemsUpdater import SystemsUpdater

import os

if __name__ == "__main__":

    info = retrieveCoreInfo()

    romHeaders = RomHeaderOffsetUpdater(tableName="headers", coreInfo=info)
    romHeaders.updateTable()

    firmware = FirmwareUpdater(tableName="firmware", coreInfo=info)
    firmware.updateTable()

    cores = CoresUpdater(tableName="cores", coreInfo=info)
    cores.updateTable()

    extensions = ExtensionUpdater(tableName="extensions", coreInfo=info)
    extensions.updateTable()

    systems = SystemsUpdater(tableName="systems", coreInfo=info)
    systems.updateTable();

    #db.createTable( "schema_version", {"version": "INTEGER NOT NULL"} )


