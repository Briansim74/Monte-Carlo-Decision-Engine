#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <iomanip>
#include <ranges>
#include <random>
using namespace std;
using std::cout;


struct Player {
    int id;
    string hand;
    vector<string> cards;
    double stack;
    double starting_stack;
    double contribution = 0;
    bool folded = false;
};


struct Action {
    int player_id;
    string hand;
    string position;
    string action;
    double bet;
    string street;
};


struct BoardTexture {
    bool flush_draw;
    bool paired;
};


struct HandCombo {
    int rank;
    vector<int> tiebreak; // numeric ranks in descending order
    string combo;
    vector<string> combo_cards; // actual 5-card combo
};


struct Game_State {
    double pot = 0.0;
    vector<Action> actions;
    double current_bet = 0.0;
    double last_raize_size = 0.0;
    double sb_amount;

    vector<string> positions_order;
    vector<int> seating;
    vector<Player> player_states;

    int num_players;
    int my_player_id;
    bool game_ended = false;
    int action_index;
    int players_to_act;

    unordered_map<string, vector<string>> board;
    BoardTexture board_texture;
};


class Poker_Simulation {
public:
    // Function
    void run_simulations(int simulations, double stack, int num_players, int my_player_id, double sb_amount);

private:
    // Variables and constants
    Game_State game_state;
    static unordered_map<string, unordered_map<string, double>> preflop_ranges;
    static unordered_map<char, int> rank_value;
    static const unordered_map<char,int> rank_prime;
    static const unordered_map<char,int> suit_map;

    // Functions
    // 1. Initialization / Setup
    void get_positions_order();
    string get_general_position(int& seating_index);
    void initialize_parameters(int num_players, double stack, int my_player_id, double sb_amount);
    void initialize_preflop();
    void initialize_postflop(const string& street);

    // 2. Hand / card evaluation
    double evaluate_hand_strength(const vector<string>& cards, const vector<string>& board);
    vector<string> get_cards_by_value(const vector<string>& all_cards, int val, int count);
    vector<string> get_straight_cards(const vector<string>& all_cards, int high);
    HandCombo evaluate_hand_strength_combo(const vector<string>& cards, const vector<string>& board);
    int encode_card(const string& card);
    int evaluate_7card_fast(const vector<string>& hand, const vector<string>& board);
    int evaluate_7card(const vector<string>& hand, const vector<string>& board);
    int evaluate_5card(const vector<string>& cards);
    double monte_carlo_equity_multiway(const vector<string>& hero_cards, const string& street, int num_opponents, int simulations = 2000);
    void analyze_board_texture(const string& street);

    // 3. Decision - making
    double adjust_weight_for_previous_raises(double& weight, int num_raises);
    pair<string, double> decide_preflop_action(Player& player, const string& position);
    pair<string, double> decide_flop_action(Player& player, int num_opponents);
    pair<string, double> decide_turn_action(Player& player, int num_opponents);
    pair<string, double> decide_river_action(Player& player, int num_opponents);
    pair<string, double> decide_postflop_action(Player& player, int num_opponents, const string& street);

    // 4. Simulation / gameplay
    string cards_to_hand(const string& card1, const string& card2, const string& ranks);
    void deal_poker_hand(int& sb_index);
    void simulate_street(const string& street);
    void simulate_showdown();
    void end_game(const string& street);
    void run_simulation();
};


int main(){
    int simulations, num_players, my_player_id;
    double stack, sb_amount;
    Poker_Simulation poker_simulation;

    cout << "\n--------- POKER SIMULATION ---------\n\n";
    cout << "Please enter [simulations, stack, num_players, my_player_id (0 == UTG), sb_amount] (or 0 to quit):";
    cin >> simulations;

    if(simulations == 0){
        cout << "\nExiting Poker Simulation...\n\n";
        return 0;
    }

    cin >> stack >> num_players >> my_player_id >> sb_amount;
    poker_simulation.run_simulations(simulations, stack, num_players, my_player_id, sb_amount);

    return 0;
}


// Full pre-flop hand ranges by position
unordered_map<string, unordered_map<string, double>> Poker_Simulation::preflop_ranges = {
    {
        "early",
        {
            // Pocket pairs
            {"AA", 1.0}, {"KK", 1.0}, {"QQ", 1.0}, {"JJ", 1.0},
            {"TT", 0.3}, {"99", 0.2}, {"88", 0.1}, {"77", 0.0},
            {"66", 0.0}, {"55", 0.0}, {"44", 0.0}, {"33", 0.0}, {"22", 0.0},

            // Suited broadways
            {"AKs", 1.0}, {"AQs", 1.0}, {"AJs", 0.8}, {"ATs", 0.3},
            {"KQs", 1.0}, {"KJs", 0.5}, {"QJs", 0.5},

            // Offsuit broadways
            {"AKo", 1.0}, {"AQo", 1.0}, {"AJo", 0.8},
            {"KQo", 1.0}, {"KJo", 0.3}, {"QJo", 0.2},

            // Suited connectors & one-gappers
            {"JTs", 0.2}, {"T9s", 0.2}, {"98s", 0.1}, {"87s", 0.1},
            {"76s", 0.0}, {"65s", 0.0}, {"54s", 0.0},

            // Suited aces
            {"A2s", 0.0}, {"A3s", 0.0}, {"A4s", 0.0}, {"A5s", 0.0}
        }
    },
    {
        "middle",
        {
            // Pocket pairs
            {"AA", 1.0}, {"KK", 1.0}, {"QQ", 1.0}, {"JJ", 1.0},
            {"TT", 0.8}, {"99", 0.6}, {"88", 0.5}, {"77", 0.4},
            {"66", 0.3}, {"55", 0.2}, {"44", 0.1}, {"33", 0.0}, {"22", 0.0},

            // Suited broadways
            {"AKs", 1.0}, {"AQs", 1.0}, {"AJs", 1.0}, {"ATs", 0.8},
            {"A9s", 0.5}, {"A8s", 0.3},
            {"KQs", 1.0}, {"KJs", 0.8}, {"KTs", 0.5},
            {"QJs", 0.8}, {"QTs", 0.5},

            // Offsuit broadways
            {"AKo", 1.0}, {"AQo", 1.0}, {"AJo", 0.8}, {"ATo", 0.5},
            {"KQo", 1.0}, {"KJo", 0.5}, {"QJo", 0.5}, {"QTo", 0.3},

            // Suited connectors & one-gappers
            {"JTs", 0.5}, {"T9s", 0.5}, {"98s", 0.3}, {"87s", 0.3},
            {"76s", 0.2}, {"65s", 0.1}, {"54s", 0.1},
            {"64s", 0.1}, {"75s", 0.1}, {"86s", 0.1}, {"97s", 0.1},

            // Suited aces
            {"A2s", 0.1}, {"A3s", 0.1}, {"A4s", 0.1}, {"A5s", 0.1}
        }
    },
    {
        "late",
        {
            // Pocket pairs
            {"AA", 1.0}, {"KK", 1.0}, {"QQ", 1.0}, {"JJ", 1.0},
            {"TT", 1.0}, {"99", 1.0}, {"88", 1.0}, {"77", 1.0},
            {"66", 0.8}, {"55", 0.8}, {"44", 0.7}, {"33", 0.7}, {"22", 0.7},

            // Suited broadways
            {"AKs", 1.0}, {"AQs", 1.0}, {"AJs", 1.0}, {"ATs", 1.0},
            {"A9s", 0.8}, {"A8s", 0.7}, {"A7s", 0.6},
            {"A6s", 0.5}, {"A5s", 0.5}, {"A4s", 0.5}, {"A3s", 0.5}, {"A2s", 0.5},
            {"KQs", 1.0}, {"KJs", 1.0}, {"KTs", 0.8}, {"K9s", 0.6},
            {"QJs", 1.0}, {"QTs", 0.8}, {"Q9s", 0.6},

            // Offsuit broadways
            {"AKo", 1.0}, {"AQo", 1.0}, {"AJo", 0.8}, {"ATo", 0.8},
            {"KQo", 1.0}, {"KJo", 0.8}, {"KTo", 0.5},
            {"QJo", 0.8}, {"QTo", 0.5},

            // Suited connectors & one-gappers
            {"JTs", 0.8}, {"T9s", 0.8}, {"98s", 0.6}, {"87s", 0.6},
            {"76s", 0.5}, {"65s", 0.5}, {"54s", 0.5},
            {"64s", 0.4}, {"75s", 0.4}, {"86s", 0.4}, {"97s", 0.4},

            // Suited aces
            {"A2s", 0.5}, {"A3s", 0.5}, {"A4s", 0.5}, {"A5s", 0.5}
        }
    }
};

