#include "stdio.h"
#include "string.h"
#include "usart_comm.h"
#include "usart.h"    // 你的底层串口驱动头文件
#include "cJSON.h"  // cJSON库头文件
#include "cjson_pool.h"
#include "stdio.h"
#include "string.h"
#include "json_key.h"
#include "app_control.h"
#include "dev_bldc_ctrl.h"
#include "dev_od_ctrl.h"
#include "dev_ph_ctrl.h"
#include "dev_temp_ctrl.h"


// ====================== 工具函数 ======================
/**
 * @brief 安全读取JSON数字字段（避免空指针/类型错误）
 * @param json_obj JSON对象
 * @param key 字段名
 * @param default_val 默认值（字段缺失/错误时返回）
 * @return 字段数值
 */
static double safe_get_json_number(cJSON *json_obj, const char *key, double default_val) {
    cJSON *item = cJSON_GetObjectItem(json_obj, key);
    if (item && cJSON_IsNumber(item)) {
        return item->valuedouble;
    }
    return default_val;
}

// ====================== 组包JSON函数 ======================
// 通用错误处理：释放cJSON对象并返回NULL
static char* json_error_handler(cJSON *root) {
    if (root != NULL) {
        cJSON_Delete(root);
        cjson_pool_reset();
    }
    return NULL;
}

/**
 * 直流无刷电机JSON封装（cmd=0）
 */
char* send_bldc_data(uint8_t no, uint16_t speed, uint8_t mode, const char *error)
{
    // 1. 创建JSON根对象
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        return json_error_handler(root);
    }

    // 2. 添加cmd字段（直流无刷电机=0，宏定义键名JSON_KEY_CMD）
    if (!cJSON_AddNumberToObject(root, JSON_KEY_CMD, CMD_BRUSHLESS_MOTOR)) {
        return json_error_handler(root);
    }

    // 3. 创建motor_data子对象（宏定义键名JSON_KEY_MOTOR_DATA）
    cJSON *motor_data = cJSON_CreateObject();
    if (motor_data == NULL) {
        return json_error_handler(root);
    }
    cJSON_AddItemToObject(root, JSON_KEY_MOTOR_DATA, motor_data);

    // 4. 填充motor_data字段（全部用宏定义键名）
    if (!cJSON_AddNumberToObject(motor_data, JSON_KEY_DATA_NO, no) ||
        !cJSON_AddNumberToObject(motor_data, JSON_KEY_MOTOR_DATA_SPEED, speed) ||
        !cJSON_AddNumberToObject(motor_data, JSON_KEY_DATA_MODE, mode)) {
        return json_error_handler(root);
    }

    // 5. 最后添加error字段（宏定义键名JSON_KEY_ERROR）
    const char *error_str = (error != NULL) ? error : "";
    if (!cJSON_AddStringToObject(root, JSON_KEY_ERROR, error_str)) {
        return json_error_handler(root);
    }

    // 6. 生成带格式的JSON字符串（便于调试）
    char *json_str = cJSON_Print(root);
    uint16_t json_len = strlen(json_str);
 
    host_transmit((uint8_t *)json_str, json_len);

    cJSON_Delete(root);
    cjson_pool_reset();
    return json_str;
}

/**
 * 步进电机JSON封装（cmd=1，用宏定义键名）
 */
