# Monte-Carlo-Decision-Engine
A high-performance decision engine using Monte Carlo simulation for imperfect-information environments, applied to No-Limit Texas Hold’em (NLHE). The engine evaluates possible actions, simulates outcomes, and determines optimal strategies under uncertainty. Supports multi-agent gameplay, dynamic stack sizes, and stochastic policy evaluation, subject to capital constraints and risk of ruin dynamics.

## Project Overview
This project implements a Monte Carlo–based decision engine for No-Limit Texas Hold’em. Players’ decisions are simulated across many random game continuations, allowing evaluation of actions in terms of expected value (EV) and risk. This setup mirrors real-world high-dimensional decision-making problems like quantitative trading.

## This repository includes:
A C++ Monte Carlo decision engine for high-speed simulations
A Python prototype for conceptual understanding and visualization
Example game states and simulations for testing multi-agent interactions

## Features

| Feature                      | Description                                                                             |
| -----------------------------|-----------------------------------------------------------------------------------------|
| Monte Carlo Simulation       | Evaluates actions by simulating many random game outcomes                               |
| Multi-Agent Gameplay         | Supports multiple players with independent strategies                                   |
| Stack & Pot Awareness        | Considers dynamic stack sizes and pot sizes when evaluating decisions                   |
| Action Space Flexibility     | Handles folding, calling, and raising with variable bet sizes                           |
| Expected Value Calculation   | Computes EV for each action to guide optimal decision-making                            |
| Game State Snapshots         | Inspect the current game state, including player stacks, community cards, and pot       |
| Logging & Analysis           | Tracks decision outcomes and EV distributions for review and further analysis           |
| C++ & Python                 | Fast C++ engine for performance, Python version for prototyping and visualization       |

## Project Structure
- poker.cpp (High-performance C++ Monte Carlo engine)
- poker.exe (Compiled C++ Executable)
- poker.ipynb (Python version for rapid prototyping and visualization)

## How it works

### Monte Carlo Evaluation Logic
The engine evaluates possible actions by simulating random continuations of the game:

The engine maintains a simulation game state for the overall game as well as individual player states, structured as:

```cpp
struct Player {
    int id
    string hand
    vector<string> cards
    double stack
    double starting_stack
    double contribution = 0
    bool folded = false
};

struct Game_State {
    double pot = 0.0
    vector<Action> actions   <- Logs player actions
    double current_bet = 0.0
    double last_raize_size = 0.0
    double sb_amount

    vector<string> positions_order
    vector<int> seating
    vector<Player> player_states

    int num_players
    int my_player_id
    bool game_ended = false
    int action_index
    int players_to_act

    unordered_map<string, vector<string>> board
    BoardTexture board_texture
};
```

- Players initialized with initial stacks, and dealt hole cards during 
- Sell orders match the highest buy price first
- Market orders are immediate or cancel (IOC)
- Limit orders match if possible, otherwise, they are added to the book



## No-Limit Texas Hold'em Monte Carlo Decision Engine Pipeline
The engine simulates large-scale batches of No-Limit Texas Hold’em hands from initialization to termination, modeling a full decision pipeline under uncertainty. 
By running repeated Monte Carlo simulations, it estimates action-level expected value (EV), hand equities, and strategic outcomes across diverse game states.

Deal → Preflop → Flop → Turn → River → Showdown / End Game → Reset / New Game

1. Deal (Initialization)
Construct and shuffle a 52-card deck
Assign positions (UTG → BTN → SB → BB)
Deal 2 hole cards to each player
Post blinds and initialize pot
Reset player states and contributions

2. Preflop
Players act in positional order
Decisions based on:
Position-based hand ranges
Prior action (raises, calls)
Stack sizes and pot state
Actions:
Fold / Call / Raise (with dynamic sizing)
Updates:
Pot size
Player stacks
Action history

4. Flop
Reveal 3 community cards
Initialize postflop betting round
Evaluate:
Monte Carlo equity (multiway)
Board texture (flush draw, paired board)
Strategy:
Strong hands → bet/raise
Medium hands → mixed strategy
Weak hands → fold or occasional bluff

