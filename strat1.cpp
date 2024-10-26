#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace std;
constexpr unsigned int width = 13;
constexpr unsigned int height = 11;

enum CellType { Free, Crane, Wall, Enemy };

struct CellInfo {
    enum CellType type = Free;
    int crane_value = 0;
    bool mined = false;
    int mine_turn_count = 0;
    bool dangerous = false;
    int dangerous_turn_count = 0;
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
typedef std::pair<uint32_t, uint32_t> position_t;

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
    CellInfo cell = get_cell_info(grid, i, j);
    if (cell.type != free_type || cell.dangerous) {
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
    size_t component_count = 1;
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

uint32_t clamp_i(uint32_t i) { return max(min(i, height - 1), 0u); }

uint32_t clamp_j(uint32_t j) { return max(min(j, width - 1), 0u); }

void flag_bomb(grid_t &grid, uint32_t i, uint32_t j, int turns_left,
               int range) {
    CellInfo &cell = grid.at(std::make_pair(i, j));
    cell.mined = true;
    cell.mine_turn_count = turns_left;
    for (size_t row = clamp_i(i - range); row < clamp_i(i + range - 1) + 1;
         i++) {
        for (size_t col = clamp_i(j - range); col < clamp_j(j + range - 1) + 1;
             i++) {
            CellInfo &endangered_cell = grid.at(std::make_pair(row, col));
            endangered_cell.dangerous = true;
            endangered_cell.dangerous_turn_count = turns_left;
        }
    }
}

position_t make_position(uint32_t i, uint32_t j) { return make_pair(i, j); }

enum State { Grind };
enum Action { Goto, Plant };

bool reached(position_t &position, position_t &target) {
    return position.first == target.first && position.second == target.second;
}

size_t distance(position_t &a, position_t &b) {
    return abs((int)b.first - (int)a.first) +
           abs((int)b.second - (int)a.second);
}

void pick_new_target(grid_t &grid, field_t &components, position_t &position,
                     position_t &target) {
    field_value_t current_value = components[position.first][position.second];
    position_t best = position;
    size_t best_distance = 0;
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            field_value_t v = components[i][j];
            if (v == current_value) {
                position_t candidate_position = make_position(i, j);
                size_t d = distance(position, candidate_position);
                if (d > best_distance) {
                    best_distance = d;
                    best = candidate_position;
                }
            }
        }
    }
    cerr << "best candidate for new position is " << best.first << ", "
         << best.second << endl;
    target = best;
}

void pick_safe_actual_target(grid_t &grid, field_t &components,
                             position_t &position, position_t &target,
                             position_t &actual_target) {
    field_value_t current_value = components[position.first][position.second];
    position_t best = target;
    size_t best_distance = 1000;
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            field_value_t v = components[i][j];
            if (v != current_value)
                continue;
            position_t candidate_position = make_position(i, j);
            if (grid.at(candidate_position).dangerous)
                continue;
            size_t d = distance(target, candidate_position);
            if (d < best_distance) {
                best_distance = d;
                best = candidate_position;
            }
        }
    }
    actual_target = best;
}

void grind(grid_t &grid, field_t &components, enum State &state,
           enum Action &action, position_t &position, position_t &target,
           position_t &actual_target, bool &plant) {
    if (reached(position, target)) {
        switch (action) {
        case Plant:
            plant = true;
        case Goto:
            action = Plant;
            pick_new_target(grid, components, position, target);
        }
    }
    // pick_safe_actual_target(grid, components, position, target,
    // actual_target);
    actual_target = target;
}

int main() {
    int my_id;
    int given_width, given_height;
    cin >> given_width >> given_height >> my_id;
    cin.ignore();
    position_t my_position;

    enum State state = Grind;
    enum Action action = Goto;
    grid_t grid;
    uint32_t turn = 0;
    uint32_t last_action_turn = -1000;
    int drop_right = 1;
    position_t target = {0, 0};
    position_t actual_target = {0, 0};
    // game loop
    while (1) {
        turn += 1;
        for (size_t i = 0; i < height; i++) {
            string row;
            cin >> row;
            for (size_t j = 0; j < row.size(); j++) {
                const char c = row[j];
                grid.insert_or_assign(std::make_pair(i, j), make_cell_info(c));
            }
            cin.ignore();
        }

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
            switch (entity_type) {
            // Player
            case 0:
                if (owner == my_id) {
                    my_position = std::make_pair(y, x);
                }
                break;
            // Bomb
            case 1:
                flag_bomb(grid, y, x, param_1, param_2);
            }
            cin.ignore();
        }

        field_t components;
        size_t component_count = compute_components(components, grid);
        bool plant = false;

        switch (state) {
        case Grind:
            grind(grid, components, state, action, my_position, target,
                  actual_target, plant);
            break;
        }

        if (plant) {
            cout << "BOMB " << actual_target.second << " "
                 << actual_target.first << endl;
        } else {
            cout << "MOVE " << actual_target.second << " "
                 << actual_target.first << endl;
        }

        // cout << "BOMB 6 5" << endl;
    }
}