#include "TestState.h"
#include "fixed.h"
#include <stddef.h>
#include <gbfs.h>

static void TestState_construct(const GBFS_FILE* dat)
{
    
}

static void TestState_update(FIXED dt)
{
    
}

static void TestState_draw(void)
{
    
}

static void TestState_destroy(void)
{
    
}

const STATE test_state =
{
    TestState_construct,
    TestState_update,
    TestState_draw,
    TestState_destroy
};
