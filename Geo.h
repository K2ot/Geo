#pragma once

#include <sstream>
#include <iostream>
#include <libpq-fe.h>
#include <random>
#include <iomanip>

const int NO_OF_POINTS = 10;
const int MIN_DISTANCE = 3000;
const int MIN_X = 130000;
const int MAX_X = 790000;
const int MIN_Y = 160000;
const int MAX_Y = 862000;
const int MIN_Z = 0;
const int MAX_Z = 300;

bool isPointInPL(PGconn* conn, int x, int y)
{
	std::stringstream ss;
	ss << "SELECT st_contains(geom, ST_GeomFromText('POINT(" << x << " " << y << ")', 4326)) FROM redlining;"; //test czy jest w obrysie
	std::string query = ss.str();
	PGresult* res = PQexec(conn, query.c_str());

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		std::cerr << "B³¹d zapytania: " << PQerrorMessage(conn) << std::endl;
		PQclear(res);
		return false;
	}
	std::string val = PQgetvalue(res, 0, 0);
	PQclear(res);
	return (val == "t");
}

int RandomNum(std::mt19937& generator, int min, int max)
{
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(generator);
}

bool testDistance(PGconn* conn, int x, int y)
{
	std::stringstream ssTest;
	ssTest << "SELECT * FROM (SELECT ST_Distance(ST_MakePoint(" << x << ", " << y << "), ST_MakePoint(x, y)) FROM punkty_w_pl) as q where st_distance < " << MIN_DISTANCE << ";";
	std::string query = ssTest.str();
	PGresult* resTest = PQexec(conn, query.c_str());
	int ntuples = PQntuples(resTest);
	PQclear(resTest);
	return ntuples == 0;
}

void insertPoint(PGconn* conn, int x, int y, int z)
{
	std::stringstream ssIn;
	ssIn << "INSERT INTO punkty_w_pl (x, y, z) VALUES (" << x << " , " << y << " , " << z << ");";
	
	std::string query = ssIn.str();
	PGresult* resIn = PQexec(conn, query.c_str());

	if (PQresultStatus(resIn) != PGRES_COMMAND_OK) {
		// W razie b³êdu wyœwietlenie informacji o b³êdzie
		std::cerr << "B³¹d wykonania polecenia INSERT: " << PQerrorMessage(conn) << std::endl;
	}
	PQclear(resIn);
}

void showPointByWoj(PGconn* conn)
{
	std::stringstream ssSh;
	ssSh << " SELECT gid, jpt_nazwa_ FROM redlining_woj;";

	std::string query = ssSh.str();
	PGresult* resSh = PQexec(conn, query.c_str());

	//if (PQresultStatus(resSh) != PGRES_COMMAND_OK) {
	//	// W razie b³êdu wyœwietlenie informacji o b³êdzie
	//	std::cerr << "B³¹d wykonania polecenia" << PQerrorMessage(conn) << std::endl;
	//}

	int nWoj = PQntuples(resSh);
	for (size_t i = 0; i < nWoj; i++)
	{
		std::cout << std::endl << PQgetvalue(resSh, i, 1) << std::endl;

		std::stringstream ssSh2;
		ssSh2 << "SELECT x, y ,z FROM punkty_w_pl as punkty, redlining_woj as woj WHERE ";
		ssSh2 << "woj.gid = "<< PQgetvalue(resSh, i, 0 )<<" AND st_contains(woj.geom, ST_GeomFromText('POINT(' || punkty.x || ' ' || punkty.y || ')', 4326)); ";

		std::string query2 = ssSh2.str();		
		PGresult* resSh2 = PQexec(conn, query2.c_str());
		int nPoint = PQntuples(resSh2);
		for(size_t j = 0; j < nPoint; j++)
		{
			std::cout << PQgetvalue(resSh2, j, 0) << " " << PQgetvalue(resSh2, j, 1) << " " << PQgetvalue(resSh2, j, 2) << std::endl;
		}
		PQclear(resSh2);
	}

	PQclear(resSh);
}

void cleanDB(PGconn* conn)
{
	std::stringstream ssCln;
	ssCln << "TRUNCATE TABLE punkty_w_pl;";
	std::string query = ssCln.str();
	PGresult* resCln = PQexec(conn, query.c_str());
	PQclear(resCln);
}