unordered_map<char, int> Poker_Simulation::rank_value = {
    {'2', 2}, {'3', 3},{'4', 4}, {'5', 5}, {'6', 6},
    {'7', 7}, {'8', 8}, {'9', 9}, {'T', 10},
    {'J', 11}, {'Q', 12}, {'K', 13}, {'A', 14}
};


// Primes for 2-A
// Map rank char → prime number (for hand evaluation)
const unordered_map<char,int> Poker_Simulation::rank_prime = {
    {'2', 2}, {'3', 3}, {'4', 5}, {'5', 7}, {'6', 11}, {'7', 13}, {'8', 17}, {'9', 19},
    {'T', 23}, {'J', 29}, {'Q', 31}, {'K', 37}, {'A', 41}
};


// Map suit char → 0-3
const unordered_map<char,int> Poker_Simulation::suit_map = {
    {'c', 0}, {'d', 1}, {'h', 2}, {'s', 3}
};


// 1. Initialization / Setup


void Poker_Simulation::get_positions_order(){
    /*
    Generate poker seat names for a table of num_players.
    Seats are ordered clockwise from UTG (first to act pre-flop after BB).
    Uses standard naming:
        Early: UTG, UTG+1, UTG+2...
        Middle: MP, MP+1...
        Late: CO, BTN
        Blinds: SB, BB
    */

    if(game_state.num_players == 2) game_state.positions_order = {"SB", "BB"};

    else if(game_state.num_players == 3) game_state.positions_order = {"UTG", "SB", "BB"};

    else if(game_state.num_players == 4) game_state.positions_order = {"UTG", "MP", "SB", "BB"};

    else if(game_state.num_players == 5) game_state.positions_order = {"UTG", "MP", "CO", "SB", "BB"};

    else if(game_state.num_players == 6) game_state.positions_order = {"UTG", "MP", "CO", "BTN", "SB", "BB"};

    else if(game_state.num_players == 7) game_state.positions_order = {"UTG", "UTG+1", "MP", "CO", "BTN", "SB", "BB"};

    else if(game_state.num_players == 8) game_state.positions_order = {"UTG", "UTG+1", "UTG+2", "MP", "CO", "BTN", "SB", "BB"};

    else if(game_state.num_players == 9) game_state.positions_order = {"UTG", "UTG+1", "UTG+2", "MP", "MP+1", "CO", "BTN", "SB", "BB"};

    else{
        cout << "Supports 2-9 players only, reverting to 6 players...\n";
        game_state.my_player_id = 0;
        game_state.num_players = 6;
        game_state.positions_order = {"UTG", "UTG+1", "UTG+2", "MP", "MP+1", "CO", "BTN", "SB", "BB"};
    }
}

    
string Poker_Simulation::get_general_position(int& seating_index){
    string pos_name = game_state.positions_order[seating_index];

    if(pos_name == "SB" || pos_name == "BB" || pos_name.starts_with("UTG")) return "early";

    else if(pos_name.starts_with("MP")) return "middle";

    else return "late";
}


void Poker_Simulation::initialize_parameters(int num_players, double stack, int my_player_id, double sb_amount){
    game_state.my_player_id = my_player_id;
    game_state.num_players = num_players;
    game_state.player_states.resize(num_players); // Set number of players for array of player_states
    game_state.sb_amount = sb_amount;

    cout << "\nInitializing parameters -> players: " << num_players << ", stack: " << fixed << setprecision(2) << stack << ", my_player_id: " << my_player_id << ", sb_amount: " << sb_amount << "\n\n";

    for(int i = 0; i < num_players; i++){
        game_state.player_states[i].id = i;
        game_state.player_states[i].stack = stack;
        game_state.player_states[i].starting_stack = stack;
    }
}


void Poker_Simulation::initialize_preflop(){
    
    // Initialize seating if empty
    if(game_state.seating.empty()){
        for(int i = 0; i < game_state.num_players; i++) game_state.seating.push_back(i);
    }

    // Set game_state
    game_state.pot = 0.0;
    game_state.current_bet = 0.0;
    game_state.last_raize_size = 0.0;
    game_state.game_ended = false;
    for(Player& player: game_state.player_states){
        player.starting_stack = player.stack;
        player.folded = false;
    }

    // Initialize positions order 
    get_positions_order(); // ["UTG", "MP", "CO", "BTN", "SB", "BB"]

    cout << left << setw(10) << "Player" 
         << left << setw(15) << "Current Position" << "\n";

    for(const Player& player: game_state.player_states){
        string player_label = to_string(player.id);
        if(player.id == game_state.my_player_id) player_label += " (YOU)";

        // Print row, everything left-aligned
        cout << left << setw(10) << player_label
            << left << setw(15) << game_state.positions_order[game_state.seating[player.id]]
            << "\n";
    }

    // Blinds based on seat index
    int sb_index = game_state.num_players - 2;
    int bb_index = game_state.num_players - 1;
    int bb_amount = game_state.sb_amount * 2;

    // Deal cards
    cout << "\nDEALING CARDS...\n\n";
    cout << "--- PREFLOP ---\n\n";
    deal_poker_hand(sb_index);

    // ---- POST BLINDS ----
    vector<tuple<int, int, string>> blinds = {
        {sb_index, game_state.sb_amount, "SB"},
        {bb_index, bb_amount, "BB"}
    };

    for(auto& [seat_idx, amt, name]: blinds) {
        int player_id = game_state.seating[seat_idx];
        Player& player = game_state.player_states[player_id];
        string position = get_general_position(seat_idx);

        double bet = min((double)amt, player.stack);

        player.stack -= bet;
        player.contribution += bet;
        game_state.pot += bet;
        game_state.current_bet = max(game_state.current_bet, player.contribution);

        game_state.actions.push_back({
            player.id,
            player.hand,
            position,
            "post " + name,
            bet,
            "Pre-Flop"
        });

        cout << "Player " << player_id << ": (" << game_state.positions_order[seat_idx] << ", " << position << ") with " << player.hand
             << " -> post " << name << " " << fixed << setprecision(2) << bet << "\n";
    }

    game_state.last_raize_size = bb_amount - game_state.sb_amount;

    game_state.action_index = (bb_index + 1) % game_state.num_players;
    game_state.players_to_act = game_state.num_players;
}


/*
Initialize Postflop
*/

void Poker_Simulation::initialize_postflop(const string& street){
    
    // Street
    string street_upper = street;
    for(char& ch: street_upper){
        ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    }
    cout << "\n--- " << street_upper << " ---\n\n";

    // Get the board for this street
    cout << "Board - " << street << ": [ ";
    for(int i = 0; i < game_state.board[street].size() - 1; i++) cout << game_state.board[street][i] << ", ";
    cout << game_state.board[street][game_state.board[street].size() - 1] << " ]\n\n";

    // Reset street betting
    game_state.current_bet = 0.0;
    game_state.last_raize_size = 0.0;

    for(Player& player: game_state.player_states){
        player.contribution = 0.0;
    }

    // Start action from SB (small blind) position
    game_state.action_index = 0;
    game_state.players_to_act = game_state.num_players;
}


// 2. Hand / card evaluation

/*
HAND STRENGTH EVALUATION

This is primitive but good enough for engine testing.

Can swap it with a real evaluator (like Cactus Kev / 7-card evaluator).
*/

