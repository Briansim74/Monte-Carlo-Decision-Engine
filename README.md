# Monte Carlo Decision Engine (Imperfect Information Systems)

A high-performance C++ Monte Carlo decision engine for sequential decision-making under uncertainty. The system evaluates optimal actions by simulating large-scale stochastic outcomes in imperfect-information environments.

The primary implementation applies to No-Limit Texas Hold’em Poker, used as a structured testbed for modeling:
- Probabilistic decision-making under uncertainty
- Multi-agent interaction in partially observable systems
- Risk-adjusted expected value maximization
- Sequential decision processes with state evolution

The framework is designed to generalize to **market-like environments where outcomes depend on latent information, opponent behavior, and stochastic state transitions**, closely resembling problems in quantitative trading and execution.

## Core Idea
At each decision point, the engine evaluates possible actions by:
1. Sampling future state trajectories via Monte Carlo simulation
2. Modeling opponent strategies as stochastic policies
3. Propagating uncertainty through sequential game states
4. Estimating **action-level expected value (EV) distributions**
5. Selecting actions via risk-adjusted decision rules

</br>

This mirrors trading problems such as:
- Execution under uncertain fill probabilities
- Adversarial market participants
- Path-dependent payoff structures
- Risk-constrained decision-making under uncertainty

## Key Features
| Feature                           | Description                                                                                             |
|-----------------------------------|---------------------------------------------------------------------------------------------------------|
| Monte Carlo Decision Engine       | Evaluates actions by simulating large numbers of stochastic future trajectories under uncertainty.      |
| Sequential State Simulation       | Models full game lifecycle (preflop → river → terminal state) as a path-dependent decision process.     |
| Multi-Agent Interaction           | Supports independent agents with stochastic policies and heterogeneous decision strategies.             |
| Expected Value Evaluation         | Computes action-level EV and outcome distributions to guide optimal decision-making.                    |
| Risk-Constrained Decision Making  | Incorporates capital dynamics, stack evolution, and risk-of-ruin constraints into action selection.     |
| Fast C++ Simulation Core          | High-performance engine optimized for large-scale Monte Carlo rollouts.                                 |
| Bitmask Hand Evaluator            | Efficient 7-card hand evaluation using bitwise operations for scalable simulation.                      |
| Stochastic Policy Sampling        | Introduces randomized decision policies to model mixed-strategy equilibrium behavior.                   |
| Board & State Awareness           | Dynamically adapts decisions based on evolving public state and information structure.                  |
| Python Research Prototype         | Lightweight implementation for validation, visualization, and strategy testing.                         |

### Monte Carlo Decision Framework
- Large-scale rollout simulation of future trajectories
- Action evaluation via expected value and outcome distributions
- Robust under hidden information and stochastic policies

### Multi-Agent Stochastic System
- Independent agents with heterogeneous strategies
- Opponent modeling via probabilistic policy sampling
- Interaction-driven state evolution

### Sequential Decision Process
- Full hand lifecycle simulation (preflop → river → terminal state)
- Path-dependent state transitions
- Dynamic adaptation across decision stages

### Risk-Constrained Decision Making
- Explicit capital tracking across simulations
- Risk-of-ruin modeling via stack depletion dynamics
- All-in / boundary condition handling

### Efficient Game State Engine
- Optimized C++ simulation core
- Fast hand evaluation via bitmask-based 7-card evaluator
- Scalable Monte Carlo execution (10,000+ rollouts per decision)

## System Architecture
The engine models a full sequential decision pipeline:
```
Initialize State
→ Sample Private Information
→ Preflop Decision
→ Flop Decision
→ Turn Decision
→ River Decision
→ Terminal Evaluation
→ Outcome Recording
→ Repeat (Monte Carlo Loop)
```

Each trajectory represents one possible realization of the environment under uncertainty.

## Decision Mechanism
At each decision node:
```
1. Generate legal action set (fold / call / raise)

2. Simulate downstream outcomes for each action

3. Compute:
- Expected Value (EV)
- Variance / outcome distribution
- Risk-adjusted payoff (capital survival weighted)

4. Select action using stochastic policy evaluation
```

This enables **mixed-strategy behavior rather than deterministic rules**, consistent with equilibrium-style decision-making under uncertainty.

## Multi-Agent Environment
The system models multiple interacting agents with:
- Partial observability (hidden cards / information asymmetry)
- Independent stochastic strategies
- Position-aware decision structures
- Adaptive response to prior actions

