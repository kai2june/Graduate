#pragma once
#include <vector>
#include <unordered_map>
#include <numeric>
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
        for(const ReadPair& rp : rps.getReadPairs())
            for(Transcript* tr : rp.getAlignedTxps())
                tr->increaseTotalCount( 1.0 / static_cast<double>(rp.getAlignedTxps().size()) );

/*     如果有unique_count==0的transcript, 則需要處理一下(加個pseudocount)    */
        for(std::size_t i=0; i<txps.size(); ++i)
            txps[i].setAbundance( txps[i].getTotalCount() / static_cast<double>(rps.getReadPairs().size()) );
        
        for(Transcript& tr : txps)
        {
            tr.setEffectiveLength( calculateEffectiveLength(tr, rps.getFLD()) );
        }
    }

    void run(std::vector<Transcript>& txps, SAMToReadPairs& rps)
    {
        do
        {
            Estep(rps);
            Mstep(rps);
        } while(!convergence_);
    }

    void Estep(SAMToReadPairs& rps)
    {
        std::cerr << "I'm in Estep." << std::endl;
        resetTxpsTotalCount(rps);

        for(const ReadPair& rp : rps.getReadPairs())
        {
            std::vector<double> prob;
            const std::set<Transcript*, ReadPair::TranscriptPtrComp>& txps = rp.getAlignedTxps();
            for(Transcript* tr : txps)
            {
                uint32_t tlen = rp.getPosTlen().at(tr->getName()).second;
                prob.emplace_back(tr->getAbundance() * rps.getFLD().getPDF(tlen) / (tr->getLength() - tlen +1) );
            }

            double sum = std::accumulate(prob.begin(), prob.end(), 0.0);
            std::size_t i(0);
            for(Transcript* tr : txps)
                tr->increaseTotalCount(prob[i++] / sum);
        }
    }

    void Mstep(SAMToReadPairs& rps)
    {
        std::cerr << "I'm in Mstep." << std::endl;
        double sum = 0.0;
        for(Transcript& tr : rps.accessRefTxps())
            sum += tr.getTotalCount() / tr.getEffectiveLength();

        double threshold{1.0 / static_cast<double>(rps.accessRefTxps().size()) };
        for(Transcript& tr : rps.accessRefTxps())
        {
            double cur_abundance = tr.getTotalCount() / tr.getEffectiveLength() / sum;
            if(cur_abundance - tr.getAbundance() > threshold)
                convergence_ = false;
            double prev = tr.getAbundance();
            tr.setAbundance(tr.getTotalCount() / tr.getEffectiveLength() / sum);
        }
    }

    double calculateEffectiveLength(Transcript& tr, const FragmentLengthDistribution& FLD)
    {
        double sum = 0.0;
        for(std::size_t i(1); i<=tr.getLength(); ++i)
            sum += FLD.getPDF(i) * (tr.getLength() - i + 1);
        return sum;
    }

    void resetTxpsTotalCount(SAMToReadPairs& rps)
    {
        for(Transcript& tr : rps.accessRefTxps())
            tr.setTotalCount(0.0);
    }

  private:
    bool convergence_{true};
};