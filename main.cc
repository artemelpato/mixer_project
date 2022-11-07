#include <iostream>
#include <memory>
#include <sstream>
#include <iomanip>

#include <TTree.h>
#include <TTreeReader.h>
#include <TFile.h>

#include <Mixer.h>

struct Gamma {
    double px, py, pz, E;
    
    friend auto operator<<(std::ostream& out, const Gamma& gamma) -> std::ostream& {
        auto str = std::ostringstream();
        str << std::setw(20) << gamma.px  
            << std::setw(20) << gamma.py  
            << std::setw(20) << gamma.pz  
            << std::setw(20) << gamma.E;
        out << str.str();
        return out;
    }
};

auto main() -> int {
    auto&& inputFile = TFile("input/se-out.root","read");
    auto tree = std::unique_ptr<TTree>{inputFile.Get<TTree>("AnalisysTree")};

    auto&& treeReader = TTreeReader(tree.get());
    auto px = TTreeReaderValue<double>(treeReader, "gamma.px");
    auto py = TTreeReaderValue<double>(treeReader, "gamma.py");
    auto pz = TTreeReaderValue<double>(treeReader, "gamma.pz");
    auto e  = TTreeReaderValue<double>(treeReader, "gamma.E");

//    while (treeReader.Next()) {
//        auto gamma = Gamma{*px, *py, *pz, *e};
//        std::cout << gamma << "\n";
//    }

    auto mixer = Mixer<double>(10, 6, 15);
    mixer.SetMixingType11();
    mixer.Print();

    return 0;
}