5. Turn
Reveal 4th community card
Recompute:
Equity
Draw potential (flush / straight)
Strategy adjustments:
Reduced bluff frequency
Increased aggression for strong hands
Semi-bluffing with draws

6. River
Reveal final community card
Compute final equity (no uncertainty remaining)
Strategy:
Strong hands → value bet / raise
Medium hands → call/check
Weak hands → fold
Bluffing is minimal

7. Showdown (if required)
Triggered if multiple players remain
Evaluate best 5-card hands from 7 cards
Compare scores using fast evaluator
Determine winner(s)
Split pot in case of ties

8. End Game (Early Termination)
Triggered when only one player remains
Remaining player:
Wins entire pot
Skips remaining streets and showdown

9. Reset / New Game
Clear game state:
Pot
Player contributions
Board cards
Action history
Prepare for next simulation

Players continue with their current remaining stack for the next game.

Simulation Loop Integration

The full pipeline is executed repeatedly during Monte Carlo simulations:

```cpp
for each simulation:
    initialize game state
    run full pipeline (deal → termination)
    record outcome (win / loss / tie)
```

This enables:

Large-scale probabilistic evaluation
Stable expected value (EV) estimation
Efficient exploration of uncertain outcomes

## Multi-Agent Strategy
Each player is treated as an independent agent

Policies can be:
- Uniform random
- Predefined strategies
- Learned / historical tendencies
- The engine aggregates expected outcomes across all opponents to inform the optimal action


## Stack & Pot Awareness
Raises and bets are evaluated relative to the current stack sizes
EV calculations incorporate potential future stack fluctuations
Fold, call, and raise options are dynamically adjusted depending on the situation


## Bankruptcy & Risk of Ruin Modeling

The engine incorporates explicit capital constraints by tracking stack evolution across simulations. Players reaching zero capital are removed (bankrupt), introducing a natural stopping condition and enabling analysis of risk of ruin.

Enforces budget constraints on all actions
Models all-in transitions as boundary conditions
Enables evaluation of EV vs. drawdown risk
Supports large-scale simulation of capital survival probabilities

## Key System Features

| Feature                      | Description                                      |
| -----------------------------|--------------------------------------------------|
| End-to-End Simulation        | Models full lifecycle of a poker hand            |
| Stochastic Decision Flow     | Randomized actions simulate mixed strategies     |
| Multi-Agent Interaction      | Multiple players acting independently            |
| Early Termination            | Optimized exit when hand is decided              |
| Scalable Monte Carlo Loop    | Thousands of independent simulations             |
| Bankruptcy Handling          | Handling risk management / capital constraints   |


## Preflop Decision Logic

Preflop actions are determined using position-based hand ranges, action history, and stochastic weighting. Each hand is assigned a probability weight depending on position (early, middle, late), which is then dynamically adjusted based on prior aggression.

When a decision is evaluated, the engine:

Maps the player’s position to a predefined hand range (early / middle / late)
Retrieves the base weight for the specific hand (e.g. AKs, TT)
Adjusts the weight based on the number of prior raises
Samples a random value to introduce stochastic action selection
Determines whether to fold, call, or raise based on the adjusted weight
Key Mechanics
Position-Based Ranges
Different hand strengths are played depending on table position:
Early → tight range
Middle → balanced range
Late → wider range
Dynamic Weight Adjustment
Hand strength is penalized as aggression increases:
No raises → full range used
1 raise → medium/weak hands reduced
Multiple raises → only strong hands continue
Raise Sizing Logic
Raise amounts are determined relative to:
Current pot size
Previous raise size (minimum raise rule)
Player stack (supports all-in scenarios)
Stochastic Action Selection
Randomization is applied to:
Prevent deterministic play
Simulate mixed strategies
Postflop Decision Logic

## Postflop decisions (Flop, Turn, River) are driven by Monte Carlo equity estimation, board texture analysis, and game context. The engine evaluates hand strength probabilistically rather than relying on static heuristics.

