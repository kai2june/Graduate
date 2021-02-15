#pragma once
#include <vector>
#include <unordered_map>
#include <numeric>
#include <fstream>
#include <iomanip>
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
        std::size_t cnt{0};
        do
        {
            std::cerr << "round" << cnt++ << std::endl;
            Estep(rps);
            Mstep(rps);
            if( sum_.size() >= 3 and
                sum_[sum_.size()-1] == sum_[sum_.size()-2] and 
                sum_[sum_.size()-2] == sum_[sum_.size()-3]
            )
                convergence_ = true;
        } while(!convergence_);

        std::string filename("/mammoth/Graduate/unit_test/data/transcript_abundance.txt");
        std::ofstream ofs(filename, std::ios::app);
        for(const auto& txp : txps)
        {
            ofs.write(txp.getName().c_str(), txp.getName().size());
            ofs.write(" ", 1);

            ofs.write(std::to_string(txp.getAbundance()).c_str(), std::to_string(txp.getAbundance()).size());
            ofs.write("\n", 1);
        }
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
            {
                double inc = sum > 0.0 ? prob[i]/sum : 1/prob.size();
                ++i;
                tr->increaseTotalCount(inc);
            }
        }
    }

    void Mstep(SAMToReadPairs& rps)
    {
        std::cerr << "I'm in Mstep." << std::endl;
        double sum = 0.0;
        for(Transcript& tr : rps.accessRefTxps())
        {
            sum = sum + tr.getTotalCount() / tr.getEffectiveLength();
        }
        uint64_t s = sum*10000.0;
        sum_.emplace_back(static_cast<double>(s/10000.0));
        std::cerr << "sum: " << sum_.back() << std::endl;

        double threshold{1.0 / static_cast<double>(rps.accessRefTxps().size()) };
        for(Transcript& tr : rps.accessRefTxps())
        {
            double cur_abundance = tr.getTotalCount() / tr.getEffectiveLength() / sum;
            double prev_abundance = tr.getAbundance();
            // if(cur_abundance - prev_abundance > threshold)
            //     convergence_ = false;
            
            tr.setAbundance(tr.getTotalCount() / tr.getEffectiveLength() / sum);
        }
    }

    double calculateEffectiveLength(Transcript& tr, const FragmentLengthDistribution& FLD)
    {
        double sum = 0.0;
        for(std::size_t i(1); i<=tr.getLength(); ++i)
        {
            sum += FLD.getPDF(i) * (tr.getLength() - i + 1);
            // std::cerr << "sum= " << sum << std::endl;
        }
        return sum;
    }

    void resetTxpsTotalCount(SAMToReadPairs& rps)
    {
        for(Transcript& tr : rps.accessRefTxps())
            tr.setTotalCount(0.0);
    }

  private:
    std::vector<double> sum_;
    bool convergence_{false};
};