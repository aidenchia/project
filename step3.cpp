#include <iostream>

#include "CsvFeeder.h"
#include "Msg.h"
#include "VolSurfBuilder.h"
#include "CubicSmile.h"

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cerr << "Usage: "
                  << argv[0] << " tick_data.csv"
                  << " outputFile.csv" << std::endl;
        return 1;
    }
    const char *ticker_filename = argv[1];
    const char *output_filename = argv[2];
    

    VolSurfBuilder<CubicSmile> volBuilder;
    auto feeder_listener = [&volBuilder](const Msg &msg)
    {
        if (msg.isSet)
        {
            volBuilder.Process(msg);
        }
    };

    auto timer_listener = [&volBuilder, &output_filename](uint64_t now_ms)
    {
        // fit smile
        auto smiles = volBuilder.FitSmiles();
        // TODO: stream the smiles and their fitting error to outputFile.csv
        std::ofstream outputFile(output_filename);
        if (outputFile.is_open()) {
            outputFile << "Date,Fitting Error" << std::endl;
            
            for (const auto& e: smiles) {
                outputFile << e.first << "," << e.second.second << std::endl;
            }

            outputFile.close();
        }

        else {
            std::cout << "Error opening the file" << std::endl;
        }
            
    };

    const auto interval = std::chrono::minutes(1); // we call timer_listener at 1 minute interval
    CsvFeeder csv_feeder(ticker_filename,
                         feeder_listener,
                         interval,
                         timer_listener);

    while (csv_feeder.Step())
    {
    }
    return 0;
}