char* send_steo_motor_data(uint8_t no, uint16_t speed, uint16_t step, uint8_t mode, const char *error)
{
    // 1. 创建JSON根对象
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        return json_error_handler(root);
    }

    // 2. 添加cmd字段（步进电机=1，宏定义键名JSON_KEY_CMD）
    if (!cJSON_AddNumberToObject(root, JSON_KEY_CMD, CMD_STEPPER_MOTOR)) {
        return json_error_handler(root);
    }

    // 3. 创建motor_data子对象（宏定义键名JSON_KEY_MOTOR_DATA）
    cJSON *motor_data = cJSON_CreateObject();
    if (motor_data == NULL) {
        return json_error_handler(root);
    }
    cJSON_AddItemToObject(root, JSON_KEY_MOTOR_DATA, motor_data);

    // 4. 填充motor_data字段（含步进数step，宏定义键名）
    if (!cJSON_AddNumberToObject(motor_data, JSON_KEY_DATA_NO, no) ||
        !cJSON_AddNumberToObject(motor_data, JSON_KEY_MOTOR_DATA_SPEED, speed) ||
        !cJSON_AddNumberToObject(motor_data, JSON_KEY_MOTOR_DATA_STEP, step) ||
        !cJSON_AddNumberToObject(motor_data, JSON_KEY_DATA_MODE, mode)) {
        return json_error_handler(root);
    }

    // 5. 最后添加error字段（宏定义键名JSON_KEY_ERROR）
    const char *error_str = (error != NULL) ? error : "";
    if (!cJSON_AddStringToObject(root, JSON_KEY_ERROR, error_str)) {
        return json_error_handler(root);
    }

    // 6. 生成带格式的JSON字符串（便于调试）
    char *json_str = cJSON_Print(root);
    uint16_t json_len = strlen(json_str);
    host_transmit((uint8_t *)json_str, json_len);

    cJSON_Delete(root);
    cjson_pool_reset();
    return json_str;
}

/**
 * 电磁阀JSON封装（cmd=2，用宏定义键名）
 */
char* send_valve_data(uint8_t no, uint8_t mode, const char *error) 
{
    // 1. 创建JSON根对象
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        return json_error_handler(root);
    }

    // 2. 添加cmd字段（电磁阀=2，宏定义键名JSON_KEY_CMD）
    if (!cJSON_AddNumberToObject(root, JSON_KEY_CMD, CMD_SOLENOID_VALVE)) {
        return json_error_handler(root);
    }

    // 3. 创建valve_data子对象（宏定义键名JSON_KEY_VALVE_DATA）
    cJSON *valve_data = cJSON_CreateObject();
    if (valve_data == NULL) {
        return json_error_handler(root);
    }
    cJSON_AddItemToObject(root, JSON_KEY_VALVE_DATA, valve_data);

    // 4. 填充valve_data字段（宏定义键名）
    if (!cJSON_AddNumberToObject(valve_data, JSON_KEY_DATA_NO, no) ||
        !cJSON_AddNumberToObject(valve_data, JSON_KEY_DATA_MODE, mode)) {
        return json_error_handler(root);
    }

    // 5. 最后添加error字段（宏定义键名JSON_KEY_ERROR）
    const char *error_str = (error != NULL) ? error : "";
    if (!cJSON_AddStringToObject(root, JSON_KEY_ERROR, error_str)) {
        return json_error_handler(root);
    }

    // 6. 生成带格式的JSON字符串（便于调试）
    char *json_str = cJSON_Print(root);
    uint16_t json_len = strlen(json_str);
    host_transmit((uint8_t *)json_str, json_len);

    cJSON_Delete(root);
    cjson_pool_reset();
    return json_str;
}

/**
 * PH板JSON封装（cmd=3，用宏定义键名）
 */
char* send_ph_data(uint8_t ph_cmd, uint8_t phTime, uint8_t phFactor, 
                         float phValue, float setK, float setB, const char *error) 
{
    // 1. 创建JSON根对象
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        return json_error_handler(root);
    }

    // 2. 添加cmd字段（PH板=3，宏定义键名JSON_KEY_CMD）
    if (!cJSON_AddNumberToObject(root, JSON_KEY_CMD, CMD_PH_BOARD)) {
        return json_error_handler(root);
    }

    // 3. 创建ph_data子对象（宏定义键名JSON_KEY_PH_DATA）
    cJSON *ph_data = cJSON_CreateObject();
    if (ph_data == NULL) {
        return json_error_handler(root);
    }
    cJSON_AddItemToObject(root, JSON_KEY_PH_DATA, ph_data);

    // 4. 填充ph_data字段（全部用宏定义键名）
    if (!cJSON_AddNumberToObject(ph_data, JSON_KEY_PH_CMD, ph_cmd) ||
        !cJSON_AddNumberToObject(ph_data, JSON_KEY_PH_TIME, phTime) ||
        !cJSON_AddNumberToObject(ph_data, JSON_KEY_PH_FACTOR, phFactor) ||
        !cJSON_AddNumberToObject(ph_data, JSON_KEY_PH_VALUE, phValue) ||
        !cJSON_AddNumberToObject(ph_data, JSON_KEY_PH_SET_K, setK) ||
        !cJSON_AddNumberToObject(ph_data, JSON_KEY_PH_SET_B, setB)) {
        return json_error_handler(root);
    }

    // 5. 最后添加error字段（宏定义键名JSON_KEY_ERROR）
    const char *error_str = (error != NULL) ? error : "";
    if (!cJSON_AddStringToObject(root, JSON_KEY_ERROR, error_str)) {
        return json_error_handler(root);
    }

    // 6. 生成带格式的JSON字符串（便于调试）
    char *json_str = cJSON_Print(root);
    uint16_t json_len = strlen(json_str);
    host_transmit((uint8_t *)json_str, json_len);

    cJSON_Delete(root);
    cjson_pool_reset();
    return json_str;
}