double Poker_Simulation::evaluate_hand_strength(const vector<string>& cards, const vector<string>& board){

    vector<string> all_cards = cards;
    all_cards.insert(all_cards.end(), board.begin(), board.end());

    // Count ranks and suits
    map<char, int> rank_counts;
    map<char, int> suit_counts;
    vector<int> values;

    for(const string& card: all_cards) {
        char rank = card[0];  // first character is rank
        char suit = card[1];  // second character is suit

        rank_counts[rank]++;
        suit_counts[suit]++;

        values.push_back(rank_value[rank]); // convert to numeric value
    }

    sort(values.begin(), values.end());

    // ---------- FLUSH ----------
    bool flush = false;
    for(auto& [suit, count]: suit_counts){
        if(count >= 5){
            flush = true;
            break;
        }
    }

    // ---------- STRAIGHT ----------
    set<int> unique_vals(values.begin(), values.end());
    vector<int> arr(unique_vals.begin(), unique_vals.end());
    sort(arr.begin(), arr.end());

    bool straight = false;
    for(int i = 0; i + 4 < arr.size(); i++){
        if(arr[i + 4] - arr[i] == 4) straight = true;
    }

    // Wheel straight A2345
    if(unique_vals.count(14) && unique_vals.count(2) && unique_vals.count(3) &&
        unique_vals.count(4) && unique_vals.count(5)){
        straight = true;
    }

    // ---------- MULTIPLES ----------
    vector<int> counts;
    for(auto& [rank, count]: rank_counts) counts.push_back(count);
    sort(counts.rbegin(), counts.rend()); // descending

    bool four = find(counts.begin(), counts.end(), 4) != counts.end();
    bool three = find(counts.begin(), counts.end(), 3) != counts.end();
    int pairs = count(counts.begin(), counts.end(), 2);

    // ---------- HAND TYPE ----------
    double score;

    if (straight && flush) score = 0.98;

    else if (four) score = 0.95;

    else if (three && pairs >= 1) score = 0.90;

    else if (flush) score = 0.85;

    else if (straight) score = 0.80;

    else if (three) score = 0.70;

    else if (pairs >= 2) score = 0.60;

    else if (pairs == 1) score = 0.50;

    else{
        // High card strength scaling
        int high = *max_element(values.begin(), values.end());
        score = 0.2 + (static_cast<double>(high) / 14.0) * 0.2;
    }

    return score;
}


// Get cards of a certain rank
vector<string> Poker_Simulation::get_cards_by_value(const vector<string>& all_cards, int val, int count) {
    vector<string> res;
    
    for(const string& card: all_cards){
        if(rank_value[card[0]] == val && find(res.begin(), res.end(), card) == res.end()){
            res.push_back(card);

            if(res.size() == count) break;
        }
    }

    return res;
}


// Get straight cards by high card
vector<string> Poker_Simulation::get_straight_cards(const vector<string>& all_cards, int high) {
    vector<int> needed = {high, high - 1, high - 2, high - 3, high - 4};
    vector<string> res; // {"8h","9s","Ts","Jc","Qd"}

    for(const int value: needed){
        for(const string& card: all_cards){
            if(rank_value[card[0]] == value && find(res.begin(), res.end(), card) == res.end()) {
                res.push_back(card);
                break;
            }
        }
    }

    return res;
}


HandCombo Poker_Simulation::evaluate_hand_strength_combo(const vector<string>& cards, const vector<string>& board){
    
    // Count ranks and suits
    vector<string> all_cards = cards;
    all_cards.insert(all_cards.end(), board.begin(), board.end());

    map<char, int> rank_counts;
    map<char, int> suit_counts;
    vector<int> values;

    for(const string& card: all_cards) {
        char rank = card[0];  // first character is rank
        char suit = card[1];  // second character is suit

        rank_counts[rank]++;
        suit_counts[suit]++;

        values.push_back(rank_value[rank]); // convert to numeric value
    }

    // ---------- FLUSH ----------
    char flush_suit = 0;
    for(auto& [suit, count]: suit_counts){
        if(count >= 5){
            flush_suit = suit;
            break;
        }
    }

    vector<string> flush_cards;
    if(flush_suit){
        for(const string& card: all_cards){
            if(card[1] == flush_suit) flush_cards.push_back(card);
        }
    }

    // ---------- STRAIGHT ----------
    set<int> unique_vals(values.begin(), values.end());
    vector<int> arr(unique_vals.begin(), unique_vals.end());
    sort(arr.begin(), arr.end());

    int straight_high = 0;

    for(int i = 0; i + 4 < arr.size(); i++){
        if (arr[i + 4] - arr[i] == 4) straight_high = arr[i + 4];
    }

    // Wheel straight A2345
    if(unique_vals.count(14) && unique_vals.count(2) && unique_vals.count(3) 
        && unique_vals.count(4) && unique_vals.count(5)){
        straight_high = 5;
    }

    // ---------- MULTIPLES ----------
    // Sort counts by frequency then rank
    vector<pair<int, int>> counts; // {rank_value, count}

    for(auto& [rank, count]: rank_counts) counts.push_back({rank_value[rank], count});

    sort(counts.begin(), counts.end(), [](auto a, auto b){
        if(a.second != b.second) return a.second > b.second;

        return a.first > b.first;
    });

    // ---------- HAND TYPES ----------
    // Straight Flush
    if(flush_suit && straight_high){
        vector<string> straight_flush_cards;
        vector<int> straight_flush_values;

        for(const string& card: flush_cards) straight_flush_values.push_back(rank_value[card[0]]);
        
        set<int> straight_flush_unique(straight_flush_values.begin(), straight_flush_values.end());
        vector<int> straight_flush_arr(straight_flush_unique.begin(), straight_flush_unique.end());
        sort(straight_flush_arr.begin(), straight_flush_arr.end());

        for(int i = 0; i + 4 <straight_flush_arr.size(); i++)
            if(straight_flush_arr[i + 4] - straight_flush_arr[i] == 4){
                straight_flush_cards = get_straight_cards(all_cards, straight_flush_arr[i + 4]);

                vector<int> tiebreak;
                for(const string& card: straight_flush_cards) tiebreak.push_back(rank_value[card[0]]);
                
                return {8, tiebreak, "Straight Flush", straight_flush_cards};
            }
        
        // Wheel straight flush A2345
        if(straight_flush_unique.count(14) && straight_flush_unique.count(2) && straight_flush_unique.count(3) 
            && straight_flush_unique.count(4) && straight_flush_unique.count(5)){
            straight_flush_cards = get_straight_cards(all_cards, 5);

            vector<int> tiebreak;
            for(const string& card: straight_flush_cards) tiebreak.push_back(rank_value[card[0]]);

            return {8, tiebreak, "Straight Flush", straight_flush_cards};
        }
    }

    // Four of a kind
    if(counts[0].second == 4){
        vector<string> quad = get_cards_by_value(all_cards, counts[0].first, 4);
        vector<string> kicker;

        for(const string& card: all_cards){
            if(rank_value[card[0]] != counts[0].first){
                kicker.push_back(card); // Finds kicker for quad (highest card)
                break;
            }
        }
        quad.insert(quad.end(), kicker.begin(), kicker.end());

        vector<int> tiebreak;
        for(const string& card: quad) tiebreak.push_back(rank_value[card[0]]);

        return {7, tiebreak, "Four of a Kind", quad};
    }

    // Full House
    if(counts[0].second == 3 && counts.size() > 1 && counts[1].second >= 2){
        vector<string> trips = get_cards_by_value(all_cards, counts[0].first, 3);
        vector<string> pair = get_cards_by_value(all_cards, counts[1].first, 2);

        trips.insert(trips.end(), pair.begin(), pair.end());
        
        vector<int> tiebreak;
        for(const string& card: trips) tiebreak.push_back(rank_value[card[0]]);
        
        return {6, tiebreak, "Full House", trips};
    }

    // Flush
    if(flush_suit){
        vector<int> tiebreak;
        for(const string& card: flush_cards) tiebreak.push_back(rank_value[card[0]]);
        
        return {5, tiebreak, "Flush", flush_cards};
    }

    // Straight
    if(straight_high){
        vector<string> straight_cards = get_straight_cards(all_cards, straight_high);
        
        vector<int> tiebreak;
        for(const string& card: straight_cards) tiebreak.push_back(rank_value[card[0]]);
        
        return {4, tiebreak, "Straight", straight_cards};
    }

    // Three of a kind
    if(counts[0].second == 3){
        vector<string> trips = get_cards_by_value(all_cards, counts[0].first, 3);
        vector<string> kickers;

        for(const string& card: all_cards){
            if(rank_value[card[0]] != counts[0].first && kickers.size() < 2){
                kickers.push_back(card);
            }
        }

        trips.insert(trips.end(), kickers.begin(), kickers.end());
        
        vector<int> tiebreak;
        for(const string& card: trips) tiebreak.push_back(rank_value[card[0]]);
        
        return {3, tiebreak, "Three of a Kind", trips};
    }

    // Two Pair
    if(counts.size() > 1 && counts[0].second == 2 && counts[1].second == 2){
        vector<string> pair1 = get_cards_by_value(all_cards, counts[0].first, 2);
        vector<string> pair2 = get_cards_by_value(all_cards, counts[1].first, 2);
        vector<string> kicker;
        
        for(const string& card: all_cards){
            if(rank_value[card[0]] != counts[0].first && rank_value[card[0]] != counts[1].first){
                kicker.push_back(card); 
                break;
            }
        }
        
        pair1.insert(pair1.end(), pair2.begin(), pair2.end());
        pair1.insert(pair1.end(), kicker.begin(), kicker.end());
        
        vector<int> tiebreak;
        for(const string& card: pair1) tiebreak.push_back(rank_value[card[0]]);
        
        return {2, tiebreak, "Two Pair", pair1};
    }

    // One Pair
    if(counts[0].second == 2){
        vector<string> pair = get_cards_by_value(all_cards, counts[0].first, 2);
        vector<string> kickers;

        for(const string& card: all_cards){
            if(rank_value[card[0]] != counts[0].first && kickers.size() < 3){
                kickers.push_back(card);
            }
        }
        
        pair.insert(pair.end(), kickers.begin(), kickers.end());
        
        vector<int> tiebreak;
        for(const string& card: pair) tiebreak.push_back(rank_value[card[0]]);
        
        return {1, tiebreak, "One Pair", pair};
    }

    // High card
    vector<string> high_cards(all_cards.begin(), all_cards.begin() + 5);
    
    vector<int> tiebreak;
    for(const string& card: high_cards) tiebreak.push_back(rank_value[card[0]]);
    
    return {0, tiebreak, "High Card", high_cards};
}


