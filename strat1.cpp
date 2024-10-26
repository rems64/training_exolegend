#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

using namespace std;
constexpr int width = 13;
constexpr int height = 11;

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
typedef int field_value_t;
typedef field_value_t field_t[height][width];

void print_grid(grid_t &grid) {
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            cerr << cell_to_char(grid.at(std::make_pair(i, j)));
        }
        cerr << endl;
    }
}

void print_field(field_t &field) {
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            cerr << field[i][j] << "\t";
        }
        cerr << endl;
    }
}

void fill_field(field_t &field, field_value_t value) {
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            field[i][j] = value;
        }
    }
    // memset(field, value, width * height);
}

std::vector<std::pair<size_t, size_t>> get_neighbors(size_t i, size_t j,
                                                     field_t &seen) {
    std::vector<std::pair<size_t, size_t>> neighbors;
    if (i > 0 && seen[i - 1][j] == 0) {
        neighbors.push_back(std::make_pair(i - 1, j));
    }
    if (j > 0 && seen[i][j - 1] == 0) {
        neighbors.push_back(std::make_pair(i, j - 1));
    }
    if (i < (height - 1) && seen[i + 1][j] == 0) {
        neighbors.push_back(std::make_pair(i + 1, j));
    }
    if (j < (width - 1) && seen[i][j + 1] == 0) {
        neighbors.push_back(std::make_pair(i, j + 1));
    }
    return neighbors;
}

CellInfo get_cell_info(grid_t &grid, size_t i, size_t j) {
    return grid.at(std::make_pair(i, j));
}

size_t flood_fill(field_t &field, grid_t &grid, field_t &seen,
                  CellType free_type, size_t i, size_t j,
                  size_t component_count) {
    if (seen[i][j] > 0)
        return component_count;
    seen[i][j] = 1;
    if (get_cell_info(grid, i, j).type != free_type) {
        field[i][j] = -1;
        return component_count;
    }
    field[i][j] = component_count;
    const auto neighbors = get_neighbors(i, j, seen);
    for (auto neighbor : neighbors) {
        flood_fill(field, grid, seen, free_type, neighbor.first,
                   neighbor.second, component_count);
    }
    return component_count + 1;
}

size_t compute_components(field_t &components, grid_t &grid) {
    field_t seen;
    size_t component_count = 0;
    fill_field(seen, 0);
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            if (seen[i][j] > 0) {
                continue;
            }
            component_count =
                flood_fill(components, grid, seen, Free, i, j, component_count);
        }
    }
    return component_count;
}

int main() {
    int my_id;
    int given_width, given_height;
    cin >> given_width >> given_height >> my_id;
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

        field_t components;
        size_t component_count = compute_components(components, grid);

        cerr << "there are " << component_count << " components" << endl;
        print_field(components);

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