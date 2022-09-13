#include "logservice.hpp"



// Database
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

int check_same_time_callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    if(argv[0] == NULL)
    {
        *((int *)NotUsed) = 0;
        // cout << "[CHECK SAME TIME CALLBACK] : NO DATA" << endl;
    }
    else
    {
        *((int *)NotUsed) = 1;
        // cout << "[CHECK SAME TIME CALLBACK] : YES DATA" << endl;
    }

    // for(int i=0; i<argc; i++)
    // {
    //     // cout << "현재 i : " << i << endl;
    //     cout << azColName[i] << " = " << argv[i] << endl;
    // }

    return 0;
}

const std::string DB_currentDateTime(void)
{
    time_t     now = time(0); //현재 시간을 time_t 타입으로 저장
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct); // YYYY-MM-DD.HH:mm:ss 형태의 스트링
    strftime(buf, sizeof(buf), "%Y-%m-%d %R", &tstruct); // YYYY-MM-DD HH:mm 형태의 스트링

    return buf;
}


/**
 * @brief 센서측정값 Reading Table에 insert하는 함수
 * @param _sensor_id - 센서id
 * @param _module - 센서가 위치한 모듈 ex> CMM1, CM1..
 * @param _type - 센서의 위치,타입 정보 ex> power, thermal..
 * @param _detail - 센서의 상세타입 ex> powercontrol, powersupply, temperature, fan...
 * @param _value - 센서 측정값 @param _time - 측정된 시각(yyyy-mm-dd hh::mm::ss)
 */
// void insert_reading_table(string _sensor_id, string _type, int _value, tm _tm, string _location)
// void insert_reading_table(string _sensor_id, string _type, int _value, string _time, string _module)
void insert_reading_table(string _sensor_id, string _module, string _type, string _detail, int _value, string _time)
{
    sqlite3 *db;
    char *err_msg=0;
    char query[500] = {0};
    int rc = sqlite3_open(LOG_DB, &db);
    int callback_value = 0;

    // Table Check
    // sprintf(query, "SELECT * FROM Reading");
    sprintf(query, "SELECT COUNT(*) FROM sqlite_master where name=\"Reading\";");
    rc = sqlite3_exec(db, query, check_db_callback, &callback_value, &err_msg);
    // cout << " CALLBACK RETURN " << endl;
    // cout << callback_value << endl;

    if(callback_value == 0)
    {
      // create table
      sprintf(query, "CREATE TABLE Reading(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, SensorID TEXT NOT NULL, Module TEXT NOT NULL, Type TEXT NOT NULL, Detail TEXT NOT NULL, Value REAL, Time TEXT);");
      sqlite3_exec(db, query, 0, 0, &err_msg);
      log(info) << "TABLE [Reading] is Created in " << LOG_DB;
    }

    // Make Time Format
    // cout << " TIME TIME " << endl;
    // char outout[100] = {0};
    // make_time_with_tm(_tm, outout);
    // printf("%s\n", outout);

    // 추가 로직
    // 동일 센서의 같은 시각으로 들어오는거 방지
    // 1970의 로컬타임 아직 설정전의 시각으로 들어오는거 방지
    // 현재 시각에서 한 20년정도 차이나는 타임을 갖는 데이터를 드랍하려했더니 현재 시각이 로컬타임으로 설정되어있을때가 좀 걸림
    // 그래서 그냥 2000년 이전 데이터를 드랍하게끔 설정해 놓음
    // string currentDate, currentYear, inputYear;
    // get_current_date_info(&currentDate);
    // currentYear = currentDate.substr(0, 4);

    string inputYear = _time.substr(0,4);
    // cout << "[INPUT YEAR CHECK] : " << inputYear << endl;

    if(inputYear < "2000")
        return ;

    // 여기는 이제 같은 _sensor_id 값의 동일 시간 _time 의 데이터가 DB에 있는지 서치
    int existSameData;
    sprintf(query, "SELECT * FROM Reading WHERE Module=\"%s\" and SensorID=\"%s\" and Time=\"%s\";"
    , _module.c_str(), _sensor_id.c_str(), _time.c_str());
    sqlite3_exec(db, query, check_same_time_callback, &existSameData, &err_msg);

    if(existSameData == 1)
        return ;

    

    


    // Insert Query
    sprintf(query, "INSERT INTO Reading (SensorID, Module, Type, Detail, Value, Time) VALUES(\"%s\", \"%s\", \"%s\", \"%s\", %d, \"%s\");"
    , _sensor_id.c_str(), _module.c_str(), _type.c_str(), _detail.c_str(), _value, _time.c_str());
    sqlite3_exec(db, query, 0, 0, &err_msg);

    sqlite3_free(err_msg);
    sqlite3_close(db);
}

