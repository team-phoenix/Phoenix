from collections import OrderedDict

from updaters.retrieve_core_info import retrieveCoreInfo

from updaters.sqldatabase import SqlDatabase

from updaters.updateSystem import SystemUpdater
from updaters.updateCore import CoreUpdater
from updaters.updateSystemToCore import SystemToCoreUpdater
from updaters.updateExtension import ExtensionUpdater
from updaters.updateFirmware import FirmwareUpdater
from updaters.updateRomHeaderOffset import RomHeaderOffsetUpdater

import os
import json

if __name__ == "__main__":

    info = retrieveCoreInfo()

    system = SystemUpdater(tableName="system", coreInfo=info)
    system.updateTable();

    core = CoreUpdater(tableName="core", coreInfo=info)
    core.updateTable()

    systemToCore = SystemToCoreUpdater(tableName="systemToCore", coreInfo=info)
    systemToCore.updateTable()

    extensions = ExtensionUpdater(tableName="extension", coreInfo=info)
    extensions.updateTable()

    firmware = FirmwareUpdater(tableName="firmware", coreInfo=info)
    firmware.updateTable()

    romHeader = RomHeaderOffsetUpdater(tableName="header", coreInfo=info)
    romHeader.updateTable()