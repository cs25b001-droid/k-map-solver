#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int grid[4][4];
int rows = 0;
int cols = 0;
int nVars = 0;
int rowBits = 0;
int colBits = 0;

int mintermsTarget = 0;

int piValMask[64];
int piDcMask[64];
int piCoveredBits[64];
string piExpr[64];
int piCount = 0;

int bestSolutionSize = 999;
int selectedPIs[64];
int currentSelected[64];

const int grayRow[4] = {0, 1, 3, 2};
const int grayCol[4] = {0, 1, 3, 2};

bool loadGrid(const string& filename) {
    ifstream fin(filename);
    if (!fin.is_open()) return false;

    rows = 0;
    cols = 0;
    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        int c = 0;
        for (char ch : line) {
            if (ch == '0' || ch == '1') {
                grid[rows][c++] = ch - '0';
            }
        }
        if (c > 0) {
            cols = c;
            rows++;
        }
    }
    fin.close();

    rowBits = (rows == 4) ? 2 : (rows == 2 ? 1 : 0);
    colBits = (cols == 4) ? 2 : (cols == 2 ? 1 : 0);
    nVars = rowBits + colBits;

    mintermsTarget = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (grid[r][c] == 1) {
                int m = (grayRow[r] << colBits) | grayCol[c];
                mintermsTarget |= (1 << m);
            }
        }
    }
    return true;
}


string buildLiteral(int valMask, int dcMask) {
    string out = "";
    for (int i = 0; i < nVars; i++) {
        int bitPos = nVars - 1 - i;
        if (!((dcMask >> bitPos) & 1)) {
            char var = 'a' + i;
            out += var;
            if (!((valMask >> bitPos) & 1)) {
                out += '\'';
            }
        }
    }
    return out.empty() ? "1" : out;
}


void extractPrimeImplicants() {
    int totalCells = (1 << nVars);
    
   
    int candVal[256], candDc[256], candCover[256];
    string candExpr[256];
    int candCount = 0;

    for (int dc = 0; dc < totalCells; dc++) {
        for (int val = 0; val < totalCells; val++) {
            if ((val & dc) != 0) continue;

            bool isAllOnes = true;
            int currentCover = 0;

            for (int m = 0; m < totalCells; m++) {
                if ((m & ~dc) == val) {
                    if (!((mintermsTarget >> m) & 1)) {
                        isAllOnes = false;
                        break;
                    }
                    currentCover |= (1 << m);
                }
            }

            if (isAllOnes && currentCover != 0) {
                candVal[candCount] = val;
                candDc[candCount] = dc;
                candCover[candCount] = currentCover;
                candExpr[candCount] = buildLiteral(val, dc);
                candCount++;
            }
        }
    }

    for (int i = 0; i < candCount; i++) {
        bool isSubsumed = false;
        for (int j = 0; j < candCount; j++) {
            if (i == j) continue;
            if ((candCover[i] & candCover[j]) == candCover[i] && candCover[i] != candCover[j]) {
                isSubsumed = true;
                break;
            }
        }

        if (!isSubsumed) {
            bool alreadyIn = false;
            for (int k = 0; k < piCount; k++) {
                if (piCoveredBits[k] == candCover[i]) {
                    alreadyIn = true;
                    break;
                }
            }
            if (!alreadyIn) {
                piValMask[piCount] = candVal[i];
                piDcMask[piCount] = candDc[i];
                piCoveredBits[piCount] = candCover[i];
                piExpr[piCount] = candExpr[i];
                piCount++;
            }
        }
    }
}

void solveMinCover(int index, int currentCover, int selectedCount) {
    if (selectedCount >= bestSolutionSize) return;

    if (currentCover == mintermsTarget) {
        bestSolutionSize = selectedCount;
        for (int i = 0; i < selectedCount; i++) {
            selectedPIs[i] = currentSelected[i];
        }
        return;
    }

    if (index >= piCount) return;

    
    currentSelected[selectedCount] = index;
    solveMinCover(index + 1, currentCover | piCoveredBits[index], selectedCount + 1);

    
    solveMinCover(index + 1, currentCover, selectedCount);
}

int main(int argc, char* argv[]) {
    string path = (argc > 1) ? argv[1] : "kmap.txt";

    if (!loadGrid(path)) {
        cout << "Could not open file." << endl;
        return 1;
    }

    if (mintermsTarget == 0) {
        cout << "0\n";
        return 0;
    }

    int totalStates = (1 << nVars);
    if (mintermsTarget == ((1 << totalStates) - 1)) {
        cout << "1\n";
        return 0;
    }

    extractPrimeImplicants();
    solveMinCover(0, 0, 0);

   
    for (int i = 0; i < bestSolutionSize; i++) {
        cout << piExpr[selectedPIs[i]];
        if (i + 1 < bestSolutionSize) cout << " + ";
    }
    cout << "\n";

    return 0;
}