/**
 * OD板JSON封装（cmd=4，用宏定义键名）
 */
char* send_od_data(uint16_t odValue, const char *error) {
    // 1. 创建JSON根对象
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        return json_error_handler(root);
    }

    // 2. 添加cmd字段（OD板=4，宏定义键名JSON_KEY_CMD）
    if (!cJSON_AddNumberToObject(root, JSON_KEY_CMD, CMD_OD_BOARD)) {
        return json_error_handler(root);
    }

    // 3. 创建od_data子对象（宏定义键名JSON_KEY_OD_DATA）
    cJSON *od_data = cJSON_CreateObject();
    if (od_data == NULL) {
        return json_error_handler(root);
    }
    cJSON_AddItemToObject(root, JSON_KEY_OD_DATA, od_data);

    // 4. 填充od_data字段（宏定义键名JSON_KEY_OD_VALUE）
    if (!cJSON_AddNumberToObject(od_data, JSON_KEY_OD_VALUE, odValue)) {
        return json_error_handler(root);
    }

    // 5. 最后添加error字段（宏定义键名JSON_KEY_ERROR）
    const char *error_str = (error != NULL) ? error : "";
    if (!cJSON_AddStringToObject(root, JSON_KEY_ERROR, error_str)) {
        return json_error_handler(root);
    }

    // 6. 生成带格式的JSON字符串（便于调试）
    char *json_str = cJSON_Print(root);
    uint16_t json_len = strlen(json_str);
    host_transmit((uint8_t *)json_str, json_len);

    cJSON_Delete(root);
    cjson_pool_reset();
    return json_str;
}

/**
 * 温控JSON封装（cmd=5，用宏定义键名）
 */
char* send_temp_data(uint8_t temp_cmd, float tempValue, const char *error) {
    // 1. 创建JSON根对象
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        return json_error_handler(root);
    }

    // 2. 添加cmd字段（温控=5，宏定义键名JSON_KEY_CMD）
    if (!cJSON_AddNumberToObject(root, JSON_KEY_CMD, CMD_TEMPERATURE)) {
        return json_error_handler(root);
    }

    // 3. 创建temperature_data子对象（宏定义键名JSON_KEY_TEMP_DATA）
    cJSON *temp_data = cJSON_CreateObject();
    if (temp_data == NULL) {
        return json_error_handler(root);
    }
    cJSON_AddItemToObject(root, JSON_KEY_TEMP_DATA, temp_data);

    // 4. 填充temperature_data字段（宏定义键名）
    if (!cJSON_AddNumberToObject(temp_data, JSON_KEY_TEMP_CMD, temp_cmd) ||
        !cJSON_AddNumberToObject(temp_data, JSON_KEY_TEMP_VALUE, tempValue)) {
        return json_error_handler(root);
    }

    // 5. 最后添加error字段（宏定义键名JSON_KEY_ERROR）
    const char *error_str = (error != NULL) ? error : "";
    if (!cJSON_AddStringToObject(root, JSON_KEY_ERROR, error_str)) {
        return json_error_handler(root);
    }

    // 6. 生成带格式的JSON字符串（便于调试）
    char *json_str = cJSON_Print(root);
    uint16_t json_len = strlen(json_str);
    host_transmit((uint8_t *)json_str, json_len);

    cJSON_Delete(root);
    cjson_pool_reset();
    return json_str;
}

/**
 * RGB灯JSON封装（cmd=6，用宏定义键名）
 */
