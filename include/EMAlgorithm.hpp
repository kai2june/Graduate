#pragma once
#include <map>
#include "Transcript.hpp"
#include "ReadPair.hpp"

class EMAlgorithm
{
  public:
    EMAlgorithm(std::vector<Transcript>& txps, std::vector<ReadPair>& read_pairs)
        : abundance_(txps.begin(), txps.end())
    {
        std::cerr << "I'm in EMAlgorithm constructor" << std::endl;
    }
    void run()
    {
        Estep();
    }

    void Estep()
    {
        
    }

    void Mstep()
    {
        
    }

    bool isConverged()
    {
        
    }

  private:
    std::map<Transcript, float> abundance_;
};