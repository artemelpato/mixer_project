#ifndef __MIXER_H__
#define __MIXER_H__

#include <vector>
#include <functional>
#include <array>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <memory>
#include <string>

#include <TH1D.h>
#include <TH2D.h> 
#include <TH3D.h>
#include <TFile.h>


template<class EventType>
class Mixer {
    using PoolType = std::vector<EventType>;

private:
    std::array<int, 3> mixingTypes_;
    std::size_t centPools_;
    std::size_t vertexPools_;
    std::size_t poolDepth_;
    std::vector<PoolType> pools_;

    std::array<double, 2> centRange_ = {-0.01, 100.01};
    std::array<double, 2> vertRange_ = {-30.0, 30.0};
    
    std::function<double(const EventType&)> getCent_;
    std::function<double(const EventType&)> getVert_;
    std::function<std::size_t(const EventType&)> getSize_;

    std::vector<std::string> dirnames_;

    std::vector<std::vector<std::unique_ptr<TH1D>>> oneDimHists_;
    std::vector<std::vector<std::unique_ptr<TH2D>>> twoDimHists_;
    std::vector<std::vector<std::unique_ptr<TH3D>>> threeDimHists_;

    std::vector<std::function<double(
            const EventType&, std::size_t,
            const EventType&, std::size_t)>> oneDimXFuncs_;

    std::vector<std::function<double(
            const EventType&, std::size_t,
            const EventType&, std::size_t)>> twoDimXFuncs_;
    std::vector<std::function<double(
            const EventType&, std::size_t,
            const EventType&, std::size_t)>> twoDimYFuncs_;

    std::vector<std::function<double(
            const EventType&, std::size_t,
            const EventType&, std::size_t)>> threeDimXFuncs_;
    std::vector<std::function<double(
            const EventType&, std::size_t,
            const EventType&, std::size_t)>> threeDimYFuncs_;
    std::vector<std::function<double(
            const EventType&, std::size_t,
            const EventType&, std::size_t)>> threeDimZFuncs_;

    std::function<double(
            const EventType&, std::size_t,
            const EventType&, std::size_t)> pairIsGood_;

    std::unique_ptr<TFile> outputFile_;

public: 
    Mixer() : mixingTypes_(), centPools_(1), vertexPools_(1), poolDepth_(5),
              outputFile_() {

        TH1::AddDirectory(false);
        pools_.resize(centPools_ * vertexPools_);
        mixingTypes_.at(0) = 1; // default mixing type is 11;
        oneDimHists_.resize(centPools_ * vertexPools_);
        twoDimHists_.resize(centPools_ * vertexPools_);
        threeDimHists_.resize(centPools_ * vertexPools_);
        dirnames_.resize(centPools_ * vertexPools_);
        
        std::ostringstream str{};
        for (auto icent = 0ull; icent < centPools_; ++icent) {
            for (auto ivert = 0ull; ivert < vertexPools_; ++ivert) {
                str << "c0" << icent << "_z0" << ivert << "_r00"; 
                const auto poolIndex = icent * vertexPools_ + ivert;
                dirnames_[poolIndex] = str.str();
                str.str("");
            }
        }
    }

    Mixer(int centPools, int vertexPools, int poolDepth, const char* name) 
      : mixingTypes_(), centPools_(centPools), vertexPools_(vertexPools), 
        poolDepth_(poolDepth), outputFile_(std::make_unique<TFile>(name, "recreate")) {
        
        TH1::AddDirectory(false);
        mixingTypes_.at(0) = 1;
        pools_.resize(centPools * vertexPools);
        oneDimHists_.resize(centPools * vertexPools);
        twoDimHists_.resize(centPools * vertexPools);
        threeDimHists_.resize(centPools * vertexPools);
        dirnames_.resize(centPools * vertexPools);

        std::ostringstream str{};
        for (auto icent = 0ull; icent < centPools_; ++icent) {
            for (auto ivert = 0ull; ivert < vertexPools_; ++ivert) {
                str << "c0" << icent << "_z0" << ivert << "_r00"; 
                const auto poolIndex = icent * vertexPools_ + ivert;
                dirnames_[poolIndex] = str.str();
                str.str("");
            }
        }
    }

    //~Mixer() {};

    auto SetMixingType11(int type) -> void { mixingTypes_.at(0) = type; }
    auto SetMixingType12(int type) -> void { mixingTypes_.at(1) = type; }
    auto SetMixingType22(int type) -> void { mixingTypes_.at(2) = type; }
    auto UnsetMixingTypes() -> void { 
        std::fill(begin(mixingTypes_), end(mixingTypes_), 0); 
        return;
    }

