#ifndef HELPER_H
#define HELPER_H

#include <utility>
#include <vector>
#include <chrono>
#include "Msg.h"
#include "CubicSmile.h"

#include "helper.h"

inline std::pair<double, double> GetMaxValues(const std::vector<TickData> &volTickerSnap)
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

inline double CalculateWeight(const TickData &tickdata, double maxOpenInterest, double maxSpread)
{

  double normOpenInterest = tickdata.OpenInterest / maxOpenInterest;
  double normSpread = (tickdata.BestAskPrice - tickdata.BestBidPrice) / maxSpread;
  // std::cout << "normOpenInterest = " << normOpenInterest << std::endl;
  // std::cout << "normSpread = " << normSpread << std::endl;
  return (normOpenInterest + normSpread) / 2.0;
}

inline double CalculateFittingError(const std::vector<TickData> &volTickerSnap, const CubicSmile &sm)
{
  double fittingError = 0.0;
  double sumWeights = 0.0;

  // GetMaxValues(volTickerSnap); //

  // Get the maximum values for weight calculation
  std::pair<double, double> maxValues = GetMaxValues(volTickerSnap);
  double maxOpenInterest = maxValues.first;
  double maxSpread = maxValues.second;

  for (const TickData &tickData : volTickerSnap)
  {
    // Calculate weight based on liquidity, open interest, bid-ask spread
    double weight = CalculateWeight(tickData, maxOpenInterest, maxSpread);
    // std::cout << "TickData contract = " << tickData.ContractName << std::endl;
    // std::cout << "weight = " << weight << std::endl;
    // Calculate average implied volatility
    double sigma_i = (tickData.BestBidIV + tickData.BestAskIV) / 2.0 / 100;

    // Calculate model implied volatility
    // TODO: Check why is this nan
    double sigma_ki = sm.Vol(tickData.Strike) / 100;

    // Calculate difference and multiply by weight
    double diff = sigma_i - sigma_ki;
    double weightedDiff = diff * weight;
    // std::cout << "sigma difference = " << diff << std::endl;
    // std::cout << "weightedDiff = " << weightedDiff << std::endl;

    // Update fitting error and sum of weights
    fittingError += weightedDiff;
    sumWeights += weight;
  }

  // Divide the sum of weighted differences by the sum of weights to get the fitting error
  if (sumWeights != 0.0)
  {
    fittingError /= sumWeights;
  }

  // std::cout << "fittingError = " << fittingError << std::endl;
  // std::cout << "sumWeights = " << sumWeights << std::endl;
  return fittingError;
}

// function to convert epoch time in millisec to string eg 2022-07-02T01:38:07.232Z
inline std::string convert_msec_to_string(uint64_t m)
{
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
  sss << std::setfill('0') << std::setw(3) << ss.count();
  sss << "Z";

  sss >> time_in_str;

  return time_in_str;
}

#endif