When a postflop decision is evaluated, the engine:

- Simulates random game outcomes using Monte Carlo rollouts
- Computes equity against multiple opponents
- Analyzes board texture (flush draws, paired boards, etc.)
- Adjusts strategy based on street (flop, turn, river)
- Selects an action using both equity thresholds and stochastic sampling



## Hand Strength Evaluation

The engine evaluates poker hands using multiple approaches, with a focus on performance and scalability for Monte Carlo simulations.

7-Card Fast Evaluator

The primary evaluation method is a custom bitmask-based 7-card evaluator (evaluate_7card_fast), optimized for speed during large-scale simulations.

The evaluator:

Encodes each card into an integer representation:
Rank (2–14)
Suit (0–3)
Prime number (for uniqueness)
Uses:
Bitmasks for rank and suit tracking
Frequency arrays for duplicate detection (pairs, trips, quads)
Key Mechanics
Flush Detection
Uses suit-specific bitmasks to count cards per suit:
A flush is detected if ≥5 cards share the same suit
Straight Detection
Uses a rank bitmask:
Checks consecutive sequences using bitwise operations
Handles edge cases like wheel straight (A-2-3-4-5)
Hand Classification
Determines hand type in descending order:
Straight Flush
Four of a Kind
Full House
Flush
Straight
Three of a Kind
Two Pair
One Pair
High Card
Score Encoding
Each hand is converted into a single comparable integer:
Combines hand rank and kicker values using a base-15 system
Ensures fast comparison between hands without complex structures

This approach avoids expensive combinatorics and enables efficient evaluation across thousands of simulations.


Alternative Evaluators

The engine also includes:
- Hand Strength evaluator (Simple fast evaluator but lacks kicker logic)
- Hand Strength combo evaluator (Detailed evaluator used during Showdown with details on individual hand combos)
- Full 7-card evaluator (evaluate_7card)
- Iterates over all 5-card combinations
- More explicit but slower (used for validation)
- 5-card evaluator (evaluate_5card)
- Evaluates fixed 5-card hands
- Used internally by the full evaluator

These implementations provide correctness benchmarks for the optimized fast evaluator.


## Monte Carlo Equity Simulation

The core of the engine is a Monte Carlo–based equity calculator (monte_carlo_equity_multiway) used for postflop decision-making.

Simulation Process

For each simulation:

Deck Construction
Generate a full deck
Remove:
Hero’s cards
Known board cards
Opponent Sampling
Randomly assign hole cards to each opponent
Ensures no card duplication
Board Completion
Deal remaining community cards up to 5 total
Hand Evaluation
Evaluate hero and opponent hands using evaluate_7card_fast
Outcome Tracking
Win → +1
Tie → +0.5
Loss → +0
Equity Calculation

After all simulations:
```cpp
equity = (wins + 0.5 × ties) / total_simulations
```
This represents the probability of winning against multiple opponents.

Multi-Agent Considerations
Supports multiple opponents simultaneously
Evaluates outcomes against all opponent hands
Requires hero to beat all opponents to win the pot
Performance Considerations
Uses:
Efficient deck copying per simulation
Fast evaluator (evaluate_7card_fast)
Random shuffling via mt19937
Typical simulation counts:
1,000 → fast approximation
10,000+ → stable equity estimates
Why Monte Carlo?

Closed-form evaluation is infeasible due to:

Combinatorial explosion of possible hands
Hidden information (opponent cards)
Multiple players

Monte Carlo simulation provides:

Scalable approximation
Flexibility for complex game states
Realistic modeling of uncertainty



## Board Texture Analysis

Board texture significantly influences optimal strategy. The engine analyzes the structure of community cards to adjust decision-making dynamically.

Texture Features

The engine evaluates:

Flush Draw Potential
Triggered when ≤2 suits are present on the board
Indicates high likelihood of flush completion
Paired Boards
Detected when duplicate ranks exist
Increases probability of full houses or trips
How It’s Used