</br>

This creates a **non-stationary decision environment**, similar in structure to:
- Limit order book interaction
- Adversarial execution environments
- Competitive liquidity provision

## Risk & Capital Dynamics
The engine explicitly tracks:
- Stack evolution over time
- Drawdown and depletion events
- Bankruptcy (absorbing state conditions)

</br>

This enables:
- Probability of ruin estimation
- Survival-weighted EV evaluation
- Capital-constrained decision optimization

## Implementation
### C++ Core Engine
- High-performance Monte Carlo simulation loop
- Bitmask-based 7-card hand evaluation
- Optimized random sampling (mt19937)
- Efficient state transition system

### Python Prototype
- Research and visualization layer
- Rapid testing of decision logic
- Validation of Monte Carlo convergence behavior

## Why Monte Carlo?
Closed-form evaluation is infeasible due to:
- Combinatorial explosion of future states
- Hidden information asymmetry
- Multi-agent strategy dependence
- Path-dependent outcomes

</br>

Monte Carlo provides:
- Scalable approximation of expected value
- Flexible modeling of complex strategies
- Robust decision-making under uncertainty

## Key Insight
The system demonstrates that optimal decisions in uncertain sequential environments depend not on deterministic rules, but on:

**Distributional outcome modeling over future state trajectories**
<br><br>

This directly parallels quantitative trading problems such as:
- Execution strategy optimization
- Market-making under uncertainty
- Adversarial order flow interaction

## Trading & Systems Mapping
Although implemented in a poker environment, the underlying decision framework generalizes to financial markets under uncertainty.

| Poker Concept                         | Trading Analogy                                       |
|---------------------------------------|-------------------------------------------------------|
| Hole cards (private information)      | Private signal / inventory / alpha signal             |
| Community cards                       | Public market information (order flow, price action)  |
| Betting action (fold / call / raise)  | Order decision (pass / trade / size up / aggress)     |
| Stack size                            | Capital allocation / risk budget                      |
| Pot size                              | Market opportunity / payoff size                      |
| Blinds                                | Transaction costs / spread / fees                     |
| Opponents                             | Other market participants / liquidity providers       |
| Betting rounds                        | Sequential execution / decision stages                |
| Showdown                              | Trade resolution / PnL realization                    |
| Stochastic opponent policies          | Market microstructure uncertainty                     |


## Summary
This project implements a Monte Carlo-based decision system for sequential multi-agent environments, emphasizing:
- Probabilistic reasoning
- Risk-aware optimization
- Stochastic policy evaluation
- Scalable simulation of uncertain futures

While implemented in a poker environment, the underlying structure generalizes to quantitative trading, execution modeling, and reinforcement learning under uncertainty.

## Example Outputs
Below are sample outputs illustrating how the Monte Carlo decision engine behaves across different simulation scenarios and state transitions.

### System Initialization
<img width="724" height="149" alt="initialize_parameters" src="https://github.com/user-attachments/assets/2fa6d3bd-fd47-4c58-9c00-1e6f5fd6ea32" />

</br>
System initializes game state, player positions, and simulation parameters prior to Monte Carlo rollout execution.
</br></br>

### Early Termination Event
<img width="397" height="705" alt="early_termination" src="https://github.com/user-attachments/assets/344ef7ed-33bd-4623-a69f-1e2219a937c7" />

</br>
Simulation terminates early when a single agent accumulates all available capital, representing an absorbing state in the stochastic process.
</br></br>

### Full Game Simulation - Single Bankruptcy
<img width="701" height="929" alt="full_game_simulation_self_bankruptcy" src="https://github.com/user-attachments/assets/d30dc978-faae-4a0c-b2d9-923b89361589" />

</br>
Example Monte Carlo trajectory showing full sequential decision process and capital depletion dynamics for one agent.
</br></br>

### Full Game Simulation - Multiple Bankruptcies
<img width="678" height="952" alt="full_game_simulation_multiple_bankruptcies" src="https://github.com/user-attachments/assets/86924520-803e-4e8d-87b0-a7e23db2d5dc" />

<br>
Multi-agent simulation illustrating concurrent capital depletion across stochastic trajectories.
</br></br>

### Python Prototype (Validation Layer)
Below is a sample output from the Python reference implementation used for rapid prototyping and validation of decision logic.

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

</br></br></br>


