#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>


int depqbf_run_with_timeout()
{
    std::chrono::milliseconds span (100);
    std::mutex m;
    std::condition_variable cv;
    int retValue;
    
    std::thread t([&m, &cv, &retValue]() 
    {
        retValue = system("./src/bash_script.sh");
        cv.notify_one();
    });
    
    t.detach();
    
    {
        std::unique_lock<std::mutex> l(m);
        if(cv.wait_for(l, span) == std::cv_status::timeout) 
            throw std::runtime_error("Timeout");
    }
   
    if(retValue == -1){
      std::cout << "SYTEM ERROR !!!\n";
     }

 //   std::cout << "Return Value is : " << retValue << "\n";
    return retValue;    
}