char* send_rgb_data(uint8_t r, uint8_t g, uint8_t b, uint8_t mode, const char *error) {
    // 1. 创建JSON根对象
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        return json_error_handler(root);
    }

    // 2. 添加cmd字段（RGB灯=6，宏定义键名JSON_KEY_CMD）
    if (!cJSON_AddNumberToObject(root, JSON_KEY_CMD, CMD_RGB_LIGHT)) {
        return json_error_handler(root);
    }

    // 3. 创建rgb_data子对象（宏定义键名JSON_KEY_RGB_DATA）
    cJSON *rgb_data = cJSON_CreateObject();
    if (rgb_data == NULL) {
        return json_error_handler(root);
    }
    cJSON_AddItemToObject(root, JSON_KEY_RGB_DATA, rgb_data);

    // 4. 填充rgb_data字段（宏定义键名）
    if (!cJSON_AddNumberToObject(rgb_data, JSON_KEY_RGB_SET_R, r) ||
        !cJSON_AddNumberToObject(rgb_data, JSON_KEY_RGB_SET_G, g) ||
        !cJSON_AddNumberToObject(rgb_data, JSON_KEY_RGB_SET_B, b) ||
        !cJSON_AddNumberToObject(rgb_data, JSON_KEY_DATA_MODE, mode)) {
        return json_error_handler(root);
    }

    // 5. 最后添加error字段（宏定义键名JSON_KEY_ERROR）
    const char *error_str = (error != NULL) ? error : "";
    if (!cJSON_AddStringToObject(root, JSON_KEY_ERROR, error_str)) {
        return json_error_handler(root);
    }

    // 6. 生成带格式的JSON字符串（便于调试）
    char *json_str = cJSON_Print(root);
    uint16_t json_len = strlen(json_str);
    host_transmit((uint8_t *)json_str, json_len);

    cJSON_Delete(root);
    cjson_pool_reset();
    return json_str;
}




// ====================== 各指令解析函数 ======================
/**
 * @brief 解析直流无刷电机指令
 */
static void parse_bldc_motor(cJSON *root, SysCtrlCmd_t *cmd) {
    cJSON *motor_data = cJSON_GetObjectItem(root, JSON_KEY_MOTOR_DATA);
    if (!motor_data || !cJSON_IsObject(motor_data)) {
        return;
    }

    // 直接解析数值字段
    cmd->data.bldc_motor.no = (uint8_t)safe_get_json_number(motor_data, JSON_KEY_DATA_NO, 0);
    cmd->data.bldc_motor.speed = (uint16_t)safe_get_json_number(motor_data, JSON_KEY_MOTOR_DATA_SPEED, 0);
    cmd->data.bldc_motor.mode = (uint8_t)safe_get_json_number(motor_data, JSON_KEY_DATA_MODE, 0);

    switch(cmd->data.bldc_motor.no)
    {
        case TRAIN_MOTOR:
            //设置转速,速度设置为0代表关闭
            if(cmd->data.bldc_motor.mode == MOTOR_MODE_CW_KEEP)
            {
                bldc_ctrl_set_speed(MODBUS_ADDR_TRAIN_BLDC,cmd->data.bldc_motor.speed);
            }
            else if(cmd->data.bldc_motor.mode == MOTOR_MODE_STOP)
            {
                bldc_ctrl_set_speed(MODBUS_ADDR_TRAIN_BLDC,0);
            }


            break;
        case FEDDING_MOTOR:
            //设置转速,速度设置为0代表关闭
            if(cmd->data.bldc_motor.mode == MOTOR_MODE_CW_KEEP)
            {
                bldc_ctrl_set_speed(MODBUS_ADDR_FEEDING_BLDC,cmd->data.bldc_motor.speed);
            }
            else if(cmd->data.bldc_motor.mode == MOTOR_MODE_STOP)
            {
                bldc_ctrl_set_speed(MODBUS_ADDR_FEEDING_BLDC,0);
            }

            break;
        default:
            break;
    }

    //上报数据给上位机
    send_bldc_data(cmd->data.bldc_motor.no,cmd->data.bldc_motor.speed,cmd->data.bldc_motor.mode,NULL);

}

/**
 * @brief 解析步进电机指令
 */
