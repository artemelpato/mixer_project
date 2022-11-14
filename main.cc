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
    double x, y, z;
    int arm, sector;
    
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

    const auto E  = gamma1.E  + gamma2.E;
    const auto px = gamma1.px + gamma2.px;
    const auto py = gamma1.py + gamma2.py;
    const auto pz = gamma1.pz + gamma2.pz;

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

auto PairIsGood(const Event& event1, std::size_t i1,
           const Event& event2, std::size_t i2) -> bool {
    const auto gamma1 = event1.clusters[i1];
    const auto gamma2 = event2.clusters[i2];

    const auto E  = gamma1.E  + gamma2.E;
    const auto deltaE = gamma1.E - gamma2.E;
    const auto alpha = std::abs(deltaE) / E;
    const auto alphaIsGood = alpha < 0.8;

    const auto sameArm = gamma1.arm == gamma2.arm;
    const auto sameSector = gamma1.sector == gamma2.sector;
    
    const auto pairIsGood = alphaIsGood && sameArm && sameSector;
    return pairIsGood;
}

auto main() -> int {
    Mixer<Event> mixer{10, 6, 15, "output/out.root"};
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

    mixer.SetPairIsGoodFunction(PairIsGood);

    mixer.AddOneDimHist("MassHist", "MIXERRRR!!!", 3000, 0, 3);
    mixer.AddOneDimXFunc(GetMass);

    mixer.AddTwoDimHist("MassPtHist", "MIXER!!!", 3000, 0, 3, 150, 0, 15);
    mixer.AddTwoDimXFunc(GetMass);
    mixer.AddTwoDimYFunc(GetPt);

    mixer.Print();

    TFile inputFile{"input/se-out.root", "read"};
    std::unique_ptr<TTree> tree{inputFile.Get<TTree>("AnalysisTree")};

    TTreeReader treeReader(tree.get());

    TTreeReaderArray<double> px{treeReader, "clusterPx"};
    TTreeReaderArray<double> py{treeReader, "clusterPy"};
    TTreeReaderArray<double> pz{treeReader, "clusterPz"};
    TTreeReaderArray<double> e{treeReader, "clusterE"};

    TTreeReaderArray<double> x{treeReader, "clusterX"};
    TTreeReaderArray<double> y{treeReader, "clusterY"};
    TTreeReaderArray<double> z{treeReader, "clusterZ"};

    TTreeReaderArray<int> arm{treeReader, "clusterArm"};
    TTreeReaderArray<int> sector{treeReader, "clusterSector"};

    TTreeReaderValue<double> centrality{treeReader, "centrality"};
    TTreeReaderValue<double> vertex{treeReader, "vertex"};

    auto nEvents = 0ull;
    while (treeReader.Next()) {
        Event event{};
        event.centrality = *centrality;
        event.vertex = *vertex;
        
        for (auto i = 0ull; i < px.GetSize(); ++i) {
            Gamma gamma{px.At(i), py.At(i), pz.At(i), e.At(i),
                        x.At(i), y.At(i), z.At(i), arm.At(i), sector.At(i)};
            event.clusters.push_back(gamma);
        }

        mixer.FillPools(event);

        if (nEvents++ % 100000 == 0) 
            std::cout << "Number of events: " << nEvents / 1000 << "k\n";
    }
    mixer.DrainAllPools();
    mixer.DumpHistos();

    return 0;
}