int Poker_Simulation::encode_card(const string& card){
    char r = card[0];  // rank
    char s = card[1];  // suit

    int val   = rank_value.at(r);   // 2-14
    int prime = rank_prime.at(r);   // prime number
    int suit  = suit_map.at(s);     // 0-3

    // Encode: high bits = prime, next bits = rank, low 4 bits = suit
    return (prime << 8) | (val << 4) | suit;
}


int Poker_Simulation::evaluate_7card_fast(const vector<string>& hand, const vector<string>& board){
    const int BASE = 15;
    vector<int> cards;
    for(const string& card: hand) cards.push_back(encode_card(card));
    for(const string& card: board) cards.push_back(encode_card(card));

    vector<int> values;
    int suits_mask[4] = {0, 0, 0, 0};
    int rank_mask = 0;
    int rank_counts[15] = {0};

    for(int card: cards){
        int val = (card >> 4) & 0xF;
        int suit = card & 0xF;
        int prime = card >> 8;

        values.push_back(val);
        suits_mask[suit] |= (1 << val);
        rank_mask |= (1 << val);
        rank_counts[val]++;
    }

    sort(values.rbegin(),values.rend());

    // ---------- FLUSH ----------
    bool flush = false;
    int flush_suit = -1;
    for(int i = 0; i < 4; i++){
        if(__builtin_popcount(suits_mask[i]) >= 5){
            flush = true;
            flush_suit = i;
            break;
        }
    }

    // ---------- STRAIGHT ----------
    auto get_straight_high = [&](int mask){
        for(int high = 14; high >= 5; high--){
            bool ok = true;
            for(int k = 0; k < 5; k++){
                if(!(mask & (1 << (high - k)))){
                    ok = false;
                    break;
                }
            }
            if(ok) return high;
        }

        // Wheel straight A2345
        if((mask & (1 << 14)) && (mask & (1 << 2)) && (mask & (1 << 3)) &&
           (mask & (1 << 4)) && (mask & (1 << 5))){
            return 5;
        }

        return 0;
    };

    int straight_high = get_straight_high(rank_mask);
    bool straight = (straight_high > 0);

    // ---------- BUILD SCORE ----------
    auto encode = [&](int hand_rank, vector<int> kickers){
        // Score = hand_rank * BASE ^ 5 + k1 * BASE ^ 4 + ... + k5
        while(kickers.size() < 5) kickers.push_back(0);

        int score = hand_rank;

        for(int& k: kickers){
            score = score * BASE + k;
        }
        return score;
    };

    // ---------- HAND TYPES ----------
    if(flush){
        int sf_high = get_straight_high(suits_mask[flush_suit]);
        if(sf_high) return encode(8, {sf_high});
    }

    // Four of a Kind or Full House
    int four = -1, three = -1;
    vector<int> pairs;

    for(int rank = 2; rank <= 14; rank++){
        int count = rank_counts[rank];
        if(count == 0) continue;

        if(count == 4) four = rank;
        else if(count == 3) three = max(three, rank);
        else if(count == 2) pairs.push_back(rank);
    }

    // Four of a kind
    if(four != -1){
        int kicker = 0;
        for(int rank = 2; rank <= 14; rank++){
            int count = rank_counts[rank];
            if(count == 0) continue;

            if(rank != four) kicker = max(kicker, rank);
        }
        return encode(7, {four, kicker});
    }

    // Full house
    if(three != -1){
        int best_pair = -1;

        for(int pair: pairs) best_pair = max(best_pair, pair);

        // handle double trips (e.g. 777 + 555)
        for(int rank = 2; rank <= 14; rank++){
            int count = rank_counts[rank];
            if(count == 0) continue;

            if(rank != three && count == 3){
                best_pair = max(best_pair, rank);
            }
        }

        if(best_pair != -1) return encode(6, {three, best_pair});
    }

    // Flush
    if(flush){
        vector<int> flush_cards;

        for(int card: cards){
            int suit = card & 0xF;
            int val = (card >> 4) & 0xF;

            if(suit == flush_suit) flush_cards.push_back(val);
        }

        sort(flush_cards.rbegin(), flush_cards.rend());
        flush_cards.resize(5);

        return encode(5, flush_cards);
    }

    // Straight
    if(straight) return encode(4, {straight_high});

    // Three of a kind
    if(three != -1){
        vector<int> kickers = {three};

        vector<int> rest;
        for(int rank = 2; rank <= 14; rank++){
            int count = rank_counts[rank];
            if(count == 0) continue;

            if(rank != three){
                for(int i = 0; i < count; i++) rest.push_back(rank);
            }
        }

        sort(rest.rbegin(), rest.rend());
        kickers.push_back(rest[0]);
        kickers.push_back(rest[1]);

        return encode(3, kickers);
    }

    // Two pair
    if(pairs.size() >= 2){
        sort(pairs.rbegin(), pairs.rend());

        int kicker = 0;
        for(int rank = 2; rank <= 14; rank++){
            int count = rank_counts[rank];
            if(count == 0) continue;

            if(rank != pairs[0] && rank != pairs[1]){
                kicker = max(kicker, rank);
            }
        }

        return encode(2, {pairs[0], pairs[1], kicker});
    }

    // One pair
    if(pairs.size() == 1){
        vector<int> rest;

        for(int rank = 2; rank <= 14; rank++){
            int count = rank_counts[rank];
            if(count == 0) continue;

            if(rank != pairs[0]){
                for(int i = 0; i < count; i++) rest.push_back(rank);
            }
        }

        sort(rest.rbegin(), rest.rend());

        return encode(1, {pairs[0], rest[0], rest[1], rest[2]});
    }

    // High card
    values.resize(5);
    return encode(0, values);
}