static void parse_step_motor(cJSON *root, SysCtrlCmd_t *cmd) {
    cJSON *motor_data = cJSON_GetObjectItem(root, JSON_KEY_MOTOR_DATA);
    if (!motor_data || !cJSON_IsObject(motor_data)) {
        return;
    }
    uint16_t steptime = 0;

    cmd->data.stepp_motor.no = (uint8_t)safe_get_json_number(motor_data, JSON_KEY_DATA_NO, 0);
    cmd->data.stepp_motor.speed = (uint16_t)safe_get_json_number(motor_data, JSON_KEY_MOTOR_DATA_SPEED, 0);
    cmd->data.stepp_motor.step = (uint16_t)safe_get_json_number(motor_data, JSON_KEY_MOTOR_DATA_STEP, 0);
    cmd->data.stepp_motor.mode = (uint8_t)safe_get_json_number(motor_data, JSON_KEY_DATA_MODE, 0);

    //控制电机
    if(cmd->data.stepp_motor.mode == MOTOR_MODE_CW)         //正转步数
    {
        step_motor_control((StepMotorId_t)cmd->data.stepp_motor.no,STEP_MOTOR_CMD_SWITCH,STEP_MOTOR_ENABLE);
        step_motor_control((StepMotorId_t)cmd->data.stepp_motor.no,STEP_MOTOR_CMD_DIR,STEP_MOTOR_FOREWARD);
        steptime = (uint16_t)((float)cmd->data.stepp_motor.step / (float)cmd->data.stepp_motor.speed * 1000);
        set_step_motor_step_number((StepMotorId_t)cmd->data.stepp_motor.no,steptime);
    }
    else if (cmd->data.stepp_motor.mode == MOTOR_MODE_CCW)   //反转步数
    {
        step_motor_control((StepMotorId_t)cmd->data.stepp_motor.no,STEP_MOTOR_CMD_SWITCH,STEP_MOTOR_ENABLE);
        step_motor_control((StepMotorId_t)cmd->data.stepp_motor.no,STEP_MOTOR_CMD_DIR,STEP_MOTOR_REVERSES);
        steptime = (uint16_t)((float)cmd->data.stepp_motor.step / (float)cmd->data.stepp_motor.speed * 1000);
        set_step_motor_step_number((StepMotorId_t)cmd->data.stepp_motor.no,steptime);
    }
    else if (cmd->data.stepp_motor.mode == MOTOR_MODE_CW_KEEP)   //持续正转
    {
        step_motor_control((StepMotorId_t)cmd->data.stepp_motor.no,STEP_MOTOR_CMD_SWITCH,STEP_MOTOR_ENABLE);
        step_motor_control((StepMotorId_t)cmd->data.stepp_motor.no,STEP_MOTOR_CMD_DIR,STEP_MOTOR_FOREWARD);
    }
    else if(cmd->data.stepp_motor.mode == MOTOR_MODE_CCW_KEEP)    //持续反转
    {
        step_motor_control((StepMotorId_t)cmd->data.stepp_motor.no,STEP_MOTOR_CMD_SWITCH,STEP_MOTOR_ENABLE);
        step_motor_control((StepMotorId_t)cmd->data.stepp_motor.no,STEP_MOTOR_CMD_DIR,STEP_MOTOR_REVERSES);
    }
    else if(cmd->data.stepp_motor.mode == MOTOR_MODE_STOP)    //停止
    {
        step_motor_control((StepMotorId_t)cmd->data.stepp_motor.no,STEP_MOTOR_CMD_SWITCH,STEP_MOTOR_DISABLE);
    }

    //设置电机转速
    step_motor_control((StepMotorId_t)cmd->data.stepp_motor.no,STEP_MOTOR_CMD_SPEED,cmd->data.stepp_motor.speed);

    //上报上位机
    send_steo_motor_data(cmd->data.stepp_motor.no,cmd->data.stepp_motor.speed,cmd->data.stepp_motor.step,cmd->data.stepp_motor.mode,NULL);

}

/**
 * @brief 解析电磁阀指令
 */
