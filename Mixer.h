#ifndef __MIXER_H__
#define __MIXER_H__

#include <vector>
#include <functional>
#include <array>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <TH1D.h>
#include <TH2D.h> 
#include <TH3D.h>

template<class DataType>
class Mixer {
    using EventType = std::vector<DataType>;
    using PoolType = std::vector<EventType>;

private:
    std::array<int, 3> mixingTypes_;
    std::size_t centPools_;
    std::size_t vertexPools_;
    std::size_t poolDepth_;
    std::vector<PoolType> pools_;

    std::function<double(DataType)> getCent_;
    std::function<double(DataType)> getVert_;
    
    std::array<std::size_t, 2> centRange_ = {0, 100};
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
        pools_.reserve(centPools_ * vertexPools_);
    }

    ~Mixer() {};

    auto SetMixingType11() -> void { mixingTypes_.at(0) = 1; }
    auto SetMixingType12() -> void { mixingTypes_.at(1) = 1; }
    auto SetMixingType22() -> void { mixingTypes_.at(2) = 1; }
    auto UnsetMixingTypes() -> void { memset(mixingTypes_.data(), 0, 3 * sizeof(int)); }

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

    auto SetCentralityGetterFunction(std::function<double(DataType)> getter) {
        getCent_ = getter;
        return;
    }
    auto SetVertexGetterFunction(std::function<double(DataType)> getter) {
        getVert_ = getter;
        return;
    }
    auto FillPools(const EventType& event) -> void {}

};

#endif