// void insert_reading_table(string _sensor_id, string _type, double _value, tm _tm, string _module)
// void insert_reading_table(string _sensor_id, string _type, double _value, string _time, string _module)
void insert_reading_table(string _sensor_id, string _module, string _type, string _detail, double _value, string _time)
{
    sqlite3 *db;
    char *err_msg=0;
    char query[500] = {0};
    int rc = sqlite3_open(LOG_DB, &db);
    int callback_value = 0;

    

    // Table Check
    // sprintf(query, "SELECT * FROM Reading");
    sprintf(query, "SELECT COUNT(*) FROM sqlite_master where name=\"Reading\";");
    rc = sqlite3_exec(db, query, check_db_callback, &callback_value, &err_msg);
    // cout << " CALLBACK RETURN " << endl;
    // cout << callback_value << endl;

    if(callback_value == 0)
    {
      // create table
      sprintf(query, "CREATE TABLE Reading(ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, SensorID TEXT NOT NULL, Module TEXT NOT NULL, Type TEXT NOT NULL, Detail TEXT NOT NULL, Value REAL, Time TEXT);");
      sqlite3_exec(db, query, 0, 0, &err_msg);
      log(info) << "TABLE [Reading] is Created in " << LOG_DB;
    }

    // Make Time Format
    // cout << " TIME TIME " << endl;
    // char outout[100] = {0};
    // make_time_with_tm(_tm, outout);
    // printf("%s\n", outout);

    // 추가 로직
    string inputYear = _time.substr(0,4);
    // cout << "[INPUT YEAR CHECK] : " << inputYear << endl;

    if(inputYear < "2000")
        return ;

    // 여기는 이제 같은 _sensor_id 값의 동일 시간 _time 의 데이터가 DB에 있는지 서치
    int existSameData;
    sprintf(query, "SELECT * FROM Reading WHERE Module=\"%s\" and SensorID=\"%s\" and Time=\"%s\";"
    , _module.c_str(), _sensor_id.c_str(), _time.c_str());
    sqlite3_exec(db, query, check_same_time_callback, &existSameData, &err_msg);

    if(existSameData == 1)
        return ;


    // Insert Query
    sprintf(query, "INSERT INTO Reading (SensorID, Module, Type, Detail, Value, Time) VALUES(\"%s\", \"%s\", \"%s\", \"%s\", %lf, \"%s\");"
    , _sensor_id.c_str(), _module.c_str(), _type.c_str(), _detail.c_str(), _value, _time.c_str());
    sqlite3_exec(db, query, 0, 0, &err_msg);

    sqlite3_free(err_msg);
    sqlite3_close(db);
}

int extract_column_callback(void *_vector, int argc, char **argv, char **azColName)
{
    ((vector<string> *)_vector)->push_back(argv[0]);

    return 0;
}


int make_json_min_reading_callback(void *_vector, int argc, char **argv, char **azColName)
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

int make_json_hour_reading_callback(void *_struct, int argc, char **argv, char **azColName)
{
    // ((Hour_Reading_Info *)_struct)  복사용
    if(((Hour_Reading_Info *)_struct)->items.size() >= ((Hour_Reading_Info *)_struct)->max_count)
      return 0;

    json::value jv;
    string time_val;
    
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
            if(((Hour_Reading_Info *)_struct)->cur_time == "")
            {
              time_val = val;
            }
            else
            {
              // 조건안맞으면 바로 리턴 , 조건 맞으면 아래 추가하고 시간 다시 구하고 ㅇㅇ
              if(val > ((Hour_Reading_Info *)_struct)->compare_time)
              {
                return 0;
              }
              else
                time_val = val;
            }
            jv[col] = json::value::string(val);
        }

    }

    ((Hour_Reading_Info *)_struct)->items.push_back(jv);
    ((Hour_Reading_Info *)_struct)->cur_time = time_val; // item에 푸시된 녀석의 Time
    // cur_time(time_val) 가지고 1시간 이전의 시간을 구할거임
    
    time_t init_time;
    struct tm *tm_cur;
    time(&init_time);
    tm_cur = localtime(&init_time);

    // 현재시간 string -> tm
    strptime(time_val.c_str(), "%F %H:%M", tm_cur);
    // strptime(time_val.c_str(), "%F %T", tm_cur);

    // 현재시간 tm -> time_t + 1시간 이전시간 계산
    time_t t1 = mktime(tm_cur);
    // cout << "t1 : " << t1 << endl;
    t1 -= 3600;
    // cout << "t1 re : " << t1 << endl;

    // 1시간 이전시간 time_t -> tm
    struct tm *tm_pre;
    tm_pre = localtime(&t1);
    tm_pre->tm_sec = 0;

    // 1시간 이전시간 tm -> string
    char outout[30];
    make_time_with_tm(*tm_pre, outout);
    string str_out(outout);

    cout << "CUR : " << time_val << endl;
    cout << "Hour Before : " << str_out << endl;

    ((Hour_Reading_Info *)_struct)->compare_time = str_out;


    return 0;
}

// 바뀐구조로 새로 만든 select 함수
json::value select_min_reading(string _module, string _type, string _detail, int _count)
{
    sqlite3 *db;
    char *err_msg=0;
    char query[500] = {0};
    int rc = sqlite3_open(LOG_DB, &db);

    // #1 아예 type detail module 다 필수로 받기로했으니깐 첨에 sensorid얻어올때
    // module, type, detail 로 필터링해서 얻어오기
    // #2 그런다음에 json만들어주고 어쨋든 items 배열에는 value time값만 들어가니깐 얻어오는 콜백함수에서
    // 2개만 가져오면 될듯하다

    vector<string> v_sensor;
    sprintf(query, "SELECT SensorID FROM Reading WHERE Module=\"%s\" and Type=\"%s\" and Detail=\"%s\" Group by SensorID;"
    , _module.c_str(), _type.c_str(), _detail.c_str());
    sqlite3_exec(db, query, extract_column_callback, &v_sensor, &err_msg);


    json::value jv_result;
    jv_result["ModuleID"] = json::value::string(_module);
    jv_result["Readings"] = json::value::array();
    // cout << "EXTRACT ~~~~~~~" << endl;
    for(int i=0; i<v_sensor.size(); i++)
    {
        // cout << v_sensor[i] << endl;
        json::value jv_reading;
        jv_reading["SensorID"] = json::value::string(v_sensor[i]);
        jv_reading["Type"] = json::value::string(_type);
        jv_reading["Detail"] = json::value::string(_detail);
        
        vector<json::value> v_item;
        sprintf(query, "SELECT * FROM Reading WHERE SensorID=\"%s\" and Module=\"%s\" order by Time DESC limit %d;"
        , v_sensor[i].c_str(), _module.c_str(), _count);
        sqlite3_exec(db, query, make_json_min_reading_callback, &v_item, &err_msg);

        // item 을 jv_result에 담아주기
        jv_reading["Items"] = json::value::array(v_item);
        jv_result["Readings"][i] = jv_reading;
    }

    // cout << " RESULT !! " << endl;
    // cout << jv_result << endl;
    // cout << " RESULT !! " << endl;

    sqlite3_free(err_msg);
    sqlite3_close(db);

    return jv_result;
}

