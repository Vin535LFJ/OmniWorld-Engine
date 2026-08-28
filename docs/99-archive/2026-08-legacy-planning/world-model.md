# World Runtime Model

## What is a World?

A World is the runtime-owned state space that can be observed, updated, rendered, and acted on. It may represent a real scene, synthetic scene, or hybrid scene, but it is not itself a foundation model.

## What is an Observation?

An Observation is a bounded view of WorldState for a consumer: renderer output, tensor, image, structured perception result, or textual summary. Observations must declare timestamp, coordinate frame, memory residency, and ownership.

## What is an Action?

An Action is an intentional change request produced by a brain, controller, script, or adapter. It must be validated before mutating WorldState or external systems.

## What is a WorldTransition?

A WorldTransition is a measured update from previous WorldState plus input/action to next WorldState. It records causality, timing, and confidence.

## What is Simulation?

Simulation is a provider of synthetic observations and state transitions. It is an adapter or application dependency, not Engine Core.

## What is Perception?

Perception converts sensor or rendered data into structured state estimates. It may use CUDA, CV-CUDA, TensorRT, or external models.

## What is Rendering?

Rendering converts WorldState into visual observations or display output. In this project it is a runtime/observation capability, not a full game-engine product.
