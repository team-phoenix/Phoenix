from collections import OrderedDict
from sqldatabase import SqlDatabase
from retrieve_core_info import retrieveCoreInfo
import os

class SqlTableUpdater():

    def __init__(self, tableName, tableRows=[], coreInfo={}):
        self.tableName = tableName
        self.rowsDict = OrderedDict(tableRows)
        self.dbFile = os.path.join(os.getcwd().replace("python", "metadata"), "libretro.sqlite")
        self.dbFileExists = os.path.isfile(self.dbFile)
        
        if len(coreInfo) == 0:
            self.coreInfo = retrieveCoreInfo()
        else:
            self.coreInfo = coreInfo

        self.filterSystems()

        # This map needs to have exactly the same keys that the
        # prettifySystem() function creates. Otherwise there will be a key error
        # when the keys are indexed. This is supposed to happen though.
        self.defaultCoreMap = {
            "Nintendo Entertainment System": "nestopia_libretro",
            "Super Nintendo": "bsnes_mercury_balanced_libretro",
            "Sony PlayStation": "mednafen_psx_libretro",
            "Game Boy Color": "gambatte_libretro",
            "Game Boy Advance": "vbam_libretro",
            "Sega Genesis": "genesis_plus_gx_libretro",
            "Sega Saturn": "yabause_libretro",
            "Arcade": "mame_libretro",
            "Neo Geo": "fb_alpha_neo_libretro",
            "Atari Lynx": "mednafen_lynx_libretro",
            "Neo Geo Pocket (Color)": "mednafen_ngp_libretro",
            "PC Engine": "mednafen_pce_fast_libretro",
            "PC Engine SuperGrafx": "mednafen_supergrafx_libretro",
            "PC-FX": "mednafen_pcfx_libretro",
            "Virtual Boy": "mednafen_vb_libretro",
            "WonderSwan (Color)": "mednafen_wswan_libretro",
            "Zx Spectrum": "fuse_libretro",
        }

    def updateTable(self):
        pass

    def filterSystems(self):
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

    def prettifySystem(self, platform):
        system = platform.lower()
        if "colecovision" in system:
            return "ColecoVision"
        elif "gamecube" in system:
            return "GameCube (Wii)"
        elif "game boy" in system and "color" in system:
            return "Game Boy Color"
        elif "sega 8/16" in system or "sega master system" == system:
            return "Sega Genesis"
        elif "atari st/ste" in system:
            return "Atari ST"
        elif "pc engine/pce" in system:
            return "PC Engine"
        elif "multi" in system:
            return "Arcade"
        elif "(various)" in system:
            return platform.replace(" (various)", "").title()
        elif "/ videopac+" in system:
            return " & ".join(platform.split(" / "))
        elif "saturn" == system:
            return "Sega Saturn"
        elif "playstation" == system:
            return "Sony PlayStation"
        elif "super nintendo" in system:
            return "Super Nintendo"
        elif "nintendo entertainment system" == system:
            return "Nintendo Entertainment System"
        elif "lynx" == system:
            return "Atari Lynx"
        elif "wonderswan/color" == system:
            return "WonderSwan (Color)"
        return platform

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

