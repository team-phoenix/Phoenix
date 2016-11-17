from sqldatabase import SqlDatabase
from sqlTableUpdater import SqlTableUpdater

from .updateSystemToCore import SystemToCoreUpdater


class OpenVGDBToSystemUpdater(SqlTableUpdater):

    def __init__(self, tableName: str = None, tableColumns: list = None, coreInfo: dict = None):
        if not tableColumns:
            tableColumns = (
                ("openvgdbSystemName", "TEXT"),
                ("systemUUID", "TEXT"),
            )
        if not coreInfo:
            coreInfo = {}
        super().__init__(tableName if tableName else "", tableColumns, coreInfo)

    def updateTable(self):
        with SqlDatabase(self.dbFile, autoCommit=True) as db:
            self.updateColumns(db)

            openVGDBToSystemMap = self.getOpenVGDBToPhoenixMap()

            # key, value
            for k, v in openVGDBToSystemMap.iteritems():
                values = [k, v]
                db.insert(self.tableName, self.columnsDict.keys(), values, force=False)

if __name__ == "__main__":

    from collections import OrderedDict

    updater = SystemToCoreUpdater("systemToCore", columns)
    updater.updateTable()