// Cactus Kev / 7-card evaluator
int Poker_Simulation::evaluate_7card(const vector<string>& cards, const vector<string>& board){
    vector<string> all_cards = cards;
    all_cards.insert(all_cards.end(), board.begin(), board.end());
    
    int best_score = 0;

    // Iterate all 5-card combinations
    int n = all_cards.size();
    
    for(int i = 0; i < n; i++) {
        for(int j = i + 1;j < n; j++) {
            for(int k = j + 1; k < n; k++) {
                for(int l = k + 1; l < n; l++) {
                    for(int m = l + 1; m < n; m++) {
                        vector<string> combo = {all_cards[i], all_cards[j], all_cards[k], all_cards[l], all_cards[m]};
                        int score = evaluate_5card(combo);
                        best_score = max(best_score, score);
                    }
                }
            }
        }
    }

    return best_score;
}


int Poker_Simulation::evaluate_5card(const vector<string>& cards){
    // Use BASE = 15, since card values go up to 14
    // '2' → 2
    // '3' → 3
    // '4' → 4
    // '5' → 5
    // '6' → 6
    // '7' → 7
    // '8' → 8
    // '9' → 9
    // 'T' → 10
    // 'J' → 11
    // 'Q' → 12
    // 'K' → 13
    // 'A' → 14

    const int BASE = 15;
    map<int, int> rank_counts;
    map<char, int> suit_counts;
    vector<int> values;

    for(const string& card: cards) {
        char rank = card[0];  // first character is rank
        char suit = card[1];  // second character is suit

        rank_counts[rank]++;
        suit_counts[suit]++;

        values.push_back(rank_value[rank]); // convert to numeric value
    }

    sort(values.rbegin(), values.rend()); // descending

    // ---------- FLUSH ----------
    bool flush = false;
    for(auto& [suit, count]: suit_counts){
        if(count >= 5){
            flush = true;
            break;
        }
    }

    // ---------- STRAIGHT ----------
    set<int> unique_vals(values.begin(), values.end());
    vector<int> arr(unique_vals.begin(), unique_vals.end());
    sort(arr.begin(), arr.end());

    bool straight = false;
    int straight_high = 0;

    for(int i = 0; i + 4 < arr.size(); i++) {
        if(arr[i + 4] - arr[i] == 4){
            straight = true;
            straight_high = arr[i + 4];
        }
    }

    // Wheel straight A2345
    if (unique_vals.count(14) && unique_vals.count(2) && unique_vals.count(3) 
        && unique_vals.count(4) && unique_vals.count(5)) {
        straight = true;
        straight_high = 5;
    }

    // ---------- GROUP BY COUNT ----------
    vector<pair<int,int>> groups; // {count, rank}
    for(auto& [rank, count]: rank_counts){
        groups.push_back({count, rank});
    }

    sort(groups.begin(), groups.end(), [](auto a, auto b){
        if(a.first != b.first) return a.first > b.first;
        return a.second > b.second;
    });

    // ---------- BUILD SCORE ----------
    auto encode = [&](int hand_rank, vector<int> kickers){
        // Score = hand_rank * BASE ^ 5 + k1 * BASE ^ 4 + ... + k5
        while(kickers.size() < 5) kickers.push_back(0);

        int score = hand_rank;

        for(int& k: kickers){
            score = score * BASE + k;
        }
        return score;
    };

    // ---------- HAND TYPES ----------

    // Straight Flush
    if(straight && flush) return encode(8, {straight_high});

    // Four of a Kind
    if(groups[0].first == 4) return encode(7, {groups[0].second, groups[1].second});

    // Full House
    if(groups[0].first == 3 && groups[1].first == 2){
        return encode(6, {groups[0].second, groups[1].second});
    }

    // Flush
    if(flush) return encode(5, values);

    // Straight
    if(straight) return encode(4, {straight_high});

    // Three of a Kind
    if(groups[0].first == 3){
        vector<int> kickers = {groups[0].second};

        for(int i = 1; i < groups.size(); i++){
            for(int j = 0; j < groups[i].first; j++){
                kickers.push_back(groups[i].second);
            }
        }

        return encode(3, kickers);
    }

    // Two Pair
    if(groups[0].first == 2 && groups[1].first == 2){
        return encode(2, {
            groups[0].second,
            groups[1].second,
            groups[2].second
        });
    }

    // One Pair
    if(groups[0].first == 2){
        vector<int> kickers = {groups[0].second};

        for(int i = 1; i < groups.size(); i++){
            for(int j = 0; j < groups[i].first; j++){
                kickers.push_back(groups[i].second);
            }
        }

        return encode(1, kickers);
    }

    // High Card
    return encode(0, values);
}


/*
Monte Carlo Range Equity

Instead of guessing hand strength, we simulate thousands of random runouts.

Example process:

Hero: AhKh
Board: Qh 7d 2c
Opponent: random hand

Steps repeated many times:

Generate opponent hand

Deal remaining turn / river

Evaluate both hands

Count wins / losses

After many simulations:

equity = wins / total

Example on a flop:

Hero equity: 0.62

Meaning 62% chance to win vs random hand.
*/

double Poker_Simulation::monte_carlo_equity_multiway(const vector<string>& hero_cards, const string& street, int num_opponents, int simulations){

    int wins = 0;
    int ties = 0;

    string ranks = "23456789TJQKA";
    string suits = "hdcs";

    // Build base deck
    vector<string> base_deck;

    for(char r: ranks){
        for(char s: suits){
            string card{r, s};
            
            if(!ranges::contains(hero_cards, card) && !ranges::contains(game_state.board[street], card)){ //ranges works with vectors
                base_deck.push_back(card);
            }
        }
    }
    
    mt19937 gen(random_device{}());

    for(int i = 0; i < simulations; i++){
        vector<string> deck = base_deck; // deep copy every simulation
        shuffle(deck.begin(), deck.end(), gen);

        // Opponent hands
        vector<vector<string>> opp_hands;
        for(int i = 0; i < num_opponents; i++){
            vector<string> opp_hand = {deck.back()}; 
            deck.pop_back();

            opp_hand.push_back(deck.back()); 
            deck.pop_back();

            opp_hands.push_back(opp_hand);
        }

        // Complete board
        vector<string> remaining_board = game_state.board[street];
        while(remaining_board.size() < 5){
            remaining_board.push_back(deck.back());
            deck.pop_back();
        }
        // // simple hand strength evaluator
        // double hero_strength = evaluate_hand_strength(hero_cards, remaining_board);
        // vector<double> opp_strengths;
        
        // for(const vector<string>& opp_hand: opp_hands) opp_strengths.push_back(evaluate_hand_strength(opp_hand, remaining_board));
        
        // // 7 card evaluator
        // double hero_strength = evaluate_7card(hero_cards, remaining_board);
        // vector<double> opp_strengths;

        // for(const vector<string>& opp_hand: opp_hands) opp_strengths.push_back(evaluate_7card(opp_hand, remaining_board));

        // 7 card evaluator fast
        double hero_strength = evaluate_7card_fast(hero_cards, remaining_board);
        vector<double> opp_strengths;
        
        for(const vector<string>& opp_hand: opp_hands) opp_strengths.push_back(evaluate_7card_fast(opp_hand, remaining_board));

        if(all_of(opp_strengths.begin(), opp_strengths.end(), [&](double opp_strength){return hero_strength > opp_strength;})) wins++;
        
        else if(any_of(opp_strengths.begin(), opp_strengths.end(), [&](double opp_strength){return hero_strength == opp_strength;})) ties++;
    }

    return (wins + ties * 0.5) / simulations;
}


/*
Board Texture

Flop strategy depends heavily on board texture.

Example categories:

Dry board     → A72 rainbow
Wet board     → JT9 two-tone
Paired board  → KK5
*/

