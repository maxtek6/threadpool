#include "threadpool.hpp"

#include <iostream>
#include <sstream>
#include <unordered_map>

#define PASS 0
#define FAIL 1
#ifndef __FUNCTION_NAME__
#ifdef WIN32 // WINDOWS
#define __FUNCTION_NAME__ __FUNCTION__
#else //*NIX
#define __FUNCTION_NAME__ __func__
#endif
#endif
#define TEST_ASSERT(COND) test_assert((COND), __FILE__, __FUNCTION_NAME__, __LINE__, #COND)

static void test_assert(
    bool condition,
    const std::string &file,
    const std::string &function,
    int line,
    const std::string &description);

static void test_constuctor();
static void test_submit();
static void test_shutdown();

int main(int argc, const char **argv)
{
    const std::unordered_map<std::string, std::function<void()>> unit_tests = {
        {"CONSTRUCTOR", test_constuctor},
        {"SUBMIT", test_submit},
        {"SHUTDOWN", test_shutdown},
    };
    int result;
    result = PASS;
    if (argc > 1)
    {
        try
        {
            const std::function<void()> &unit_test = unit_tests.at(argv[1]);
            unit_test();
        }
        catch (const std::exception &exception)
        {
            std::cerr << exception.what() << std::endl;
            result = FAIL;
        }
    }
    return result;
}

void test_assert(
    bool condition,
    const std::string &file,
    const std::string &function,
    int line,
    const std::string &description)
{
    std::stringstream error_stream;
    if (!condition)
    {
        error_stream << file << ":" << function << ":" << line << ":"
                     << " failed to assert \"" << description << "\"";
        throw std::runtime_error(error_stream.str());
    }
}

void test_constuctor()
{
    std::unique_ptr<maxtek::threadpool> threadpool;
    bool has_error;

    has_error = false;
    try
    {
        threadpool = std::unique_ptr<maxtek::threadpool>(new maxtek::threadpool(0));
    }
    catch(const std::exception& exception)
    {
        has_error = true;
    }
    TEST_ASSERT(has_error);

    threadpool = std::unique_ptr<maxtek::threadpool>(new maxtek::threadpool(4));
    TEST_ASSERT(threadpool != nullptr);
}

void test_submit()
{
    const std::function<bool()> time_wasting_activity = []()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return true;
    };
    std::unique_ptr<maxtek::threadpool> threadpool(new maxtek::threadpool(4));
    bool done;
    bool has_error;
    std::future<bool> future_result;

    done = false;
    future_result = threadpool->submit(time_wasting_activity);
    done = future_result.get();
    TEST_ASSERT(done);

    done = false;
    has_error = false;
    try
    {
        threadpool->shutdown();
        future_result = threadpool->submit(time_wasting_activity);
        done = future_result.get();
    }
    catch(const std::exception& e)
    {
        has_error = true;
    }
    TEST_ASSERT(!done);
    TEST_ASSERT(has_error);
}

void test_shutdown()
{
    std::unique_ptr<maxtek::threadpool> threadpool(new maxtek::threadpool(4));
    bool has_error;
    
    TEST_ASSERT(threadpool->active());
    threadpool->shutdown();
    TEST_ASSERT(!threadpool->active()); 

    has_error = false;
    try
    {
        threadpool->shutdown();
    }
    catch(const std::exception& e)
    {
        has_error = true;
    }
    TEST_ASSERT(has_error);
}