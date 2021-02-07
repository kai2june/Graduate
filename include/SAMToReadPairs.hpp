#pragma once
#include <iostream>
#include <algorithm>
#include "SAMHeader.hpp"
#include "SAM.hpp"
#include "Transcript.hpp"
#include "ReadPair.hpp"
#include "FragmentLengthDistribution.hpp"

class SAMToReadPairs
{
  public:
    SAMToReadPairs(Header& sam_header, std::ifstream& ifs, std::vector<Transcript>& txps)
        : ref_txps_(txps)
    {
        std::cerr << "Remember the sam file should be sorted by read name!!!!!!" << std::endl;
        sortTranscripts(txps); // quickly locate transcript name

        std::cerr << "Reading .sam file..." << std::endl;
        std::string prev_read_name;
        while(!ifs.eof())
        {
            SAM sam(sam_header);
            SAM::get_obj(ifs, sam);
            setReadPairs(sam, txps, prev_read_name);
        }
        setFLD();
        std::cerr << "mean" << FLD_.getMean() << " sdv:" << FLD_.getSDV() << std::endl;

    }

    void setReadPairs(SAM& sam, std::vector<Transcript>& txps, std::string& prev_read_name)
    {
        std::string cur_read_name(sam.template get_member<MEMBER_INDEX::QNAME>());
        if(cur_read_name == prev_read_name) // same read
        {
            read_pairs_.back().addAlignedTxps(sam.template get_member<MEMBER_INDEX::RNAME>());
            read_pairs_.back().addPosTlen(
                sam.template get_member<MEMBER_INDEX::RNAME>()
                , std::make_pair(
                    std::min(sam.template get_member<MEMBER_INDEX::POS>(), sam.template get_member<MEMBER_INDEX::PNEXT>())
                    , static_cast<uint32_t>( std::abs(sam.template get_member<MEMBER_INDEX::TLEN>()) )
                )
            );
        }
        else if(
            cur_read_name.substr(0, cur_read_name.size()-2) == 
            prev_read_name.substr(0, prev_read_name.size()-2)
        ) // second read among same pair
        {
            read_pairs_.back().setReadTwo(
                std::make_pair(cur_read_name, sam.template get_member<MEMBER_INDEX::SEQ>())
            );
        }
        else // different read pair 
        {
            int32_t tlen = std::abs(sam.template get_member<MEMBER_INDEX::TLEN>());
            read_pairs_.emplace_back(
                txps
                , std::make_pair(cur_read_name, sam.template get_member<MEMBER_INDEX::SEQ>())
                , sam.template get_member<MEMBER_INDEX::RNAME>()
                , std::make_pair(
                    std::min(sam.template get_member<MEMBER_INDEX::POS>(), sam.template get_member<MEMBER_INDEX::PNEXT>())
                    , static_cast<uint32_t>( tlen )
                )
            );
        }

        prev_read_name = cur_read_name;
    }

    void setFLD()
    {
        unsigned long long int total_squared_length{0uLL}, total_length{0uLL}, total_num{0uLL};
        for(ReadPair rp : read_pairs_)
        {
            for(auto iter=rp.getPosTlen().begin(); iter!=rp.getPosTlen().end(); ++iter)
            {
                total_squared_length += (iter->second.second) * (iter->second.second);
                total_length += iter->second.second;
                total_num += 1;
            }
        }
        double mean=0.0, sdv=0.0;
        mean = total_length / total_num; 
        sdv = total_squared_length / total_num - mean*mean;
        FLD_ = FragmentLengthDistribution{mean, sdv};
    }

    void sortTranscripts(std::vector<Transcript>& txps)
    {
        if(std::is_sorted(txps.begin(), txps.end())) 
        {
            std::cerr << "Reference txps are originally sorted. RETURN." << std::endl;
            return;
        }
        std::cerr << "Sorting the reference txps for quickly finding aligned transcripts" << std::endl;
        std::sort(txps.begin(), txps.end());
        if(std::is_sorted(txps.begin(), txps.end())) 
            std::cerr << "Finish sorting reference txps!!" << std::endl;
    }

    const FragmentLengthDistribution& getFLD() const {return FLD_;}
    std::vector<Transcript>& accessRefTxps() const {return ref_txps_;}
    const std::vector<ReadPair>& getReadPairs() const {return read_pairs_;}
  private:
    FragmentLengthDistribution FLD_;
    std::vector<Transcript>& ref_txps_;
    std::vector<ReadPair> read_pairs_;
#ifdef DEBUG
  public:
    SAMToReadPairs(std::vector<Transcript>& txps) : ref_txps_(txps) {}
#endif
};