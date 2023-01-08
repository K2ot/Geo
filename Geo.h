#pragma once
#include <sstream>
#include <iostream>
#include <libpq-fe.h>
#include <random>
#include <vector>

const int NO_OF_POINTS = 2000;
const int MIN_DISTANCE = 3000;
const int MIN_X = 130000;
const int MAX_X = 790000;
const int MIN_Y = 160000;
const int MAX_Y = 862000;
const int MIN_Z = 0;
const int MAX_Z = 300;

class Woj
{
	int id_;
	std::string name_;

public:

	Woj(int id, std::string name)
	{
		id_ = id;
		name_ = name;
	}
	~Woj() = default;

	int getId()
	{
		return id_;
	}

	std::string getName()
	{
		return name_;
	}
};

bool isPointInPL(PGconn* conn, int x, int y)
{
	std::stringstream ss;
	ss << "SELECT ST_Contains(geom, ST_GeomFromText('POINT(" << x << " " << y << ")', 4326)) FROM redlining;";
	std::string query = ss.str();
	PGresult* res = PQexec(conn, query.c_str());
	std::string value = PQgetvalue(res, 0, 0);
	PQclear(res);

	return (value == "t");
}

int RandomNum(std::mt19937& generator, int min, int max)
{
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(generator);
}

bool testDistance(PGconn* conn, int  x, int  y) 
{
	std::stringstream ss; 
	ss << "SELECT * FROM (SELECT ST_Distance(ST_MakePoint(" << x << ", " << y << "), ST_MakePoint(x, y)) FROM punkty_w_pl) AS q WHERE ST_Distance < " << MIN_DISTANCE << ";";
	std::string query = ss.str();
	PGresult* res = PQexec(conn, query.c_str()); 
	int ntuples = PQntuples(res);
	PQclear(res);
	return ntuples == 0;
}

void insertPoint(PGconn* conn, int x, int y, int z) 
{
	std::stringstream ss; 
	ss << "INSERT INTO punkty_w_pl (x, y, z) VALUES (" << x << " , " << y << " , " << z << ");";
	
	std::string query = ss.str();
	PGresult* res = PQexec(conn, query.c_str()); 

	PQclear(res);
}

std::vector<Woj> getWoj(PGconn* conn)
{
	std::stringstream ss;
	ss << " SELECT gid, jpt_nazwa_ FROM redlining_woj; ";
	std::string query = ss.str();
	PGresult* res = PQexec(conn, query.c_str());
	int nWoj = PQntuples(res);

	std::vector<Woj> wojs = {};
	for (size_t i = 0; i < nWoj; i++)
	{
		wojs.push_back(Woj(atoi(PQgetvalue(res, i, 0)), PQgetvalue(res, i, 1)));
	}

	PQclear(res);
	return wojs;
}

void printPoints(PGconn* conn, Woj & woj)
{
	std::stringstream ss;
	ss << " SELECT x, y, z FROM punkty_w_pl as punkty, redlining_woj AS woj WHERE ";
	ss << " woj.gid = " << woj.getId() << " AND ST_Contains(woj.geom, ST_GeomFromText('POINT(' || punkty.x || ' ' || punkty.y || ')', 4326)); ";
	std::string query = ss.str();
	PGresult* res = PQexec(conn, query.c_str());
	int nPoint = PQntuples(res);
	std::cout << "     x      y   z [m] " << std::endl;
	for (size_t j = 0; j < nPoint; j++)
	{
		std::cout << PQgetvalue(res, j, 0) << " " << PQgetvalue(res, j, 1) << " " << PQgetvalue(res, j, 2) << std::endl;
	}
	PQclear(res);
}

void showPointsByWoj(PGconn* conn)
{
	std::vector<Woj> wojs = getWoj(conn);
	for (size_t i = 0; i < wojs.size(); i++)
	{
		std::cout << std::endl << wojs[i].getName() << std::endl;
		printPoints(conn, wojs[i]);
	}
}

void cleanDB(PGconn* conn)
{
	std::stringstream ssCln;
	ssCln << "TRUNCATE TABLE punkty_w_pl;";
	std::string query = ssCln.str();
	PGresult* resCln = PQexec(conn, query.c_str());
	PQclear(resCln);
}
