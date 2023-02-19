#pragma once

#include <iostream>
#include <string>

using namespace std;

/*
*
* Standalone function that generates combinations from strings
*
*/
void generateCombinations(int N, string currentCombo, vector<string>& combos, vector<string>& letters) {
    // base case: we've reached the desired length of the combination
    if (currentCombo.length() == N) 
    {
        cout << currentCombo << endl;
    } 
    else 
    {
        // recursive case: generate all possible combinations by appending "A" or "B"
        for(string letter : letters)
        {
            generateCombinations(N, currentCombo + letter, combos, letters);
        }
    }
}


/*
*
* Standalone function that generates combinations from characters
*
*/
void generateCombinations(int N, string currentCombo, vector<string>& combos, vector<char>& letters) {
    // base case: we've reached the desired length of the combination
    if (currentCombo.length() == N) 
    {
        combos.push_back(currentCombo);
    } 
    else 
    {
        for(char letter : letters)
        {
            generateCombinations(N, currentCombo + letter, combos, letters);
        }
    }
}

