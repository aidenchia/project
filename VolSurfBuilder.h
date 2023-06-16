#ifndef QF633_CODE_VOLSURFBUILDER_H
#define QF633_CODE_VOLSURFBUILDER_H

#include <map>
#include "Msg.h"
#include "Date.h"
#include <sstream>
#include <iomanip>

template <class Smile>
class VolSurfBuilder
{
public:
    void Process(const Msg &msg); // process message
    void PrintInfo();
    std::map<datetime_t, std::pair<Smile, double>> FitSmiles();

protected:
    // we want to keep the best level information for all instruments
    // here we use a map from contract name to BestLevelInfo, the key is contract name
    std::map<std::string, TickData> currentSurfaceRaw;
};

template <class Smile>
void VolSurfBuilder<Smile>::Process(const Msg &msg)
{
    std::cout << "[VolSurfBuilder<Smile>::Process(const Msg &msg)] start..." << std::endl;

    // TODO (Step 2)
    if (msg.isSnap)
    {
        currentSurfaceRaw.clear();
        // discard currently maintained market snapshot, and construct a new copy based on the input Msg
        for (const TickData &update : msg.Updates)
        {
            currentSurfaceRaw.emplace(update.ContractName, update);
        }
    }
    else
    {
        // update the currently maintained market snapshot
        for (const TickData &update : msg.Updates)
        {
            currentSurfaceRaw.insert_or_assign(update.ContractName, update);
        }
    }
}

datetime_t GetExpiry(const std::string &underlyingIndex)
{
    std::vector<std::string> parts;
    size_t pos = underlyingIndex.find("-");

    if ((pos != std::string::npos))
    {
        std::string str_datetime = underlyingIndex.substr(pos + 1);
        std::istringstream iss(str_datetime);
        std::tm time = {};
        iss >> std::get_time(&time, "%d%b%y");

        datetime_t datetime;
        datetime.year = time.tm_year + 1900; // tm_year is years since 1900
        datetime.month = time.tm_mon + 1;    // tm_mon is 0-based, so add 1
        datetime.day = time.tm_mday;

        return datetime;
    }

    else
    {
        throw std::runtime_error("Underlying index does not contain hyphen");
    }
}

template <class Smile>
void VolSurfBuilder<Smile>::PrintInfo()
{
    // TODO (Step 2): you may print out information about VolSurfBuilder's currentSnapshot to test
    for (auto it = currentSurfaceRaw.begin(); it != currentSurfaceRaw.end(); ++it)
    {
        std::cout << "|" << it->second.LastUpdateTimeStamp << "|"
                  << it->first << "|"
                  << " MarkIV : " << it->second.MarkIV << std::endl;
    }
}

template <class Smile>
std::map<datetime_t, std::pair<Smile, double>> VolSurfBuilder<Smile>::FitSmiles()
{

    std::map<datetime_t, std::vector<TickData>> tickersByExpiry{};

    // TODO (Step 3): group the tickers in the current market snapshot by expiry date, and construct tickersByExpiry
    for (auto it = currentSurfaceRaw.begin(); it != currentSurfaceRaw.end(); ++it)
    {
        TickData td = it->second;
        datetime_t expiryDate = GetExpiry(it->second.UnderlyingIndex);

        if (tickersByExpiry.count(expiryDate) == 0)
        {
            std::vector<TickData> new_vector{td};
            tickersByExpiry.insert(std::pair<datetime_t, std::vector<TickData>>(expiryDate, new_vector));
        }
        else
        {
            tickersByExpiry[expiryDate].emplace_back(td);
        }
    }

    std::map<datetime_t, std::pair<Smile, double>> res{};
    // then create Smile instance for each expiry by calling FitSmile() of the Smile
    for (auto iter = tickersByExpiry.begin(); iter != tickersByExpiry.end(); iter++)
    {
        std::cout << iter->first << std::endl;
        for (auto i : iter->second)
        {
            std::cout << i.ContractName << std::endl;
        }
        // auto sm = Smile::FitSmile(iter->second);  
        // TODO: you need to implement FitSmile function in CubicSmile
        // double fittingError = 0;
        // TODO (Step 3): we need to measure the fitting error here
        // res.insert(std::pair<datetime_t, std::pair<Smile, double> >(iter->first,std::pair<Smile, double>(sm, fittingError)));
    }
    return res;
}

#endif // QF633_CODE_VOLSURFBUILDER_H
