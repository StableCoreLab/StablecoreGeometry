# AI Execution Spec (stablecore-geometry)

## 1. Execution Goal

Each iteration must produce a **closed capability unit**, including:

- ≥1 new capability (deterministic & testable)
- ≥1 explicit gap (clearly marked as NOT supported)
- Code + Tests + Docs

---

## 2. Coverage Rules (MANDATORY)

Each iteration MUST include:

- Capability test (happy path)
- Edge-case test (boundary condition)
- Gap test (if ambiguity exists)

All behavior must be:

- Deterministic
- Reproducible
- Stable across runs

---

## 3. Breadth Rules (MANDATORY)

Each iteration must:

- Fully complete P1
- AND touch at least one of P2 / P3

❌ Forbidden: only working on a single module

---

## 4. Capability vs Gap Discipline

- Capability = stable, deterministic, test-covered behavior
- Gap = explicitly unsupported or ambiguous behavior

❌ Forbidden:

- Hiding unstable logic as capability
- Silent fallback without diagnostics

---

## 5. Required Outputs

Each iteration MUST update:

- docs/session-handoff.md
- docs/next-task-prompt.md
- docs/test-capability-coverage.md
- docs/design-doc-sync-tracker.md

---

## 6. Code Constraints

- DO NOT modify public SDK contracts
- DO NOT introduce breaking changes
- Internal helper refactoring is allowed

---

## 7. Diagnostics Requirement (if applicable)

For modules supporting diagnostics:

- Must expose explanation or issue when ambiguity occurs
- Must keep result / diagnostics consistent

---

## 8. Forbidden Behaviors

- ❌ Only writing code without tests
- ❌ Only happy-path implementation
- ❌ Partial implementation without marking gap
- ❌ Fake completeness (claiming support without coverage)
- ❌ Silent non-deterministic behavior