#include <iostream>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <string>
#include <istream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <unordered_set>

using namespace std;

const int MULTIPLIER = 20;
const int SEED = 0;
const int START = 0;
const int END = 3;
const double cooling_rate = 0.95;

static bool parse(string filepath, vector<vector<int>>& numbers)
{
    vector <string> lines;
    int n = 0;
    ifstream file;
    file.open(filepath);
    if (file.is_open())
    {
        string line;
        while (getline(file, line))
        {
            // cout << line << endl;
            lines.push_back(line);
            n++;
        }
    }
    else
    {
        cout << "Error opening file\n";
        return false;
    }
    string token;
    for (const auto& line : lines)
    {
        vector<int> arr;
        stringstream ss(line);
        while (ss >> token)
        {
            // cout << token << endl;
            arr.push_back(stoi(token));
        }
        numbers.push_back(arr);
    }
    file.close();
    if (numbers[0].size() != 4)
    {
        return false;
    }
    for (int i = 1; i < numbers.size(); i++)
    {
        if (numbers[i][0] != numbers[i].size() - 1)
        {
            return false;
        }
    }
    return true;
}

static vector<vector<int>> initialize(vector<vector<int>>& numbers, vector<int>& xcoords, vector<int>& ycoords)
{
    int cells = numbers[0][0];
    int ny = numbers[0][2];
    int nx = numbers[0][3];
    int y;
    int x;
    vector<vector<int>> grid(ny, vector<int>(nx, -1));

    for (int i = 0; i < cells; i++)
    {
        y = rand() % ny;
        x = rand() % nx;
        if (i == 0)
        {
            grid[y][x] = i;
            xcoords[i] = x;
            ycoords[i] = y;
            continue;
        }
        if (grid[y][x] == -1)
        {
            // cout << i << endl;
            grid[y][x] = i;
            xcoords[i] = x;
            ycoords[i] = y;
        }
        else
        {
            // cout << i << endl;
            y = rand() % ny;
            x = rand() % nx;;
            while (grid[y][x] != -1)
            {
                y = rand() % ny;
                x = rand() % nx;
            }
            grid[y][x] = i;
            xcoords[i] = x;
            ycoords[i] = y;
        }
    }
    return grid;
}

static void print(vector<vector<int>>& grid)
{
    for (int i = 0; i < grid.size(); i++)
    {
        for (int j = 0; j < grid[i].size(); j++)
        {
            if (grid[i][j] == -1)
            {
                cout << "-- ";
            }
            else
            {
                if ((grid[i][j] / 10) != 0)
                {
                    cout << grid[i][j] << " ";
                }
                else
                {
                    cout << "0" << grid[i][j] << " ";
                }
            }
        }
        cout << endl;
    }
}

static int calculate_hpwl(const vector<int>& xcoords, const vector<int>& ycoords, const vector<vector<int>>& numbers, int net)
{
    int cells = numbers[net][0];
    int xmax = xcoords[numbers[net][1]];
    int xmin = xmax;
    int ymax = ycoords[numbers[net][1]];
    int ymin = ymax;

    for (int i = 2; i <= cells; ++i)
    {
        int cell = numbers[net][i];
        int xcoord = xcoords[cell];
        int ycoord = ycoords[cell];

        if (xcoord > xmax)
        {
            xmax = xcoord;
        }
        else if (xcoord < xmin)
        {
            xmin = xcoord;
        }
        if (ycoord > ymax)
        {
            ymax = ycoord;
        }
        else if (ycoord < ymin)
        {
            ymin = ycoord;
        }
    }
    return (xmax - xmin) + (ymax - ymin);
}

static void print_bin(vector<vector<int>>& grid)
{
    for (int i = 0; i < grid.size(); i++)
    {
        for (int j = 0; j < grid[i].size(); j++)
        {
            if (grid[i][j] == -1)
            {
                cout << 0;
            }
            else
            {
                cout << 1;
            }
        }
        cout << endl;
    }

}

static void output(unordered_map<int, unordered_set<int>> cells_nets)
{
    for (const auto& pair : cells_nets)
    {
        int cell = pair.first;
        const unordered_set<int>& nets = pair.second;
        cout << "Cell " << cell << " is connected to nets: ";
        for (int net : nets)
        {
            cout << net << " ";
        }
        cout << endl;
    }
}

unordered_set<int> set_union(unordered_set<int>& set1, unordered_set<int>& set2)
{
    unordered_set<int> resultSet = set1;
    for (int elem : set2) {
        resultSet.insert(elem);
    }

    return resultSet;
}