<details>
<summary><strong>Detailed Implementation & Usage</strong></summary>

# Monte-Carlo-Decision-Engine
A high-performance decision engine using Monte Carlo simulation for imperfect-information environments, applied to No-Limit Texas Hold’em Poker (NLHE). The engine evaluates possible actions, simulates outcomes, and determines optimal strategies under uncertainty. Supports multi-agent gameplay, dynamic stack sizes, and stochastic policy evaluation, subject to capital constraints and risk of ruin dynamics.

## Project Overview
This project implements a Monte Carlo-based decision engine for No-Limit Texas Hold’em Poker. Players’ decisions are simulated across many random game continuations, allowing evaluation of actions in terms of expected value (EV) and risk. This setup mirrors real-world high-dimensional decision-making problems like quantitative trading.

## This repository includes:
- A C++ Monte Carlo decision engine for high-speed simulations
- A Python prototype for conceptual understanding and visualization
- Example game states and simulations for testing multi-agent interactions

<br>

### Features

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

<br>

## Project Structure
- poker.cpp (High-performance C++ Monte Carlo engine)
- poker.exe (Compiled C++ Executable)
- poker.ipynb (Python version for rapid prototyping and visualization)

<br>

## No-Limit Texas Hold'em Monte Carlo Decision Engine Pipeline
The engine simulates large-scale batches of No-Limit Texas Hold’em hands from initialization to termination, modeling a full decision pipeline under uncertainty. By running repeated Monte Carlo simulations, it estimates action-level expected value (EV), hand equities, and strategic outcomes across diverse game states.

<br>

### Deal → Preflop → Flop → Turn → River → Showdown → New Game

<br>

### 1. Deal (Initialization)
- Reset game state, player states and contributions
- Assign positions (UTG → MP → CO → BTN → SB → BB)
- Construct and shuffle a 52-card deck
- Deal 2 hole cards to each player
- Post blinds and initialize pot

<br>


### 2. Preflop

Players act in positional order

Decisions based on:
- Position-based hand ranges
- Prior action (raises, calls)
- Stack sizes, current bet size, pot state

Actions:
- Fold / Call / Raise (with dynamic sizing)

Updates:
- Pot size
- Player stacks
- Player contributions
- Action history

<br>


### 3. Flop

Reveal 3 community cards

Initialize postflop betting round

Evaluate:
- Hand strength evaluations for equity calculation (7card_fast, hand_strength, hand_strength_combo, 7card)
- Monte Carlo equity (multiway)
- Board texture (flush draw, paired board)

Strategy:
- Strong hands → bet / raise
- Medium hands → mixed strategy
- Weak hands → fold or occasional bluff

<br>


### 4. Turn

Reveal 4th community card

Recompute:
- Equity
- Draw potential (flush / straight)

Strategy adjustments:
- Reduced bluff frequency
- Increased aggression for strong hands
- Semi-bluffing with draws

<br>

### 5. River

Reveal final community card

Compute final equity (no uncertainty remaining)

Strategy:
- Strong hands → value bet / raise
- Medium hands → call/check
- Weak hands → fold
- Bluffing is minimal

<br>

### 6. Showdown

Triggered if multiple players remain:
- Evaluate best 5-card hands from 7 cards
- Compare scores using hand_strength_combo
- Determine winner(s)
- Split pot in case of ties

<br>

### 7. End Game (Early Termination)

Triggered when only one player remains

Remaining player:
- Wins entire pot
- Skips remaining streets and showdown

<br>

### 8. New Game

Clear game state:
- Pot
- Board cards
- Player contributions
- Prepare for next simulation

Players continue with their current remaining stack for the next game.

<br>

## Simulation Loop Integration

The full pipeline is executed repeatedly during Monte Carlo simulations:

**For each simulation:**
- initialize game state
- run full pipeline (deal → termination)
- record outcome (win / loss / tie)

**This enables:**
- Large-scale probabilistic evaluation<br>
- Stable expected value (EV) estimation<br>
- Efficient exploration of uncertain outcomes<br>

<br>

## Multi-Agent Strategy
Each player is modeled as an independent agent operating in an imperfect-information environment,<br>
with access only to private hole cards and shared public state.

**Policies can be:**
- Uniform random
- Predefined strategies
- Learned / historical tendencies

**Stack & Pot Awareness:**
- Raises and bets are evaluated relative to the current stack sizes
- EV calculations incorporate potential future stack fluctuations
- Fold, call, and raise options are dynamically adjusted depending on the situation

