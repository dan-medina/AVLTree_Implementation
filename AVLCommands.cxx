#include <iostream>
#include <fstream>
#include <string>

#include "AVL.h"
#include "json.hpp"

using namespace std;

int main (int argc, char**argv){
    ifstream file;
    file.open(argv[1]);

    nlohmann::json jsonObject;
    if (file.is_open()) {
        file >> jsonObject;
    }

    AVL avl;

    for (auto itr = jsonObject.begin(); itr != jsonObject.end(); ++itr) {
        if (itr.key().compare("metadata") == 0) {
            break;
        }

        avl.Insert(jsonObject[itr.key()]["key"]);

    }

    nlohmann::json jsonOutput = avl.JSON();
    cout << jsonOutput.dump(2) << endl;
};

