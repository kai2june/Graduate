#pragma once
#include <iostream>
#include <algorithm>
#include "SAMHeader.hpp"
#include "SAM.hpp"
#include "Transcript.hpp"
#include "ReadPair.hpp"

class SAMToReadPairs
{
  public:
    SAMToReadPairs(Header& sam_header, std::ifstream& ifs, std::vector<Transcript>& txps)
    {
        std::cerr << "Remember the sam file should be sorted by read name!!!!!!" << std::endl;
        sortTranscripts(txps); // quickly locate transcript name

        std::string prev_read_name;
        while(!ifs.eof())
        {
            SAM sam(sam_header);
            SAM::get_obj(ifs, sam);
            std::string cur_read_name(sam.template get_member<MEMBER_INDEX::QNAME>());
            if(cur_read_name == prev_read_name) // same read
            {
                
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
                    txps, std::make_pair(cur_read_name, sam.template get_member<MEMBER_INDEX::SEQ>())
                );
            }
            read_pairs_.back().appendAlignedTxps(sam.template get_member<MEMBER_INDEX::RNAME>());
            prev_read_name = cur_read_name;
        }
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

    const std::vector<ReadPair>& getReadPairs() {return read_pairs_;}
  private:
    std::vector<ReadPair> read_pairs_;
};