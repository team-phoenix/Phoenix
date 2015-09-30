from collections import OrderedDict
from sqldatabase import SqlDatabase
from retrieve_core_info import retrieveCoreInfo
import os

class SqlTableUpdater():

    def __init__(self, tableName, tableRows=[], coreInfo={}):
        self.tableName = tableName
        self.rowsDict = OrderedDict(tableRows)
        self.dbFile = os.path.join(os.getcwd().replace("python", "database"), "systems.db")
        self.dbFileExists = os.path.isfile(self.dbFile)
        
        if len(coreInfo) == 0:
            self.coreInfo = retrieveCoreInfo()
        else:
            self.coreInfo = coreInfo

    def updateTable(self):
        pass

    def prettifySystem(self, platform):
        system = platform.lower()
        if "colecovision" in system:
            return "ColecoVision"
        elif "gamecube" in system:
            return "GameCube (Wii)"
        elif "game boy" in system:
            return "Game Boy (Color)"
        elif "sega 8/16" in system:
            return "Sega 8 & 16 Bit"
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
        return platform

    def updateColumns(self, database):

        if not self.dbFileExists:
            database.createTable(self.tableName, self.rowsDict )
        else:
            try:
                database.deleteTable(self.tableName)
            except:
                database.createTable(self.tableName, self.rowsDict )

    def __del__(self):
        print("Updated " + self.tableName + " table.")

