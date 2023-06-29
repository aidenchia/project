#include <iostream>

#include "CsvFeeder.h"
#include "Msg.h"
#include "VolSurfBuilder.h"
#include "CubicSmile.h"

//create header in output file
void init_output_file(std::ofstream& file){
    std::vector<std::string> col_vec{"TIME","EXPIRY","FUT_PRICE","ATM","BF25","RR25","BF10","RR10", "ERROR",
                                        "BEFORE_ATM","BEFORE_BF25","BEFORE_RR25","BEFORE_BF10","BEFORE_RR10", "BEFORE_ERROR"};
        // Send column names to the stream
    for(auto j = 0U; j < col_vec.size(); ++j)
    {
        file << col_vec.at(j);
        if(j != col_vec.size() - 1) file << ","; // No comma at end of line
    }
    file << "\n";
}


//ouput each smile at interval to file
void output_file(std::ofstream& file, std::map<datetime_t, std::pair<CubicSmile, double> > &sonrisas, std::string tiempo_ahora){
    for (auto iter = sonrisas.begin(); iter != sonrisas.end(); iter++) {
        //auto sonrisa_obj = iter->second.first;

        //std::cout <<iter->first  <<  iter->second.second << std::endl;
        vector< pair<double, double> > strikemarks = iter->second.first.GetStrikeMarks();

        double v_qd90 = strikemarks[0].second;
        double v_qd75 = strikemarks[1].second;
        double atmvol = strikemarks[2].second;
        double v_qd25 = strikemarks[3].second;
        double v_qd10 = strikemarks[4].second;

        double bf25 = ((v_qd75 + v_qd25)/2.0) - atmvol;
        double bf10 = ((v_qd90 + v_qd10)/2.0) - atmvol;
        double rr25 = v_qd25 - v_qd75;
        double rr10 = v_qd10 - v_qd90;

        file << tiempo_ahora;
        file << ",";
        file << iter->first.year;
        file << std::setw(2) << std::setfill('0') << iter->first.month;
        file << std::setw(2) << std::setfill('0') << iter->first.day;
        file << ",";
        file << iter->second.first.precio_futuro;
        file << ",";

        file << atmvol;
        file << ",";

        file << bf25;
        file << ",";

        file << rr25;
        file << ",";

        file << bf10;
        file << ",";

        file << rr10;
        file << ",";

        file << iter->second.second;
        file << ",";

        file << iter->second.first.primer_guess[0];
        file << ",";

        file << iter->second.first.primer_guess[1];
        file << ",";

        file << iter->second.first.primer_guess[2];
        file << ",";

        file << iter->second.first.primer_guess[3];
        file << ",";

        file << iter->second.first.primer_guess[4];
        file << ",";

        file << iter->second.first.primer_error;
        file << "\n";

    }
}

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

    std::ofstream outputFile(output_filename);
    init_output_file(outputFile);
    

    VolSurfBuilder<CubicSmile> volBuilder;
    auto feeder_listener = [&volBuilder](const Msg &msg)
    {
        if (msg.isSet)
        {
            volBuilder.Process(msg);
        }
    };

    auto timer_listener = [&](uint64_t now_ms)
    {
        std::string now_time_str = convert_msec_to_string(now_ms);
        // fit smile
        auto smiles = volBuilder.FitSmiles();
        // TODO: stream the smiles and their fitting error to outputFile.csv
        output_file(outputFile, smiles, now_time_str);
            
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