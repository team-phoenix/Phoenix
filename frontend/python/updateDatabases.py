from collections import OrderedDict

from updaters.retrieve_core_info import retrieveCoreInfo

from updaters.sqldatabase import SqlDatabase

from updaters.updateRomHeaderOffsets import RomHeaderOffsetUpdater
from updaters.updateFirmware import FirmwareUpdater
from updaters.updateCores import CoresUpdater
from updaters.updateExtensions import ExtensionUpdater
from updaters.updateSystems import SystemsUpdater

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
