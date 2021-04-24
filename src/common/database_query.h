#ifndef DATABASE_QUERY_H
#define DATABASE_QUERY_H

#include <QString>

namespace DataBase {

// CREATE TABLE Agents(
//   MAC    	TEXT NOT NULL UNIQUE PRIMARY KEY, 
//   HostName	TEXT NOT NULL UNIQUE,
//   Address	TEXT,
//   Status	INTEGER DEFAULT 0 CHECK(Status IN(0,1))
// );
// CREATE TABLE AgentData(
//   MAC		TEXT NOT NULL UNIQUE,
//   JsonData	TEXT,
//   FOREIGN KEY(MAC) REFERENCES Agents(MAC)
// );


const static char* createDataBase =
      R"(CREATE TABLE IF NOT EXISTS "AgentData" (
            "MAC"        TEXT NOT NULL UNIQUE,
            "JsonData"	TEXT,
            ")PRIMARY KEY("MAC");
        "CREATE TABLE IF NOT EXISTS "Agents" (
            "MAC"        TEXT NOT NULL UNIQUE,
            "HostName"	TEXT NOT NULL UNIQUE,
            "Address"	TEXT,
            "Status"	INTEGER DEFAULT 0 CHECK(Status IN(0,1)),
            "PRIMARY KEY("MAC"));
            )";

const static char* insertAgent = "INSERT INTO Agents (MAC, HostName, Address, Status) VALUES(:mac, :hostname, :address, :status);";

const static char* updateAgent = "UPDATE Agents SET MAC=:mac, Address=:address, Status=:status WHERE HostName=:hostname;";

const static char* deleteAgent = "DELETE FROM Agents WHERE HostName=:hostname;";

const static char* insertAgentData = "INSERT INTO AgentData (MAC, JsonData) VALUES(:mac, :jsondata);";

const static char* updateAgentData = "UPDATE AgentData SET JsonData=:jsondata WHERE MAC=:mac;";

const static char* deleteAgentData = "DELETE FROM AgentData WHERE MAC=:mac;";

}

#endif // DATABASE_QUERY_H