json::value select_hour_reading(string _module, string _type, string _detail, int _count)
{
    sqlite3 *db;
    char *err_msg=0;
    char query[500] = {0};
    int rc = sqlite3_open(LOG_DB, &db);

    // #1 아예 type detail module 다 필수로 받기로했으니깐 첨에 sensorid얻어올때
    // module, type, detail 로 필터링해서 얻어오기
    // #2 센서id 얻어오는것 까지는 똑같은데 센서id로 item뽑을때 첫 item으로부터 1시간 이전 시간인거까지 
    // continue로 지나쳐야함 그리고 조건에 맞는 녀석이 나오면 그때 item에넣고 시간갱신하고
    // min하고 콜백함수가 다른걸 써야겠네

    vector<string> v_sensor;
    sprintf(query, "SELECT SensorID FROM Reading WHERE Module=\"%s\" and Type=\"%s\" and Detail=\"%s\" Group by SensorID;"
    , _module.c_str(), _type.c_str(), _detail.c_str());
    sqlite3_exec(db, query, extract_column_callback, &v_sensor, &err_msg);


    json::value jv_result;
    jv_result["ModuleID"] = json::value::string(_module);
    jv_result["Readings"] = json::value::array();
    // cout << "EXTRACT ~~~~~~~" << endl;
    for(int i=0; i<v_sensor.size(); i++)
    {
        // cout << v_sensor[i] << endl;
        json::value jv_reading;
        jv_reading["SensorID"] = json::value::string(v_sensor[i]);
        jv_reading["Type"] = json::value::string(_type);
        jv_reading["Detail"] = json::value::string(_detail);
        

        Hour_Reading_Info reading_info;
        reading_info.max_count = _count;
        reading_info.cur_time = "";
        // vector<json::value> v_item;
        sprintf(query, "SELECT * FROM Reading WHERE SensorID=\"%s\" and Module=\"%s\" order by Time DESC;"
        , v_sensor[i].c_str(), _module.c_str());
        sqlite3_exec(db, query, make_json_hour_reading_callback, &reading_info, &err_msg);

        // item 을 jv_result에 담아주기
        jv_reading["Items"] = json::value::array(reading_info.items);
        jv_result["Readings"][i] = jv_reading;
    }

    // cout << " RESULT !! " << endl;
    // cout << jv_result << endl;
    // cout << " RESULT !! " << endl;

    sqlite3_free(err_msg);
    sqlite3_close(db);

    return jv_result;
  
}

// json::value select_all_time_reading(string _module, string _type)
// {
//     sqlite3 *db;
//     char *err_msg=0;
//     char query[500] = {0};
//     int rc = sqlite3_open(LOG_DB, &db);

//     // #1 Reading Table에서 SensorID부터 추출,파악
//     // #2 SensorID별로 최신 10개의 로그내용 추출 근데 일단 3개만 하는걸로 할게 테스트단계니까
//     // #3 Json Format으로 만들기

//     // NEW!!!!
//     // 파라미터로는 type하고 module만 받을게 그리고 시간은 쿼리로하는게 아니라 시간계산을 하고 쿼리를 해야돼서
//     // 함수자체를 여러개 만들거임
//     // INTERSECT로 SensorID뽑은거 교집합해서 구하고
//     // SELECT * 할때 센서id만 하면 다른모듈 같은 센서id나올수있으니까 조건에 module까지는 걸어줘야할듯

//     // Module Query
//     char query_module[500] = {0}, query_type[500] = {0};
//     if(_module == "all")
//     {
//       sprintf(query_module, "SELECT SensorID FROM Reading Group by SensorID");
//     }
//     else
//     {
//       sprintf(query_module, "SELECT SensorID FROM Reading WHERE Module=\"%s\" Group by SensorID", _module.c_str());
//     }

//     if(_type == "all")
//     {
//       sprintf(query_type, "SELECT SensorID FROM Reading Group by SensorID");
//     }
//     else
//     {
//       sprintf(query_type, "SELECT SensorID FROM Reading WHERE Type=\"%s\" Group by SensorID", _type.c_str());
//     }

//     vector<string> v_sensor;
//     sprintf(query, "%s INTERSECT %s;", query_module, query_type);
//     // sprintf(query, "SELECT SensorID from Reading where Type=\"%s\" Group by SensorID;", _type.c_str());
//     sqlite3_exec(db, query, extract_column_callback, &v_sensor, &err_msg);

//     json::value jv_result;
//     // cout << "EXTRACT ~~~~~~~" << endl;
//     for(int i=0; i<v_sensor.size(); i++)
//     {
//         // cout << v_sensor[i] << endl;
//         vector<json::value> v_item;
//         sprintf(query, "SELECT * from Reading where SensorID=\"%s\" order by Time DESC limit 3;", v_sensor[i].c_str());
//         sqlite3_exec(db, query, make_json_reading_callback, &v_item, &err_msg);

//         // item 을 jv_result에 담아주기
//         jv_result[v_sensor[i]] = json::value::array(v_item);
//     }

//     // cout << " RESULT !! " << endl;
//     // cout << jv_result << endl;
//     // cout << " RESULT !! " << endl;

//     sqlite3_free(err_msg);
//     sqlite3_close(db);

//     return jv_result;
// }


