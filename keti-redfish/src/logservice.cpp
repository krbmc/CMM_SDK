#include "logservice.hpp"



void make_test_db()
{
    sqlite3 *db;
    char *err_msg=0;
    char query[500] = {0};
    int rc = sqlite3_open(LOG_DB, &db);

    sprintf(query, "CREATE TABLE Reading(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, SensorID TEXT NOT NULL, Type TEXT NOT NULL, Value REAL, Time TEXT, Location TEXT);");
    sqlite3_exec(db, query, 0, 0, &err_msg);

    sprintf(query, "INSERT INTO Reading (SensorID, Type, Value, Time, Location) VALUES(\"Sensor1\", \"Temperature\", 36.5, \"21-10-29 16:00:05\", \"Chassis\");");
    sqlite3_exec(db, query, 0, 0, &err_msg);

    sprintf(query, "INSERT INTO Reading (SensorID, Type, Value, Time, Location) VALUES(\"Sensor2\", \"Temperature\", 40.8, \"21-10-29 16:11:43\", \"Chassis\");");
    sqlite3_exec(db, query, 0, 0, &err_msg);

    sprintf(query, "INSERT INTO Reading (SensorID, Type, Value, Time, Location) VALUES(\"Sensor6\", \"Fan\", 5000, \"21-10-29 17:01:25\", \"Chassis\");");
    sqlite3_exec(db, query, 0, 0, &err_msg);

    sqlite3_close(db);
    sqlite3_free(err_msg);

    cout << " FIN!! " << endl;

}

int check_db_callback(void *NotUsed, int argc, char **argv, char **azColName) {
  if(argv[0] != NULL)
    table_check = atoi(argv[0]);
  else
    table_check = 0;
  
  return 0;
}

void insert_reading_table(string _sensor_id, string _type, double _value, tm _tm, string _location)
{
    sqlite3 *db;
    char *err_msg=0;
    char query[500] = {0};
    int rc = sqlite3_open(LOG_DB, &db);

    // #1 log.db로 만들었으니까 이 안에 테이블 여러개로 로그관리할거
    // #2 그 안에서 Reading 테이블이 현재 temp랑 fan의 주기적인 기록을 담을거임
    // #3 센서id, 센서타입(fan,temp), 센서측정값(이거땜에 int,double오버로딩), 측정시간(tm), 센서위치
    // #4 insert에서는 우선 table 존재여부 검사하고 없으면 테이블 생성
    // #5 인자로 받은 정보가지고 하나 넣어주면됨


}

