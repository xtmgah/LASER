//
// Created by dtaliun on 2/7/20.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <utility>
#include <cmath>

using namespace std;

inline int fcmp(double x, double y, double epsilon) {
    int max_exponent = 0;
    double delta = 0.0;
    double diff = 0.0;

    frexp(fabs(x) > fabs(y) ? x : y, &max_exponent);
    delta = ldexp(epsilon, max_exponent);

    diff = x - y;

    if (diff > delta) {
        return 1;
    } else if (diff < -delta) {
        return -1;
    } else {
        return 0;
    }
}

bool get_line_tokens(ifstream& stream, vector<string>& tokens) {
    string line("");
    tokens.clear();
    if (!getline(stream, line).eof()) {
        stringstream ss(line);
        string token;
        while (getline(ss, token, '\t')) {
            tokens.push_back(move(token));
        }
        return true;
    }
    return false;
}

// column_types = string where each character represents a column type e.g. s -> string, d -> decimal, f -> float
int test_propc(const char* file_name1, const char* file_name2, const char* column_types) {
    ifstream ifile_stream1, ifile_stream2;
    vector<string> tokens1, tokens2;

    ifile_stream1.open(file_name1, ios::binary);
    if (ifile_stream1.fail()) {
        return 1;
    }

    ifile_stream2.open(file_name2, ios::binary);
    if (ifile_stream2.fail()) {
        return 1;
    }

    // compare headers
    if (!get_line_tokens(ifile_stream1, tokens1)) {
        return 1;
    }
    if (!get_line_tokens(ifile_stream2, tokens2)) {
        return 1;
    }
    if (tokens1.size() != tokens2.size()) {
        return 1;
    }
    if (tokens1.size() != strlen(column_types)) {
        return 1;
    }
    for (unsigned int i = 0u; i < tokens1.size(); ++i) {
        if (tokens1[i].compare(tokens2[i]) != 0) {
            return 1;
        }
    }

    // compare lines
    while (get_line_tokens(ifile_stream1, tokens1) && get_line_tokens(ifile_stream2, tokens2)) {
        if (tokens1.size() != tokens2.size()) {
            return 1;
        }
        if (tokens1.size() != strlen(column_types)) {
            return 1;
        }
        for (unsigned int i = 0u; i < strlen(column_types); ++i) {
            if ((column_types[i] == 's') || (column_types[i] == 'd')) {
                if (tokens1[i].compare(tokens2[i]) != 0) {
                    return 1;
                }
            } else if (column_types[i] == 'f') {
                size_t pos = 0;
                float value1 = stof(tokens1[i], &pos);
                if ((pos != tokens1[i].length()) || (isinf(value1))) {
                    return 1;
                }
                float value2 = stof(tokens2[i], &pos);
                if ((pos != tokens2[i].length()) || (isinf(value2))) {
                    return 1;
                }
                if (fcmp(value1, value2, 0.01) != 0) {
                    return 1;
                }
            } else {
                return 1;
            }
        }
    }

    while (get_line_tokens(ifile_stream1, tokens1)) {
        if (tokens1.size() > 0) {
            return 1;
        }
    }
    while (get_line_tokens(ifile_stream2, tokens2)) {
        if (tokens2.size() > 0) {
            return 1;
        }
    }

    if (!ifile_stream1.eof() || !ifile_stream2.eof()) {
        return 1;
    }

    ifile_stream1.close();
    ifile_stream2.close();

    return 0;
}


int main(int argc, char** argv) {
    if (argc < 2) {
        return 1;
    }
    if (strcmp(argv[1], "test_propc_files") == 0) {
        if (argc != 5) {
            return 1;
        }
        return test_propc(argv[2], argv[3], argv[4]);
    }
    return 1;
}