// #오픈시스넷 임시 리딩 반환함수
json::value opensys_thermal_min()
{
  json::value j;
  j["ModuleID"]= json::value::string("CMM1");
  j["Readings"] = json::value::array();
  json::value sensor1;
  sensor1["SensorID"] = json::value::string("Sensor 1");
  sensor1["Type"] = json::value::string("thermal");
  sensor1["Detail"] = json::value::string("temperature");
  sensor1["Items"] = json::value::array();
  json::value item1;
  item1["Value"] = json::value::number(20.5);
  item1["Time"] = json::value::string("2021-11-15 11:00");
  sensor1["Items"][0]=item1;
  json::value item2;
  item2["Value"] = json::value::number(22.5);
  item2["Time"] = json::value::string("2021-11-15 11:01");
  sensor1["Items"][1] = item2;
  j["Readings"][0] = sensor1;

  json::value sensor2;
  sensor2["SensorID"] = json::value::string("Sensor 2");
  sensor2["Type"] = json::value::string("thermal");
  sensor2["Detail"] = json::value::string("temperature");
  sensor2["Items"] = json::value::array();
  json::value item3;
  item3["Value"] = json::value::number(20.5);
  item3["Time"] = json::value::string("2021-11-15 11:00");
  sensor2["Items"][0]=item3;
  json::value item4;
  item4["Value"] = json::value::number(22.5);
  item4["Time"] = json::value::string("2021-11-15 11:01");
  sensor2["Items"][1]=item4;
  j["Readings"][1] = sensor2;
  return j;
}

json::value opensys_thermal_hour()
{
  json::value j;
  j["ModuleID"]= json::value::string("CMM1");
  j["Readings"] = json::value::array();
  json::value sensor1;
  sensor1["SensorID"] = json::value::string("Sensor 1");
  sensor1["Type"] = json::value::string("thermal");
  sensor1["Detail"] = json::value::string("temperature");
  sensor1["Items"] = json::value::array();
  json::value item1;
  item1["Value"] = json::value::number(20.5);
  item1["Time"] = json::value::string("2021-11-15 11:00");
  sensor1["Items"][0]=item1;
  json::value item2;
  item2["Value"] = json::value::number(22.5);
  item2["Time"] = json::value::string("2021-11-15 12:00");
  sensor1["Items"][1] = item2;
  j["Readings"][0] = sensor1;

  json::value sensor2;
  sensor2["SensorID"] = json::value::string("Sensor 2");
  sensor2["Type"] = json::value::string("thermal");
  sensor2["Detail"] = json::value::string("temperature");
  sensor2["Items"] = json::value::array();
  json::value item3;
  item3["Value"] = json::value::number(20.5);
  item3["Time"] = json::value::string("2021-11-15 11:00");
  sensor2["Items"][0]=item3;
  json::value item4;
  item4["Value"] = json::value::number(22.5);
  item4["Time"] = json::value::string("2021-11-15 12:01");
  sensor2["Items"][1]=item4;
  j["Readings"][1] = sensor2;
  return j;
}
json::value opensys_powercontrol_min()
{
  json::value j;
  j["ModuleID"]= json::value::string("CMM1");
  j["Readings"] = json::value::array();
  json::value sensor1;
  sensor1["SensorID"] = json::value::string("Sensor 1");
  sensor1["Type"] = json::value::string("power");
  sensor1["Detail"] = json::value::string("powercontrol");
  sensor1["Items"] = json::value::array();
  json::value item1;
  item1["Value"] = json::value::number(20.5);
  item1["Time"] = json::value::string("2021-11-15 11:00");
  sensor1["Items"][0]=item1;
  json::value item2;
  item2["Value"] = json::value::number(22.5);
  item2["Time"] = json::value::string("2021-11-15 11:01");
  sensor1["Items"][1] = item2;
  j["Readings"][0] = sensor1;

  json::value sensor2;
  sensor2["SensorID"] = json::value::string("Sensor 2");
  sensor2["Type"] = json::value::string("power");
  sensor2["Detail"] = json::value::string("powercontrol");
  sensor2["Items"] = json::value::array();
  json::value item3;
  item3["Value"] = json::value::number(20.5);
  item3["Time"] = json::value::string("2021-11-15 11:00");
  sensor2["Items"][0]=item3;
  json::value item4;
  item4["Value"] = json::value::number(22.5);
  item4["Time"] = json::value::string("2021-11-15 11:01");
  sensor2["Items"][1]=item4;
  j["Readings"][1] = sensor2;
  return j;
}

json::value opensys_powercontrol_hour()
{
  json::value j;
  j["ModuleID"]= json::value::string("CMM1");
  j["Readings"] = json::value::array();
  json::value sensor1;
  sensor1["SensorID"] = json::value::string("Sensor 1");
  sensor1["Type"] = json::value::string("power");
  sensor1["Detail"] = json::value::string("powercontrol");
  sensor1["Items"] = json::value::array();
  json::value item1;
  item1["Value"] = json::value::number(20.5);
  item1["Time"] = json::value::string("2021-11-15 11:00");
  sensor1["Items"][0]=item1;
  json::value item2;
  item2["Value"] = json::value::number(22.5);
  item2["Time"] = json::value::string("2021-11-15 12:01");
  sensor1["Items"][1] = item2;
  j["Readings"][0] = sensor1;

  json::value sensor2;
  sensor2["SensorID"] = json::value::string("Sensor 2");
  sensor2["Type"] = json::value::string("power");
  sensor2["Detail"] = json::value::string("powercontrol");
  sensor2["Items"] = json::value::array();
  json::value item3;
  item3["Value"] = json::value::number(20.5);
  item3["Time"] = json::value::string("2021-11-15 11:00");
  sensor2["Items"][0]=item3;
  json::value item4;
  item4["Value"] = json::value::number(22.5);
  item4["Time"] = json::value::string("2021-11-15 12:01");
  sensor2["Items"][1]=item4;
  j["Readings"][1] = sensor2;
  return j;
}
json::value opensys_powervoltage_min()
{
  json::value j;
  j["ModuleID"]= json::value::string("CMM1");
  j["Readings"] = json::value::array();
  json::value sensor1;
  sensor1["SensorID"] = json::value::string("Sensor 1");
  sensor1["Type"] = json::value::string("power");
  sensor1["Detail"] = json::value::string("powervoltage");
  sensor1["Items"] = json::value::array();
  json::value item1;
  item1["Value"] = json::value::number(20.5);
  item1["Time"] = json::value::string("2021-11-15 11:00");
  sensor1["Items"][0]=item1;
  json::value item2;
  item2["Value"] = json::value::number(22.5);
  item2["Time"] = json::value::string("2021-11-15 11:01");
  sensor1["Items"][1] = item2;
  j["Readings"][0] = sensor1;

  json::value sensor2;
  sensor2["SensorID"] = json::value::string("Sensor 2");
  sensor2["Type"] = json::value::string("power");
  sensor2["Detail"] = json::value::string("powervoltage");
  sensor2["Items"] = json::value::array();
  json::value item3;
  item3["Value"] = json::value::number(20.5);
  item3["Time"] = json::value::string("2021-11-15 11:00");
  sensor2["Items"][0]=item3;
  json::value item4;
  item4["Value"] = json::value::number(22.5);
  item4["Time"] = json::value::string("2021-11-15 11:01");
  sensor2["Items"][1]=item4;
  j["Readings"][1] = sensor2;
  return j;
}