Board texture directly impacts:

Bet sizing
Bluff frequency
Aggression levels

Examples:

Wet boards (draw-heavy)
→ Larger bets, more semi-bluffing
Dry boards (low connectivity)
→ More checking, lower bluff frequency
Paired boards
→ More cautious play, reduced bluffing





## Flop Strategy

The flop introduces the first community cards and the highest uncertainty.

The engine:

Computes equity using multiway Monte Carlo simulation
Detects board texture:
Flush draw potential
Paired boards
Applies action thresholds:
Strong hands → aggressive betting / raising
Medium hands → mixed strategy (check/call/bet)
Weak hands → fold or occasional bluff

Bluff frequency increases on:

Draw-heavy boards
Situations with no prior aggression

## Turn Strategy

The turn reduces uncertainty and increases the importance of draw evaluation.

The engine:

Recomputes equity with updated board
Detects:
Flush draws (4 cards of same suit)
Straight draw potential
Adjusts strategy:
Strong hands → larger bets and raises
Medium hands → more cautious (call/check)
Weak hands → fold unless semi-bluffing
Semi-Bluffing
Enabled when:
Flush draw OR straight draw is present
Allows:
Calling with drawing hands
Occasional raises to apply pressure

Bluff frequency is reduced compared to the flop.

## River Strategy

The river represents a fully realized game state with no future cards.

The engine:

Computes final equity (no uncertainty remaining)
Eliminates most bluffing behavior
Focuses on value extraction and risk control

Action tendencies:

Strong hands → value bet / raise
Medium hands → mostly call or check
Weak hands → fold

Bluffing is minimal and only occurs in rare edge cases.

## Key Mechanics Across Streets
Monte Carlo Equity Estimation
Simulates thousands of random outcomes to estimate win probability
Multi-Agent Evaluation
Equity is calculated against multiple opponents simultaneously
Board Texture Awareness
Strategy adapts to:
Draw-heavy boards
Paired boards
Dry vs wet textures
Dynamic Bet Sizing
Bet sizes scale with:
Pot size
Board texture
Hand strength
Stochastic Decision Making
Randomization ensures:
Non-deterministic play
Realistic mixed strategies

## Showdown Resolution

At the end of a hand, the engine determines the winner(s) by evaluating all remaining active players’ hands and distributing the pot accordingly.

Showdown Process

When a showdown is triggered, the engine:

Identifies all non-folded players
Combines each player’s:
Hole cards
Community board cards
Evaluates the best 5-card hand using the 7-card evaluator
Compares all hands to determine the winner(s)
Hand Comparison
Each player’s hand is converted into a numeric score using the evaluator
Scores encode:
Hand rank (e.g. flush, straight)
Kicker values (tie-breakers)
The engine:
Finds the maximum score across all players
Identifies all players with that score
Tie Handling (Split Pots)

If multiple players share the best hand:

The pot is split evenly among winners
Each winner receives:
```cpp
pot / number_of_winners
```
Remaining chips (if any due to rounding) can be:
Distributed to earliest position (optional), or
Ignored depending on implementation
Pot Distribution

For each winning player:

Add winnings to their stack
Update final stack values

For losing players:

Their contributions remain in the pot
Example
Board: Ah Kh Qh Jh 2c

Player 1: Th 9h → Straight Flush  
Player 2: Ac Ad → Three of a Kind  

Result:
Player 1 wins entire pot
Multi-Agent Considerations
Supports multiway pots (3+ players)
Requires the winning hand to beat all opponents
Integrates seamlessly with Monte Carlo simulations:
Each simulation ends in a showdown evaluation
Integration with Simulation

Showdown resolution is used in:

Full hand simulations (simulate_showdown)
Monte Carlo rollouts for equity calculation

Each simulation:

Runs to completion
Resolves a showdown
Returns the outcome (win / loss / tie)



## Getting Started
1. Compile the C++ Engine
g++ -std=c++17 -O2 -o engine poker.cpp

2. Run the Simulation

