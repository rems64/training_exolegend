#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

using namespace std;
int width;
int height;

enum CellType { Free, Crane, Wall, Enemy };

struct CellInfo {
    enum CellType type;
    int crane_value;
};

CellInfo make_cell_info(const char c) {
    CellInfo cell;
    switch (c) {
    case '.':
        cell.type = Free;
        break;
    case 'X':
        cell.type = Wall;
        break;
    default:
        cell.type = Crane;
        cell.crane_value = c - '0';
    }
    return cell;
}

const char cell_to_char(CellInfo cell) {
    switch (cell.type) {
    case Free:
        return '.';
    case Crane:
        return '0' + cell.crane_value;
    case Wall:
        return 'H';
    case Enemy:
        return 'x';
    }
}

typedef std::map<std::pair<size_t, size_t>, CellInfo> grid_t;

void print_grid(grid_t &grid) {
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            cerr << cell_to_char(grid.at(std::make_pair(i, j)));
        }
        cerr << endl;
    }
}

int main() {
    int my_id;
    cin >> width >> height >> my_id;
    cin.ignore();

    grid_t grid;

    // game loop
    while (1) {
        for (size_t i = 0; i < height; i++) {
            string row;
            cin >> row;
            for (size_t j = 0; j < row.size(); j++) {
                const char c = row[j];
                grid.insert_or_assign(std::make_pair(i, j), make_cell_info(c));
            }
            cin.ignore();
        }
        print_grid(grid);
        int entities;
        cin >> entities;
        cin.ignore();
        for (int i = 0; i < entities; i++) {
            int entity_type;
            int owner;
            int x;
            int y;
            int param_1;
            int param_2;
            cin >> entity_type >> owner >> x >> y >> param_1 >> param_2;
            cin.ignore();
        }

        // cout << "BOMB 6 5" << endl;
    }
}