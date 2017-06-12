import sqlite3

class SqlDatabase(object):

    def __init__(self, filePath: str, autoCommit: bool = True):
        '''Initializes the class.
            filePath = absolute file path to the SQL database,
            if autoCommit is True, this class will automatically commit
            executed transactions. This goes for createTable() and execute()
        '''
        self.filePath = filePath
        self.connection = None
        self.cursor = None
        self.autoCommit = autoCommit

    def createTable(self, tableName, rowsAndTypes: dict = None, additionalStatement: str = ""):
        ''' Helper function for creating a new SQL table.
            Just pass it the table's name, and a dictionary containing
            the table rows and keys and the types as the values.
            e.g.
                { "rowIndex": "INTEGER NOT NULL" }
        '''
        if not rowsAndTypes:
            return False

        statement = "CREATE TABLE {} ({} {})".format(tableName, ", ".join([k + " " + v for k, v in rowsAndTypes.items()]), additionalStatement)

        print(statement)

        self.cursor.execute(statement)

        if self.autoCommit:
            self.commit()

        return True

    def open(self, filePath: str):
        '''Opens a connection to the SQL database.'''
        self.connection = sqlite3.connect(filePath)
        self.cursor = self.connection.cursor()

    def close(self):
        '''Closes the SQL database, setting the connection and cursour to None.'''
        self.connection.close()
        self.connection = None
        self.cursor = None

    def commit(self):
        '''Commits changes to the SQL database.'''
        self.connection.commit()

    def insert(self, table: str = None, rows: list = None, values: list = None, force=False):
        '''Executes a SQL insert command and returns True upon success.
           If 'force' is 'True', this makes the insert statement ignore
           duplicate key errors, effectively ignoring them.
        '''
        if not values or not table:
            return False

        preStatement = None

        if force:
            preStatement = "INSERT OR IGNORE INTO {}".format(table)
        else:
            preStatement = "INSERT INTO {}".format(table)

        rowStatement = " ({}) ".format(",".join(i for i in rows))
        fullStatement = preStatement + rowStatement + "VALUES ({})".format(",".join("?" for i in values))
        # print(fullStatement, values)

        self.cursor.execute(fullStatement, values)

        if self.autoCommit:
            self.commit()

        return True

    def execute(self, table: str = None, rows: list = None, values: list = None):
        '''Executes a SQL statement and returns True upon success.'''
        # Avoid mutable defaults
        if not rows:
            rows = []

        if not values:
            values = []

        if not values or not table:
            return False

        preStatement = "INSERT OR IGNORE INTO {}".format(table)
        rowStatement = " ({}) ".format(",".join(i for i in rows))
        fullStatement = preStatement + rowStatement + "VALUES ({})".format(",".join("?" for i in values))
        self.cursor.execute(fullStatement, values)

        if self.autoCommit:
            self.commit()

        return True

    def deleteTable(self, table: str = None):
        self.cursor.execute("DELETE FROM " + table if table else "")

    def __enter__(self):
        '''Opens the SQL database when using a "with" statement.'''
        self.open(self.filePath)
        return self

    def __exit__(self, type, value, traceback):
        '''Closes the SQL database when using a "with" statement.'''
        if self.autoCommit:
            self.commit()
        self.close()
