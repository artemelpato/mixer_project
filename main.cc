#include <iostream>
#include <memory>
#include <sstream>
#include <iomanip>

#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderArray.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>

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

struct Event {
    double centrality;
    double vertex;

    std::vector<Gamma> clusters;
};

auto main() -> int {
    //auto mixer = Mixer<Event>(10, 6, 15);
    //mixer.SetMixingType11();
    //mixer.SetCentralityGetterFunction([](const Event& event) -> double {
    //        return 99.0;
    //    });
    //mixer.SetVertexGetterFunction([](const Event& event) -> double {
    //        return 29.0;
    //    });

    //mixer.Print();

    auto inputFile = std::make_unique<TFile>("input/se-out.root", "read");
    auto tree = std::unique_ptr<TTree>{inputFile->Get<TTree>("AnalysisTree")};

    auto&& treeReader = TTreeReader(tree.get());
    auto px = TTreeReaderArray<double>(treeReader, "clusterPx");
    auto py = TTreeReaderArray<double>(treeReader, "clusterPy");
    auto pz = TTreeReaderArray<double>(treeReader, "clusterPz");
    auto e  = TTreeReaderArray<double>(treeReader, "clusterE");

    auto centrality = TTreeReaderValue<double>(treeReader, "centrality");
    auto vertex = TTreeReaderValue<double>(treeReader, "vertex");

    auto hist = std::make_unique<TH2D>("hist", "", 3000, 0, 3, 150, 0, 15);
    auto outputFile = std::make_unique<TFile>("output/out.root", "recreate");
    while (treeReader.Next()) {
        for (auto i = 0ull; i < px.GetSize(); ++i) {
            for (auto j = i + 1; j < px.GetSize(); ++j) {
                const auto etaPx = px.At(i) + px.At(j);
                const auto etaPy = py.At(i) + py.At(j);
                const auto etaPz = pz.At(i) + pz.At(j);
                const auto etaE  = e.At(i)  + e.At(j);

                const auto mass = std::sqrt(etaE*etaE - etaPx*etaPx - etaPy*etaPy - etaPz*etaPz);
                const auto pt = std::sqrt(etaPx*etaPx + etaPy*etaPy);
                std::cout << mass << "\n";
                hist->Fill(mass, pt);
            }
        }
    }
    outputFile->cd();
    hist->Write();

    return 0;
}
