/// benchmarking software that measures computing performance.
/// start simple: single thread single instruction (inc)
#include<cmath>
#include<string>
#include<vector>
#include<iomanip>
#include"main.h++"
#include"resources.h++"
//#include"main2.h++"

std::size_t outputwidth;

void printinfo(const char *const instruction, std::size_t data[2], std::size_t instopratio=1){
    std::cout<<resources["information.finish"]<<std::endl<<std::endl;

    std::cout<<std::left<<std::setw(outputwidth)<<resources["results.header"];
    std::cout<<std::right<<std::setw(std::string(instruction).size())<<instruction<<std::endl;
    std::cout<<std::left<<std::setw(outputwidth)<<resources["results.instructions.count"];
    std::cout<<std::right<<std::setw(std::string(instruction).size())<<data[0]<<std::endl;
    std::cout<<std::left<<std::setw(outputwidth)<<resources["results.operations.count"];
    std::cout<<std::right<<std::setw(std::string(instruction).size())<<data[0]*instopratio<<std::endl;
    std::cout<<std::left<<std::setw(outputwidth)<<resources["results.time"];
    std::cout<<std::right<<std::setw(std::string(instruction).size())<<data[1]<<std::endl;
    std::cout<<std::left<<std::setw(outputwidth)<<resources["results.instructions.throughput"];
    std::cout<<std::right<<std::setw(std::string(instruction).size())<<((double)data[0])/data[1]<<std::endl;
    std::cout<<std::left<<std::setw(outputwidth)<<resources["results.operations.throughput"];
    std::cout<<std::right<<std::setw(std::string(instruction).size())<<((double)data[0]*instopratio)/data[1]<<std::endl;
    std::cout<<std::left<<std::setw(outputwidth)<<resources["results.instructions.nanosecondsper"];
    std::cout<<std::right<<std::setw(std::string(instruction).size())<<data[1]/((double)data[0])<<std::endl;
    std::cout<<std::left<<std::setw(outputwidth)<<resources["results.operations.nanosecondsper"];
    std::cout<<std::right<<std::setw(std::string(instruction).size())<<data[1]/((double)data[0]*instopratio)<<std::endl;
    std::cout<<"\n";
    std::string tmp;
    std::getline(std::cin, tmp);
}


int main(int c, char **v){
    static std::string outputresources[]={
        "results.header",
        "results.instructions.count",
        "results.operations.count",
        "results.time",
        "results.instructions.throughput",
        "results.instructions.nanosecondsper",
        "results.operations.nanosecondsper"
    };
    initresources("./resources.mkbrs");
    // determine the output width
    outputwidth=0;
    for(auto i=0;i<sizeof(outputresources)/sizeof(std::string);i++){
        outputwidth=std::max(outputwidth,resources[outputresources[i]].size()+4);
    }
    if(c)for(auto i=0;i<c;i++){
        if(std::string("--clock").find(v[i])!=std::string::npos){
            // to find our clock speed within a reasonable amount of 
            // time, let's find 100 clock speeds
            std::vector<std::size_t> clocks(100);
            for(auto i=0;i<clocks.size();i++){
                std::cout<<resources["running.test"];
                std::cout<<std::setw(3)<<std::right<<(i+1);
                std::cout<<std::internal<<resources["running.of"];
                std::cout<<clocks.size()<<resources["running.ellipses"]<<std::endl;
                clocks[i]=estclockspeed()[0];
            }
            std::cout<<resources["information.finish"]<<std::endl;
            // get these as doubles in GHz
            std::cout<<resources["test.clock.bigdata.conversion"]<<std::endl;
            std::vector<double> dclocks(clocks.size());
            for(auto i=0;i<clocks.size();i++)dclocks[i]=clocks[i]/5000000000.0;
            // find the mean
            std::cout<<resources["test.clock.bigdata.mean"]<<std::endl;
            double mean=0.0;
            for(auto i=0;i<dclocks.size();i++)mean+=dclocks[i];
            mean/=dclocks.size();
            std::cout<<resources["test.clock.bigdata.stddev"]<<std::endl;
            double sigma=0.0;
            // find the standard deviation
            for(auto i=0;i<dclocks.size();i++){
                sigma+=pow((dclocks[i]-mean),2);
            }
            sigma/=dclocks.size();
            sigma=std::sqrt(sigma);

            std::cout<<resources["test.clock.bigdata.confidence.1"]<<std::endl;
            std::cout<<resources["test.clock.bigdata.confidence.2"]<<std::endl;

            // finding 95% confidence
            constexpr double zstar=1.9599639859915;

            std::cout<<resources["test.clock.bigdata.report.1"]<<std::endl;
            std::cout<<mean-zstar*sigma;
            std::cout<<resources["test.clock.bigdata.report.2"];
            std::cout<<mean+zstar*sigma;
            std::cout<<resources["test.clock.bigdata.report.3"]<<std::endl;
            std::cout<<resources["test.clock.bigdata.report.4"];
            std::cout<<resources["test.clock.bigdata.report.5"];
            std::cout<<resources["test.clock.bigdata.report.6"];
            return 0;
        }
    }
    for(;;){
        int i=0;
        while(!i){    
            for(auto j=0;j<sizeof(tests)/sizeof(test_s);j++){
                std::cout<<(j+1)<<". "<<resources[std::string(tests[j].name)+".name"]<<std::endl;
            }
            std::cout<<resources["prompt.user.1"]<<std::endl;
            std::cout<<resources["prompt.user.2"]<<std::endl;
            std::cin>>i;
            if(i>sizeof(tests)/sizeof(test_s)+2)i=0;
            if(i==sizeof(tests)/sizeof(test_s)+1)return 0;
            if(i<0)i=0;
        }
        std::string title=resources[std::string(tests[i-1].title)+".title"];
        std::cout<<resources["running.test"]<<i<<": ";
        std::cout<<title<<std::endl;
        printinfo(title.c_str(), tests[i-1].testfn(), tests[i-1].inoprat);
    }
    
}