#ifndef QF633_CODE_VOLSURFBUILDER_H
#define QF633_CODE_VOLSURFBUILDER_H

#include <map>
#include "Msg.h"
#include "Date.h"
#include <sstream>
#include <iomanip>
#include "CubicSmile.h"

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

        datetime_t expiryDate = td.ExpiryDate;

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
        // for (auto i : iter->second)
        // {
        //     std::cout << i.ContractName << std::endl;
        // }
        auto sm = Smile::FitSmile(iter->second);
        // TODO: you need to implement FitSmile function in CubicSmile
        // TODO (Step 3): we need to measure the fitting error here

        // CalculateFittingError is only called when Smile is CubicSmile.
        if constexpr (std::is_same<Smile, CubicSmile>::value) {
            double fittingError = sm.CalculateFittingError(iter->second, sm);
            res.insert(std::pair<datetime_t, std::pair<Smile, double>>(iter->first, std::pair<Smile, double>(sm, fittingError)));
        }
    }
    return res;
}

//function to convert epoch time in millisec to string eg 2022-07-02T01:38:07.232Z
std::string convert_msec_to_string(uint64_t m){
    using Clock = std::chrono::system_clock;
    using Precision = std::chrono::milliseconds;
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>
        tp{std::chrono::milliseconds{m}};
    std::stringstream sss;
    std::string time_in_str;

    // extract std::time_t from time_point
    std::time_t t = Clock::to_time_t(tp);

    // output the part supported by std::tm
    sss << std::put_time(std::localtime(&t), "%FT%T.");

    // get duration since epoch
    auto dur = tp.time_since_epoch();

    // extract the sub second part from the duration since epoch
    auto ss =
        std::chrono::duration_cast<Precision>(dur) % std::chrono::seconds{1};

    // output the millisecond part
    sss<< std::setfill('0') << std::setw(3) << ss.count();
    sss<< "Z";

    sss>>time_in_str;

    return time_in_str;
}

#endif // QF633_CODE_VOLSURFBUILDER_H