void Poker_Simulation::analyze_board_texture(const string& street){
    set<char> suits;
    vector<char> ranks;

    for(string& card: game_state.board[street]) {
        suits.insert(card[1]);
        ranks.push_back(card[0]);
    }

    set<char> set_ranks(ranks.begin(), ranks.end());

    game_state.board_texture.flush_draw = (suits.size() <= 2); // Check for 2 or less different suits for flush draw
    game_state.board_texture.paired = (set_ranks.size() < ranks.size()); // Check for duplicate ranks, aka pairs or trips
}


// 3. Decision - making
double Poker_Simulation::adjust_weight_for_previous_raises(double& weight, int num_raises){
    /*
    Adjust hand weight dynamically based on previous raises.
    */

    if(num_raises == 0) return weight;

    else if(num_raises == 1) return weight * 0.5; // reduce medium/weak hands

    else return max(0.0, weight - 0.3); // strong raises only play strong hands
}


pair<string, double> Poker_Simulation::decide_preflop_action(Player& player, const string& position){
    /*
        Decide pre-flop action using hand, position, pot, previous actions, and stack.
        Enforces minimum raise rules.
    */

    double weight = preflop_ranges[position][player.hand];

    int num_raises = 0;
    for(const Action& action: game_state.actions) {
        if(action.action == "raise") num_raises++;
    }

    weight = adjust_weight_for_previous_raises(weight, num_raises);

    if(weight < 0.3) return {"fold", 0.0};

    double call_amount = max(0.0, game_state.current_bet - player.contribution);

    mt19937 gen(random_device{}());
    uniform_real_distribution<> dist(0.0, 1.0);
    double action_roll = dist(gen);

    // Determine raise amount respecting min-raise
    auto get_raise_amount = [&](double multiplier){
        // Intended_raise = extra chips you *want to add* on top of calling
        double intended_raise = call_amount + game_state.pot * multiplier;

        // Min_total_raise = minimum total contribution required to make a legal raise
        double min_total_raise = game_state.current_bet + game_state.last_raize_size;

        // Total_raise = total contribution you will have in the pot after your raise
        // Ensures raise respects the minimum
        double total_raise = max(intended_raise + player.contribution, min_total_raise);

        // Actual chips to put in now, capped by your stack, allows for 'all in' without raising to the required legal amount
        return min(player.stack, total_raise - player.contribution);
    };

    if(weight < 0.7){
        if(num_raises == 0){
            if(action_roll < 0.6){
                double bet = get_raise_amount(0.5 + weight);
                return {"raise", bet};
            }

            else return {"call", min(player.stack, call_amount)};
        }

        else return {"call", min(player.stack, call_amount)};
    }

    else{
        if(num_raises == 0) return {"raise", get_raise_amount(1 + weight)};

        else if(num_raises == 1) return {"raise", get_raise_amount(1.5 + weight)};

        else{
            if(weight > 0.9) return {"raise", get_raise_amount(2 + weight)};
    
            else return {"fold", 0.0};
        }
    }
}


/*
Flop

Inputs should include:

hand_strength

position

pot

current_bet

stack

board_texture

previous_actions
*/

pair<string, double> Poker_Simulation::decide_flop_action(Player& player, int num_opponents){
    /*
    Make flop decisions based on equity vs multiple opponents and board texture.
    */

    const vector<string>& hero_cards = player.cards;

    analyze_board_texture("flop");
    
    double equity = monte_carlo_equity_multiway(hero_cards, "flop", num_opponents);

    double call_amount = max(0.0, game_state.current_bet - player.contribution);
    
    mt19937 gen(random_device{}());
    uniform_real_distribution<> dist(0.0, 1.0);
    double action_roll = dist(gen);

    auto get_raise_amount = [&](double multiplier){
        double intended_raise = call_amount + game_state.pot * multiplier;

        double min_total_raise = game_state.current_bet + game_state.last_raize_size;
        
        double total_raise = max(intended_raise + player.contribution, min_total_raise);
        
        return min(player.stack, total_raise - player.contribution);
    };

    if(equity > 0.65){
        if(game_state.current_bet == player.contribution){
            return {"bet", min(player.stack, game_state.pot * (game_state.board_texture.flush_draw ? 0.7 : 0.6))};
        }

        else return {"raise", get_raise_amount(game_state.board_texture.flush_draw ? 1.2 : 1.0)};
    }

    else if(equity > 0.35){
        if(game_state.current_bet == player.contribution){
            if(game_state.board_texture.paired) return {"check", 0.0};
            
            else if(action_roll < 0.4) return {"bet", min(player.stack, game_state.pot * 0.5)};
            
            else return {"check", 0.0};
        }

        else{
            if(action_roll < 0.8) return {"call", min(player.stack, call_amount)};
            
            else return {"raise", get_raise_amount(0.7)};
        }
    }

    else{
        if(game_state.current_bet == player.contribution) {
            double bluff_chance = 0.15 + (game_state.board_texture.flush_draw ? 0.2 : 0.0);
            
            if(action_roll < bluff_chance) return {"bet", min(player.stack, game_state.pot * 0.5)};
            
            else return {"check", 0.0};
        }

        else{
            double fold_chance = 0.7 + (game_state.board_texture.paired ? 0.2 : 0.0);
            
            if(action_roll < fold_chance) return {"fold", 0.0};
            
            else return {"call", min(player.stack, call_amount)};
        }
    }
}

/*
Turn

Turn should:

reduce bluff frequency

increase aggression with strong hands

allow semi-bluffs if board is wet
*/


pair<string, double> Poker_Simulation::decide_turn_action(Player& player, int num_opponents){
    /*
    Make turn decisions based on equity vs multiple opponents and board texture.
    
    Turn strategy:
    - Lower bluff frequency
    - Higher aggression with strong hands
    - Semi-bluffing on wet boards
    */

    const vector<string>& hero_cards = player.cards;

    analyze_board_texture("turn");

    double equity = monte_carlo_equity_multiway(hero_cards, "turn", num_opponents);

    double call_amount = max(0.0, game_state.current_bet - player.contribution);
    
    mt19937 gen(random_device{}());
    uniform_real_distribution<> dist(0.0, 1.0);
    double action_roll = dist(gen);

    auto get_raise_amount = [&](double multiplier){
        double intended_raise = call_amount + game_state.pot * multiplier;

        double min_total_raise = game_state.current_bet + game_state.last_raize_size;
        
        double total_raise = max(intended_raise + player.contribution, min_total_raise);
        
        return min(player.stack, total_raise - player.contribution);
    };

    // --- Detect draws (very important for semi-bluffs) ---
    map<char, int> suit_counts;

    for(const string& card: game_state.board["turn"]) suit_counts[card[1]]++;
    suit_counts[hero_cards[0][1]]++;
    suit_counts[hero_cards[1][1]]++;

    bool has_flush_draw = ranges::any_of(suit_counts, [](const auto& suit_count) {return suit_count.second == 4;}); // checks if any suit appears exactly 4 times

    // Simple straight draw detection
    string ranks = "23456789TJQKA";
    set<int> rank_values;

    for(const string& card: game_state.board["turn"]) rank_values.insert(ranks.find(card[0]));
    rank_values.insert(ranks.find(hero_cards[0][0]));
    rank_values.insert(ranks.find(hero_cards[1][0]));

    bool has_straight_draw = false;
    vector<int> vals(rank_values.begin(), rank_values.end());

    // vals = [5, 6, 7, 8] -> 8 - 5 = 3 ≤ 4 -> true -> check for open draws and some gunshots
    // vals = [5, 6, 7, 9] -> 9 - 5 = 4 -> still true
    for(int i = 0; i + 3 < vals.size(); i++){
        if(vals[i + 3] - vals[i] <= 4) has_straight_draw = true;
    }

    bool semi_bluff = has_flush_draw || has_straight_draw;

    // --- STRONG HANDS ---
    if(equity > 0.7){
        // Bigger bets on turn
        if(game_state.current_bet == player.contribution) return {"bet", min(player.stack, game_state.pot * (game_state.board_texture.flush_draw ? 0.8 : 0.7))};
        
        // More aggressive raises
        else return {"raise", get_raise_amount(game_state.board_texture.flush_draw ? 1.3 : 1.1)};
    }

    // --- MEDIUM HANDS ---
    else if(equity > 0.4){
        if(game_state.current_bet == player.contribution){
            if(action_roll < 0.3) return {"bet", min(player.stack, game_state.pot * 0.5)};

            else return {"check",0.0};
        }
        
        else{
            if(action_roll < 0.75) return {"call", min(player.stack, call_amount)};
            
            else return {"fold", 0.0};
        }
    }

    // --- WEAK HANDS / SEMI-BLUFF ---
    else{
        if(game_state.current_bet == player.contribution){
            // Only semi-bluff, not pure bluff
            if(semi_bluff && game_state.board_texture.flush_draw && action_roll < 0.4) return {"bet", min(player.stack, game_state.pot * 0.6)};

            return {"check", 0.0};
        }

        else{
            if(semi_bluff){
                // Continue with draws sometimes
                if(action_roll < 0.5) return {"call", min(player.stack, call_amount)};
                
                else if(action_roll < 0.7) return {"raise", get_raise_amount(0.8)};
            }

            return {"fold", 0.0};
        }
    }
}