<br>

## Bankruptcy & Risk of Ruin Modeling
The engine incorporates explicit capital constraints by tracking stack evolution across simulations. Once any players reaches zero capital, the entire simulation ends, introducing a natural stopping condition and enabling analysis of risk of ruin.

- Enforces budget constraints on all actions
- Models all-in transitions as boundary conditions
- Enables evaluation of EV vs. drawdown risk
- Supports large-scale simulation of capital survival probabilities

<br>

## Key System Features

| Feature                      | Description                                      |
| -----------------------------|--------------------------------------------------|
| End-to-End Simulation        | Models full lifecycle of a poker hand            |
| Stochastic Decision Flow     | Randomized actions simulate mixed strategies     |
| Multi-Agent Interaction      | Multiple players acting independently            |
| Early Termination            | Optimized exit when hand is decided              |
| Scalable Monte Carlo Loop    | Thousands of independent simulations             |
| Bankruptcy Handling          | Handling risk management / capital constraints   |

<br>

## Game States and Player States
The engine maintains an overall game state as well as individual player states, structured as:

```cpp
struct Player {
    int id;
    string hand;
    vector<string> cards;
    double stack;
    double starting_stack;
    double contribution = 0;
    bool folded = false;
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
```

<br>

## Preflop Decision Logic

Preflop actions are determined using position-based hand ranges, action history, and stochastic weighting. Each hand is assigned a probability weight depending on position (early, middle, late), which is then dynamically adjusted based on prior aggression.

<br>

When a decision is evaluated, the engine:
- Maps the player’s seating position to a predefined hand range (UTG → early / MP → middle / BTN → late)
- Retrieves the base weight for the specific hand (e.g. AKs, TT)
- Adjusts the weight based on the number of prior raises
- Samples a random value to introduce stochastic action selection
- Determines whether to fold, call, or raise based on the adjusted weight

<br>

**Position-Based Ranges**<br>

Different hand strengths are played depending on table position:
- Early → tight range
- Middle → balanced range
- Late → wider range

<br>

**Dynamic Weight Adjustment**<br>

Hand strength is penalized as aggression increases:
- No raises → full range used
- 1 raise → medium / weak hands reduced
- Multiple raises → only strong hands continue

<br>

**Raise Sizing Logic** <br>

Raise amounts are determined relative to:
- Hand strength
- Current pot size
- Previous raise size (minimum raise rule)
- Player stack (supports all-in scenarios)
- Stochastic action selection

<br>

**Stochastic Weighting** <br>

Randomization is applied to:
- Prevent deterministic play
- Simulate mixed strategies

<br>

## Postflop Decision Logic
Postflop decisions (Flop, Turn, River) are driven by Monte Carlo equity estimation, board texture analysis, and game context.
The engine evaluates hand strength probabilistically rather than relying on static heuristics.

<br>

When a postflop decision is evaluated, the engine:
- Simulates random game outcomes using Monte Carlo rollouts
- Computes equity against multiple opponents
- Analyzes board texture (flush draws, paired boards, etc.)
- Adjusts strategy based on street (flop, turn, river)
- Selects an action using both equity thresholds and stochastic sampling

<br>

## Hand Strength Evaluation
The engine evaluates poker hands using multiple approaches, with a focus on performance and scalability for Monte Carlo simulations.

### 7-Card Fast Evaluator
The primary evaluation method is a custom bitmask-based 7-card evaluator (evaluate_7card_fast), optimized for speed during large-scale simulations. The evaluator encodes each card into an integer representation:
- Rank (2-14)
- Suit (0-3)
- Prime number (for uniqueness)

It uses bitmasks for rank and suit tracking and frequency arrays for duplicate detection (pairs, trips, quads)

<br>

**Flush Detection**<br>
Uses suit-specific bitmasks to count cards per suit:<br>
- A flush is detected if ≥5 cards share the same suit

<br>

**Straight Detection**<br>
Uses a rank bitmask:<br>
- Checks consecutive sequences using bitwise operations<br>
- Handles edge cases like wheel straight (A-2-3-4-5)

<br>

**Hand Classification**<br>
Determines hand type in descending order:
- Straight Flush
- Four of a Kind
- Full House
- Flush
- Straight
- Three of a Kind
- Two Pair
- One Pair
- High Card

<br>