json::value opensys_powervoltage_hour()
{
  json::value j;
  j["ModuleID"]= json::value::string("CMM1");
  j["Readings"] = json::value::array();
  json::value sensor1;
  sensor1["SensorID"] = json::value::string("Sensor 1");
  sensor1["Type"] = json::value::string("power");
  sensor1["Detail"] = json::value::string("powervoltage");
  sensor1["Items"] = json::value::array();
  json::value item1;
  item1["Value"] = json::value::number(20.5);
  item1["Time"] = json::value::string("2021-11-15 11:00");
  sensor1["Items"][0]=item1;
  json::value item2;
  item2["Value"] = json::value::number(2.5);
  item2["Time"] = json::value::string("2021-11-15 12:01");
  sensor1["Items"][1] = item2;
  j["Readings"][0] = sensor1;

  json::value sensor2;
  sensor2["SensorID"] = json::value::string("Sensor 2");
  sensor2["Type"] = json::value::string("power");
  sensor2["Detail"] = json::value::string("powervoltage");
  sensor2["Items"] = json::value::array();
  json::value item3;
  item3["Value"] = json::value::number(22.5);
  item3["Time"] = json::value::string("2021-11-15 11:00");
  sensor2["Items"][0]=item3;
  json::value item4;
  item4["Value"] = json::value::number(22.5);
  item4["Time"] = json::value::string("2021-11-15 12:01");
  sensor2["Items"][1]=item4;
  j["Readings"][1] = sensor2;
  return j;
}
json::value opensys_powersupply_min()
{
  json::value j;
  j["ModuleID"]= json::value::string("CMM1");
  j["Readings"] = json::value::array();
  json::value sensor1;
  sensor1["SensorID"] = json::value::string("Sensor 1");
  sensor1["Type"] = json::value::string("power");
  sensor1["Detail"] = json::value::string("powersupply");
  sensor1["Items"] = json::value::array();
  json::value item1;
  item1["Value"] = json::value::number(22.5);
  item1["Time"] = json::value::string("2021-11-15 11:00");
  sensor1["Items"][0]=item1;
  json::value item2;
  item2["Value"] = json::value::number(22.5);
  item2["Time"] = json::value::string("2021-11-15 11:01");
  sensor1["Items"][1] = item2;
  j["Readings"][0] = sensor1;

  json::value sensor2;
  sensor2["SensorID"] = json::value::string("Sensor 2");
  sensor2["Type"] = json::value::string("power");
  sensor2["Detail"] = json::value::string("powersupply");
  sensor2["Items"] = json::value::array();
  json::value item3;
  item3["Value"] = json::value::number(23.5);
  item3["Time"] = json::value::string("2021-11-15 11:00");
  sensor2["Items"][0]=item3;
  json::value item4;
  item4["Value"] = json::value::number(22.5);
  item4["Time"] = json::value::string("2021-11-15 11:01");
  sensor2["Items"][1]=item4;
  j["Readings"][1] = sensor2;
  return j;
}