static void parse_valve(cJSON *root, SysCtrlCmd_t *cmd) {
    cJSON *valve_data = cJSON_GetObjectItem(root, JSON_KEY_VALVE_DATA);
    if (!valve_data || !cJSON_IsObject(valve_data)) {
        return;
    }

    cmd->data.valve.no = (uint8_t)safe_get_json_number(valve_data, JSON_KEY_DATA_NO, 0);
    cmd->data.valve.mode = (uint8_t)safe_get_json_number(valve_data, JSON_KEY_DATA_MODE, 0);

    switch(cmd->data.valve.no)
    {
        case VALVE_FEDDING:     //补料阀
        if(cmd->data.valve.mode == VALVE_OPEN)
        {
            feed_valve_on();
        }
        else if(cmd->data.valve.mode == VALVE_CLOSE)
        {
            feed_valve_off();
        }
        break;

        case VALVE_AIR_SUPPLY:  //补气阀
        if(cmd->data.valve.mode == VALVE_OPEN)
        {
            air_supply_valve_on();
        }
        else if(cmd->data.valve.mode == VALVE_CLOSE)
        {
            air_supply_valve_off();
        }
        break;

        case VALVE_FLOODLIGHT:  //照明灯
        if(cmd->data.valve.mode == VALVE_OPEN)
        {
            light_on();
        }
        else if(cmd->data.valve.mode == VALVE_CLOSE)
        {
            light_off();
        }
        break;

        case VALVE_UV_LAMP:     //紫外灯
        if(cmd->data.valve.mode == VALVE_OPEN)
        {
            uv_lamp_on();
        }
        else if(cmd->data.valve.mode == VALVE_CLOSE)
        {
            uv_lamp_off();
        }
        break;

        case VALVE_LIGHT_SOURCE_SHUTTER:    //光源快门
        if(cmd->data.valve.mode == VALVE_OPEN)
        {
            // uv_lamp_on();
        }
        else if(cmd->data.valve.mode == VALVE_CLOSE)
        {
            // uv_lamp_off();
        }
        break;
        
    }

    send_valve_data(cmd->data.valve.no,cmd->data.valve.mode,NULL);

}

/**
 * @brief 解析PH板指令
 */
static void parse_ph_board(cJSON *root, SysCtrlCmd_t *cmd) {
    cJSON *ph_data = cJSON_GetObjectItem(root, JSON_KEY_PH_DATA);
    if (!ph_data || !cJSON_IsObject(ph_data)) {
        return;
    }

    cmd->data.ph_board.ph_cmd = (uint8_t)safe_get_json_number(ph_data, JSON_KEY_PH_CMD, 0);
    cmd->data.ph_board.phTime = (uint8_t)safe_get_json_number(ph_data, JSON_KEY_PH_TIME, 0);
    cmd->data.ph_board.phFactor = (uint8_t)safe_get_json_number(ph_data, JSON_KEY_PH_FACTOR, 0);
    cmd->data.ph_board.phValue = (float)safe_get_json_number(ph_data, JSON_KEY_PH_VALUE, 0.0f);
    cmd->data.ph_board.setK = (float)safe_get_json_number(ph_data, JSON_KEY_PH_SET_K, 0.0f);
    cmd->data.ph_board.setB = (float)safe_get_json_number(ph_data, JSON_KEY_PH_SET_B, 0.0f);

    if(cmd->data.ph_board.ph_cmd == PH_CMD_OPEN)  //设置PH值：需要做的操作：1.实时获取当前PH值，2.根据当前PH值选择酸碱泵电机运转时间，3.达到设定PH值后停止PH流程
    {
        PhCtrl_t ph_ctrl;
        ph_ctrl.active = true;
        ph_ctrl.ph_factor = cmd->data.ph_board.phFactor;
        ph_ctrl.ph_value = cmd->data.ph_board.phValue;
        ph_ctrl.ph_time = cmd->data.ph_board.phTime;

        set_ph_ctrl_start(ph_ctrl);
        send_ph_data( cmd->data.ph_board.ph_cmd, cmd->data.ph_board.phTime,cmd->data.ph_board.phFactor,cmd->data.ph_board.phValue,cmd->data.ph_board.setK,cmd->data.ph_board.setB,NULL);

    }
    else if(cmd->data.ph_board.ph_cmd == PH_CMD_CLOSE) //停止PH值设置，终止PH流程
    {
        set_ph_ctrl_stop();
        send_ph_data( cmd->data.ph_board.ph_cmd, cmd->data.ph_board.phTime,cmd->data.ph_board.phFactor,cmd->data.ph_board.phValue,cmd->data.ph_board.setK,cmd->data.ph_board.setB,NULL);
    }
    else if(cmd->data.ph_board.ph_cmd == PH_CMD_GET) //获取当前PH值
    {
        float ph = 0;
        float ph_k = 0;
        float ph_b = 0;
        int ret = ph_ctrl_read_value(&ph);
        get_ph_kb_value(&ph_k,&ph_b);
        ph = ph_k * ph + ph_b;

        send_ph_data( cmd->data.ph_board.ph_cmd, cmd->data.ph_board.phTime,cmd->data.ph_board.phFactor,ph,cmd->data.ph_board.setK,cmd->data.ph_board.setB,NULL);

        
    }
    else if(cmd->data.ph_board.ph_cmd == PH_CMD_SET_KB) //设定K,B标定值
    {
        printf("setK:%f,setB:%f\n",cmd->data.ph_board.setK,cmd->data.ph_board.setB);
        set_ph_kb_value(cmd->data.ph_board.setK,cmd->data.ph_board.setB);

        send_ph_data( cmd->data.ph_board.ph_cmd, cmd->data.ph_board.phTime,cmd->data.ph_board.phFactor,cmd->data.ph_board.phValue,cmd->data.ph_board.setK,cmd->data.ph_board.setB,NULL);
    }

}