**Score Encoding**<br>
Each hand is converted into a single comparable integer:<br>
- Combines hand rank and kicker values using a base-15 system<br>
- Ensures fast comparison between hands without complex structures

This approach avoids expensive combinatorics and enables efficient evaluation across thousands of simulations.

<br>

**Alternative Evaluators:**
- evaluate_hand_strength (Simple fast evaluator but lacks kicker logic)
- evaluate_hand_strength_combo (Detailed evaluator used during Showdown with details on individual hand combos)
- evaluate_7card (Iterates over all 5-card combinations using evaluate_5card, more explicit but slower, used for validation)
- evaluate_5card (Evaluates fixed 5-card handstrengths, used internally by evaluate_7card)

<br>

These implementations provide correctness benchmarks for the optimized evaluate_7card_fast evaluator.

<br>

## Monte Carlo Equity Simulation
The core of the engine is a Monte Carlo-based equity calculator (monte_carlo_equity_multiway) used for postflop decision-making.

**For each simulation:**
- Deck Construction
- Generate a full deck

**Remove:**
- Hero’s cards
- Known board cards

**Opponent Sampling:**
- Randomly assign hole cards to each opponent
- Ensures no card duplication

**Board Completion:**
- Deal remaining community cards up to 5 total

**Hand Evaluation:**
- Evaluate hero and opponent hands using evaluate_7card_fast

**Outcome Tracking:**
- Win → +1
- Tie → +0.5
- Loss → +0

**Equity Calculation**

After all simulations:
```cpp
equity = (wins + 0.5 × ties) / total_simulations
```
This represents the probability of winning against multiple opponents.

<br>

**Multi-Agent Considerations:**
- Supports multiple opponents simultaneously
- Evaluates outcomes against all opponent hands
- Requires hero to beat all opponents to win the pot
- Performance considerations

**Uses:**
- Efficient deck copying per simulation
- Fast evaluator (evaluate_7card_fast)
- Random shuffling via mt19937
- Typical simulation counts:
- 2,000 → fast approximation
- 10,000+ → stable equity estimates

<br>

**Why Monte Carlo?** <br>
Closed-form evaluation is infeasible due to:
- Combinatorial explosion of possible hands
- Hidden information (opponent cards)
- Multiple players

**Monte Carlo simulation provides:**
- Scalable approximation
- Flexibility for complex game states
- Realistic modeling of uncertainty

<br>

## Board Texture Analysis
Board texture significantly influences optimal strategy. The engine analyzes the structure of community cards to adjust decision-making dynamically.

**The engine evaluates:**
- Flush Draw Potential
- Triggered when ≤2 suits are present on the board
- Indicates high likelihood of flush completion

**Paired Boards:**
- Detected when duplicate ranks exist
- Increases probability of full houses or trips

**How It’s Used**<br>
Board texture directly impacts:
- Bet sizing
- Bluff frequency
- Aggression levels

**Examples:**
- Wet boards (draw-heavy) → Larger bets, more semi-bluffing<br>
- Dry boards (low connectivity) → More checking, lower bluff frequency<br>
- Paired boards → More cautious play, reduced bluffing

<br>

## Flop Strategy
The flop introduces the first community cards and the highest uncertainty.

**The engine:**
- Computes equity using multiway Monte Carlo simulation

**Detects board texture:**
- Flush draw potential
- Paired boards

**Applies action thresholds:**
- Strong hands → aggressive betting / raising
- Medium hands → mixed strategy (check / call / bet)
- Weak hands → fold or occasional bluff

**Bluff frequency increases on:**
- Draw-heavy boards
- Situations with no prior aggression

<br>

## Turn Strategy
The turn reduces uncertainty and increases the importance of draw evaluation.

**The engine:**
- Recomputes equity with updated board

**Detects:**
- Flush draws (4 cards of same suit)
- Straight draw potential

**Adjusts strategy:**
- Strong hands → larger bets and raises
- Medium hands → more cautious (call/check)
- Weak hands → fold unless semi-bluffing

**Semi-Bluffing:**
- Enabled when flush draw OR straight draw is present

**Allows:**
- Calling with drawing hands
- Occasional raises to apply pressure

Bluff frequency is reduced compared to the flop.

<br>

## River Strategy
The river represents a fully realized game state with no future cards.

**The engine:**
- Computes final equity (no uncertainty remaining)
- Eliminates most bluffing behavior
- Focuses on value extraction and risk control

