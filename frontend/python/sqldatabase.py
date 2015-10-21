import sqlite3

class SqlDatabase():

    def __init__(self, filePath, autoCommit=True):
        '''Initializes the class.
            filePath = absolute file path to the SQL database,
            if autoCommit is True, this class will automatically commit 
            executed transactions. This goes for createTable() and execute()
        '''
        self.filePath = filePath
        self.connection = None
        self.cursor = None
        self.autoCommit = autoCommit

    def createTable(self, tableName, rowsAndTypes={}, additionalStatement=""):
        ''' Helper function for creating a new SQL table.
            Just pass it the table's name, and a dictionary containing
            the table rows and keys and the types as the values.
            e.g.
                { "rowIndex": "INTEGER NOT NULL" }
        '''
        if len(rowsAndTypes) == 0:
            return False

        statement = "CREATE TABLE {} ({} {})".format(tableName
                      , ", ".join(
                      [k + " " + v for k, v in rowsAndTypes.iteritems()])
                      , additionalStatement)

        print(statement)

        self.cursor.execute( statement )

        if self.autoCommit:
            self.commit()

        return True

    def open(self, filePath):
        '''Opens a connection to the SQL database.'''
        self.connection = sqlite3.connect( filePath )
        self.cursor = self.connection.cursor()

    def close(self):
        '''Closes the SQL database, setting the connection and cursour to None.'''
        self.connection.close()
        self.connection = None
        self.cursor = None

    def commit(self):
        '''Commits changes to the SQL database.'''
        self.connection.commit()

    def insert(self, table="", rows=[], values=[], force=False):
        '''Executes a SQL insert command and returns True upon success.
           If 'force' is 'True', this makes the insert statement ignore
           duplicate key errors, effectively ignoring them.
        '''
        if len(values) == 0 or table == "":
            return False
        
        preStatement = ""
        if force:
            preStatement = "INSERT OR IGNORE INTO {}".format(table)
        else:
            preStatement = "INSERT INTO {}".format(table)

        rowStatement = " ({}) ".format(",".join(i for i in rows))
        fullStatement = preStatement + rowStatement + "VALUES ({})".format(",".join("?" for i in values))
        
        #print( values )
        self.cursor.execute( fullStatement, values )

        if self.autoCommit:
            self.commit()

        return True

    def execute(self, table="", rows=[], values=[]):
        '''Executes a SQL statement and returns True upon success.'''
        if len(values) == 0 or table == "":
            return False

        preStatement = "INSERT OR IGNORE INTO {}".format(table)
        rowStatement = " ({}) ".format(",".join(i for i in rows))
        fullStatement = preStatement + rowStatement + "VALUES ({})".format(",".join("?" for i in values))
        self.cursor.execute( fullStatement, values )

        if self.autoCommit:
            self.commit()

        return True

    def deleteTable(self, table=""):
        self.cursor.execute("DELETE FROM " + table ) 

    def __enter__(self):
        '''Opens the SQL database when using a "with" statement.'''
        self.open(self.filePath)
        return self

    def __exit__(self, type, value, traceback):
        '''Closes the SQL database when using a "with" statement.'''
        if self.autoCommit:
            self.commit()
        self.close()