/*
River

River should:

almost no bluffs

strong hands value bet

medium hands mostly call

weak hands fold
*/

pair<string, double> Poker_Simulation::decide_river_action(Player& player, int num_opponents){
    /*
    Make river decisions based on equity and board texture.

    River strategy:
    - Almost no bluffs
    - Strong hands → value bet / raise
    - Medium hands → mostly call
    - Weak hands → fold
    */

    const vector<string>& hero_cards = player.cards;

    analyze_board_texture("river");
    
    double equity = monte_carlo_equity_multiway(hero_cards, "river", num_opponents);

    double call_amount = max(0.0, game_state.current_bet - player.contribution);
    
    mt19937 gen(random_device{}());
    uniform_real_distribution<> dist(0.0, 1.0);
    double action_roll = dist(gen);

    auto get_raise_amount = [&](double multiplier){
        double intended_raise = call_amount + game_state.pot * multiplier;

        double min_total_raise = game_state.current_bet + game_state.last_raize_size;
        
        double total_raise = max(intended_raise + player.contribution, min_total_raise);
        
        return min(player.stack, total_raise - player.contribution);
    };

    // --- STRONG HANDS ---
    if(equity > 0.75){
        // Value bet
        if(game_state.current_bet == player.contribution) return {"bet", min(player.stack, game_state.pot * (game_state.board_texture.flush_draw ? 0.8 : 0.7))};

        // Raise for value
        else return {"raise", get_raise_amount(game_state.board_texture.flush_draw ? 1.2 : 1.0)};
    }

    // --- MEDIUM HANDS ---
    else if(equity > 0.4){
        if(game_state.current_bet == player.contribution){
            // Mostly check, occasionally bet small
            if(action_roll < 0.2) return {"bet", min(player.stack, game_state.pot * 0.4)};

            else return {"check", 0.0};
        }
        
        // Mostly call
        else return {"call", min(player.stack, call_amount)};
    }

    // --- WEAK HANDS ---
    else{
        if(game_state.current_bet == player.contribution) return {"check", 0.0};
        
        else return {"fold", 0.0};
    }
}


// Decide postflop action based on street
pair<string, double> Poker_Simulation::decide_postflop_action(Player& player, int num_opponents, const string& street){

    if(street == "flop") return decide_flop_action(player, num_opponents);

    else if(street == "turn") return decide_turn_action(player, num_opponents);

    else return decide_river_action(player, num_opponents);
}


// 4. Simulation / gameplay
string Poker_Simulation::cards_to_hand(const string& card1, const string& card2, const string& ranks){
    char r1 = card1[0], s1 = card1[1];
    char r2 = card2[0], s2 = card2[1];

    // Ensure higher rank first
    if(ranks.find(r1) < ranks.find(r2)){
        swap(r1, r2);
        swap(s1, s2);
    }

    // Pocket pair
    if(r1 == r2){
        return string(1, r1) + string(1, r2);
    }

    string suited = (s1 == s2) ? "s" : "o";
    return string(1, r1) + string(1, r2) + suited;
};


void Poker_Simulation::deal_poker_hand(int& sb_index){
    /*
    Creates a shuffled deck, deals 2 cards per player,
    converts them to poker hand notation (AKs, AKo, AA),
    and deals flop, turn, river with burn cards.
    */

    int num_players = game_state.seating.size();

    string ranks = "23456789TJQKA";
    string suits = "hdcs";
    vector<string> deck;

    // Build deck
    for(char r: ranks) {
        for(char s: suits) {
            deck.push_back(string(1, r) + string(1, s));
        }
    }

    // Shuffle deck
    mt19937 gen(random_device{}());
    shuffle(deck.begin(), deck.end(), gen);

    // Deal hole cards to players from SB
    for(int i = 0; i < num_players; i++) {
        string c1 = deck.back();
        deck.pop_back();

        string c2 = deck.back();
        deck.pop_back();

        int player_id = game_state.seating[(sb_index + i) % num_players]; // ensure player in SB position in seating is dealt first
        
        game_state.player_states[player_id].hand = cards_to_hand(c1, c2, ranks);

        game_state.player_states[player_id].cards = {c1, c2};

    }

    // ---- FLOP ----
    deck.pop_back(); // burn 1

    game_state.board["flop"] = {deck.back()}; 
    deck.pop_back();

    game_state.board["flop"].push_back(deck.back()); 
    deck.pop_back();
    
    game_state.board["flop"].push_back(deck.back()); 
    deck.pop_back();

    // ---- TURN ----
    deck.pop_back(); // burn 2

    game_state.board["turn"] = game_state.board["flop"];
    game_state.board["turn"].push_back(deck.back()); 
    deck.pop_back();

    // ---- RIVER ----
    deck.pop_back(); // burn 3

    game_state.board["river"] = game_state.board["turn"];
    game_state.board["river"].push_back(deck.back());
    deck.pop_back();
}


