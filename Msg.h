#ifndef QF633_CODE_MSG_H
#define QF633_CODE_MSG_H

#include <cstdint>
#include <string>
#include <vector>
#include <iostream>

struct TickData
{
    std::string ContractName;
    double BestBidPrice;
    double BestBidAmount;
    double BestBidIV;
    double BestAskPrice;
    double BestAskAmount;
    double BestAskIV;
    double MarkPrice;
    double MarkIV;
    std::string UnderlyingIndex;
    double UnderlyingPrice;
    double LastPrice;
    double OpenInterest;
    uint64_t LastUpdateTimeStamp;

    // defining constructor for to reduce the copy and paste of objects when pushing into array
    TickData(
        std::string ContractName,
        double BestBidPrice,
        double BestBidAmount,
        double BestBidIV,
        double BestAskPrice,
        double BestAskAmount,
        double BestAskIV,
        double MarkPrice,
        double MarkIV,
        std::string UnderlyingIndex,
        double UnderlyingPrice,
        double LastPrice,
        double OpenInterest,
        uint64_t LastUpdateTimeStamp) : ContractName(ContractName),
                                        BestBidPrice(BestBidPrice),
                                        BestBidAmount(BestBidAmount),
                                        BestBidIV(BestBidIV),
                                        BestAskPrice(BestAskPrice),
                                        BestAskAmount(BestAskAmount),
                                        BestAskIV(BestAskIV),
                                        MarkPrice(MarkPrice),
                                        MarkIV(MarkIV),
                                        UnderlyingIndex(UnderlyingIndex),
                                        UnderlyingPrice(UnderlyingPrice),
                                        LastPrice(LastPrice),
                                        OpenInterest(OpenInterest),
                                        LastUpdateTimeStamp(LastUpdateTimeStamp)
    {
        // logging it out just to ensure the data is being added correctly
        LogTickData();
    }

    void LogTickData() const{
        // logging it out just to ensure the data is being added correctly
        std::cout << "-------------" << std::endl;
        std::cout << "Tick Data:" << std::endl;
        std::cout << "-------------" << std::endl;
        std::cout << "ContractName : " << ContractName<< std::endl;
        std::cout << "BestBidPrice : " << BestBidPrice<< std::endl;
        std::cout << "BestBidAmount : " << BestBidAmount<< std::endl;
        std::cout << "BestBidIV : " << BestBidIV<< std::endl;
        std::cout << "BestAskPrice : " << BestAskPrice<< std::endl;
        std::cout << "BestAskAmount : " << BestAskAmount<< std::endl;
        std::cout << "BestAskIV : " << BestAskIV<< std::endl;
        std::cout << "MarkPrice : " << MarkPrice<< std::endl;
        std::cout << "MarkIV : " << MarkIV<< std::endl;
        std::cout << "UnderlyingIndex : " << UnderlyingIndex<< std::endl;
        std::cout << "UnderlyingPrice : " << UnderlyingPrice<< std::endl;
        std::cout << "LastPrice : " << LastPrice<< std::endl;
        std::cout << "OpenInterest : " << OpenInterest<< std::endl;
        std::cout << "LastUpdateTimeStamp : " << LastUpdateTimeStamp<< std::endl;
    }
};

struct Msg
{
    uint64_t timestamp{};
    bool isSnap;
    bool isSet = false;
    std::vector<TickData> Updates;

    // Log function to print Msg object
    void LogMsg() const
    {
        // Iterate over the members of the Msg struct
        std::cout << "-------------" << std::endl;
        std::cout << "Msg object:" << std::endl;
        std::cout << "-------------" << std::endl;
        std::cout << "timestamp: " << timestamp << std::endl;
        std::cout << "isSnap: " << isSnap << std::endl;
        std::cout << "isSet: " << isSet << std::endl;
        std::cout << "Updates size: " << Updates.size() << std::endl;
        std::cout << "-------------" << std::endl;
    }
};

#endif // QF633_CODE_MSG_H
