from collections import OrderedDict
from sqldatabase import SqlDatabase
from retrieve_core_info import retrieveCoreInfo
import os

# Root class that all SQL table updaters derive from
class SqlTableUpdater():

    systemMetadata = {
        "Arcade (various)": {"openvgdbSystemName": "", "phoenixSystemName": "Arcade", "defaultCore": "mame_libretro", "isSupported": False},
        "MULTI (various)": {"openvgdbSystemName": "", "phoenixSystemName": "Arcade", "defaultCore": "mame_libretro", "isSupported": False},
        "Game Boy Advance": {"openvgdbSystemName": "", "phoenixSystemName": "Game Boy Advance", "defaultCore": "vbam_libretro", "isSupported": True},
        "Game Boy/Game Boy Color": {"openvgdbSystemName": "", "phoenixSystemName": "Game Boy Color", "defaultCore": "gambatte_libretro", "isSupported": True},
        "Lynx": {"openvgdbSystemName": "", "phoenixSystemName": "Atari Lynx", "defaultCore": "mednafen_lynx_libretro", "isSupported": False},
        "MULTI (various)": {"openvgdbSystemName": "", "phoenixSystemName": "Arcade", "defaultCore": "mame_libretro", "isSupported": False},
        "Neo Geo Pocket (Color)": {"openvgdbSystemName": "", "phoenixSystemName": "SNK Neo Geo Pocket Color", "defaultCore": "mednafen_ngp_libretro", "isSupported": False},
        "Neo Geo": {"openvgdbSystemName": "", "phoenixSystemName": "SNK Neo Geo", "defaultCore": "fb_alpha_neo_libretro", "isSupported": False},
        "Nintendo Entertainment System": {"openvgdbSystemName": "", "phoenixSystemName": "Nintendo Entertainment System", "defaultCore": "nestopia_libretro", "isSupported": True},
        "PC Engine SuperGrafx": {"openvgdbSystemName": "", "phoenixSystemName": "Super Grafx", "defaultCore": "mednafen_supergrafx_libretro", "isSupported": False},
        "PC Engine/PCE-CD": {"openvgdbSystemName": "", "phoenixSystemName": "PC Engine CD", "defaultCore": "mednafen_pce_fast_libretro", "isSupported": False},
        "PC-FX": {"openvgdbSystemName": "", "phoenixSystemName": "PC-FX", "defaultCore": "mednafen_pcfx_libretro", "isSupported": False},
        "PlayStation": {"openvgdbSystemName": "", "phoenixSystemName": "Sony PlayStation", "defaultCore": "mednafen_psx_libretro", "isSupported": True},
        "Saturn": {"openvgdbSystemName": "", "phoenixSystemName": "Sega Saturn", "defaultCore": "yabause_libretro", "isSupported": False},
        "Sega 8/16-bit (Various)": {"openvgdbSystemName": "", "phoenixSystemName": "Sega Genesis", "defaultCore": "genesis_plus_gx_libretro", "isSupported": True},
        "Sega 8/16-bit + 32X (Various)": {"openvgdbSystemName": "", "phoenixSystemName": "Sega Genesis", "defaultCore": "genesis_plus_gx_libretro", "isSupported": True},
        "Sega Master System": {"openvgdbSystemName": "", "phoenixSystemName": "Sega Master System", "defaultCore": "emux_sms_libretro", "isSupported": True},
        "Super Nintendo Entertainment System": {"openvgdbSystemName": "", "phoenixSystemName": "Super Nintendo", "defaultCore": "bsnes_mercury_balanced_libretro", "isSupported": True},
        "Virtual Boy": {"openvgdbSystemName": "", "phoenixSystemName": "Virtual Boy", "defaultCore": "mednafen_vb_libretro", "isSupported": False},
        "WonderSwan/Color": {"openvgdbSystemName": "", "phoenixSystemName": "WonderSwan Color", "defaultCore": "mednafen_wswan_libretro", "isSupported": False},
        "ZX Spectrum (various)": {"openvgdbSystemName": "", "phoenixSystemName": "Zx Spectrum", "defaultCore": "fuse_libretro", "isSupported": False},
    }

    def __init__(self, tableName, tableColumns=[], coreInfo={}):
        self.tableName = tableName
        self.rowsDict = OrderedDict(tableColumns)
        self.dbFile = os.path.join(os.getcwd().replace("python", "metadata"), "libretro.sqlite")
        self.dbFileExists = os.path.isfile(self.dbFile)
        self.coreInfo = coreInfo

        self.filterUnusedCores()

    def updateTable(self):
        pass

    def libretroToPhoenix(self, v):
        return self.systemMetadata[v["systemname"]]["phoenixSystemName"]

    def filterUnusedCores(self):
        for key in self.coreInfo["cores"].keys():
            if ("4do_libretro" == key 
                 or "81_libretro" == key
                 or "bluemsx_libretro" == key
                 or "bsnes_accuracy_libretro" == key
                 or "bsnes_balanced_libretro" == key
                 or "bsnes_performance_libretro" == key
                 or "cap32_libretro" == key
                 or "catsfc_libretro" == key
                 or "desmume_libretro" == key
                 or "dolphin_libretro" == key
                 or "dosbox_libretro" == key
                 or "emux_chip8_libretro" == key
                 or "fb_alpha_cps1_libretro" == key 
                 or "fb_alpha_cps2_libretro" == key 
                 or "fmsx_libretro" == key
                 or "gpsp_libretro" == key
                 or "gw_libretro" == key
                 or "handy_libretro" == key
                 or "hatari_libretro" == key
                 or "imame4all_libretro" == key
                 or "mame078_libretro" == key
                 or "mame2010_libretro" == key
                 or "mame2014_libretro" == key
                 or "meteor_libretro" == key
                 or "mupen64plus_libretro" == key
                 or "o2em_libretro" == key
                 or "pcsx" in key
                 or "ppsspp_libretro" == key
                 or "prosystem_libretro" == key
                 or "puae_libretro" == key
                 or "stella_libretro" == key
                 or "ume2014_libretro" == key
                 or "vecx_libretro" == key
                 or "virtualjaguar_libretro" == key ):
                del self.coreInfo["cores"][key]

    def updateColumns(self, database, additionalStatement = ""):

        if not self.dbFileExists:
            database.createTable(self.tableName, self.rowsDict, additionalStatement )
        else:
            try:
                database.deleteTable(self.tableName)
            except:
                database.createTable(self.tableName, self.rowsDict, additionalStatement )

    def __del__(self):
        print("Updated " + self.tableName + " table.")