    template <typename... Args>
    auto AddOneDimHist(Args... args) -> void {
        for (auto& hists : oneDimHists_) {
            hists.push_back(std::make_unique<TH1D>(args...));
        }
    }

    template <typename... Args>
    auto AddTwoDimHist(Args... args) -> void {
        for (auto& hists : twoDimHists_) {
            hists.push_back(std::make_unique<TH2D>(args...));
        }
    }

    template <typename... Args>
    auto AddThreeDimHist(Args... args) -> void {
        for (auto& hists : threeDimHists_) {
            hists.push_back(std::make_unique<TH3D>(args...));
        }
    }

    auto AddOneDimXFunc(std::function<double(const EventType&, std::size_t, const EventType&, std::size_t)> func) -> void {
        oneDimXFuncs_.push_back(func);
    }

    auto AddTwoDimXFunc(std::function<double(const EventType&, std::size_t, const EventType&, std::size_t)> func) -> void {
        twoDimXFuncs_.push_back(func);
    }
    auto AddTwoDimYFunc(std::function<double(const EventType&, std::size_t, const EventType&, std::size_t)> func) -> void {
        twoDimYFuncs_.push_back(func);
    }

    auto AddThreeDimXFunc(std::function<double(const EventType&, std::size_t, const EventType&, std::size_t)> func) -> void {
        threeDimXFuncs_.push_back(func);
    }
    auto AddThreeDimYFunc(std::function<double(const EventType&, std::size_t, const EventType&, std::size_t)> func) -> void {
        threeDimYFuncs_.push_back(func);
    }
    auto AddThreeDimZFunc(std::function<double(const EventType&, std::size_t, const EventType&, std::size_t)> func) -> void {
        threeDimZFuncs_.push_back(func);
    }

    auto Print() const -> void { 
        std::ostringstream str;

        str << std::setw(10) << "Mixing Types:\n";
        str << std::setw(10) << "11" << std::setw(10) << "12" << std::setw(10) << "22\n";
        for (const auto mixingType : mixingTypes_) {
            str << std::setw(10) << mixingType;
        }
        str << "\n";

        str << std::left << std::setw(20) << "Cent Pools: "  << centPools_ << "\n";
        str << std::left << std::setw(20) << "Vertex Pools: " << vertexPools_ << "\n";
        str << std::left << std::setw(20) << "Pools Depth: " << poolDepth_ << "\n";

        str << '\n';
        str << "One Dim Hists\n";
        for (const auto& hist : oneDimHists_.at(0)) {
            str << hist->GetName() << '\n';
        }

        str << '\n';
        str << "Two Dim Hists\n";
        for (const auto& hist : twoDimHists_.at(0)) {
            str << hist->GetName() << '\n';
        }

        str << '\n';
        str << "Three Dim Hists\n";
        for (const auto& hist : threeDimHists_.at(0)) {
            str << hist->GetName() << '\n';
        }

        str << "\nFuncs:\n";
        str << "1D X: " << oneDimXFuncs_.size() << "\n\n";

        str << "2D X: " << twoDimXFuncs_.size() << '\n';
        str << "2D Y: " << twoDimYFuncs_.size() << "\n\n";

        str << "3D X: " << threeDimXFuncs_.size() << '\n';
        str << "3D Y: " << threeDimYFuncs_.size() << '\n';
        str << "3D Y: " << threeDimZFuncs_.size() << "\n\n";

        std::cout << str.str();
        return;
    }

    auto SetCentralityGetterFunction(std::function<double(const EventType&)> getter) {
        getCent_ = getter;
    }
    auto SetVertexGetterFunction(std::function<double(const EventType&)> getter) {
        getVert_ = getter;
    }
    auto SetSizeGetterFunction(std::function<std::size_t(const EventType&)> func) -> void {
        getSize_ = func;
    }
    auto SetPairIsGoodFunction(std::function<bool(const EventType&, std::size_t, const EventType&, std::size_t)> func) -> void {
        pairIsGood_ = func;
    }

    auto FillPools(const EventType& event) -> void {
        const auto centrality = getCent_(event);
        const auto vertex = getVert_(event);
    

        const auto index = GetPoolIndex(centrality, vertex);
        pools_[index].push_back(event);
            
        for (auto& pool : pools_) {
            if (pool.size() >= poolDepth_) {
                DrainPool(pool);
                pool.clear();
            }
        }

        return;
    }

    auto DrainPool(const PoolType& pool) -> void {
        FillFG(pool);
        FillBG(pool);
        return;
    }

