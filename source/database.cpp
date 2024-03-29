#include <kpl/database.h>

namespace kpl {
    namespace sqlite {

        sqlite3 * openSqliteDatabase(std::string databasePath)
        {
            std::cout << "Attempting to create db at " << databasePath << std::endl;

            sqlite3 * result;
            if( sqlite3_open(databasePath.c_str(), &result) == 0 )
                return result;

            std::cout << "Failed to open " << databasePath << std::endl;

            return nullptr;
        }

        bool createDatabaseTablesFromFile(sqlite3 * database, std::string filePath)
        {
            std::string createSqlString = kpl::loadTextFile(filePath);

            std::cout << "Executing --> " + createSqlString << std::endl;

            if(SQLITE_OK != sqlite3_exec(database, createSqlString.c_str(), nullptr, nullptr, nullptr))
            {
                std::cout << "Failed to execute sql" << std::endl;
                return false;
            }

            return true;
        }

        int selectCountCallback(void *data, int numCols, char **rowVals, char **rowHeadings)
        {
            (void)numCols;
            (void)rowHeadings;

            uint16_t * result = static_cast<uint16_t*>(data);
            *result = static_cast<uint16_t>(std::atoi( *(rowVals + 0) ));

            return 0;
        }

        int count(sqlite3 * database, const std::string& tableName, const std::vector<WhereClause>& whereClauses)
        {
            std::string sqlCommand = "SELECT COUNT(*) FROM " + tableName;

            if(! whereClauses.empty())
            {
                sqlCommand += " WHERE ";

                for(const WhereClause& whereClause : whereClauses)
                    sqlCommand += whereClause.toString() + " AND ";

                sqlCommand = sqlCommand.erase(sqlCommand.size() - 4);
            }

            sqlCommand += ";";

            std::cout << "Executing --> " << sqlCommand << std::endl;

            char* errMessage;
            int count = 0;

            int returnCode = sqlite3_exec(database, sqlCommand.c_str(), selectCountCallback, &count, &errMessage);

            if( returnCode != SQLITE_OK )
            {
                std::cout << "Error: Count failed " + tableName << std::endl;
                std::cout << "Message --> " << errMessage << std::endl;
                return -1;
            }

            return count;
        }

        bool select(sqlite3 * database, const std::string& tableName, const std::vector<std::string>& selectFields, int (*callback)(void*,int,char**,char**), void* callbackData)
        {
            if(database == nullptr)
            {
                std::cout << "Error: database handle passed to select is null" << std::endl;
                return false;
            }

            if(selectFields.empty())
            {
                std::cout << "Error: No select fields passed to select statement" << std::endl;
                return false;
            }

            std::string statement = "SELECT ";

            for(const std::string& field : selectFields)
                statement += field + ",";

            // Remove trailing comma
            statement.erase( statement.end() - 1 );

            statement += " FROM " + tableName + ";";

            std::cout << "Executing --> " << statement << std::endl;

            char* errMessage;

            int returnCode = sqlite3_exec(database, statement.c_str(), callback, callbackData, &errMessage);

            if( returnCode != SQLITE_OK )
            {
                std::cout << "Error: Failed to select from " + tableName << std::endl;
                std::cout << "Message --> " << errMessage << std::endl;
                return false;
            }

            return true;
        }

        bool select( sqlite3 * database , const std::string& tableName, const std::vector<std::string>& selectFields,
                     int (*callback)(void*,int,char**,char**), void *callbackData, const std::vector<WhereClause>& whereClauses )
        {
            if(database == nullptr)
            {
                std::cout << "Error: database handle passed to select is null" << std::endl;
                return false;
            }

            if(selectFields.empty())
            {
                std::cout << "Error: No select fields passed to select statement" << std::endl;
                return false;
            }

            if(whereClauses.empty())
            {
                std::cout << "Error: Empty whereClause array passed to select statement" << std::endl;
                return false;
            }

            std::string statement = "SELECT ";

            for(const std::string& field : selectFields)
                statement += field + ",";

            // Remove trailing comma
            statement.erase( statement.end() - 1 );

            statement += " FROM " + tableName + " WHERE ";

            for(const WhereClause& whereClause : whereClauses)
                statement += whereClause.toString() + " AND ";

            statement = statement.erase(statement.size() - 4); // Remove trailing AND
            statement += ";";

            std::cout << "Executing --> " << statement << std::endl;

            char* errMessage;

            int returnCode = sqlite3_exec(database, statement.c_str(), callback, callbackData, &errMessage);

            if( returnCode != SQLITE_OK )
            {
                std::cout << "Error: Failed to select from " + tableName << std::endl;
                std::cout << "Message --> " << errMessage << std::endl;
                return false;
            }

            return true;
        }

        int insertCallback(void *NotUsed, int argc, char **argv, char **azColName)
        {
            (void)NotUsed;

            for(int i = 0; i < argc; i++)
                printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");

            return 0;
        }

        bool insertIfUnique(sqlite3 * database, const std::string& tableName, const std::vector<std::pair<std::string, std::string>>& parameters)
        {
            std::vector<WhereClause> whereClauses;

            for(const std::pair<std::string, std::string>& parameter : parameters)
                whereClauses.push_back( WhereClause::makeEqualsClause(parameter.first, parameter.second) );

            if(count(database, tableName, whereClauses) > 0)
                return true;

            return insertInto(database, tableName, parameters);
        }

        bool insertInto(sqlite3 * database, const std::string& tableName, const std::vector<std::pair<std::string, std::string>>& parameters)
        {
            if(parameters.size() == 0 || database == nullptr || tableName.empty())
                return false;

            std::string sqlStatement = "INSERT INTO " + tableName + " (";

            /* Specify columns */
            for(auto attribute : parameters)
                sqlStatement += attribute.first + ",";

            // Remove trailing comma
            sqlStatement.erase( sqlStatement.end() - 1 );

            sqlStatement += ") VALUES (";

            /* Supply insert data */
            for(auto attribute : parameters)
                sqlStatement += attribute.second + ",";

            // Remove trailing comma
            sqlStatement.erase( sqlStatement.end() - 1 );

            sqlStatement += ");";

            std::cout << "Executing: " + sqlStatement << std::endl;

            char * errMessage;
            int returnCode = sqlite3_exec(database, sqlStatement.c_str(), insertCallback, nullptr, &errMessage);

            if(returnCode != SQLITE_OK)
            {
                std::cout << "An error occurred while trying to insert into " + tableName + " --> " << errMessage << std::endl;
                return false;
            }

            return true;
        }
    }
}

