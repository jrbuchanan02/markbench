#pragma once


#include<cstdint>
#include<cstddef>
#include<functional>
#include<thread>
#include<iostream>
#include<algorithm>


#define ___TEST_SETUP___ auto time=std::chrono::steady_clock(); \
    std::size_t counter=0; \
    bool go=true;
#define ___TEST_CLNUP___ \
    auto thread=std::thread(fn); \
    auto start=time.now();\
    thread.detach();\
    using namespace std::chrono_literals;\
    std::this_thread::sleep_for(5s);\
    go=false;\
    std::size_t *data=new std::size_t[2];\
    do{\
        data[0]=counter;\
        data[1]=(time.now()-start).count();\
    }while (counter&0x0f);\
    return data;
#define ___TEST_MULTI_SETUP___ static auto maxthreads=std::thread::hardware_concurrency;\
    auto time=std::chrono::steady_clock();\
    std::size_t *counter=new std::size_t[maxthreads()];\
    for (unsigned i=0;i<maxthreads();i++) counter[i]=0;\
    bool go=true;
#define ___TEST_MULTI_CLNUP___ std::thread *threads=new std::thread[maxthreads()];\
    for (unsigned i=0;i<maxthreads();i++){\
        threads[i]=std::thread(std::bind(fn, i));\
    }\
    auto start=time.now();\
    for (unsigned i=0;i<maxthreads();i++){\
        threads[i].detach();\
    }\
    using namespace std::chrono_literals;\
    std::this_thread::sleep_for(5s);\
    go=false;\
    std::size_t *data=new std::size_t[2];\
    data[0]=1;\
    do{\
        data[0]=0;\
        for(unsigned i=0;i<maxthreads();i++)data[0]+=counter[i];\
        data[1]=(time.now()-start).count();\
    }while (data[0]%48);\
    return data;

#define ___DOFOR16___(OP) \
    OP\
    OP\
    OP\
    OP\
    OP\
    OP\
    OP\
    OP\
    OP\
    OP\
    OP\
    OP\
    OP\
    OP\
    OP\
    OP

std::size_t *addregregfortime(){
    ___TEST_SETUP___
    auto fn=[&](){
        while(go){
            ___DOFOR16___(__asm__("add %rax, %rax");)
            counter+=0x10;
        }
    };
    ___TEST_CLNUP___
}

std::size_t *maxaddinstructs(){
    ___TEST_SETUP___
    auto fn=[&](){
        while(go){
            ___DOFOR16___(__asm__("add %rax, %rax");
            __asm__("add %rbx, %rbx");
            __asm__("vpaddq %ymm0, %ymm0, %ymm0");)
            counter+=48; // 3 * 16
        }
    };
    ___TEST_CLNUP___
}

std::size_t *addregavxfortime(){
    ___TEST_SETUP___
    auto fn=[&](){
        while(go){
            ___DOFOR16___(__asm__("vpaddq %ymm0, %ymm0, %ymm0");)
            counter+=0x10;
        }
    };
    ___TEST_CLNUP___
}

std::size_t *doubleaddregregfortime(){
    ___TEST_SETUP___
    auto fn=[&](){
        while(go){
            ___DOFOR16___(__asm__("vaddpd %ymm0, %ymm0, %ymm0");)
            counter+=0x10;
        }
    };
    ___TEST_CLNUP___
}

std::size_t *floataddregregfortime(){
    ___TEST_SETUP___
    auto fn=[&](){
        while(go){
            ___DOFOR16___(__asm__("vaddps %ymm0, %ymm0, %ymm0");)
            counter+=0x10;
        }
    };
    ___TEST_CLNUP___
}

std::size_t *lshiftsfortime(){
    ___TEST_SETUP___
    auto fn=[&](){
        while(go){
            ___DOFOR16___(__asm__("sal $1, %rax");)
            counter+=0x10;
        }
    };
    ___TEST_CLNUP___
}

std::size_t *rshiftsfortime(){
    ___TEST_SETUP___
    auto fn=[&](){
        while(go){
            ___DOFOR16___(__asm__("sar $1, %rax");)
            counter+=0x10;
        }
    };
    ___TEST_CLNUP___
}

