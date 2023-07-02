#ifndef HELPER_H
#define HELPER_H

#include <utility>
#include <vector> 
#include <chrono>
#include "Msg.h"
#include "CubicSmile.h"


std::pair<double, double> GetMaxValues(const std::vector<TickData> &volTickerSnap);
double CalculateWeight(const TickData &tickdata, double maxOpenInterest, double maxSpread);
double CalculateFittingError(const std::vector<TickData> &volTickerSnap, const CubicSmile &sm);

#endif