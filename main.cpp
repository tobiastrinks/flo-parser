#include <iostream>
#include <fstream>
#include <map>
#include <regex>

using namespace std;


string getInputStringFrom(const string& path) {
    ifstream inFile;
    inFile.open(path);

    if (inFile.fail()) {
        cerr << "File not found" << endl;
        exit(1);
    }
    string stringLine;
    string result;

    while(getline(inFile,stringLine)){
        result = result + stringLine + "\n";
    }
    return result;
}

void writeOutputLinesToFile(const vector<string>& outputLines, const string& path) {
    ofstream outFile;
    outFile.open(path);
    for (const auto & outputLine : outputLines) {
        outFile << outputLine + ";\n";
    }
}

vector<string> extractDataLinesFrom(const string& input) {

    vector<string> commands;
    string separator = ";\n";
    unsigned long offset = 0;

    for (
        int separatorPos = input.find(separator, offset);
        separatorPos != -1;
        separatorPos = input.find(separator, offset)
    ) {
        commands.push_back(input.substr(offset, separatorPos - offset - 1));
        offset = separatorPos + separator.length();
    }

    vector<string> dataLines;

    for (const auto & command : commands){
        if (command.find('#') == 0) {
            dataLines.push_back(command);
        }
    }

    return dataLines;
}

map<string, string> generateDataMapBy(const vector<string>& dataLines) {
    map<string, string> dataMap;
    string separator = " =";
    for (auto & dataLine : dataLines){
        int sepInd = dataLine.find(separator);
        dataMap.insert(
            pair<string,string>(
                dataLine.substr(0, sepInd),
                dataLine.substr(sepInd + separator.length())
            )
        );
    }
    return dataMap;
}

vector<string> getSortedKeysOf(const map<string, string>& dataMap) {
    vector<int> hashNumbers;
    hashNumbers.reserve(dataMap.size());
    for (auto & it : dataMap) {
        hashNumbers.push_back(stoi(it.first.substr(1)));
    }

    sort(hashNumbers.begin(), hashNumbers.end());

    vector<string> sortedHashKeys;
    sortedHashKeys.reserve(hashNumbers.size());
    for (auto & number : hashNumbers) {
        sortedHashKeys.push_back("#" + to_string(number));
    }
    return sortedHashKeys;
}

vector<string> findHashtags(const string& searchText){
    smatch match;
    regex exp(R"regex(\B\#\w+\b)regex");
    smatch res;
    vector<string> result;

    string::const_iterator searchStart( searchText.cbegin() );
    while ( regex_search( searchStart, searchText.cend(), match, exp ) ) {
        result.push_back(match[0]);
        searchStart = match.suffix().first;
    }
    return result;
}

string resolveLine(string hash, map<string, string>& dataMap) {
    vector<string> foundHashTags = findHashtags(dataMap[hash]);
    for (const auto& replaceHash : foundHashTags) {
        int startPos = dataMap[hash].find(replaceHash);
        dataMap[hash] = dataMap[hash].replace(startPos, replaceHash.length(), resolveLine(replaceHash, dataMap));
    }
    return dataMap[hash];
}

int main() {
    string inputPath = "../input";
    string outputPath = "../output";

    string inputString = getInputStringFrom(inputPath);
    vector<string> filteredDataLines = extractDataLinesFrom(inputString);
    map<string, string> dataMap = generateDataMapBy(filteredDataLines);

    vector<string> outputLines;
    outputLines.reserve(dataMap.size());

    vector<string> sortedKeys = getSortedKeysOf(dataMap);

    for (auto & hashKey : sortedKeys) {
        cout << hashKey << endl;
        outputLines.push_back(
            hashKey + " =" + resolveLine(hashKey, dataMap)
        );
    }
    writeOutputLinesToFile(outputLines, outputPath);
}
