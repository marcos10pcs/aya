

#pragma once

#include "util/ScopedSingleton.hpp"

namespace Aya
{

class WordList
{
private:
    std::set<std::string> blacklist;
    void decrypt(std::string& str);

public:
    WordList();
    ~WordList();

    bool ContainsProfanity(std::string str);
};


class ProfanityFilter : public ScopedSingleton<ProfanityFilter>
{
private:
    WordList* wordlist;
    bool ContainsProfanityWorker(std::string str);

public:
    ProfanityFilter();
    ~ProfanityFilter();

    static bool ContainsProfanity(const std::string& str);
};

} // namespace Aya