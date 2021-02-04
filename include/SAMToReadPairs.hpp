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
    {
        std::cerr << "Remember the sam file should be sorted by read name!!!!!!" << std::endl;
        sortTranscripts(txps); // quickly locate transcript name

        std::string prev_read_name;
        std::atomic<unsigned long long int> total_squared_length{0}, total_length{0}, total_num{0}; 
        while(!ifs.eof())
        {
            SAM sam(sam_header);
            SAM::get_obj(ifs, sam);
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
                total_squared_length.fetch_add(tlen*tlen, std::memory_order::memory_order_seq_cst);
                total_length.fetch_add(tlen, std::memory_order::memory_order_seq_cst);
                total_num.fetch_add(1, std::memory_order::memory_order_seq_cst);
            }

            prev_read_name = cur_read_name;
        }
        double mean=0.0, sdv=0.0;
        mean = total_length.load() / total_num.load(); 
        sdv = total_squared_length.load() / total_num.load() - mean*mean;
        FLD_ = FragmentLengthDistribution{mean, sdv};
    }

    void sortTranscripts(std::vector<Transcript>& txps)
    {
        if(std::is_sorted(txps.begin(), txps.end())) 
        {
            std::cerr << "Reference txps already sorted. RETURN." << std::endl;
            return;
        }
        std::cerr << "Sorting the reference txps for quickly finding aligned transcripts" << std::endl;
        std::sort(txps.begin(), txps.end());
        if(std::is_sorted(txps.begin(), txps.end())) 
            std::cerr << "Finish sorting reference txps!!" << std::endl;
    }

    const std::vector<ReadPair>& getReadPairs() const {return read_pairs_;}
  private:
    FragmentLengthDistribution FLD_;
    std::vector<ReadPair> read_pairs_;
};