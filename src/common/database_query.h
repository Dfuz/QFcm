#ifndef DATABASE_QUERY_H
#define DATABASE_QUERY_H

#include <QString>

namespace DataBase {

const static QString createDataBase =
R"(
CREATE TABLE IF NOT EXISTS "Agents" (
    "HostName"  TEXT NOT NULL UNIQUE,
    "Address"	TEXT,
    "Status"	INTEGER DEFAULT 0 CHECK(Status IN (0,1)),
    "MAC"       TEXT UNIQUE,
    PRIMARY KEY("MAC"));
CREATE TABLE IF NOT EXISTS "AgentData" (
    "HostName"  TEXT NOT NULL,
    "KeyData"   TEXT,
    "Clock"     INTEGER,
    "Value"     TEXT,
    FOREIGN KEY("HostName") REFERENCES "Agents"("HostName"));)";

const static QString foreignKeysOn = "PRAGMA foreign_keys = ON;";

const static QString insertAgent = "INSERT INTO Agents (MAC, HostName, Address, Status) VALUES('%1', '%2', '%3', %4);";

const static QString updateAgent = "UPDATE Agents SET MAC=%1, Address=%2, Status=%3 WHERE HostName=%4;";

const static QString deleteAgent = "DELETE FROM Agents WHERE HostName=%1;";

const static QString insertAgentData = "INSERT INTO AgentData (HostName, KeyData, Clock, Value) VALUES('%1', '%2', %3, '%4');";

const static QString selectAllAgents = "SELECT MAC, HostName, Address, Status FROM Agents;";

const static QString checkAgentExists = "SELECT HostName FROM Agents WHERE HostName='%1'";

const static QString updateAgentData = "UPDATE AgentData SET JsonData=:jsondata WHERE MAC=:mac;";

const static QString deleteAgentData = "DELETE FROM AgentData WHERE MAC=:mac;";


}

#endif // DATABASE_QUERY_H