std::size_t *multithreadmaxaddinstfortime(){
    ___TEST_MULTI_SETUP___
    auto fn=[&](std::size_t mythread){
        while(go){
            ___DOFOR16___(__asm__("add %rax, %rax");
            __asm__("add %rbx, %rbx");
            __asm__("vpaddq %ymm0, %ymm0, %ymm0");)
            counter[mythread]+=48;
        }
    };
    ___TEST_MULTI_CLNUP___
}

std::size_t *addandaddfortime(){
    ___TEST_SETUP___
    auto fn=[&](){
        while(go){
            ___DOFOR16___(__asm__("add %rax, %rax");
                          __asm__("add %rbx, %rbx");)
            counter+=0x20;
        }
    };
    ___TEST_CLNUP___
}

std::size_t *fisrfortime(){
    union numberlong{
        std::uint32_t n;
        float f;
    };
    constexpr auto ____LENGTH=1000;
    constexpr float threehalfs=1.5f;
    static float _data[____LENGTH];
    for(auto i=0;i<____LENGTH;i++){
        _data[i]=0;
        while(_data<=0)_data[i]=rand();
    }
    auto index=0;
    ___TEST_SETUP___
    auto fn=[&](){
        while(go){
            float *ptr;
            ___DOFOR16___(
                index++;
                if(index+4>=____LENGTH)index=0;
                ptr=index*sizeof(float)+_data;
                __asm__("vmovups %0, %%XMM0" : "=a"(ptr));
                __asm__("rsqrtps %XMM0, %XMM0");
            )
            counter+=0x10;
        }
    };
    ___TEST_CLNUP___
}

std::size_t *estclockspeed(){
    ___TEST_SETUP___
    __asm__("vpaddq %ymm0, %ymm0, %ymm0"); // warm up!
    auto fn=[&](){
        while(go){
            ___DOFOR16___(__asm__("add %rax, %rax");)
            ___DOFOR16___(__asm__("add %rax, %rax");)
            counter+=0x10;
        }
    };
    ___TEST_CLNUP___
}

std::size_t *avxencryptfortime(){
    // create a large buffer to perform avx encryption on
    // assuming 1 GB / second, let's get a 512 MB buffer
    // to go beyond L3 cache.
    void *avxbuffer=nullptr;
    while(!avxbuffer){
        std::cout<<"Allocating buffer for AES..."<<std::endl;
        avxbuffer=malloc(512*1024*1024);
    }
    std::uint64_t seed[2]={};
    seed[0]=rand()<<31+rand();
    seed[1]=rand()<<31+rand();
    // fill avxbuffer with randomness
    for(auto i=0;i<512*1024*1024/sizeof(int);i++){
        ((int *)avxbuffer)[i]=rand();
    }
    auto index=0;
    ___TEST_SETUP___
    auto fn=[&](){
        while(go){
            
        }
    };
    return nullptr; // FIXME: complete
}

struct test_s{
    const char *const title;
    const char *const name;
    std::size_t *(*testfn)();
    std::size_t inoprat;
};

test_s tests[]={
    {"test.addregreg", "test.addregreg", &addregregfortime, 1LLU},
    {"test.addregavx", "test.addregavx", &addregavxfortime, 4LLU},
    {"test.maxaddinstructs", "test.maxaddinstructs", &maxaddinstructs, 3LLU},
    {"test.multithreadmaxaddinst", "test.multithreadmaxaddinst", &multithreadmaxaddinstfortime, 3LLU},
    {"test.lshifts", "test.lshifts", &lshiftsfortime, 1LLU},
    {"test.rshifts", "test.rshifts", &rshiftsfortime, 1LLU},
    {"test.doubleaddregreg", "test.doubleaddregreg", &doubleaddregregfortime, 4LLU},
    {"test.singleaddregreg", "test.singleaddregreg", &floataddregregfortime, 8LLU},
    {"test.addandadd","test.addandadd", &addandaddfortime, 1LLU},
    {"test.fisr","test.fisr", &fisrfortime, 4LLU,},
    {"test.clock", "test.clock", &estclockspeed,1LLU}
};