/**
 * @brief 解析OD板指令
 */
static void parse_od_board(cJSON *root, SysCtrlCmd_t *cmd) {
    cJSON *od_data = cJSON_GetObjectItem(root, JSON_KEY_OD_DATA);
    if (!od_data || !cJSON_IsObject(od_data)) {
        return;
    }

    cmd->data.od_board.odValue = (uint16_t)safe_get_json_number(od_data, JSON_KEY_OD_VALUE, 0);

    //获取OD值
    uint16_t od_value = 0;
    od_ctrl_read_value(&od_value);

    send_od_data(od_value,NULL);
}

/**
 * @brief 解析温控指令
 */
static void parse_temperature(cJSON *root, SysCtrlCmd_t *cmd) {
    cJSON *temp_data = cJSON_GetObjectItem(root, JSON_KEY_TEMP_DATA);
    if (!temp_data || !cJSON_IsObject(temp_data)) {
        return;
    }

    cmd->data.temperature_board.temperature_cmd = (uint8_t)safe_get_json_number(temp_data, JSON_KEY_TEMP_CMD, 0);
    cmd->data.temperature_board.temperatureValue = (float)safe_get_json_number(temp_data, JSON_KEY_TEMP_VALUE, 0.0f);

    //开启温控模块
    if(cmd->data.temperature_board.temperature_cmd == TEMP_CTRL_OPEN)
    {
        uint16_t temp_set = (uint16_t)(cmd->data.temperature_board.temperatureValue * 100);
        temp_ctrl_set_temperature(temp_set);
        
        temp_ctrl_switch_ctrl_temperature(1);
        send_temp_data(cmd->data.temperature_board.temperature_cmd,cmd->data.temperature_board.temperatureValue,NULL);
    }   //关闭温控模块
    else if(cmd->data.temperature_board.temperature_cmd == TEMP_CTRL_CLOSE)
    {
        temp_ctrl_switch_ctrl_temperature(0);
        send_temp_data(cmd->data.temperature_board.temperature_cmd,0,NULL);
    }   //获取当前温度值
    else if(cmd->data.temperature_board.temperature_cmd == TEMP_CTRL_GET)
    {
        uint16_t temp_get = 0;
        temp_ctrl_read_temperature(&temp_get);
        float f_temp = (float)temp_get / 100.0f;
        send_temp_data(cmd->data.temperature_board.temperature_cmd,f_temp,NULL);
    }

}

/**
 * @brief 解析RGB灯指令
 */