json::value opensys_powersupply_hour()
{
  json::value j;
  j["ModuleID"]= json::value::string("CMM1");
  j["Readings"] = json::value::array();
  json::value sensor1;
  sensor1["SensorID"] = json::value::string("Sensor 1");
  sensor1["Type"] = json::value::string("power");
  sensor1["Detail"] = json::value::string("powersupply");
  sensor1["Items"] = json::value::array();
  json::value item1;
  item1["Value"] = json::value::number(23.5);
  item1["Time"] = json::value::string("2021-11-15 11:00");
  sensor1["Items"][0]=item1;
  json::value item2;
  item2["Value"] = json::value::number(12.5);
  item2["Time"] = json::value::string("2021-11-15 12:01");
  sensor1["Items"][1] = item2;
  j["Readings"][0] = sensor1;

  json::value sensor2;
  sensor2["SensorID"] = json::value::string("Sensor 2");
  sensor2["Type"] = json::value::string("power");
  sensor2["Detail"] = json::value::string("powersupply");
  sensor2["Items"] = json::value::array();
  json::value item3;
  item3["Value"] = json::value::number(20.5);
  item3["Time"] = json::value::string("2021-11-15 11:00");
  sensor2["Items"][0]=item3;
  json::value item4;
  item4["Value"] = json::value::number(12.5);
  item4["Time"] = json::value::string("2021-11-15 12:01");
  sensor2["Items"][1]=item4;
  j["Readings"][1] = sensor2;
  return j;
}
json::value opensys_fan_min()
{
  json::value j;
  j["ModuleID"]= json::value::string("CMM1");
  j["Readings"] = json::value::array();
  json::value sensor1;
  sensor1["SensorID"] = json::value::string("Sensor 1");
  sensor1["Type"] = json::value::string("thermal");
  sensor1["Detail"] = json::value::string("fan");
  sensor1["Items"] = json::value::array();
  json::value item1;
  item1["Value"] = json::value::number(5.5);
  item1["Time"] = json::value::string("2021-11-15 11:00");
  sensor1["Items"][0]=item1;
  json::value item2;
  item2["Value"] = json::value::number(22.5);
  item2["Time"] = json::value::string("2021-11-15 11:01");
  sensor1["Items"][1] = item2;
  j["Readings"][0] = sensor1;

  json::value sensor2;
  sensor2["SensorID"] = json::value::string("Sensor 2");
  sensor2["Type"] = json::value::string("thermal");
  sensor2["Detail"] = json::value::string("fan");
  sensor2["Items"] = json::value::array();
  json::value item3;
  item3["Value"] = json::value::number(21.5);
  item3["Time"] = json::value::string("2021-11-15 11:00");
  sensor2["Items"][0]=item3;
  json::value item4;
  item4["Value"] = json::value::number(22.5);
  item4["Time"] = json::value::string("2021-11-15 11:01");
  sensor2["Items"][1]=item4;
  j["Readings"][1] = sensor2;
  return j;
}
json::value opensys_fan_hour()
{
  json::value j;
  j["ModuleID"]= json::value::string("CMM1");
  j["Readings"] = json::value::array();
  json::value sensor1;
  sensor1["SensorID"] = json::value::string("Sensor 1");
  sensor1["Type"] = json::value::string("thermal");
  sensor1["Detail"] = json::value::string("fan");
  sensor1["Items"] = json::value::array();
  json::value item1;
  item1["Value"] = json::value::number(23.5);
  item1["Time"] = json::value::string("2021-11-15 11:00");
  sensor1["Items"][0]=item1;
  json::value item2;
  item2["Value"] = json::value::number(22.5);
  item2["Time"] = json::value::string("2021-11-15 12:01");
  sensor1["Items"][1] = item2;
  j["Readings"][0] = sensor1;

  json::value sensor2;
  sensor2["SensorID"] = json::value::string("Sensor 2");
  sensor2["Type"] = json::value::string("thermal");
  sensor2["Detail"] = json::value::string("fan");
  sensor2["Items"] = json::value::array();
  json::value item3;
  item3["Value"] = json::value::number(25.5);
  item3["Time"] = json::value::string("2021-11-15 11:00");
  sensor2["Items"][0]=item3;
  json::value item4;
  item4["Value"] = json::value::number(22.5);
  item4["Time"] = json::value::string("2021-11-15 12:01");
  sensor2["Items"][1]=item4;
  j["Readings"][1] = sensor2;
  return j;
}

json::value opensys_smartheater_min()
{
  json::value j;
  j["ModuleID"]= json::value::string("CMM1");
  j["Readings"] = json::value::array();
  json::value sensor1;
  sensor1["SensorID"] = json::value::string("Sensor 1");
  sensor1["Type"] = json::value::string("thermal");
  sensor1["Detail"] = json::value::string("smartheater");
  sensor1["Items"] = json::value::array();
  json::value item1;
  item1["Value"] = json::value::number(5.5);
  item1["Time"] = json::value::string("2021-11-15 11:00");
  sensor1["Items"][0]=item1;
  json::value item2;
  item2["Value"] = json::value::number(22.5);
  item2["Time"] = json::value::string("2021-11-15 11:01");
  sensor1["Items"][1] = item2;
  j["Readings"][0] = sensor1;

  json::value sensor2;
  sensor2["SensorID"] = json::value::string("Sensor 2");
  sensor2["Type"] = json::value::string("thermal");
  sensor2["Detail"] = json::value::string("smartheater");
  sensor2["Items"] = json::value::array();
  json::value item3;
  item3["Value"] = json::value::number(21.5);
  item3["Time"] = json::value::string("2021-11-15 11:00");
  sensor2["Items"][0]=item3;
  json::value item4;
  item4["Value"] = json::value::number(22.5);
  item4["Time"] = json::value::string("2021-11-15 11:01");
  sensor2["Items"][1]=item4;
  j["Readings"][1] = sensor2;
  return j;
}
json::value opensys_smartheater_hour()
{
  json::value j;
  j["ModuleID"]= json::value::string("CMM1");
  j["Readings"] = json::value::array();
  json::value sensor1;
  sensor1["SensorID"] = json::value::string("Sensor 1");
  sensor1["Type"] = json::value::string("thermal");
  sensor1["Detail"] = json::value::string("smartheater");
  sensor1["Items"] = json::value::array();
  json::value item1;
  item1["Value"] = json::value::number(23.5);
  item1["Time"] = json::value::string("2021-11-15 11:00");
  sensor1["Items"][0]=item1;
  json::value item2;
  item2["Value"] = json::value::number(22.5);
  item2["Time"] = json::value::string("2021-11-15 12:01");
  sensor1["Items"][1] = item2;
  j["Readings"][0] = sensor1;

  json::value sensor2;
  sensor2["SensorID"] = json::value::string("Sensor 2");
  sensor2["Type"] = json::value::string("thermal");
  sensor2["Detail"] = json::value::string("smartheater");
  sensor2["Items"] = json::value::array();
  json::value item3;
  item3["Value"] = json::value::number(25.5);
  item3["Time"] = json::value::string("2021-11-15 11:00");
  sensor2["Items"][0]=item3;
  json::value item4;
  item4["Value"] = json::value::number(22.5);
  item4["Time"] = json::value::string("2021-11-15 12:01");
  sensor2["Items"][1]=item4;
  j["Readings"][1] = sensor2;
  return j;
}