    auto DrainAllPools() -> void { 
        for (const auto& pool : pools_) {
            DrainPool(pool);
        }
        return;
    }

    auto DumpHistos() -> void {
        outputFile_->cd();
        for (const auto& dirname : dirnames_) {
            outputFile_->mkdir(dirname.c_str());
        }

        for (auto i = 0ull; i < dirnames_.size(); ++i) {
            outputFile_->cd(dirnames_[i].c_str());
            for (const auto& hist : oneDimHists_[i]) hist->Write();
            for (const auto& hist : twoDimHists_[i]) hist->Write();
            for (const auto& hist : threeDimHists_[i]) hist->Write();
        }
        outputFile_->Close();

        return;
    }
    
private:
    auto GetPoolIndex(const double centrality, const double vertex) -> std::size_t {
        assert(centrality > centRange_[0] && centrality < centRange_[1] 
                && "Centrality out of range!");
        assert(vertex > vertRange_[0] && vertex < vertRange_[1]
                && "Vertex out of range");

        const auto centWidth = (centRange_[1] - centRange_[0]) / centPools_;
        const auto vertWidth = (vertRange_[1] - vertRange_[0]) / vertexPools_;

        const auto centIndex = static_cast<std::size_t>(
                (centrality - centRange_[0]) / centWidth);
        const auto vertIndex = static_cast<std::size_t>(
                (vertex - vertRange_[0]) / vertWidth);

        const auto poolIndex = centIndex * vertexPools_ + vertIndex;
        return poolIndex;
    }

    auto FillFG(const PoolType& pool) -> void {
        for (const auto& event : pool) {
            const auto eventSize = getSize_(event);
            const auto centrality = getCent_(event);
            const auto vertex = getVert_(event); 
            const auto poolIndex = GetPoolIndex(centrality, vertex);

            for (auto i1 = 0ull; i1 < eventSize; ++i1) {
                for (auto i2 = i1 + 1; i2 < eventSize; ++i2) {
                    
                    if (!pairIsGood_(event, i1, event, i2)) continue;

                    for (auto iOneDim = 0ull; iOneDim < oneDimHists_.at(0).size(); ++iOneDim) {
                        const auto fillXValue = oneDimXFuncs_[iOneDim](event, i1, event, i2);
                        oneDimHists_[poolIndex][iOneDim]->Fill(fillXValue);
                    }

                    for (auto iTwoDim = 0ull; iTwoDim < twoDimHists_.at(0).size(); ++iTwoDim) {
                        const auto fillXValue = twoDimXFuncs_[iTwoDim](event, i1, event, i2);
                        const auto fillYValue = twoDimYFuncs_[iTwoDim](event, i1, event, i2);
                        twoDimHists_[poolIndex][iTwoDim]->Fill(fillXValue, fillYValue);
                    }

                    for (auto iThreeDim = 0ull; iThreeDim < threeDimHists_.at(0).size(); ++iThreeDim) {
                    }

                }
            }
        }
        return;
    }

    auto FillBG(const PoolType& pool) -> void {
        for (auto ev1 = begin(pool); ev1 != end(pool); ++ev1) {
            for (auto ev2 = ev1 + 1; ev2 != end(pool); ++ev2) {
                const auto size1 = getSize_(*ev1);
                const auto size2 = getSize_(*ev2);

                for (auto i1 = 0ull; i1 < size1; ++i1) {
                    for (auto i2 = 0ull; i2 < size2; ++i2) {
                        if (!pairIsGood_(*ev1, i1, *ev2, i2)) continue;

                        for (auto iOneDim = 0ull; iOneDim < oneDimHists_.at(0).size(); ++iOneDim) {
                            const auto fillXValue = oneDimXFuncs_[iOneDim](*ev1, i1, *ev2, i2);
                            //oneDimHists_[iOneDim]->Fill(fillXValue);
                        }

                        for (auto iTwoDim = 0ull; iTwoDim < twoDimHists_.at(0).size(); ++iTwoDim) {
                            const auto fillXValue = twoDimXFuncs_[iTwoDim](*ev1, i1, *ev2, i2);
                            const auto fillYValue = twoDimYFuncs_[iTwoDim](*ev1, i1, *ev2, i2);
                            //twoDimHists_[iTwoDim]->Fill(fillXValue, fillYValue);
                        }

                        for (auto iThreeDim = 0ull; iThreeDim < threeDimHists_.at(0).size(); ++iThreeDim) {
                        }
                    }
                }
            }
        }

        return;
    }
};

#endif