static void parse_rgb_light(cJSON *root, SysCtrlCmd_t *cmd) {
    cJSON *rgb_data = cJSON_GetObjectItem(root, JSON_KEY_RGB_DATA);
    if (!rgb_data || !cJSON_IsObject(rgb_data)) {
        return;
    }

    RgbColor_t rgb_color;
    
    cmd->data.rgb_light.r = (uint8_t)safe_get_json_number(rgb_data, JSON_KEY_RGB_SET_R, 0);
    cmd->data.rgb_light.g = (uint8_t)safe_get_json_number(rgb_data, JSON_KEY_RGB_SET_G, 0);
    cmd->data.rgb_light.b = (uint8_t)safe_get_json_number(rgb_data, JSON_KEY_RGB_SET_B, 0);
    cmd->data.rgb_light.mode = (uint8_t)safe_get_json_number(rgb_data, JSON_KEY_DATA_MODE, 0);

    if(cmd->data.rgb_light.mode == VALVE_OPEN) //开
    {
        rgb_color.r = cmd->data.rgb_light.r;
        rgb_color.g = cmd->data.rgb_light.g;
        rgb_color.b = cmd->data.rgb_light.b;

        rgb_set_color(rgb_color);
    }
    else if(cmd->data.rgb_light.mode == VALVE_CLOSE) //关
    {
        rgb_switch_ctrl(VALVE_CLOSE);
    }

    send_rgb_data(cmd->data.rgb_light.r,cmd->data.rgb_light.g,cmd->data.rgb_light.b,cmd->data.rgb_light.mode,"NULL");

}



// ====================== 顶层解析函数 ======================
/**
 * @brief 解析JSON指令字符串（仅解析数值字段，error无需解析）
 * @param json_str 上位机下发的JSON字符串（纯数值字段）
 * @param cmd 输出指令结构（error字段初始化置空）
 * @return 0=成功，-1=JSON格式错误，-2=指令类型未知
 */
int parse_motor_ctrl_json(const char *json_str, SysCtrlCmd_t *cmd) {
    // 初始化指令结构
    memset(cmd, 0, sizeof(SysCtrlCmd_t));
    cmd->cmd_type = CMD_UNKNOWN;
    memset(cmd->error, 0, sizeof(cmd->error)); // error字段初始化

    // 解析JSON根对象
    cJSON *root = cJSON_Parse(json_str);
    if (!root) {
        cjson_pool_reset();
        return -1; // JSON格式错误
    }

    // 读取cmd字段
    int cmd_num = (int)safe_get_json_number(root, "cmd", 0);
    switch (cmd_num) {
        case CMD_BRUSHLESS_MOTOR:
            cmd->cmd_type = CMD_BRUSHLESS_MOTOR;
            parse_bldc_motor(root, cmd);
            break;
        case CMD_STEPPER_MOTOR:
            cmd->cmd_type = CMD_STEPPER_MOTOR;
            parse_step_motor(root, cmd);
            break;
        case CMD_SOLENOID_VALVE:
            cmd->cmd_type = CMD_SOLENOID_VALVE;
            parse_valve(root, cmd);
            break;
        case CMD_PH_BOARD:
            cmd->cmd_type = CMD_PH_BOARD;
            parse_ph_board(root, cmd);
            break;
        case CMD_OD_BOARD:
            cmd->cmd_type = CMD_OD_BOARD;
            parse_od_board(root, cmd);
            break;
        case CMD_TEMPERATURE:
            cmd->cmd_type = CMD_TEMPERATURE;
            parse_temperature(root, cmd);
            break;
        case CMD_RGB_LIGHT:
            cmd->cmd_type = CMD_RGB_LIGHT;
            parse_rgb_light(root, cmd);
            break;
        default:
            cJSON_Delete(root);
            cjson_pool_reset();
            return -2; // 未知指令类型
    }

    // 4. 释放JSON内存
    cJSON_Delete(root);
    cjson_pool_reset(); // 释放cJSON内存池
    return 0;
}



void uart_comm_task(void *argument)
{
    /* USER CODE BEGIN uart_comm_task */
    SysCtrlCmd_t cmd;
    
    UartMsg_t rx_msg;
    
    for(;;)
    {
        if (osMessageQueueGet(uartRxQueueHandle, &rx_msg, NULL, osWaitForever) == osOK)
        {
            printf("[DEBUG] Receive UART data:\n%.*s\n",rx_msg.len, rx_msg.data_ptr);
            int ret = parse_motor_ctrl_json((const char *)rx_msg.data_ptr, &cmd);
        }

        osDelay(10);
    }
    /* USER CODE END uart_comm_task */
}
