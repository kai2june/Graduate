#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <locale>
#include <stdexcept>
#include <sstream>
#include <atomic>

class Transcript
{
  public:
    Transcript(std::string str) : name_(str) {}

    Transcript() = delete;
    Transcript(Transcript&& txp)
    {
        name_ = std::move(txp.name_);
        seq_ = std::move(txp.seq_);
        length_ = std::move(txp.length_);
        uniqueCount_.store(txp.uniqueCount_);
        totalCount_.store(txp.totalCount_);
    }
    Transcript& operator=(Transcript&& txp)
    {
        name_ = std::move(txp.name_);
        seq_ = std::move(txp.seq_);
        length_ = std::move(txp.length_);
        uniqueCount_.store(txp.uniqueCount_);
        totalCount_.store(txp.totalCount_);
    }
    Transcript(const Transcript& txp) = delete;
    Transcript& operator=(const Transcript& txp) = delete;

    Transcript(std::ifstream& ifs)
    {
        if(ifs.eof())
            return;
        parseFastaHeader(ifs);
        parseFastaSeq(ifs);
    }

    void parseFastaHeader(std::ifstream& ifs)
    {
        std::string str;
        std::getline(ifs, str);
        if(isValidName(str))
        {
            std::istringstream iss(std::move(str));
            iss >> name_;
            name_.erase(name_.begin()); 
        }
        else
            throw std::runtime_error("Transcript name should start with '>'");
    }

    void parseFastaSeq(std::ifstream& ifs)
    {
        std::string str;
        while(ifs.peek()!='>' && !ifs.eof())
        {
            std::getline(ifs, str);
            makeUpperCase(str);
            if(isValidSeq(str))
                seq_.append(str);
            else 
                throw std::runtime_error("Transcript sequence contain invalid character.");
        }
        length_ = seq_.size();
    }

    bool isValidName(const std::string& str) const
    {
        if(str[0] != '>')
            return false;
        return true;
    }

    bool isValidSeq(const std::string& str) const
    {
        const std::string valid_char("ACGTN"); 
        for(const auto& ch : str)
        {
            if (std::none_of(
                    valid_char.begin()
                  , valid_char.end()
                  , [&ch](const char& va){return va==ch;}
                )
            )
                return false;
        }

        return true;
    }

    void makeUpperCase(std::string& str)
    {
        std::locale loc;
        for(size_t i(0); i<str.size(); ++i)
            std::toupper(str[i], loc);
    }

    bool operator<(const Transcript& rhs) const { return name_ < rhs.name_; }
    bool operator==(const Transcript& rhs) const { return name_ == rhs.name_; }       

    const std::string& getName() const { return name_; }
    const std::string& getSeq() const { return seq_; }
    const uint32_t& getLength() const { return length_; }
    
friend std::ostream& operator<<(std::ostream& os, const Transcript& txp)
{
    os << txp.getName() << "\n" << txp.getSeq() << "\n" << txp.getLength() << std::endl;
}

  private:
    std::string name_;
    std::string seq_;
    uint32_t length_;
    std::atomic<size_t> uniqueCount_;
    std::atomic<size_t> totalCount_;
};