#ifndef __JSON_KEY_H
#define __JSON_KEY_H

#define JSON_KEY_CMD                    "cmd"                   //指令 0-直流无刷电机，1-步进电机，2-电磁阀，3-PH，4-OD，5-温控，6-RGB灯
#define JSON_KEY_MOTOR_DATA             "motor_data"
#define JSON_KEY_DATA_NO                "no"                    //电机序号：0-培养电机，1-补料电机
#define JSON_KEY_MOTOR_DATA_SPEED       "speed"                 //电机速度
#define JSON_KEY_DATA_MODE              "mode"                  //电机方向：0-正转，1-反转，2-持续正转，3-持续反转，4-停止
#define JSON_KEY_MOTOR_DATA_STEP        "step"                  //电机步进数
#define JSON_KEY_VALVE_DATA             "valve_data"
#define JSON_KEY_PH_DATA                "ph_data"       
#define JSON_KEY_PH_CMD                 "ph_cmd"                //PH指令：0-设置，1-关闭，2-设定ph值，3-设置k,b值
#define JSON_KEY_PH_TIME                "phTime"                //PH搅拌时间
#define JSON_KEY_PH_FACTOR              "phFactor"              //PH酸碱银子 0-酸因子，1-碱因子
#define JSON_KEY_PH_VALUE               "phValue"               //PH值
#define JSON_KEY_PH_SET_K               "setK"                  //PH标定K值
#define JSON_KEY_PH_SET_B               "setB"                  //PH标定B值
#define JSON_KEY_OD_DATA                "od_data"        
#define JSON_KEY_OD_VALUE               "odValue"               //OD值
#define JSON_KEY_TEMP_DATA              "temperature_data"      //温度数据
#define JSON_KEY_TEMP_CMD               "temperature_cmd"       //温度指令：0-设置，1-关闭，2-获取
#define JSON_KEY_TEMP_VALUE             "temperatureValue"      //温度值
#define JSON_KEY_RGB_DATA               "rgb_data"
#define JSON_KEY_RGB_SET_R               "r"                    //R值设置
#define JSON_KEY_RGB_SET_G               "g"                    //G值设置
#define JSON_KEY_RGB_SET_B               "b"                    //B值设置
#define JSON_KEY_FIRMWARE_VERSION        "firmware_version"     //固件信息
#define JSON_KEY_SOFTWARE_VERSION        "software_version"     //软件信息
#define JSON_KEY_ERROR                   "error"                //报警信息


#endif
