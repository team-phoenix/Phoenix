from .sqlTableUpdater import SqlTableUpdater
from .sqldatabase import SqlDatabase
import hashlib


class RomHeaderOffsetUpdater(SqlTableUpdater):

    def __init__(self, tableName: str = None, tableColumns: list = None, coreInfo: dict = None):
        if not tableColumns:
            tableColumns = (
                ("system", "TEXT NOT NULL UNIQUE"),
                ("byteLength", "INTEGER NOT NULL"),
                ("seekIndex", "INTEGER NOT NULL"),
                ("result", "TEXT NOT NULL")
            )

            if not coreInfo:
                coreInfo = {}

        super().__init__(tableName if tableName else "", tableColumns, coreInfo)

    def getHeaderData(self, system):
        if "Sony - PlayStation" == system:
            return (37664, 11, "504c415953544154494f4e")
        if "Nintendo - GameCube" == system:
            return (24, 4, "5D1C9EA3")
        if "Nintendo - Wii" == system:
            return (24, 4, "C2339F3D")
        return ()

    def updateTable(self):

        with SqlDatabase(self.dbFile, autoCommit=True) as db:
            self.updateColumns(db)

            systems = self.phoenixSystems()

            # System, metadata
            for s, m in systems.items():

                offset = self.getHeaderData(s)

                if len(offset) > 0:

                    seek, size, result = offset

                    db.insert(
                        self.tableName,
                        self.columnsDict.keys(),
                        values=[s, size, seek, result],
                        force=True
                    )

if __name__ == "__main__":

    updater = RomHeaderOffsetUpdater(tableName="header")
    updater.updateTable()