void Poker_Simulation::simulate_street(const string& street){

    // Initialize preflop game_state
    if(street == "preflop") initialize_preflop();

    // Initialize postflop game_state
    else{
        if(game_state.game_ended) return;

        initialize_postflop(street);
    }

    while(game_state.players_to_act > 0){

        int player_id = game_state.seating[game_state.action_index];
        Player& player = game_state.player_states[player_id];

        // Skip folded players
        if(!player.folded){
            string position = get_general_position(game_state.action_index);
            
            int active_players = 0;
            for(auto& p: game_state.player_states) if(!p.folded) active_players++;

            int num_opponents = active_players - 1;

            string action;
            double bet;

            if(street == "preflop") tie(action, bet) = decide_preflop_action(player, position);

            else tie(action, bet) = decide_postflop_action(player, num_opponents, street);
            
            // Fold
            if(action == "fold") player.folded = true;
            
            // Call / Call / Bet / Raise
            else{
                if(action == "bet" && bet == 0.00) action = "check";
                else if(action == "raise" && bet == 0.00) action = "all-in";
                
                player.stack -= bet;
                player.contribution += bet;
                game_state.pot += bet;
                
                // Detect new bet / raise
                if(player.contribution > game_state.current_bet) {
                    game_state.last_raize_size = player.contribution - game_state.current_bet;
                    game_state.current_bet = player.contribution;

                    // reset action cycle
                    game_state.players_to_act = game_state.num_players;
                }
            }

            // Record action
            game_state.actions.push_back({
                player.id,
                player.hand,
                position,
                action,
                bet,
                street
            });
            
            if(action == "all-in"){
                cout << "Player " << player.id << ": (" << game_state.positions_order[game_state.action_index] << ", " << position << ") with " << player.hand
                << " -> all - in" << ((player.id == game_state.my_player_id) ? " (YOU)" : "") << "\n";
            }

            else{
                cout << "Player " << player.id << ": (" << game_state.positions_order[game_state.action_index] << ", " << position << ") with " << player.hand
                 << " -> " << action << " " << fixed << setprecision(2) << bet << ((player.id == game_state.my_player_id) ? " (YOU)" : "") << "\n";
            }
        }

        // Move to next player
        game_state.action_index = (game_state.action_index + 1) % game_state.num_players;
        game_state.players_to_act--;

        // Skip folded players
        while(game_state.player_states[game_state.seating[game_state.action_index]].folded && game_state.players_to_act > 0){
            game_state.action_index = (game_state.action_index + 1) % game_state.num_players;
            game_state.players_to_act--;
        }

        // End hand if only 1 player remains
        int active_players = 0;
        int last_player_id = -1;

        for(const Player& player: game_state.player_states) {
            if(!player.folded){
                active_players++; 
                last_player_id = player.id;
            }
        }

        if(active_players == 1){
            Player& winner = game_state.player_states[last_player_id];
            winner.stack += game_state.pot;

            game_state.actions.push_back({winner.id, 
                                        winner.hand, 
                                        get_general_position(game_state.seating[winner.id]), 
                                        "win_uncontested", 
                                        game_state.pot, 
                                        street});

            cout << "Player " << winner.id << ": (" << game_state.positions_order[winner.id] << ", " << get_general_position(game_state.seating[winner.id])
            << ") with " << game_state.player_states[winner.id].hand << " -> win_uncontested " << fixed << setprecision(2) << game_state.pot << ((winner.id == game_state.my_player_id) ? " (YOU)" : "") << "\n";
            
            cout << "Player " << winner.id << ": (" << game_state.positions_order[winner.id] << ", " << get_general_position(game_state.seating[winner.id])
            << ") wins uncontested " << fixed << setprecision(2) << game_state.pot << ((street != "flop") ? " during " : "post") << street << "!" << ((winner.id == game_state.my_player_id) ? " (YOU)" : "") << "\n";
            
            // End game
            game_state.game_ended = true;
            end_game(street);
            return;
        }
    }
}


void Poker_Simulation::simulate_showdown(){
    /*
    Resolve the final showdown at the river for remaining active players.
    */

    if(game_state.game_ended) return;

    cout << "\n-- SHOWDOWN --\n\n";
    vector<string> board = game_state.board["river"];

    cout << "Board - river: [ ";
    for(int i = 0; i < game_state.board["river"].size() - 1; i++) cout << game_state.board["river"][i] << ", ";
    cout << game_state.board["river"][game_state.board["river"].size() - 1] << " ]\n\n";

    map<int, HandCombo> hand_strengths;

    for(Player& player: game_state.player_states){
        if(!player.folded){
            HandCombo hand_combo = evaluate_hand_strength_combo(player.cards, board);
            hand_strengths[player.id] = hand_combo;

            cout << "Player " << player.id << ": (" << game_state.positions_order[player.id] << ", " << get_general_position(game_state.seating[player.id]) << ") with " << player.hand
            << " -> cards: (" << player.cards[0] << ", " << player.cards[1] << "), hand_type: " << hand_combo.combo;
            
            cout << ", combo: [ ";
            for(int i = 0; i < hand_combo.combo_cards.size() - 1; i++) cout << hand_combo.combo_cards[i] << ", ";
            cout << hand_combo.combo_cards[hand_combo.combo_cards.size() - 1] << " ]" << ((player.id == game_state.my_player_id) ? " (YOU)" : "") << "\n";
        }
    }

    // Determine winners
    auto is_better = [](const HandCombo& a, const HandCombo& b) {
        if(a.rank != b.rank) return a.rank > b.rank; // Rank by rank

        return a.tiebreak > b.tiebreak; // Else rank by tiebreaker -> vector comparison is lexicographic
    };

    // Determine best hand
    HandCombo best_hand;
    bool is_empty = true; // when best_hand is uninitialized

    for(auto& [player_id, hand_combo]: hand_strengths){
        if(is_empty || is_better(hand_combo, best_hand)){
            best_hand = hand_combo;
            is_empty = false;
        }
    }

    // Determine winners
    vector<int> winners;

    for(auto& [player_id, hand_combo]: hand_strengths){
        if(hand_combo.rank == best_hand.rank && hand_combo.tiebreak == best_hand.tiebreak){
            winners.push_back(player_id);
        }
    }

    // Split pot
    double pot_share = game_state.pot / winners.size();

    for(int& winner_id: winners) {
        game_state.player_states[winner_id].stack += pot_share;

        game_state.actions.push_back({winner_id,
                                    game_state.player_states[winner_id].hand,
                                    get_general_position(game_state.seating[winner_id]),
                                    "win_showdown",
                                    pot_share,
                                    "river"});
        
        cout << "Player " << winner_id << ": (" << game_state.positions_order[winner_id] << ", " << get_general_position(game_state.seating[winner_id]) << ") with " << game_state.player_states[winner_id].hand << " -> win_showdown" << ((winner_id == game_state.my_player_id) ? " (YOU)" : "") << "\n";
        cout << "Player " << winner_id << ": (" << game_state.positions_order[winner_id] << ", " << get_general_position(game_state.seating[winner_id]) << ") wins " << pot_share << " from the showdown!" << ((winner_id == game_state.my_player_id) ? " (YOU)" : "") << "\n";
    }

    // End game
    game_state.game_ended = true;
    end_game("showdown");
}


void Poker_Simulation::end_game(const string& street){
    cout << "\n-- END OF GAME --\n";
    cout << "Final Pot Size: " << fixed << setprecision(2) << game_state.pot << "\n\n";

    rotate(game_state.seating.rbegin(), game_state.seating.rbegin() + 1, game_state.seating.rend());

    cout << left << setw(10) << "Player" << left << setw(15) << "New Position" << left << setw(20) << "Stack" << "\n";

    for(const Player& player: game_state.player_states){
        string player_label = to_string(player.id);
        if(player.id == game_state.my_player_id) player_label += " (YOU)";

        double net_profit = player.stack - player.starting_stack;

        // Stack + Net Profit as one string
        ostringstream stack_stream;
        stack_stream << fixed << setprecision(2) << player.stack << " (" << (net_profit >= 0 ? "+" : "") << fixed << setprecision(2) << net_profit << ")";
        string stack_with_net_profit = stack_stream.str();

        cout << left << setw(10) << player_label << left << setw(15) << game_state.positions_order[game_state.seating[player.id]]
        << left << setw(20) << stack_with_net_profit << "\n";
    }

    cout << "\nHand ended " << street << ".\n";
}


// Run a single hand simulation
void Poker_Simulation::run_simulation(){

    // Preflop
    simulate_street("preflop");

    // Postflop
    simulate_street("flop");

    // Turn
    simulate_street("turn");

    // River
    simulate_street("river");

    // Showdown
    simulate_showdown();
}


// Run multiple simulations
void Poker_Simulation::run_simulations(int simulations, double stack, int num_players, int my_player_id, double sb_amount) {

    // Initialize players and stacks
    initialize_parameters(num_players, stack, my_player_id, sb_amount);

    cout << "\n--------- RUNNING SIMULATION ---------\n";

    for(int i = 1; i <= simulations; i++){
        // Check for bankrupt players
        vector<int> broke_players;
        for(Player& player: game_state.player_states){
            if(player.stack == 0.0) broke_players.push_back(player.id);
        }

        if(broke_players.size() == 0){
            cout << "\n--------- GAME " << i << " ---------\n\n";
            run_simulation();
        }
        
        else if(broke_players.size() == 1){
            cout << "\nPlayer " << broke_players[0] << " has run out of funds.\n";
            cout << "\n--------- END OF SIMULATION ---------\n";
            return;
        }
        
        else{
            cout << "Players ";
            for(int i = 0; i < broke_players.size() - 1; i++) cout << broke_players[i] << ", ";
            cout << "\n" << broke_players[broke_players.size() - 1] << " have run out of funds.\n";
            cout << "\n--------- END OF SIMULATION ---------\n";
            return;
        }
    }

    cout << "\n--------- END OF SIMULATION ---------\n";
}