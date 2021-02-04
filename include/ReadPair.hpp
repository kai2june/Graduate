#pragma once
#include <string>
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <utility>
#include "SAMHeader.hpp"
#include "SAM.hpp"
#include "Transcript.hpp"

class ReadPair
{
  public:
    ReadPair(std::vector<Transcript>& txps
           , std::pair<std::string, std::string> read_one
           , std::string txp_name
           , std::pair<uint32_t, uint32_t> pos_tlen
    )
        : ref_txps_(txps), read_one_(read_one)
    {
        addAlignedTxps(txp_name);
        addPosTlen(txp_name, pos_tlen);
    }

    struct TranscriptPtrComp
    {
        bool operator()(const Transcript* lhs, const Transcript* rhs) const
        {
            return lhs->getName() < rhs->getName();
        }
    };

    void setReadTwo(std::pair<std::string, std::string> read_two)
    {
        read_two_ = read_two;
    }
    
    void addAlignedTxps(std::string name)
    {
        if(!std::is_sorted(ref_txps_.begin(), ref_txps_.end()))
            throw std::runtime_error("Transcripts should be sorted at SAMToReadPairs.hpp!!!");
        auto lower = std::lower_bound(ref_txps_.begin(), ref_txps_.end(), Transcript{name});
        // std::cerr << "distance:" << lower - ref_txps_.begin() << std::endl;
        aligned_txps_.emplace(&(*lower));
    }

    void addPosTlen(std::string name, std::pair<uint32_t, uint32_t> pos_tlen)
    {
        pos_tlen_[name] = pos_tlen;
    }    

friend std::ostream& operator<<(std::ostream& os, const ReadPair& rp)
{
    os << "ref_txps_:\n";
    for(const auto& i : rp.ref_txps_) os << i;
    os << "aligned_txps_:\n";
    for(const auto& i : rp.aligned_txps_) os << *i;
    os << "read_one_:\nname_:" << rp.read_one_.first << "\nseq:" << rp.read_one_.second << std::endl;
    os << "read_two_:\nname_:" << rp.read_two_.first << "\nseq:" << rp.read_two_.second << std::endl;
    for(const auto& i : rp.pos_tlen_) 
        os << "transcript_name:" << i.first << "\npos:" << i.second.first << "\nlen:" << i.second.second << std::endl;

    return os;
}
    bool operator==(const ReadPair& rhs) const
    {
        return ref_txps_ == rhs.ref_txps_ &&
               aligned_txps_ == rhs.aligned_txps_ &&
               read_one_ == rhs.read_one_ &&
               read_two_ == rhs.read_two_ &&
               pos_tlen_ == rhs.pos_tlen_;
    }

    const std::vector<Transcript>& getRefTxps() const {return ref_txps_;}
    const std::set<Transcript*, TranscriptPtrComp>& getAlignedTxps() const {return aligned_txps_;}
    const std::pair<std::string, std::string>& getReadOne() const {return read_one_;}
    const std::pair<std::string, std::string>& getReadTwo() const {return read_two_;}
    const std::map<std::string, std::pair<uint32_t, uint32_t>>& getPosTlen() const {return pos_tlen_;}

  private:
    std::vector<Transcript>& ref_txps_;
    std::set<Transcript*, TranscriptPtrComp> aligned_txps_;
    std::pair<std::string, std::string> read_one_; // <read name, read seq>
    std::pair<std::string, std::string> read_two_; // <read name, read seq>
    std::map<std::string, std::pair<uint32_t, uint32_t>> pos_tlen_; // <POS, TLEN>

#ifdef DEBUG
  public:
    ReadPair(std::vector<Transcript>& txps
           , const std::set<Transcript*, ReadPair::TranscriptPtrComp>& aligned_txps
           , std::pair<std::string, std::string> read_one
           , std::pair<std::string, std::string> read_two
           , std::string txp_name
           , std::pair<uint32_t, uint32_t> pos_tlen
    ) : ref_txps_(txps), aligned_txps_(aligned_txps), read_one_(read_one), read_two_(read_two) 
    {
        pos_tlen_[txp_name] = pos_tlen;
    }
#endif
};