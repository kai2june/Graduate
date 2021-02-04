#pragma once
#include <vector>
#include "Transcript.hpp"
#include "ReadPair.hpp"
#include "SAMToReadPairs.hpp"

class EMAlgorithm
{
  public:
    EMAlgorithm(std::vector<Transcript>& txps, SAMToReadPairs& rps)
    {
        std::cerr << "I'm in EMAlgorithm constructor" << std::endl;
        init(txps, rps);
        run(txps, rps);
    }
    
    void init(std::vector<Transcript>& txps, SAMToReadPairs& rps)
    {
        std::atomic<std::size_t> unique_count{0};
        for(std::size_t i=0; i<rps.getReadPairs().size(); ++i)
        {
            const std::set<Transcript*, ReadPair::TranscriptPtrComp>& aligned_txps = rps.getReadPairs()[i].getAlignedTxps();
            if(aligned_txps.size() == 1)
            {
                (*aligned_txps.begin())->increaseUniqueCount(1);
                ++unique_count;
            }
        }
/*     如果有unique_count==0的transcript, 則需要處理一下(加個pseudocount)    */
        for(std::size_t i=0; i<txps.size(); ++i)
        {
            txps[i].setAbundance( txps[i].getUniqueCount()/unique_count.load() );
        }
    }

    void run(std::vector<Transcript>& txps, SAMToReadPairs& rps)
    {
        Estep(rps);
        Mstep(txps);
    }

    void Estep(SAMToReadPairs& rps)
    {
        const std::vector<ReadPair>& readpairs = rps.getReadPairs();
        for(std::size_t i=0; i<readpairs.size(); ++i)
        {
            
        }
    }

    void Mstep(std::vector<Transcript>& txps)
    {
        
    }

    bool isConverged() const
    {
        
    }
};