int main()
{
    srand(SEED);
    vector<vector<int>> numbers;
    string filepath = "";
    string filename = "d";
    string file_extension = ".txt";
    for (int index = START; index <= END; index++)
    {
        filename += to_string(index);
        filepath = filename + file_extension;
        if (parse(filepath, numbers))
        {
            cout << "Netlist parsed successfully" << endl;
        }
        else
        {
            cout << "Error parsing netlist" << endl;
        }
        unordered_map<int, unordered_set<int>> cells_nets;
        for (int i = 1; i < numbers.size(); i++)
        {
            for (int j = 1; j < numbers[i].size(); j++)
            {
                cells_nets[numbers[i][j]].insert(i - 1);
            }
        }
        //output(cells_nets);

        int cells = numbers[0][0];
        int nets = numbers[0][1];
        int ny = numbers[0][2];
        int nx = numbers[0][3];

        vector<int> xcoords(cells);
        vector<int> ycoords(cells);

        // Create an initial random placement
        vector<vector<int>> grid = initialize(numbers, xcoords, ycoords);
        cout << "Initial Random Placement\n";
        print(grid);
        print_bin(grid);

        int initial_cost = 0;
        for (int i = 0; i < nets; i++)
        {
            initial_cost += calculate_hpwl(xcoords, ycoords, numbers, i + 1);
            //cout << hpwls[i] << endl;
        }
        cout << "TWL after the initial random placement: " << initial_cost << endl;
        double initial_temp = 500 * initial_cost; // Very high temp
        //cout << initial_temp << endl;
        double temp = initial_temp;
        double final_temp = 5e-6 * initial_cost / nets;
        int new_cost = 0;
        int old_cost = initial_cost;
        int moves = MULTIPLIER * cells;
        auto start = chrono::high_resolution_clock::now();

        while (temp > final_temp)
        {
            for (int i = 0; i < moves; i++)
            {
                // Pick two random cells and swap them
                int xrand1, xrand2, yrand1, yrand2;
                do
                {
                    xrand1 = rand() % nx;
                    yrand1 = rand() % ny;
                    xrand2 = rand() % nx;
                    yrand2 = rand() % ny;
                } while (((xrand1 == xrand2 && yrand1 == yrand2) || ((grid[yrand1][xrand1] == -1) && (grid[yrand2][xrand2] == -1))));
                
                int cell1 = grid[yrand1][xrand1];
                int cell2 = grid[yrand2][xrand2];
                old_cost = 0;
                unordered_set<int> nets;
                if (cell1 != -1 && cell2 != -1)
                {
                    nets = set_union(cells_nets[grid[yrand1][xrand1]], cells_nets[grid[yrand2][xrand2]]);
                }
                else if (cell1 != -1 && cell2 == -1)
                {
                    nets = cells_nets[grid[yrand1][xrand1]];
                }
                else
                {
                    nets = cells_nets[grid[yrand2][xrand2]];
                }
                for (int net : nets)
                {
                    old_cost += calculate_hpwl(xcoords, ycoords, numbers, net + 1);
                }
                swap(grid[yrand1][xrand1], grid[yrand2][xrand2]);
                if (cell1 != -1)
                {
                    xcoords[cell1] = xrand2;
                    ycoords[cell1] = yrand2;
                }
                if (cell2 != -1)
                {
                    xcoords[cell2] = xrand1;
                    ycoords[cell2] = yrand1;
                }
                // calculate the change in WL (ΔL) due to the swap
                new_cost = 0;
                for (int net : nets)
                {
                    new_cost += calculate_hpwl(xcoords, ycoords, numbers, net + 1);
                }
                //new_cost = calculate_cost(hpwls);
                // if ΔL < 0 then accept
                int delta = new_cost - old_cost;
                // else reject with probability (1-e^-ΔL/T)
                if(delta >= 0)
                {
                    double probability = 1 - exp(-delta / temp);
                    //cout << "Probability: " << probability << endl;
                    double r = (double)(rand() % 1000) / 1000;
                    // cout << r << endl;
                    if (r < probability)
                    {
                        //cout << "Bad move rejected\n";
                        swap(grid[yrand1][xrand1], grid[yrand2][xrand2]);
                        if (cell1 != -1)
                        {
                            xcoords[cell1] = xrand1;
                            ycoords[cell1] = yrand1;
                        }
                        if (cell2 != -1)
                        {
                            xcoords[cell2] = xrand2;
                            ycoords[cell2] = yrand2;
                        }
                    }
                }
            }
            // T = schedule_temp()
            temp *= cooling_rate;
        }

        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
        cout << "Time taken: " << duration.count() << " milliseconds" << endl;
        cout << "Final Placement\n";
        print(grid);
        print_bin(grid);
        int final_cost = 0;
        for (int i = 0; i < nets; i++)
        {
            final_cost += calculate_hpwl(xcoords, ycoords, numbers, i+1);
        }
        cout << "TWL after finishing the SA: " << final_cost << endl;

        filename = "d";
        numbers.clear();
        grid.clear();
        xcoords.clear();
        ycoords.clear();
    }
    return 0;
}