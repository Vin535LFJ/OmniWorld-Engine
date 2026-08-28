# ADR-0002: Why CUDA

## Context

OmniWorld Engine is a small-team, GPU-native real-time runtime project. It must avoid becoming a broad game engine, autonomous-driving stack, simulator, ROS replacement, or model-training platform.

## Problem

The project needs stable architectural decisions that explain why a technology or boundary exists, what alternatives were considered, and when the decision should be revisited.

## Considered Options

- Build the capability in Engine Core.
- Integrate a third-party implementation behind an adapter.
- Reuse an ecosystem as an external application dependency.
- Defer the capability until benchmark evidence exists.

## Decision

Prefer a runtime-first decision: Engine Core owns memory/resource lifetime, scheduling, synchronization, dataflow contracts, metrics, and minimal world-state contracts; external ecosystems are adapters unless they directly define the core GPU dataflow problem.

## Why

This keeps the project valuable, learnable, and executable for a personal or small-team roadmap while preserving technical depth in GPU runtime engineering.

## Trade-offs

- More adapter work is required.
- Some third-party convenience abstractions are not used directly in core code.
- More benchmarks are required before claims become project facts.

## Consequences

The project remains focused on measured GPU data movement and avoids cloning large systems.

## Revisit Conditions

Revisit if benchmarks show the decision blocks performance, maintainability, or the ability to deliver MVP milestones.
