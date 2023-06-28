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
    std::pair<double, double> GetMaxValues(const std::vector<TickData> &volTickerSnap);
    double CalculateWeight(const TickData &TickData);
    double CalculateFittingError(const std::vector<TickData> &volTickerSnap, const Smile &sm);

protected:
    // we want to keep the best level information for all instruments
    // here we use a map from contract name to BestLevelInfo, the key is contract name
    std::map<std::string, TickData> currentSurfaceRaw;
    double maxOpenInterest, maxSpread;
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
std::pair<double, double> VolSurfBuilder<Smile>::GetMaxValues(const std::vector<TickData> &volTickerSnap)
{
    double maxOpenInterest = 0.0;
    double maxSpread = 0.0;

    for (const TickData &tickData : volTickerSnap)
    {
        // Update the maximum open interest if necessary
        if (tickData.OpenInterest > maxOpenInterest)
            maxOpenInterest = tickData.OpenInterest;

        // Calculate the spread
        double spread = tickData.BestAskPrice - tickData.BestBidPrice;

        // Update the maximum spread if necessary
        if (spread > maxSpread)
            maxSpread = spread;
    }

    return std::make_pair(maxOpenInterest, maxSpread);
}

template <class Smile>
double VolSurfBuilder<Smile>::CalculateWeight(const TickData &tickdata)
{
    double normOpenInterest = tickdata.OpenInterest / maxOpenInterest;
    std::cout << "normOpenInterest = " << normOpenInterest << std::endl;
    double normSpread = (tickdata.BestAskPrice - tickdata.BestBidPrice) / maxSpread;
    std::cout << "normSpread = " << normSpread << std::endl;
    return (normOpenInterest + normSpread) / 2.0;
}

template <class Smile>
double VolSurfBuilder<Smile>::CalculateFittingError(const std::vector<TickData> &volTickerSnap, const Smile &sm)
{
    double fittingError = 0.0;
    double sumWeights = 0.0;

    for (const TickData &tickData : volTickerSnap)
    {
        // Calculate weight based on liquidity, open interest, bid-ask spread
        double weight = CalculateWeight(tickData);
        std::cout << "TickData contract = " << tickData.ContractName << std::endl;
        std::cout << "weight = " << weight << std::endl;
        // Calculate average implied volatility
        double sigma_i = (tickData.BestBidIV + tickData.BestAskIV) / 2.0;

        // Calculate model implied volatility
        // TODO: Check why is this nan 
        double sigma_ki = sm.Vol(tickData.Strike);

        // Calculate difference and multiply by weight
        double diff = sigma_i - sigma_ki;
        std::cout << "sigma difference = " << diff << std::endl;
        double weightedDiff = diff * weight;
        std::cout << "weightedDiff = " << weightedDiff << std::endl;

        // Update fitting error and sum of weights
        fittingError += weightedDiff;
        sumWeights += weight;
        std::cout << "fittingError = " << fittingError << std::endl;
        std::cout << "sumWeights = " << sumWeights << std::endl;

    }

    // Divide the sum of weighted differences by the sum of weights to get the fitting error
    if (sumWeights != 0.0)
    {
        fittingError /= sumWeights;
    }

    return fittingError;
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

        std::tie(maxOpenInterest, maxSpread) = GetMaxValues(iter->second);
        double fittingError = CalculateFittingError(iter->second, sm);

        // TODO (Step 3): we need to measure the fitting error here
        res.insert(std::pair<datetime_t, std::pair<Smile, double> >(iter->first,std::pair<Smile, double>(sm, fittingError)));
    }
    return res;
}

#endif // QF633_CODE_VOLSURFBUILDER_H
