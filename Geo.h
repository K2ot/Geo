#pragma once

#include <sstream>
#include <iostream>
#include <libpq-fe.h>
#include <random>
#include <iomanip>

const int NO_OF_POINTS = 5000;
const int MIN_DISTANCE = 3000;
const int MIN_X = 130000;
const int MAX_X = 790000;
const int MIN_Y = 160000;
const int MAX_Y = 862000;
const int MIN_Z = 0;
const int MAX_Z = 300;

bool isPointInPL(PGconn* conn, double& x, double& y)
{
	std::stringstream ss;
	ss << "SELECT st_contains(geom, ST_GeomFromText('POINT(" << x << " " << y << ")', 4326)) FROM redlining;"; //test czy jest w obrysie
	std::string query = ss.str();
	PGresult* res = PQexec(conn, query.c_str());

	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		std::cerr << "B³¹d zapytania: " << PQerrorMessage(conn) << std::endl;
		PQclear(res);
		return 1;
	}
	//
	char* value = PQgetvalue(res, 0, 0);
	PQclear(res);
	return (strcmp(value, "f"));
}

double RandomNum(std::mt19937& generator, int min, int max)
{
	min *= 100;
	max *= 100;
	std::uniform_int_distribution<int> distribution(min, max);
	double number = 0.0;
	number = distribution(generator);
	return number / 100;
}

bool testDistance(PGconn* conn, double& x, double& y) // testowanie czy mamy 3000 m odleg³oœci 
{
	std::stringstream ssTest;
	ssTest << "SELECT * from (SELECT ST_Distance(ST_MakePoint(" << x << ", " << y << "), ST_MakePoint(x, y)) FROM punkty_w_pl) as q where st_distance < " << MIN_DISTANCE << ";";
	std::string query = ssTest.str();
	PGresult* resTest = PQexec(conn, query.c_str());
	int ntuples = PQntuples(resTest);
	PQclear(resTest);
	return ntuples == 0;
}

void insertPoint(PGconn* conn, double& x, double& y, double& z)
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
	ssSh << " select woj.jpt_nazwa_, punkty.* from punkty_w_pl punkty, redlining_woj woj where ";
	ssSh << "woj.jpt_nazwa_ =  and st_contains(woj.geom, ST_GeomFromText('POINT(' || punkty.x || ' ' || punkty.y || ')', 4326)); ";
	std::string query = ssSh.str();
	PGresult* resSh = PQexec(conn, query.c_str());

	int ntuples = PQntuples(resSh);
	for (size_t i = 0; i < 4; i++)
	{
		
	PQgetvalue(resSh, i, 0);
	}
	
	;

	PQclear(resSh);
}
