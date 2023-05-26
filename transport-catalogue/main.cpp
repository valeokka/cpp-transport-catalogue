#include <iostream>
#include <fstream>
#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"
// #include "tests.h"

int main(){
    //std::ifstream stream("/tests/test1.txt");
    TransportCatalogue::TransportCatalogue tc;

    //tc.AddBus("sA50azC97OMPt3BPX", {"tPniDCwlR","UsaQup0GVKc2oPtsPdvk"});
    TransportCatalogue::IReader IRead(std::cin);
    TransportCatalogue::SReader SRead(std::cin);
    //auto b =tc.GetBus("sA50azC97OMPt3BPX");
    //SRead.OutBusStat(tc,"sA50azC97OMPt3BPX" );
   // std::cout << b->name << " route =";
    // for(auto s : b->route){
    //     std::cout << " " << s->name << ",";
    // }
    

    IRead.ReadInput(tc);
    SRead.ReadStat(tc);
    // stream.close();
}