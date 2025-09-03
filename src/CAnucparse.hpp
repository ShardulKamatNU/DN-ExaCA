#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <stdexcept>

struct CSVDoubleData {
    std::vector<std::string> headers;
    std::vector<std::vector<double>> rows;
};

// Helper function to split a string by delimiter
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        // Trim whitespace from token
        size_t start = token.find_first_not_of(" \t\r\n");
        size_t end = token.find_last_not_of(" \t\r\n");
        
        if (start != std::string::npos) {
            token = token.substr(start, end - start + 1);
        } else {
            token = "";
        }
        
        tokens.push_back(token);
    }
    
    return tokens;
}

// Convert string to double with error handling
double stringToDouble(const std::string& str, int row, const std::string& column) {
    try {
        return std::stod(str);
    } catch (const std::invalid_argument& e) {
        throw std::runtime_error("Invalid number format in row " + std::to_string(row + 1) + 
                                ", column '" + column + "': '" + str + "'");
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("Number out of range in row " + std::to_string(row + 1) + 
                                ", column '" + column + "': '" + str + "'");
    }
}

// Main function to read CSV file with double data
CSVDoubleData readCSVDoubles(const std::string& filename) {
    CSVDoubleData data;
    std::ifstream file(filename);
    std::string line;
    
    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open file " + filename);
    }
    
    // Read header line
    if (std::getline(file, line)) {
        data.headers = split(line, ',');
        if (data.headers.empty()) {
            file.close();
            throw std::runtime_error("Error: No headers found in file");
        }
    } else {
        file.close();
        throw std::runtime_error("Error: File is empty or could not read header");
    }
    
    // Read data rows
    int rowIndex = 0;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            std::vector<std::string> stringRow = split(line, ',');
            std::vector<double> doubleRow;
            
            for (size_t i = 0; i < stringRow.size(); ++i) {
                std::string columnName = (i < data.headers.size()) ? data.headers[i] : "Column" + std::to_string(i);
                double value = stringToDouble(stringRow[i], rowIndex, columnName);
                doubleRow.push_back(value);
            }
            
            data.rows.push_back(doubleRow);
            rowIndex++;
        }
    }
    
    file.close();
    return data;
}

// Alternative function that returns data as a map for easier column access
std::vector<std::map<std::string, double>> readCSVDoublesAsMap(const std::string& filename) {
    std::vector<std::map<std::string, double>> result;
    CSVDoubleData data = readCSVDoubles(filename);
    
    for (const auto& row : data.rows) {
        std::map<std::string, double> rowMap;
        
        for (size_t i = 0; i < data.headers.size() && i < row.size(); ++i) {
            rowMap[data.headers[i]] = row[i];
        }
        
        result.push_back(rowMap);
    }
    
    return result;
}

// Function to get a specific column as a vector
std::vector<double> getColumn(const CSVDoubleData& data, const std::string& columnName) {
    std::vector<double> column;
    
    // Find column index
    auto it = std::find(data.headers.begin(), data.headers.end(), columnName);
    if (it == data.headers.end()) {
        throw std::runtime_error("Column '" + columnName + "' not found");
    }
    
    size_t columnIndex = std::distance(data.headers.begin(), it);
    
    // Extract column data
    for (const auto& row : data.rows) {
        if (columnIndex < row.size()) {
            column.push_back(row[columnIndex]);
        }
    }
    
    return column;
}

// Function to get a specific column by index
std::vector<double> getColumnByIndex(const CSVDoubleData& data, size_t columnIndex) {
    std::vector<double> column;
    
    if (columnIndex >= data.headers.size()) {
        throw std::runtime_error("Column index " + std::to_string(columnIndex) + " out of range");
    }
    
    for (const auto& row : data.rows) {
        if (columnIndex < row.size()) {
            column.push_back(row[columnIndex]);
        }
    }
    
    return column;
}

