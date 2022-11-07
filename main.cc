#include <iostream>
#include <memory>
#include <sstream>
#include <iomanip>

#include <TTree.h>
#include <TTreeReader.h>
#include <TFile.h>

struct Gamma {
    double px, py, pz, E;
    
    friend auto operator<<(std::ostream& out, const Gamma& gamma) -> std::ostream& {
        auto str = std::ostringstream();
        str << std::setw(10) << gamma.px  
            << std::setw(10) << gamma.py  
            << std::setw(10) << gamma.pz  
            << std::setw(10) << gamma.E;
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

    while (treeReader.Next()) {
        auto gamma = Gamma{*px, *py, *pz, *e};
        std::cout << gamma << "\n";
    }

    return 0;
}
