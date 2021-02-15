#pragma once
#include <iostream>
#include <algorithm>
#include <type_traits>
#include "SAMHeader.hpp"
#include "SAM.hpp"
#include "Transcript.hpp"
#include "ReadPair.hpp"
#include "FragmentLengthDistribution.hpp"

class SAMToReadPairs
{
  public:
    SAMToReadPairs(std::vector<SAM>& vec_sam, std::vector<Transcript>& txps)
        : ref_txps_(txps)
    {
        if( vec_sam[0].get_header().template get_member<HEADER_INDEX::ALIGNMENT_SORT_ORDER>() != SORT_TYPE::QUERYNAME)
            throw std::runtime_error("SAM vector should be sorted by QUERYNAME!!!!");
        sortTranscripts(txps); // quickly locate transcript name

        std::cerr << "Parsing .sam vector..." << std::endl;
        std::string prev_read_name;
        std::size_t i{0};
        for(SAM& sam : vec_sam)
        {
            if(i++ % 100000 == 0)
                std::cerr << i << "th record to readpair" << std::endl;
            setReadPairs(sam, txps, prev_read_name);
        }
        setFLD();
        std::cerr << "mean" << FLD_.getMean() << " sdv:" << FLD_.getSDV() << std::endl;
    }

    SAMToReadPairs(Header& sam_header, std::ifstream& ifs, std::vector<Transcript>& txps)
        : ref_txps_(txps)
    {
        if( sam_header.template get_member<HEADER_INDEX::ALIGNMENT_SORT_ORDER>() != SORT_TYPE::QUERYNAME)
            throw std::runtime_error("SAM vector should be sorted by QUERYNAME!!!!");
        sortTranscripts(txps); // quickly locate transcript name

        std::cerr << "Reading .sam file..." << std::endl;
        std::string prev_read_name;
        std::size_t i{0};
        while(!ifs.eof())
        {
            if(i++ % 10000 == 0)
                std::cerr << i << "th record" << std::endl;
            SAM sam(sam_header);
            SAM::get_obj(ifs, sam);
            setReadPairs(sam, txps, prev_read_name);
        }
        setFLD();
        std::cerr << "mean" << FLD_.getMean() << " sdv:" << FLD_.getSDV() << std::endl;

    }

    void setReadPairs(SAM& sam, std::vector<Transcript>& txps, std::string& prev_read_name)
    {
        int32_t tlen = std::abs(sam.template get_member<MEMBER_INDEX::TLEN>());
        if(tlen == 0)
        {
            // std::cerr << "tlen==0, unaligned reads found." << std::endl;
            return;
        }
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
        /// @brief tlen equals 0 should be handled
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
        sdv = std::sqrt(total_squared_length / total_num - mean*mean);
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