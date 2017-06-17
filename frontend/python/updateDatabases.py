import os
import json
from collections import OrderedDict

from updaters.updateCore import CoreUpdater
from updaters.sqldatabase import SqlDatabase
from updaters.updateSystem import SystemUpdater
from updaters.updateFirmware import FirmwareUpdater
from updaters.updateExtension import ExtensionUpdater
from updaters.retrieve_core_info import retrieveCoreInfo
from updaters.updateSystemToCore import SystemToCoreUpdater
from updaters.updateRomHeaderOffset import RomHeaderOffsetUpdater
from updaters.updateOpenVGDBToSystem import OpenVGDBToSystemUpdater


if __name__ == "__main__":

    info = retrieveCoreInfo()

    system = SystemUpdater(tableName="system", coreInfo=info)
    system.updateTable()

    core = CoreUpdater(tableName="core", coreInfo=info)
    core.updateTable()

    systemToCore = SystemToCoreUpdater(tableName="systemToCore", coreInfo=info)
    systemToCore.updateTable()

    openVGDBToSystem = OpenVGDBToSystemUpdater(tableName="openVGDBToSystem", coreInfo=info)
    openVGDBToSystem.updateTable()

    extensions = ExtensionUpdater(tableName="extension", coreInfo=info)
    extensions.updateTable()

    firmware = FirmwareUpdater(tableName="firmware", coreInfo=info)
    firmware.updateTable()

    romHeader = RomHeaderOffsetUpdater(tableName="header", coreInfo=info)
    romHeader.updateTable()
