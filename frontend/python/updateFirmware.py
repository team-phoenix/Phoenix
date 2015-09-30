from sqldatabase import SqlDatabase
from sqlTableUpdater import SqlTableUpdater
import hashlib

class FirmwareUpdater(SqlTableUpdater):
    '''
        Base methods:
            SqlDatabase.updateTable( self, tableRows=O )
            SqlDatabase.prettifySystem( system="" )
            SqlDatabase.updateColumns( self, sqlDatabase = SqlDatabase() )
    '''

    def __init__(self, tableName="", tableRows=[], coreInfo={}):

        if len(tableRows) == 0:
            tableRows = ( ("systemIndex", "TEXT NOT NULL")
                        , ("biosFile", "TEXT NOT NULL UNIQUE")
                        , ("sha1", "TEXT")
                        , ("md5", "TEXT")
                        , ("region", "TEXT") )

        SqlTableUpdater.__init__(self, tableName, tableRows, coreInfo)

    def updateTable(self):

        with SqlDatabase(self.dbFile, autoCommit=True) as db:
            self.updateColumns(db)

            for k, v in self.coreInfo['cores'].iteritems():

                if "categories" not in v or v["categories"] != "Emulator":
                    continue

                name = ""
                if "systemname" in v:
                   name = v["systemname"]
                else:
                    if "display_name" in v:
                        name = v["display_name"]

                name = self.prettifySystem(name)
                firmware = self.getFirmwareTuple(name)

                if len(firmware) > 0:

                    h = hashlib.sha1(name).hexdigest()

                    for item in firmware:
                        bios, sha1, md5, region = item
                        db.insert(self.tableName, self.rowsDict.keys(), values=[h, bios, sha1, md5, region], force=True)

    def getFirmwareTuple(self, system):
        if system == "Sony PlayStation":
            return (
                      ("scph5500.bin", None, "8dd7d5296a650fac7319bce665a6a53c", "J")
                    , ("scph5501.bin", None, "490f666e1afb15b7362b406ed1cea246", "U")
                    , ("scph5502.bin", None, "32736f17079d0b2b7024407c39bd3050", "E")
                   )
        return ()

if __name__ == "__main__":

    updater = FirmwareUpdater(tableName="firmware" )
    updater.updateTable()







