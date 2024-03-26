#include <iostream>
#include <threadpool.hpp>
#include <random>
#include <condition_variable>

const int BUF_SIZE = 10;
const int POOL_SIZE = 100;

std::mutex mtx;
std::condition_variable cv;
std::vector<int> item_buf; 

using namespace maxtek;


int buf[BUF_SIZE];

int get_random_int()
{
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(1,100); 

    return distr(eng);
}

void producerThread()
{
    int count = 0;
    int item;
    while (1)
    {
      item = get_random_int();
      std::unique_lock<std::mutex> lock(mtx);
      std::cout << "Hello from task: " << "bytes written: " << sizeof(int) * count << "  " << std::this_thread::get_id() << "\n";
      if (item_buf.size() == item_buf.capacity())
      {
          cv.wait(lock, [] { return (item_buf.size() < item_buf.capacity()); });
      }
      item_buf.push_back(item);
      ++count;
      lock.unlock();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void consumerThread()
{
    while (1)
    {
	int item;
        std::unique_lock<std::mutex> lock(mtx);
        // time now
        if (item_buf.size() == 0)
	{
            cv.wait(lock, [] { return (item_buf.size() > 0); });
	}
        item = item_buf.back();
        item_buf.pop_back();
	std::cout << "thread id: " << std::this_thread::get_id() << " consumed item: " << item << "\n";
	cv.notify_one();
	// compute time diff, sleep to be one second
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main()
{
    item_buf.reserve(BUF_SIZE);
    threadpool myThreadPool(POOL_SIZE);

  
    for (int i = 0; i < POOL_SIZE/2; ++i)
    {
        std::future<void> future = myThreadPool.submit(producerThread);
    }

    for (int i = 0; i < POOL_SIZE/2; ++i)
    {
        std::future<void> future = myThreadPool.submit(consumerThread);
    }
    // TODO: Use future properly
    //future.wait();
    while (1);
    return 0;
}