./poker.exe

You’ll be prompted:

Please enter simulations, stack, num_players, my_player_id, sb_amount (or 0 to quit):
simulations: Number of game simulations
stack: Chips available to each player
num_players: Number of players
my_player_id: id of your player (Defaults to player.id = 0, where you start off as the UTG)
sb_amount: Small blind amount



## Python Version (For Prototyping)
The Python version (poker.py) provides a simplified version of the Monte Carlo decision engine logic. Simple for visualization and concept validation but not optimized for speed.

The Python version allows:
- Rapid experimentation with smaller simulation counts
- Visualization of EV distributions
- Understanding multi-agent dynamics
- Simple testing and debugging of hand strength evaluation methods

## Features
| Feature                | Description                                                                      |
| -----------------------|----------------------------------------------------------------------------------|
| Limit & Market Orders  | Rapid experimentation with smaller simulation counts                                  |
| FIFO Matching          | Visualization of EV distributions |
| Cancel Orders          |Understanding multi-agent dynamics                 |
| Hand Strength Evaluators           | Simple testing and debugging of hand strength evaluation methods                             |
| CSV Order Generator    | Generates randomized test data with cancel logic                                 |
| Snapshot Querying      | View order book state at any time for a specific ticker                          |
| C++ & Python           | Dual implementation for learning and conceptual understanding                    |


## Example Outputs
Here is how the engine behaves when initializing a session:

### C++
### C++ (Initializing Parameters)
simulations: 5
stack: 1000
num_players: 6
my_player_id: 0 (Starting position: UTG)
sb_amount: 10


### C++ (Full Game Simulation)


### Python (Full Game Simulation)

```python
--------- GAME 2 ---------

my_player_id 1
Current Seating: [5, 0, 1, 2, 3, 4]

DEALING CARDS...

PRE-FLOP

Player 3: (SB, early) with 86s -> post SB 10.00
Player 4: (BB, early) with Q6o -> post BB 20.00
Player 5: (UTG, early) with AJo -> raise 74.00
Player 0: (MP, middle) with Q3o -> fold 0.00
Player 1: (CO, late) with AKo -> call 74.00 (YOU)
Player 2: (BTN, late) with 53o -> fold 0.00
Player 3: (SB, early) with 86s -> fold 0.00
Player 4: (BB, early) with Q6o -> fold 0.00

FLOP

Board - flop: ['7h', '5h', '7s'] 

Player 5: (UTG, early) with AJo -> check 0.00
Player 1: (CO, late) with AKo -> check 0.00 (YOU)

TURN

Board - turn: ['7h', '5h', '7s', '2d'] 

Player 5: (UTG, early) with AJo -> check 0.00
Player 1: (CO, late) with AKo -> check 0.00 (YOU)

RIVER

Board - river: ['7h', '5h', '7s', '2d', '7d'] 

Player 5: (UTG, early) with AJo -> check 0.00
Player 1: (CO, late) with AKo -> check 0.00 (YOU)

SHOWDOWN

Board - river: ['7h', '5h', '7s', '2d', '7d'] 

Player 1: (MP, middle) with AKo -> cards: (Kh, As), hand_type: Three of a Kind, combo: ['7h', '7s', '7d', 'As', 'Kh'] (YOU)
Player 5: (BB, early) with AJo -> cards: (Ah, Jc), hand_type: Three of a Kind, combo: ['7h', '7s', '7d', 'Ah', 'Jc']
Player 1: (MP, middle) with AKo -> win_showdown (YOU)
Player 1: (MP, middle) wins 178.00 from the showdown!

END OF GAME
Final pot size: 178.00

Updated Stacks and Net Profits:
Player 0: 1000 (0)
Player 1: 1104.0 (+104.0)
Player 2: 1000 (0)
Player 3: 990 (-10)
Player 4: 1000.0 (-20.0)
Player 5: 906.0 (-74.0)

Hand ended showdown.

Rotating seating...
New Seating: [4, 5, 0, 1, 2, 3]

```
