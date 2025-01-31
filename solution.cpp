#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <iomanip>

void buildIndex(const std::string& logFilePath, const std::string& indexFilePath) {
    std::ifstream logFile(logFilePath, std::ios::binary);
    std::ofstream indexFile(indexFilePath);

    std::string line;
    std::streampos lastPos = logFile.tellg();
    std::unordered_map<std::string, std::streampos> index;

    while (std::getline(logFile, line)) {
        std::string date = line.substr(0, 10); 
        if (index.find(date) == index.end()) {
            index[date] = lastPos;
        }
        lastPos = logFile.tellg();
    }

    for (const auto& entry : index) {
        indexFile << entry.first << " " << entry.second << std::endl;
    }
}


void extractLogs(const std::string& logFilePath, const std::string& indexFilePath, const std::string& date, const std::string& outputFilePath) {
    std::ifstream logFile(logFilePath, std::ios::binary);
    std::ifstream indexFile(indexFilePath);
    std::ofstream outputFile(outputFilePath);

    std::unordered_map<std::string, std::streampos> index;
    std::string line;
    while (std::getline(indexFile, line)) {
        std::istringstream iss(line);
        std::string idxDate;
        std::streampos pos;
        iss >> idxDate >> pos;
        index[idxDate] = pos;
    }

    if (index.find(date) == index.end()) {
        std::cerr << "No logs found for the specified date." << std::endl;
        return;
    }

    logFile.seekg(index[date]);
    while (std::getline(logFile, line)) {
        std::string currentDate = line.substr(0, 10);
        if (currentDate != date) {
            break;
        }
        outputFile << line << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <YYYY-MM-DD>" << std::endl;
        return 1;
    }

    std::string date = argv[1];
    std::string logFilePath = "test_logs.log";
    std::string indexFilePath = "index.txt";
    std::string outputFilePath = "output/output_" + date + ".txt";

    // Build the index if it doesn't exist
    std::ifstream indexFile(indexFilePath);
    if (!indexFile.good()) {
        buildIndex(logFilePath, indexFilePath);
    }

    // Extract logs for the specified date
    extractLogs(logFilePath, indexFilePath, date, outputFilePath);

    std::cout << "Logs extracted to " << outputFilePath << std::endl;

    return 0;
}
