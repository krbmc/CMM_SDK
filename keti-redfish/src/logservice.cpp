#include "logservice.hpp"



void make_test_db()
{
    sqlite3 *db;
    char *err_msg=0;
    char query[500] = {0};
    int rc = sqlite3_open(LOG_DB, &db);

    sprintf(query, "CREATE TABLE Reading(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, SensorID CHAR (50) NOT NULL, Type CHAR(50) NOT NULL, Value REAL, Time CHAR(60), Location CHAR(50));");
    sqlite3_exec(db, query, 0, 0, &err_msg);

    sprintf(query, "INSERT INTO Reading (SensorID, Type, Value, Time, Location) VALUES(\"Sensornumber123\", \"Temperature\", 36.5, \"2021-10-29 16:00:05\", \"Chassis\");");
    sqlite3_exec(db, query, 0, 0, &err_msg);

    sprintf(query, "INSERT INTO Reading (SensorID, Type, Value, Time, Location) VALUES(\"Sensornumber123456\", \"Temperature\", 40.8, \"2021-10-29 16:11:43\", \"Chassis\");");
    sqlite3_exec(db, query, 0, 0, &err_msg);

    sprintf(query, "INSERT INTO Reading (SensorID, Type, Value, Time, Location) VALUES(\"Sensornumber00700\", \"Fan\", 5000, \"2021-10-29 17:01:25\", \"Chassis\");");
    sqlite3_exec(db, query, 0, 0, &err_msg);

    sqlite3_close(db);
    sqlite3_free(err_msg);

    cout << " FIN!! " << endl;

}

int check_db_callback(void *NotUsed, int argc, char **argv, char **azColName)
{
  if(argv[0] != NULL){
    *((int *)NotUsed) = atoi(argv[0]);
  }
  
  return 0;
}

int result_callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    NotUsed = 0;

    for(int i=0; i<argc; i++)
    {
        // cout << "현재 i : " << i << endl;
        cout << azColName[i] << " = " << argv[i] << endl;
    }

    return 0;
}

// void insert_reading_table(string _sensor_id, string _type, int _value, tm _tm, string _location)
void insert_reading_table(string _sensor_id, string _type, int _value, string _time, string _location)
{
    sqlite3 *db;
    char *err_msg=0;
    char query[500] = {0};
    int rc = sqlite3_open(LOG_DB, &db);
    int callback_value = 0;

    // #1 log.db로 만들었으니까 이 안에 테이블 여러개로 로그관리할거
    // #2 그 안에서 Reading 테이블이 현재 temp랑 fan의 주기적인 기록을 담을거임
    // #3 센서id, 센서타입(fan,temp), 센서측정값(이거땜에 int,double오버로딩), 측정시간(tm), 센서위치
    // #4 insert에서는 우선 table 존재여부 검사하고 없으면 테이블 생성
    // #5 인자로 받은 정보가지고 하나 넣어주면됨
    

    // Table Check
    // sprintf(query, "SELECT * FROM Reading");
    sprintf(query, "SELECT COUNT(*) FROM sqlite_master where name=\"Reading\";");
    rc = sqlite3_exec(db, query, check_db_callback, &callback_value, &err_msg);
    // cout << " CALLBACK RETURN " << endl;
    // cout << callback_value << endl;

    if(callback_value == 0)
    {
      // create table
      sprintf(query, "CREATE TABLE Reading(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, SensorID TEXT NOT NULL, Type TEXT NOT NULL, Value REAL, Time TEXT, Location TEXT);");
      sqlite3_exec(db, query, 0, 0, &err_msg);
      log(info) << "TABLE [Reading] is Created in " << LOG_DB;
    }

    // Make Time Format
    // cout << " TIME TIME " << endl;
    // char outout[100] = {0};
    // make_time_with_tm(_tm, outout);
    // printf("%s\n", outout);

    // Insert Query
    sprintf(query, "INSERT INTO Reading (SensorID, Type, Value, Time, Location) VALUES(\"%s\", \"%s\", %d, \"%s\", \"%s\");"
    , _sensor_id.c_str(), _type.c_str(), _value, _time.c_str(), _location.c_str());
    sqlite3_exec(db, query, 0, 0, &err_msg);

    sqlite3_free(err_msg);
    sqlite3_close(db);
}

