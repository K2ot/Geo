#include "Geo.h"

const std::string HOST = "localhost";
const std::string USER = "postgres";
const std::string PASSWORD = "1234";
const std::string DBNAME = "polandDB_xyz";




int main()
{
	setlocale(LC_CTYPE, "Polish");

	std::stringstream connectionString;
	connectionString << "host=" << HOST << " user = " << USER << " password = " << PASSWORD << " dbname = " << DBNAME;
	std::string conne = connectionString.str();
	PGconn* conn = PQconnectdb(conne.c_str());

	if (PQstatus(conn) != CONNECTION_OK)
	{
		std::cerr << "B³¹d po³¹czenia z baz¹ danych: " << PQerrorMessage(conn) << std::endl;
		return 1;
	}

	//cleanDB(conn);

	int i = 0;
	while (i < NO_OF_POINTS)
	{ 
		
		Point xyz = Point::randomPoint();
		if (isPointInPL(conn, xyz) == true)
		{
			if (!testDistance(conn, xyz))
			{
				continue;
			}
			insertPoint(conn, xyz);
			i++;
		}
	}
	showPointsByWoj(conn);
	PQfinish(conn);
	return 0;
}
