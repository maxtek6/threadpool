#include <iostream>
#include <threadpool.hpp>

using namespace maxtek;

void trivialFunc()
{
    while (1)
    {
      std::cout << "Hello from task\n";
    }
}

int main()
{
    // Testing commit
    threadpool myThreadPool(5);
  
    std::future<void> future = myThreadPool.submit(trivialFunc);
    // TODO: Use future properly
    //future.wait();
    while (1);
    return 0;
}