**Action tendencies:**
- Strong hands → value bet / raise
- Medium hands → mostly call or check
- Weak hands → fold

Bluffing is minimal and only occurs in rare edge cases.

<br>

## Key Mechanics Across Streets
**Monte Carlo Equity Estimation**<br>
Simulates thousands of random outcomes to estimate win probability

**Multi-Agent Evaluation**<br>
Equity is calculated against multiple opponents simultaneously

**Board Texture Awareness**<br>
Strategy adapts to:
- Draw-heavy boards
- Paired boards
- Dry vs wet textures

**Dynamic Bet Sizing**<br>
Bet sizes scale with:
- Pot size
- Board texture
- Hand strength
- Stochastic Decision Making

**Randomization ensures:**
- Non-deterministic play
- Realistic mixed strategies

<br>

## Showdown
At the end of a hand, the engine determines the winner(s) by evaluating all remaining active players’ hands and distributing the pot accordingly.

**When a showdown is triggered, the engine:**
- Identifies all non-folded players
- Combines each player’s hole cards and community board cards
- Evaluates the best 5-card hand using the evaluate_hand_strength_combo evaluator
- Compares all hands to determine the winner(s)

**Hand Comparison:**
- Each player’s hand is converted into a numeric score using the evaluator

**Scores encode:**
- Hand rank (e.g. flush, straight)
- Kicker values (tie-breakers)

**The engine:**
- Finds the maximum score across all players
- Identifies all players with that score

**Tie Handling (Split Pots):**
- If multiple players share the best hand, the pot is split evenly among winners

Each winner receives:
```cpp
pot / number_of_winners
```

<br>

**Remaining chips (if any due to rounding) can be:**
- Distributed to earliest position (optional), or
- Ignored depending on implementation

<br>

**Pot Distribution** <br>
For each winning player:
- Add winnings to their stack
- Update final stack values

**For losing players:**
- Their contributions remain in the pot

<br>

**Example Board:**  Ah Kh Qh Jh 2c <br>

Player 1: Th 9h → Straight Flush (Wins entire pot) <br>  
Player 2: Ac Ad → Three of a Kind  

<br>

## Getting Started
1. Compile the C++ Engine
g++ -std=c++17 -O2 -o engine poker.cpp

2. Run the simulation on shell: ./poker.exe

3. You’ll be prompted: Please enter simulations, stack, num_players, my_player_id, sb_amount (or 0 to quit):

<br>

| Parameter         | Description                                                                      | Example  |
| ------------------|----------------------------------------------------------------------------------|----------|
| simulations       | Number of game simulations                                                       | 5        |
| stack             | Starting chips available to each player                                          | 1000     |
| num_players       | Number of players                                                                | 6        |
| my_player_id      | id of your player (Defaults to player.id = 0, where you start off as the UTG)    | 0        |
| sb_amount         | Small blind amount                                                               | 10       |
| quit              | End Simulation                                                                   | 0        |

<br>

## Python Version (For Prototyping)
The Python version (poker.py) provides a simplified version of the Monte Carlo decision engine logic. Simple for visualization and concept validation but not optimized for speed.<br>

<br>

| Features                                                          |
| ------------------------------------------------------------------|
| Rapid experimentation with smaller simulation counts              |
| Visualization of EV distributions                                 |
| Understanding multi-agent dynamics                                |
| Simple testing and debugging of hand strength evaluation methods  |

<br>

## Example Outputs
Here is how the engine behaves when initializing a session:

### C++ (Initializing Parameters)
<img width="724" height="149" alt="initialize_parameters" src="https://github.com/user-attachments/assets/2fa6d3bd-fd47-4c58-9c00-1e6f5fd6ea32" />

### C++ (Early Termination)
<img width="397" height="705" alt="early_termination" src="https://github.com/user-attachments/assets/344ef7ed-33bd-4623-a69f-1e2219a937c7" />

### C++ (Full Game Simulation, Single Bankruptcy - Self)
<img width="701" height="929" alt="full_game_simulation_self_bankruptcy" src="https://github.com/user-attachments/assets/d30dc978-faae-4a0c-b2d9-923b89361589" />

### C++ (Full Game Simulation, Multiple Bankruptices)
<img width="678" height="952" alt="full_game_simulation_multiple_bankruptcies" src="https://github.com/user-attachments/assets/86924520-803e-4e8d-87b0-a7e23db2d5dc" /><br>


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

</details>
</br></br>
