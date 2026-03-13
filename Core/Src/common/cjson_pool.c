#include "cjson_pool.h"
#include "cJSON.h"
#include "stm32f4xx_hal.h"
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

// ************************ 私有宏定义 ************************
// 4字节对齐掩码
#define CJSON_POOL_ALIGN_MASK ((1U << CJSON_POOL_ALIGN_BITS) - 1U)
// 向上取整对齐计算
#define CJSON_ALIGN_SIZE(size) ((size + CJSON_POOL_ALIGN_MASK) & ~CJSON_POOL_ALIGN_MASK)

// ************************ 裸机临界区保护************************
// 关全局中断
#define CJSON_ENTER_CRITICAL() __disable_irq()
// 开全局中断
#define CJSON_EXIT_CRITICAL() __enable_irq()

// ************************ DEBUG日志 ************************
#if CJSON_POOL_DEBUG
#define CJSON_POOL_LOG(fmt, ...)                           \
    do                                                     \
    {                                                      \
        printf("[CJSON_POOL] " fmt "\r\n", ##__VA_ARGS__); \
    } while (0)

#else
#define CJSON_POOL_LOG(fmt, ...) \
    do                           \
    {                            \
    } while (0)
#endif

// ************************ 私有静态变量bss区，不占Flash）************************
// 静态内存池（4字节对齐，Cortex-M4指令优化）
static uint8_t cjson_pool[CJSON_POOL_SIZE] __attribute__((aligned(4)));
static uint8_t *pool_ptr = cjson_pool;                               // 内存池当前指针
static const uint8_t *const pool_end = cjson_pool + CJSON_POOL_SIZE; // 内存池结束地址
static uint8_t hooks_inited = 0U;                                    // Hooks初始化标记（避免重复设置）
static volatile uint16_t pool_alloc_count = 0U;                      // 分配块计数，跟踪malloc/free的匹配情况

// ************************ cJSON 内存分配回调（裸机专属）************************
static void *cjson_pool_malloc(size_t size)
{
    void *ptr = NULL;
    
    // 快速处理特殊情况
    if (size == 0)
    {
        return NULL;
    }

    // 4字节对齐处理（Cortex-M4非对齐访问会触发HardFault）
    size_t aligned_size = CJSON_ALIGN_SIZE(size);

    // 溢出防护：先检查size是否超过总容量（避免无符号数溢出）
    if (aligned_size > CJSON_POOL_SIZE)
    {
        CJSON_POOL_LOG("Malloc failed: size(%zu) > total(%zu)", aligned_size, CJSON_POOL_SIZE);
        return NULL;
    }

    CJSON_ENTER_CRITICAL();

    // 检查剩余空间
    if ((pool_ptr + aligned_size) <= pool_end)
    {
        ptr = pool_ptr;
        pool_ptr += aligned_size;
        pool_alloc_count++;  // 增加分配块计数
        CJSON_POOL_LOG("Malloc %zu bytes, used: %zu, alloc_count: %u", 
                      aligned_size, (size_t)(pool_ptr - cjson_pool), pool_alloc_count);
    }
    else
    {
        // 内存池耗尽
        CJSON_POOL_LOG("Out of memory! Used: %zu, Need: %zu", (size_t)(pool_ptr - cjson_pool), aligned_size);
    }

    CJSON_EXIT_CRITICAL();
    
    // 在临界区外记录断言，提升实时性
    if (ptr == NULL)
    {
        assert(0 && "cJSON pool out of memory"); // DEBUG模式触发断言
    }

    return ptr;
}

// ************************ cJSON 内存释放回调（裸机无实际释放）************************
static void cjson_pool_free(void *ptr)
{
    // 先检查空指针
    if (ptr == NULL)
    {
        return;
    }
    
    // DEBUG模式：校验指针是否在内存池范围内（防止非法释放）
#if CJSON_POOL_DEBUG
    if (ptr < (void *)cjson_pool || ptr >= (void *)pool_end)
    {
        CJSON_POOL_LOG("Free error: ptr(%p) out of pool range", ptr);
        assert(0 && "cJSON pool free invalid ptr");
    }
#endif
    
    // 减少分配块计数
    CJSON_ENTER_CRITICAL();
    if (pool_alloc_count > 0)
    {
        pool_alloc_count--;
    }
    CJSON_EXIT_CRITICAL();
    
    CJSON_POOL_LOG("Free ptr(%p), alloc_count: %u", ptr, pool_alloc_count);
}

// 获取分配块计数（用于调试，检查是否有内存泄漏）
uint16_t cjson_pool_get_alloc_count(void)
{
    uint16_t count;
    CJSON_ENTER_CRITICAL();
    count = pool_alloc_count;
    CJSON_EXIT_CRITICAL();
    return count;
}

// ************************ cJSON Hooks初始化（全局唯一）************************
static cJSON_Hooks pool_hooks = {
    .malloc_fn = cjson_pool_malloc,
    .free_fn = cjson_pool_free};

void cjson_pool_init(void)
{
    if (hooks_inited == 0U)
    {
        cJSON_InitHooks(&pool_hooks);
        hooks_inited = 1U;
        CJSON_POOL_LOG("Init success, size: %zu bytes", CJSON_POOL_SIZE);
    }
    cjson_pool_reset(); // 初始重置指针
}

size_t cjson_pool_used(void)
{
    // 裸机下无需临界区（只读操作，无并发）
    return (size_t)(pool_ptr - cjson_pool);
}

void cjson_pool_reset(void)
{
    CJSON_ENTER_CRITICAL();
        
    // 检查是否有未释放的内存块（用于调试，帮助发现内存泄漏）
    if (pool_alloc_count != 0U)
    {
        CJSON_EXIT_CRITICAL();
        CJSON_POOL_LOG("Reset warning: alloc_count is %u (possible memory leak)", pool_alloc_count);
        // 注意：这里不阻止reset，因为内存池设计就是顺序分配，reset会清空所有内存
        // 如果确实有泄漏，用户应该先调用cJSON_Delete释放所有对象
    }

    // 重置内存池指针和分配计数
    pool_ptr = cjson_pool;
    pool_alloc_count = 0U;

    CJSON_EXIT_CRITICAL();
    CJSON_POOL_LOG("Reset success, used: 0, alloc_count: 0");
}
