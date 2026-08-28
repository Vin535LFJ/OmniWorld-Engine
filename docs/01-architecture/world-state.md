# World State Architecture

WorldState is the minimal structured representation that allows perception, rendering, simulation, and Brain adapters to exchange meaning without owning each other's implementation details.

## Core terms

- World: runtime-owned state space that can be observed, updated, rendered, and acted on.
- Observation: bounded view of WorldState or sensor/render output for a consumer.
- Action: validated request to change WorldState or an external environment.
- WorldTransition: measured update from previous state plus input/action to next state.

## Constraint

WorldState is not a trained world model. Foundation models and VLA systems are Brain/model providers.
