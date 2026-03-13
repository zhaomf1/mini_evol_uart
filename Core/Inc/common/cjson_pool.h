#ifndef __CJSON_POOL_H__
#define __CJSON_POOL_H__
#include <stddef.h>

// ************************ 可配置项************************
#define CJSON_POOL_SIZE        (4 * 1024)   // 4内存池
#define CJSON_POOL_ALIGN_BITS  2U           // 4字节对齐（不可改，cJSON强制要求）
#define CJSON_POOL_DEBUG       0            // DEBUG开关：1=开启（调试），0=关闭（发布）

// ************************ 公开接口声明 ************************
/**
 * @brief  初始化cJSON内存池
 */
void cjson_pool_init(void);

/**
 * @brief  获取内存池已使用字节数
 * @return 已使用容量（字节）
 */
size_t cjson_pool_used(void);

/**
 * @brief  重置内存池
 */
void cjson_pool_reset(void);

#endif
