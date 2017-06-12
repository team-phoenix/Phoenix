from .sqldatabase import SqlDatabase
from .sqlTableUpdater import SqlTableUpdater

class FirmwareUpdater(SqlTableUpdater):

    def __init__(self, tableName: str = None, tableColumns: list = None, coreInfo: dict = None):

        if not tableColumns:
            tableColumns = (
                ("system", "TEXT NOT NULL"),
                ("biosFile", "TEXT NOT NULL UNIQUE"),
                ("sha1", "TEXT"),
                ("md5", "TEXT"),
                ("crc32", "TEXT"),
                ("region", "TEXT")
            )

        if not coreInfo:
            coreInfo = {}

        super().__init__(tableName if tableName else "", tableColumns, coreInfo)

    def updateTable(self):

        with SqlDatabase(self.dbFile, autoCommit=True) as db:
            self.updateColumns(db)

            systems = self.phoenixSystems()

            # System, metadata
            for s, m in systems.items():

                firmware = self.getFirmwareTuple(s)

                if len(firmware) > 0:

                    for item in firmware:
                        bios, sha1, md5, crc32, region = item
                        db.insert(self.tableName, self.columnsDict.keys(), values=[s, bios, sha1, md5, crc32, region], force=True)

    def getFirmwareTuple(self, system):
        if system == "Sony - PlayStation":
            return (
                ("scph5500.bin", "B05DEF971D8EC59F346F2D9AC21FB742E3EB6917", "8dd7d5296a650fac7319bce665a6a53c", "ff3eeb8c", "J"),
                ("scph5501.bin", "0555C6FAE8906F3F09BAF5988F00E55F88E9F30B", "490f666e1afb15b7362b406ed1cea246", "8d8cb7e4", "U"),
                ("scph5502.bin", "F6BC2D1F5EB6593DE7D089C425AC681D6FFFD3F0", "32736f17079d0b2b7024407c39bd3050", "d786f0b9", "E")
            )
        return ()

if __name__ == "__main__":

    updater = FirmwareUpdater(tableName="firmware")
    updater.updateTable()
