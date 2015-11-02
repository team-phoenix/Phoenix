from sqlTableUpdater import SqlTableUpdater
from sqldatabase import SqlDatabase
from collections import OrderedDict

class SystemUpdater(SqlTableUpdater):

    def __init__(self, tableName="", tableColumns=[], coreInfo={}):
        if len(tableColumns) == 0:
            tableColumns = ( ("UUID", "TEXT PRIMARY KEY"),
                             ("enabled", "INTEGER NOT NULL"),
                             ("defaultCore", "TEXT"),
                             ("friendlyName", "TEXT"),
                             ("shortName", "TEXT"),
                             ("manufacturer", "TEXT"),
                             ("openvgdbSystemName", "TEXT"),
                           )

        SqlTableUpdater.__init__(self, tableName, tableColumns, coreInfo)

    def updateTable(self):

        with SqlDatabase(self.dbFile, autoCommit=True) as db:
            self.updateColumns(db)

            systems = self.phoenixSystems()

            # System, metadata
            for s, m in systems.iteritems():

                system = s
                enabled = int(m["enabled"])
                defaultCore = m["defaultCore"]
                friendlyName = m["friendlyName"]
                shortName = m["shortName"]
                manufacturer = m["manufacturer"]
                openvgdbSystemName = self.phoenixToOpenVGDB(s)

                values = [system, enabled, defaultCore, friendlyName, shortName, manufacturer, openvgdbSystemName]

                db.insert(self.tableName, self.columnsDict.keys(), values, force=False)

if __name__ == "__main__":

    updater = SystemUpdater(tableName="system")
    updater.updateTable()