// json::value select_one_hour_reading(string _module, string _type)
// {
//     sqlite3 *db;
//     char *err_msg=0;
//     char query[500] = {0};
//     int rc = sqlite3_open(LOG_DB, &db);


//     vector<string> v_sensor;
//     sprintf(query, "SELECT SensorID from Reading where Type=\"%s\" Group by SensorID;", _type.c_str());
//     sqlite3_exec(db, query, extract_column_callback, &v_sensor, &err_msg);

//     time_t cur_time = time(NULL);
//     struct tm cur_tm;
//     char filter_time[30] = {0};
//     char origin_time[30] = {0};

//     // cur_tm = *localtime(&cur_time);
//     // make_time_with_tm(cur_tm, origin_time);
//     // cout << " CUR TIME !!!" << endl;
//     // cout << origin_time << endl;

//     cur_time -= 3600;
//     cur_tm = *localtime(&cur_time);
//     make_time_with_tm(cur_tm, filter_time);
//     // cout << " FILTER TIME !!!" << endl;
//     // cout << filter_time << endl;
    


//     json::value jv_result;
//     // cout << "EXTRACT ~~~~~~~" << endl;
//     for(int i=0; i<v_sensor.size(); i++)
//     {
//         // cout << v_sensor[i] << endl;
//         vector<json::value> v_item;
//         sprintf(query, "SELECT * from Reading where SensorID=\"%s\" and Time > \"%s\" order by Time DESC;"
//         , v_sensor[i].c_str(), filter_time);
//         sqlite3_exec(db, query, make_json_reading_callback, &v_item, &err_msg);

//         // item 을 jv_result에 담아주기
//         jv_result[v_sensor[i]] = json::value::array(v_item);
//     }

//     // cout << " RESULT !! " << endl;
//     // cout << jv_result << endl;
//     // cout << " RESULT !! " << endl;

//     sqlite3_free(err_msg);
//     sqlite3_close(db);
    
//     return jv_result;
// }


// Redfish

// Event_Info generate_event_info(string _event_id, string _event_type, string _msg_id, vector<string> _args)
// {
//   Event_Info ev;
//   ev.event_id = _event_id;
//   ev.event_timestamp = currentDateTime();
//   ev.event_type = _event_type;
//   ev.message.id = _msg_id;

//   string registry_odata = ODATA_MESSAGE_REGISTRY_ID;
//   MessageRegistry *registry = (MessageRegistry *)g_record[registry_odata];
//   int flag=0;

//   for(int i=0; i<registry->messages.v_msg.size(); i++)
//   {
//     if(registry->messages.v_msg[i].pattern == _msg_id)
//     {
//       ev.message.message = registry->messages.v_msg[i].message;
//       ev.message.severity = registry->messages.v_msg[i].severity;
//       // ev.message.message_args = registry->messages.v_msg[i].message_args;
//       ev.message.resolution = registry->messages.v_msg[i].resolution;

//       if(registry->messages.v_msg[i].number_of_args != _args.size())
//       {
//         flag = 1;
//         break;
//       }
//       else
//         ev.message.message_args = _args;
//       flag = 2;
//       break;
//     }
//   }

//   if(flag == 0)
//   {
//     log(warning) << "In MessageRegistry, No Information about Message ID : " << _msg_id;
//   }
//   else if(flag == 1)
//   {
//     log(warning) << "Message Args Count Incorrect";
//   }
//   // 이렇게 하고 message에서 message 내용 severity, message_args, resolution정도 있는데
//   // 이게 messageregistry에 지정된거 가지고 다 사용하는거면 id로 레지스트리 검색해서 message, severity, args
//   // resolution 가져오면 될듯?

//   return ev;
// }

// SEL generate_sel(unsigned int _sensor_num, string _code, string _sensor_type, string _msg, string _severity)
// {
//   SEL sel;
//   sel.sensor_number = _sensor_num;
//   sel.entry_code = _code;
//   sel.sensor_type = _sensor_type;
//   // sel.message.id = _msg_id;
//   sel.message.message = _msg;
//   sel.message.severity = _severity;

//   // sel event 정보를 따로받아야할거같은데? 메세지도 들어있고 severity라든가

//   return sel;
// }

void generate_log(string _position, string _facility, Event_Info _ev)
{
  // 파라미터 필요한게
  // #1 로그서비스 위에 chassis , system, manager 어디에 들어갈지
  // #2 /-/LogServices/[HERE] 에 어느 facility로 들어갈지
  // #3 그다음에는 Event_Info 나 SEL 필요함
  string odata;
  if(_position == "Chassis")
    odata = ODATA_CHASSIS_ID;
  else if(_position == "System")
    odata = ODATA_SYSTEM_ID;
  else if(_position == "Manager")
    odata = ODATA_MANAGER_ID;
  
  odata = odata + "/" + CMM_ID;
  odata = odata + "/LogServices/";
  odata = odata + _facility;

  if(!record_is_exist(odata))
  {
    // 로그서비스가 없다면 생성
    LogService *service = new LogService(odata, _facility);

    // TODO 로그서비스 멤버변수 값 채워넣는 정보를 어디서 얻는지 처리 필요
    // 현재는 임시값으로 넣음 // 이거 facility에 따라서 디폴트값 다르게 설정한다든가 하는방식도 있겠네
    service->max_number_of_records = 1000;
    service->log_entry_type = "Multiple";
    // service->overwrite_policy = "WrapsWhenFull";
    service->overwrite_policy = "NeverOverWrites";
    service->service_enabled = true;

    string col_odata = get_parent_object_uri(odata, "/");
    Collection *col = (Collection *)g_record[col_odata];

    col->add_member(service);

    resource_save_json(service); // logservice
    resource_save_json(col); // logservice collection
  }

  LogService *service = (LogService *)g_record[odata];
  string entry_odata = service->odata.id + "/Entries";

  if(!record_is_exist(entry_odata))
  {
    service->entry = new Collection(service->odata.id + "/Entries", ODATA_LOG_ENTRY_COLLECTION_TYPE);
    service->entry->name = "Log Entry Collection";
    resource_save_json(service->entry); // entry collection
  }
  
  Collection *en_col = (Collection *)g_record[entry_odata];

  // max record검사
  if(service->record_count >= service->max_number_of_records)
  {
    if(service->overwrite_policy == "WrapsWhenFull")
    {
      // 덮어쓰기
      service->record_count = 0;
    }
    else if(service->overwrite_policy == "NeverOverWrites")
    {
      // 로그 엔트리 소각
      log(warning) << "LogService Overwrite Policy is NeverOverWrites : Discard Log Entry";
      return ;
    }
    else
    {
      // Unknown이 있는데 안쓰일듯
      return ;
    }
  }
  unsigned int num = service->record_count + 1;
  entry_odata = entry_odata + "/" + to_string(num);
  service->record_count++;

  LogEntry *entry;
  entry = make_log(entry_odata, to_string(num), _ev);
  en_col->add_member(entry);

  resource_save_json(entry);
  resource_save_json(en_col);


  // BMC의 로그엔트리 CMM반영 >> GET요청 보내면 반영?

}

