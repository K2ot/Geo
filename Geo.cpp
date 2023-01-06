#include "Geo.h"

int main()
{
	
	setlocale(LC_CTYPE, "Polish");
	std::mt19937 generator(time(0));

	// £¹czenie z baz¹ danych
	PGconn* conn = PQconnectdb("host=localhost user=postgres password=1234 dbname=polandDB_xyz");

	// Sprawdzanie statusu po³¹czenia
	if (PQstatus(conn) != CONNECTION_OK)
	{
		std::cerr << "B³¹d po³¹czenia z baz¹ danych: " << PQerrorMessage(conn) << std::endl;
		return 1;
	}
	
	cleanDB(conn);

	
	int i = 0, x = 0, y = 0, z = 0;
	while (i < NO_OF_POINTS)
	{
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

	showPointByWoj(conn);

	PQfinish(conn);
	return 0;
}
