#ifndef LOGSERVICE_H
#define LOGSERVICE_H

#include "stdafx.hpp"
#include "resource.hpp"
#include <sqlite3.h>

#define LOG_DB "/conf/database/log.db"

void make_test_db();
void read_test_db();



void insert_reading_table(string _sensor_id, string _type, int _value, tm _tm, string _location);
void insert_reading_table(string _sensor_id, string _type, double _value, tm _tm, string _location);
// 처음 insert함수 리딩테이블에니까  센서id, 센서타입, 센서측정값(), 측정시간(tm구조체), 센서위치(str)
// 이걸 호출하는 로그가 발생하는 곳에서 tm에 localtime 그거만 해서 담아주면됨 

#endif