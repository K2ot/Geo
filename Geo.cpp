#include "Geo.h"

const std::string HOST = "localhost";
const std::string USER = "postgres";
const std::string PASSWORD = "1234";
const std::string DBNAME = "polandDB_xyz";

int main()
{
	setlocale(LC_CTYPE, "Polish");
	std::mt19937 generator(time(0));

	std::stringstream connectionString;
	connectionString << "host=" << HOST << " user = " << USER << " password = " << PASSWORD << " dbname = " << DBNAME;
	std::string conne = connectionString.str();
	PGconn* conn = PQconnectdb(conne.c_str());

	if (PQstatus(conn) != CONNECTION_OK)
	{
		std::cerr << "B��d po��czenia z baz� danych: " << PQerrorMessage(conn) << std::endl;
		return 1;
	}

	//cleanDB(conn);

	int i = 0;
	while (i < NO_OF_POINTS)
	{ 
		int x = 0, y = 0, z = 0;
		x = RandomNum(generator, MIN_X, MAX_X);
		y = RandomNum(generator, MIN_Y, MAX_Y);
		z = RandomNum(generator, MIN_Z, MAX_Z);

		if (isPointInPL(conn, x, y) == true)
		{
			if (!testDistance(conn, x, y))
			{
				continue;
			}
			insertPoint(conn, x, y, z);
			i++;
		}
	}
	showPointsByWoj(conn);
	PQfinish(conn);
	return 0;
}