// void insert_reading_table(string _sensor_id, string _type, double _value, tm _tm, string _location)
void insert_reading_table(string _sensor_id, string _type, double _value, string _time, string _location)
{
    sqlite3 *db;
    char *err_msg=0;
    char query[500] = {0};
    int rc = sqlite3_open(LOG_DB, &db);
    int callback_value = 0;

    // #1 log.db로 만들었으니까 이 안에 테이블 여러개로 로그관리할거
    // #2 그 안에서 Reading 테이블이 현재 temp랑 fan의 주기적인 기록을 담을거임
    // #3 센서id, 센서타입(fan,temp), 센서측정값(이거땜에 int,double오버로딩), 측정시간(tm), 센서위치
    // #4 insert에서는 우선 table 존재여부 검사하고 없으면 테이블 생성
    // #5 인자로 받은 정보가지고 하나 넣어주면됨
    

    // Table Check
    // sprintf(query, "SELECT * FROM Reading");
    sprintf(query, "SELECT COUNT(*) FROM sqlite_master where name=\"Reading\";");
    rc = sqlite3_exec(db, query, check_db_callback, &callback_value, &err_msg);
    // cout << " CALLBACK RETURN " << endl;
    // cout << callback_value << endl;

    if(callback_value == 0)
    {
      // create table
      sprintf(query, "CREATE TABLE Reading(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, SensorID TEXT NOT NULL, Type TEXT NOT NULL, Value REAL, Time TEXT, Location TEXT);");
      sqlite3_exec(db, query, 0, 0, &err_msg);
      log(info) << "TABLE [Reading] is Created in " << LOG_DB;
    }

    // Make Time Format
    // cout << " TIME TIME " << endl;
    // char outout[100] = {0};
    // make_time_with_tm(_tm, outout);
    // printf("%s\n", outout);

    // Insert Query
    sprintf(query, "INSERT INTO Reading (SensorID, Type, Value, Time, Location) VALUES(\"%s\", \"%s\", %lf, \"%s\", \"%s\");"
    , _sensor_id.c_str(), _type.c_str(), _value, _time.c_str(), _location.c_str());
    sqlite3_exec(db, query, 0, 0, &err_msg);

    sqlite3_free(err_msg);
    sqlite3_close(db);
}

int extract_column_callback(void *_vector, int argc, char **argv, char **azColName)
{
    ((vector<string> *)_vector)->push_back(argv[0]);

    return 0;
}

int make_json_reading_callback(void *_vector, int argc, char **argv, char **azColName)
{

    json::value jv;

    for(int i=0; i<argc; i++)
    {
        if(argv[i] == NULL)
          continue;

        string col = azColName[i];
        string val = argv[i];
        if(col == "Value")
        {
            jv[col] = json::value::number(stof(val));
        }
        else if(col == "Time")
        {
            jv[col] = json::value::string(val);
        }
    }
    // 일단 value랑 time값만

    ((vector<json::value> *)_vector)->push_back(jv);

    return 0;
}


json::value select_all_reading(string _type)
{
    sqlite3 *db;
    char *err_msg=0;
    char query[500] = {0};
    int rc = sqlite3_open(LOG_DB, &db);

    // #1 Reading Table에서 SensorID부터 추출,파악
    // #2 SensorID별로 최신 10개의 로그내용 추출 근데 일단 3개만 하는걸로 할게 테스트단계니까
    // #3 Json Format으로 만들기

    vector<string> v_sensor;
    sprintf(query, "SELECT SensorID from Reading where Type=\"%s\" Group by SensorID;", _type.c_str());
    sqlite3_exec(db, query, extract_column_callback, &v_sensor, &err_msg);

    json::value jv_result;
    // cout << "EXTRACT ~~~~~~~" << endl;
    for(int i=0; i<v_sensor.size(); i++)
    {
        // cout << v_sensor[i] << endl;
        vector<json::value> v_item;
        sprintf(query, "SELECT * from Reading where SensorID=\"%s\" order by Time DESC limit 3;", v_sensor[i].c_str());
        sqlite3_exec(db, query, make_json_reading_callback, &v_item, &err_msg);

        // item 을 jv_result에 담아주기
        jv_result[v_sensor[i]] = json::value::array(v_item);
    }

    // cout << " RESULT !! " << endl;
    // cout << jv_result << endl;
    // cout << " RESULT !! " << endl;

    sqlite3_free(err_msg);
    sqlite3_close(db);

    return jv_result;
}

json::value select_hour_reading(string _type)
{
    sqlite3 *db;
    char *err_msg=0;
    char query[500] = {0};
    int rc = sqlite3_open(LOG_DB, &db);


    vector<string> v_sensor;
    sprintf(query, "SELECT SensorID from Reading where Type=\"%s\" Group by SensorID;", _type.c_str());
    sqlite3_exec(db, query, extract_column_callback, &v_sensor, &err_msg);

    time_t cur_time = time(NULL);
    struct tm cur_tm;
    char filter_time[30] = {0};
    char origin_time[30] = {0};

    // cur_tm = *localtime(&cur_time);
    // make_time_with_tm(cur_tm, origin_time);
    // cout << " CUR TIME !!!" << endl;
    // cout << origin_time << endl;

    cur_time -= 3600;
    cur_tm = *localtime(&cur_time);
    make_time_with_tm(cur_tm, filter_time);
    // cout << " FILTER TIME !!!" << endl;
    // cout << filter_time << endl;
    


    json::value jv_result;
    // cout << "EXTRACT ~~~~~~~" << endl;
    for(int i=0; i<v_sensor.size(); i++)
    {
        // cout << v_sensor[i] << endl;
        vector<json::value> v_item;
        sprintf(query, "SELECT * from Reading where SensorID=\"%s\" and Time > \"%s\" order by Time DESC;"
        , v_sensor[i].c_str(), filter_time);
        sqlite3_exec(db, query, make_json_reading_callback, &v_item, &err_msg);

        // item 을 jv_result에 담아주기
        jv_result[v_sensor[i]] = json::value::array(v_item);        
    }

    // cout << " RESULT !! " << endl;
    // cout << jv_result << endl;
    // cout << " RESULT !! " << endl;

    sqlite3_free(err_msg);
    sqlite3_close(db);
    
    return jv_result;
}

