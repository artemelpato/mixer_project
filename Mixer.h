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

#include <TH1D.h>
#include <TH2D.h> 
#include <TH3D.h>

template<class EventType>
class Mixer {
    using PoolType = std::vector<EventType>;

private:
    std::array<int, 3> mixingTypes_;
    std::size_t centPools_;
    std::size_t vertexPools_;
    std::size_t poolDepth_;
    std::vector<PoolType> pools_;

    std::function<double(const EventType&)> getCent_;
    std::function<double(const EventType&)> getVert_;
    
    std::array<double, 2> centRange_ = {0, 100};
    std::array<double, 2> vertRange_ = {-30.0, 30.0};

public: 
    Mixer() : mixingTypes_(), centPools_(1), vertexPools_(1), poolDepth_(5) {
        pools_.reserve(centPools_ * vertexPools_);
        mixingTypes_.at(0) = 1; // default mixing type is 11;
    }

    Mixer(int centPools, int vertexPools, int poolDepth) 
      : mixingTypes_(), centPools_(centPools), vertexPools_(vertexPools), 
        poolDepth_(poolDepth) {
        
        mixingTypes_.at(0) = 1;
        pools_.resize(centPools_ * vertexPools_);
    }

    ~Mixer() {};

    auto SetMixingType11() -> void { mixingTypes_.at(0) = 1; }
    auto SetMixingType12() -> void { mixingTypes_.at(1) = 1; }
    auto SetMixingType22() -> void { mixingTypes_.at(2) = 1; }
    auto UnsetMixingTypes() -> void { 
        std::fill(begin(mixingTypes_), end(mixingTypes_), 0); 
        return;
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

        std::cout << str.str();
        return;
    }

    auto SetCentralityGetterFunction(std::function<double(const EventType&)> getter) {
        getCent_ = getter;
        return;
    }
    auto SetVertexGetterFunction(std::function<double(const EventType&)> getter) {
        getVert_ = getter;
        return;
    }
    auto FillPools(const EventType& event) -> void {
        const auto centrality = getCent_(event);
        const auto vertex = getVert_(event);
    
        assert(centrality > centRange_[0] && centrality < centRange_[1] 
                && "Centrality out of range!");
        assert(vertex > vertRange_[0] && vertex < vertRange_[1]
                && "Vertex out of range");

        const auto index = GetPoolIndex(centrality, vertex);
        pools_[index].push_back(event);
            
        for (auto& pool : pools_) {
            if (pool.size() >= poolDepth_) {
                DrainPools();
                pool.clear();
            }
        }

        return;
    }

    auto DrainPools() -> void {
        FillFG();
        FillBG();
        return;
    }
    
private:
    auto GetPoolIndex(const double centrality, const double vertex) -> std::size_t {
        const auto centWidth = (centRange_[1] - centRange_[0]) / centPools_;
        const auto vertWidth = (vertRange_[1] - vertRange_[0]) / vertexPools_;

        const auto centIndex = static_cast<std::size_t>(
                (centrality - centRange_[0]) / centWidth);
        const auto vertIndex = static_cast<std::size_t>(
                (vertex - vertRange_[0]) / vertWidth);

        const auto poolIndex = centIndex * vertexPools_ + vertIndex;
        return poolIndex;
    }

    auto FillFG() -> void {
        std::cout << "Filling FG...\n";
        return;
    }

    auto FillBG() -> void {
        std::cout << "Filling BG...\n";
        return;
    }
};

#endif
