#include <iostream>
#include "CsvFeeder.h"
#include "date/date.h"
#include <sstream>
#include <limits>

uint64_t TimeToUnixMS(std::string ts) {
    std::istringstream in{ts};
    std::chrono::system_clock::time_point tp;
    in >> date::parse("%FT%T", tp);
    const auto timestamp = std::chrono::time_point_cast<std::chrono::milliseconds>(tp).time_since_epoch().count();
    return timestamp;
}

double ConvertToDouble(const std::string& str) {
    try {
        return std::stod(str);
    }

    catch (...) {
        return std::numeric_limits<double>::quiet_NaN();
    }
}

bool ReadNextMsg(std::ifstream& file, Msg& msg, std::map<std::string,int>& column_pos) {

    if (file.eof()) {
        return false;
    }

    // TODO: your implementation to read file and create the next Msg into the variable msg

    // reading the file for the first time
    else if (msg.isSet == false) {
        std::string line;
        std::stringstream ss(line);
        std::string token;
        msg.isSet = true;
        
        // read the first line and set column positions
        std::getline(file, line);
        ss.str(line);
        int pos = 0;
        while (std::getline(ss, token, ',')) {
            column_pos[token] = pos;
            pos++;
        }

        return true;
    }
    
    std::string line;
    uint64_t current_ts = 0U;
    uint64_t last_ts = 0U;
    while (current_ts == last_ts) {
        
        // read the next line and insert into row vector
        std::getline(file, line);
        if (line.empty()) {
            return false;
        }

        std::stringstream ss2(line);
        std::vector<std::string> row_vec;
        std::string token;
        while (std::getline(ss2, token, ',')) {
            row_vec.push_back(token);
        }
        
        // check if timestamp is the same as last read
        msg.timestamp = TimeToUnixMS(row_vec[column_pos["time"]]);
        current_ts = TimeToUnixMS(row_vec[column_pos["time"]]);
        if (current_ts != last_ts && last_ts != 0U) {
            
            // hack to read the last line in the file
            if (file.tellg() == -1) {
                TickData td;
                msg.isSnap = row_vec[column_pos["msgType"]] == "snap";
                td.ContractName = row_vec[column_pos["contractName"]];
                td.BestBidPrice = ConvertToDouble(row_vec[column_pos["bestBid"]]);
                td.BestBidAmount = ConvertToDouble(row_vec[column_pos["bestBidAmount"]]);
                td.BestBidIV = ConvertToDouble(row_vec[column_pos["bestBidIV"]]);
                td.BestAskPrice = ConvertToDouble(row_vec[column_pos["bestAsk"]]);
                td.BestAskAmount = ConvertToDouble(row_vec[column_pos["bestAskAmount"]]);
                td.BestAskIV = ConvertToDouble(row_vec[column_pos["bestAskIV"]]);
                td.MarkPrice = ConvertToDouble(row_vec[column_pos["markPrice"]]);
                td.MarkIV = ConvertToDouble(row_vec[column_pos["markIV"]]);
                td.UnderlyingIndex = row_vec[column_pos["underlyingIndex"]];
                td.UnderlyingPrice = ConvertToDouble(row_vec[column_pos["underlyingPrice"]]);
                td.LastPrice = ConvertToDouble(row_vec[column_pos["lastPrice"]]);
                td.OpenInterest = ConvertToDouble(row_vec[column_pos["openInterest"]]);
                
                // add tickdata to msg
                msg.Updates.push_back(td);
                continue;
                
            }

            else {
                file.seekg(-(line.size() + 1), std::ios::cur);
                break;
            }
        }

        // update msg
        TickData td;
        msg.isSnap = row_vec[column_pos["msgType"]] == "snap";
        td.ContractName = row_vec[column_pos["contractName"]];
        td.BestBidPrice = ConvertToDouble(row_vec[column_pos["bestBid"]]);
        td.BestBidAmount = ConvertToDouble(row_vec[column_pos["bestBidAmount"]]);
        td.BestBidIV = ConvertToDouble(row_vec[column_pos["bestBidIV"]]);
        td.BestAskPrice = ConvertToDouble(row_vec[column_pos["bestAsk"]]);
        td.BestAskAmount = ConvertToDouble(row_vec[column_pos["bestAskAmount"]]);
        td.BestAskIV = ConvertToDouble(row_vec[column_pos["bestAskIV"]]);
        td.MarkPrice = ConvertToDouble(row_vec[column_pos["markPrice"]]);
        td.MarkIV = ConvertToDouble(row_vec[column_pos["markIV"]]);
        td.UnderlyingIndex = row_vec[column_pos["underlyingIndex"]];
        td.UnderlyingPrice = ConvertToDouble(row_vec[column_pos["underlyingPrice"]]);
        td.LastPrice = ConvertToDouble(row_vec[column_pos["lastPrice"]]);
        td.OpenInterest = ConvertToDouble(row_vec[column_pos["openInterest"]]);
        
        // add tickdata to msg
        msg.Updates.push_back(td);
        last_ts = TimeToUnixMS(row_vec[column_pos["time"]]);
    }


    return true;

}

CsvFeeder::CsvFeeder(const std::string ticker_filename,
                     FeedListener feed_listener,
                     std::chrono::minutes interval,
                     TimerListener timer_listener)
        : ticker_file_(ticker_filename),
          feed_listener_(feed_listener),
          interval_(interval),
          timer_listener_(timer_listener) {
    // initialize member variables with input information, prepare for Step() processing

    ReadNextMsg(ticker_file_, msg_, column_pos_);
    if (msg_.isSet) {
        // initialize interval timer now_ms_
        now_ms_ = msg_.timestamp;
    } else {
        throw std::invalid_argument("empty message at initialization");
    }
}

bool CsvFeeder::Step() {
    if (msg_.isSet) {
        // call feed_listener with the loaded Msg
        feed_listener_(msg_);

        // if current message's timestamp is crossing the given interval, call time_listener, change now_ms_ to the next interval cutoff
        if (now_ms_ < msg_.timestamp) {
            timer_listener_(now_ms_);
            now_ms_ += interval_.count();
        }
        // load tick data into Msg
        // if there is no more message from the csv file, return false, otherwise true
        return ReadNextMsg(ticker_file_, msg_, column_pos_);
    }

    return false;
}

CsvFeeder::~CsvFeeder() {
    // release resource allocated in constructor, if any
}