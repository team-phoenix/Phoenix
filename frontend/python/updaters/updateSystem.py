from sqlTableUpdater import SqlTableUpdater
from sqldatabase import SqlDatabase
from collections import OrderedDict


class SystemUpdater(SqlTableUpdater):

    def __init__(self, tableName: str = None, tableColumns: list = None, coreInfo: dict = None):
        if not tableColumns:
            tableColumns = (
                ("UUID", "TEXT PRIMARY KEY"),
                ("enabled", "INTEGER NOT NULL"),
                ("defaultCore", "TEXT"),
                ("friendlyName", "TEXT"),
                ("shortName", "TEXT"),
                ("manufacturer", "TEXT"),
                ("openvgdbSystemName", "TEXT"),
            )

        if not coreInfo:
            coreInfo = {}

        super().__init__(tableName if tableName else "", tableColumns, coreInfo)

    def updateTable(self):

        with SqlDatabase(self.dbFile, autoCommit=True) as db:
            self.updateColumns(db)

            systems = self.phoenixSystems()

            for system, metadata in systems.iteritems():

                enabled = int(metadata["enabled"])
                defaultCore = metadata["defaultCore"]
                friendlyName = metadata["friendlyName"]
                shortName = metadata["shortName"]
                manufacturer = metadata["manufacturer"]
                openvgdbSystemName = self.phoenixToOpenVGDB(system)

                values = [system, enabled, defaultCore, friendlyName, shortName, manufacturer, openvgdbSystemName]

                db.insert(self.tableName, self.columnsDict.keys(), values, force=False)

if __name__ == "__main__":

    updater = SystemUpdater(tableName="system")
    updater.updateTable()