void generate_log(string _position, string _facility, SEL _sel)
{
  string odata;
  if(_position == "Chassis")
    odata = ODATA_CHASSIS_ID;
  else if(_position == "System")
    odata = ODATA_SYSTEM_ID;
  else if(_position == "Manager")
    odata = ODATA_MANAGER_ID;
  
  odata = odata + "/" + CMM_ID;
  odata = odata + "/LogServices/";
  odata = odata + _facility;

  if(!record_is_exist(odata))
  {
    // 로그서비스가 없다면 생성
    LogService *service = new LogService(odata, _facility);

    // TODO 로그서비스 멤버변수 값 채워넣는 정보를 어디서 얻는지 처리 필요
    // 현재는 임시값으로 넣음
    service->max_number_of_records = 1000;
    service->log_entry_type = "Multiple";
    service->overwrite_policy = "WrapsWhenFull";
    service->service_enabled = true;

    string col_odata = get_parent_object_uri(odata, "/");
    Collection *col = (Collection *)g_record[col_odata];

    col->add_member(service);

    resource_save_json(service); // logservice
    resource_save_json(col); // logservice collection
  }

  LogService *service = (LogService *)g_record[odata];
  string entry_odata = service->odata.id + "/Entries";

  if(!record_is_exist(entry_odata))
  {
    service->entry = new Collection(service->odata.id + "/Entries", ODATA_LOG_ENTRY_COLLECTION_TYPE);
    service->entry->name = "Log Entry Collection";
    resource_save_json(service->entry); // entry collection
  }


  Collection *en_col = (Collection *)g_record[entry_odata];
  
  // max record검사
  if(service->record_count >= service->max_number_of_records)
  {
    if(service->overwrite_policy == "WrapsWhenFull")
    {
      // 덮어쓰기
      service->record_count = 0;
    }
    else if(service->overwrite_policy == "NeverOverWrites")
    {
      // 로그 엔트리 소각
      log(warning) << "LogService Overwrite Policy is NeverOverWrites : Discard Log Entry";
      return ;
    }
    else
    {
      // Unknown이 있는데 안쓰일듯
      return ;
    }
  }

  unsigned int num = service->record_count + 1;
  entry_odata = entry_odata + "/" + to_string(num);
  service->record_count++;

  LogEntry *entry;
  entry = make_log(entry_odata, to_string(num), _sel);
  en_col->add_member(entry);

  resource_save_json(entry);
  resource_save_json(en_col);

}


// 위에 event_info랑 sel 별도 호출해서 ev or sel 만들어놓고 로그서비스연결하는 함수호출
// cha, sys, man 어딘지 받고 퍼실리티 받고 해서 해당 uri의 로그서비스 생성 하고 이제 로그엔트리를 만드는데
// 그 안에서 만들지 그러면 odata같은건 바로들어가지만
// 로그서비스만드는건 어떻게 되는거죠?

// 로직이 어떻게 되냐면 이벤트&로그 발생하는곳에서
// #1 특정 값들 받아서 event_info, SEL 로 만들어주는거 함수하나 필요함 그거 호출해서 ev랑 sel만들고
// #2 로그서비스 만드는 함수 호출해서 여기에서 서비스랑 연결or생성
// #3 ev, sel 가지고 로그엔트리 생성하는함수를 호출 그러면 비로소 로그생김
// ##1사실 이건 로그만이고 별개로 이벤트 구독자에게 보내는 작업 필요한데 일단 로그부터하겟음
// ##2그리고 db에도 저장해야함



LogEntry* make_log(string odata_id, string id, Event_Info ev)
{
    LogEntry *le = new LogEntry(odata_id, id);
    
    le->entry_type = "Event";
    le->severity = ev.message.severity;
    le->message = ev.message;

    le->event_id = ev.event_id;
    le->event_timestamp = ev.event_timestamp;
    le->event_type = ev.event_type;

    return le;
}

LogEntry* make_log(string odata_id, string id, SEL sel)
{
    LogEntry *le = new LogEntry(odata_id, id);
    
    le->entry_type = "SEL";
    le->severity = sel.message.severity;
    le->message = sel.message;

    le->sensor_number = sel.sensor_number;
    le->sensor_type = sel.sensor_type;
    le->entry_code = sel.entry_code;

    le->event_timestamp = sel.event_timestamp;
    le->event_type = sel.event_type;

    return le;
}