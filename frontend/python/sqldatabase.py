import sqlite3

class SqlDatabase():

    def __init__(self, filePath, autoCommit=True):
        self.filePath = filePath
        self.connection = None
        self.cursor = None
        self.autoCommit = autoCommit

    def createTable(self, tableName, rowsAndTypes={}):
        if len(rowsAndTypes) == 0:
            return False

        statement = "CREATE TABLE {} ( {} )".format(tableName
                      , ", ".join(
                      [k + " " + v for k, v in rowsAndTypes.iteritems()]))

        self.cursor.execute( statement )

        if self.autoCommit:
            self.commit()

        return True

    def open(self, filePath):
        self.connection = sqlite3.connect( filePath )
        self.cursor = self.connection.cursor()

    def close(self):
        self.connection.close()
        self.connection = None
        self.cursor = None

    def commit(self):
        self.connection.commit()

    def execute(self, table="", rows=[], values=[]):
        if len(values) == 0 or table == "":
            return False

        preStatement = "INSERT INTO {}".format(table)
        rowStatement = " ({}) ".format(",".join(i for i in rows))
        fullStatement = preStatement + rowStatement + "VALUES ({})".format(",".join("?" for i in values))
        
        #statement = preStatement + " ({}) VALUES ({})".format(table
         #            , ", ".join(rows)
          #           , ", ".join(values))

        #print(fullStatement)

        self.cursor.execute( fullStatement, values )

        if self.autoCommit:
            self.commit()

        return True

    def __enter__(self):
        self.open(self.filePath)
        return self

    def __exit__(self, type, value, traceback):
        if self.autoCommit:
            self.commit()
        self.close()
