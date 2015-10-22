from sqlTableUpdater import SqlTableUpdater
from sqldatabase import SqlDatabase
from collections import OrderedDict

class SystemsUpdater(SqlTableUpdater):
    '''
        Base methods:
            SqlDatabase.updateTable( self, tableRows=O )
            SqlDatabase.prettifySystem( system="" )
            SqlDatabase.updateColumns( self, sqlDatabase = SqlDatabase() )
    '''

    def __init__(self, tableName="", tableRows=[], coreInfo={}):
        self.infoSystemsMap = {
"Arcade (various)": {"openvgdbSystemName": "", "phoenixSystemName": "Arcade", "defaultCore": "mame_libretro", "isSupported": False},
"Game Boy Advance": {"openvgdbSystemName": "", "phoenixSystemName": "Game Boy Advance", "defaultCore": "vbam_libretro", "isSupported": True},
"Game Boy/Game Boy Color": {"openvgdbSystemName": "", "phoenixSystemName": "Game Boy Color", "defaultCore": "gambatte_libretro", "isSupported": True},
"Lynx": {"openvgdbSystemName": "", "phoenixSystemName": "Atari Lynx", "defaultCore": "mednafen_lynx_libretro", "isSupported": False},
"MULTI (various)": {"openvgdbSystemName": "", "phoenixSystemName": "Arcade", "defaultCore": "mame_libretro", "isSupported": False},
"Neo Geo": {"openvgdbSystemName": "", "phoenixSystemName": "SNK Neo Geo", "defaultCore": "fb_alpha_neo_libretro", "isSupported": False},
"Neo Geo Pocket (Color)": {"openvgdbSystemName": "", "phoenixSystemName": "SNK Neo Geo Pocket Color", "defaultCore": "mednafen_ngp_libretro", "isSupported": False},
"Nintendo Entertainment System": {"openvgdbSystemName": "", "phoenixSystemName": "Nintendo Entertainment System", "defaultCore": "nestopia_libretro", "isSupported": True},
"PC Engine SuperGrafx": {"openvgdbSystemName": "", "phoenixSystemName": "Super Grafx", "defaultCore": "mednafen_supergrafx_libretro", "isSupported": False},
"PC Engine/PCE-CD": {"openvgdbSystemName": "", "phoenixSystemName": "PC Engine CD", "defaultCore": None, "isSupported": False},
"PC-FX": {"openvgdbSystemName": "", "phoenixSystemName": "PC-FX", "defaultCore": "mednafen_pcfx_libretro", "isSupported": False},
"PlayStation": {"openvgdbSystemName": "", "phoenixSystemName": "Sony PlayStation", "defaultCore": "mednafen_psx_libretro", "isSupported": True},
"Saturn": {"openvgdbSystemName": "", "phoenixSystemName": "Sega Saturn", "defaultCore": "yabause_libretro", "isSupported": False},
"Sega 8/16-bit (Various)": {"openvgdbSystemName": "", "phoenixSystemName": "Sega Genesis", "defaultCore": "genesis_plus_gx_libretro", "isSupported": True},
"Sega 8/16-bit + 32X (Various)": {"openvgdbSystemName": "", "phoenixSystemName": "Sega Genesis", "defaultCore": "genesis_plus_gx_libretro", "isSupported": True},
"Sega Master System": {"openvgdbSystemName": "", "phoenixSystemName": "Sega Master System", "defaultCore": None, "isSupported": True},
"Super Nintendo Entertainment System": {"openvgdbSystemName": "", "phoenixSystemName": "Super Nintendo", "defaultCore": "bsnes_mercury_balanced_libretro", "isSupported": True},
"Virtual Boy": {"openvgdbSystemName": "", "phoenixSystemName": "Virtual Boy", "defaultCore": "mednafen_vb_libretro", "isSupported": False},
"WonderSwan/Color": {"openvgdbSystemName": "", "phoenixSystemName": "WonderSwan Color", "defaultCore": "mednafen_wswan_libretro", "isSupported": False},
"ZX Spectrum (various)": {"openvgdbSystemName": "", "phoenixSystemName": "Zx Spectrum", "defaultCore": "fuse_libretro", "isSupported": False},
        }
        if len(tableRows) == 0:
            tableRows = ( ("infoSystemName", "TEXT NOT NULL"),
                          ("phoenixSystemName", "TEXT"),
                          ("openvgdbSystemName", "TEXT"),
                          ("defaultCore", "TEXT REFERENCES cores(core) ON DELETE CASCADE"),
                          ("isSupported", "INTEGER NOT NULL"),
                        )

        SqlTableUpdater.__init__(self, tableName, tableRows, coreInfo)

    #QStringLiteral( " FOREIGN KEY (collectionID) REFERENCES " ) + LibraryInternalDatabase::tableCollections +
     #       QStringLiteral( "(collectionID) ON DELETE CASCADE ON UPDATE CASCADE\n" )

    def updateTable(self):

        foreignKeyStatement = ", FOREIGN KEY (defaultCore) REFERENCES cores(core) ON DELETE CASCADE ON UPDATE CASCADE"
        with SqlDatabase(self.dbFile, autoCommit=True) as db:
            self.updateColumns(db, additionalStatement=foreignKeyStatement)

            systems = []
            for k, v in self.coreInfo['cores'].iteritems():

                if "categories" not in v or v["categories"] != "Emulator":
                    continue

                name = ""
                if "systemname" in v:
                   name = v["systemname"]
                else:
                    if "display_name" in v:
                        name = v["display_name"]

                systems.append( name )

            systems = list(set(systems))
            for s in systems:

                infoMap = self.infoSystemsMap[s]

                openvgdbSystem = infoMap["openvgdbSystemName"]
                phoenixSystem = infoMap["phoenixSystemName"]
                defaultCore = infoMap["defaultCore"]
                isSupported = int(infoMap["isSupported"])
                values = [s, phoenixSystem, openvgdbSystem, defaultCore, isSupported]

                db.insert(self.tableName, 
                self.rowsDict.keys()
                , values, force=False)

if __name__ == "__main__":

    updater = SystemsUpdater(tableName="systems")
    updater.updateTable()






