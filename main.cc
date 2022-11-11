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

auto GetMass(const Event& event1, std::size_t i1, 
             const Event& event2, std::size_t i2) -> double {
    const auto gamma1 = event1.clusters[i1];
    const auto gamma2 = event2.clusters[i2];

    const auto px = gamma1.px + gamma2.px;
    const auto py = gamma1.py + gamma2.py;
    const auto pz = gamma1.pz + gamma2.py;
    const auto E  = gamma1.E  + gamma2.E;

    const auto mass = std::sqrt(E*E - px*px - py*py - pz*pz);
    return mass;
}

auto GetPt(const Event& event1, std::size_t i1,
           const Event& event2, std::size_t i2) -> double {
    const auto gamma1 = event1.clusters[i1];
    const auto gamma2 = event2.clusters[i2];

    const auto px = gamma1.px + gamma2.px;
    const auto py = gamma1.py + gamma2.py;

    const auto pt = std::sqrt(px*px + py*py);
    return pt;
    
}

auto main() -> int {
    Mixer<Event> mixer(10, 6, 15, "output/out.root");
    mixer.SetMixingType11(1);
    mixer.SetCentralityGetterFunction([](const Event& event) -> double {
            return event.centrality;
        });
    mixer.SetVertexGetterFunction([](const Event& event) -> double {
            return event.vertex;
        });
    mixer.SetSizeGetterFunction([](const Event& event) -> std::size_t {
            return event.clusters.size();
            });

    mixer.AddOneDimHist("MassHist", "MIXERRRR!!!", 3000, 0, 3);
    mixer.AddOneDimXFunc(GetMass);

    mixer.AddTwoDimHist("MassPtHist", "MIXER!!!", 3000, 0, 3, 150, 0, 15);
    mixer.AddTwoDimXFunc(GetMass);
    mixer.AddTwoDimYFunc(GetPt);

    mixer.Print();

    auto inputFile = std::make_unique<TFile>("input/se-out.root", "read");
    auto tree = std::unique_ptr<TTree>{inputFile->Get<TTree>("AnalysisTree")};

    auto&& treeReader = TTreeReader(tree.get());
    auto px = TTreeReaderArray<double>(treeReader, "clusterPx");
    auto py = TTreeReaderArray<double>(treeReader, "clusterPy");
    auto pz = TTreeReaderArray<double>(treeReader, "clusterPz");
    auto e  = TTreeReaderArray<double>(treeReader, "clusterE");

    auto centrality = TTreeReaderValue<double>(treeReader, "centrality");
    auto vertex = TTreeReaderValue<double>(treeReader, "vertex");

    while (treeReader.Next()) {
        auto event = Event{};
        event.centrality = *centrality;
        event.vertex = *vertex;
        //std::cout << "Centrality: " << event.centrality << '\n';
        
        for (auto i = 0ull; i < px.GetSize(); ++i) {
            auto gamma = Gamma{px.At(i), py.At(i), pz.At(i), e.At(i)};
            //std::cout << gamma << '\n';

            event.clusters.push_back(gamma);
        }

        mixer.FillPools(event);
    }
    mixer.DrainAllPools();
    mixer.DumpHistos();

    return 0;
}
